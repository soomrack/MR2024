#include "video_streamer.h"
#include <QDebug>
#include <QDateTime>
#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>

// ================== Callback функции ==================

GstFlowReturn new_sample_cb(GstElement *sink, gpointer user_data)
{
    GStreamerThread *thread = static_cast<GStreamerThread*>(user_data);

    GstSample *sample = gst_app_sink_pull_sample(GST_APP_SINK(sink));
    if (!sample) {
        return GST_FLOW_ERROR;
    }

    GstBuffer *buffer = gst_sample_get_buffer(sample);
    GstCaps *caps = gst_sample_get_caps(sample);

    // Получаем информацию о формате видео
    GstStructure *structure = gst_caps_get_structure(caps, 0);
    int width, height;
    gst_structure_get_int(structure, "width", &width);
    gst_structure_get_int(structure, "height", &height);

    // Получаем данные кадра
    GstMapInfo map;
    gst_buffer_map(buffer, &map, GST_MAP_READ);

    // Создаем QImage из данных RGB
    if (map.size > 0 && width > 0 && height > 0) {
        QImage image(map.data, width, height, QImage::Format_RGB888);

        // Используем публичные методы для доступа к данным
        thread->lockMutex();
        thread->setCurrentFrame(image.copy());
        thread->setFrameNew(true);
        thread->unlockMutex();

        emit thread->frameUpdated();
    }

    gst_buffer_unmap(buffer, &map);
    gst_sample_unref(sample);

    return GST_FLOW_OK;
}

gboolean bus_callback(GstBus *bus, GstMessage *msg, gpointer user_data)
{
    GStreamerThread *thread = static_cast<GStreamerThread*>(user_data);

    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_ERROR: {
        GError *err;
        gchar *debug;
        gst_message_parse_error(msg, &err, &debug);

        QString errorMsg = QString("GStreamer error: %1").arg(err->message);
        thread->setLastError(errorMsg);
        emit thread->errorOccurred(errorMsg);

        g_error_free(err);
        g_free(debug);
        break;
    }
    case GST_MESSAGE_WARNING: {
        GError *err;
        gchar *debug;
        gst_message_parse_warning(msg, &err, &debug);

        qDebug() << "GStreamer warning:" << err->message;

        g_error_free(err);
        g_free(debug);
        break;
    }
    case GST_MESSAGE_EOS:
        qDebug() << "GStreamer: End of stream";
        break;
    case GST_MESSAGE_STATE_CHANGED: {
        GstState old_state, new_state, pending_state;
        gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);

        if (GST_MESSAGE_SRC(msg) == GST_OBJECT(thread->getPipeline())) {
            qDebug() << "Pipeline state changed:"
                     << gst_element_state_get_name(old_state) << "->"
                     << gst_element_state_get_name(new_state);

            if (old_state == GST_STATE_READY && new_state == GST_STATE_PAUSED) {
                emit thread->streamingStarted();
            }
        }
        break;
    }
    default:
        break;
    }

    return TRUE;
}

// ================== GStreamerThread Implementation ==================

GStreamerThread::GStreamerThread(QObject *parent)
    : QThread(parent)
    , m_running(false)
    , m_frameNew(false)
    , m_pipeline(nullptr)
    , m_appsink(nullptr)
{
}

GStreamerThread::~GStreamerThread()
{
    stop();
    wait();
}

void GStreamerThread::stop()
{
    m_running = false;

    if (m_pipeline) {
        gst_element_send_event(GST_ELEMENT(m_pipeline), gst_event_new_eos());
    }
}

QImage GStreamerThread::getCurrentFrame()
{
    QMutexLocker locker(&m_mutex);
    m_frameNew = false;
    return m_currentFrame;
}

bool GStreamerThread::isFrameNew()
{
    QMutexLocker locker(&m_mutex);
    return m_frameNew;
}

QString GStreamerThread::getLastError()
{
    return m_lastError;
}

void GStreamerThread::run()
{
    // Инициализируем GStreamer
    gst_init(nullptr, nullptr);

    m_running = true;
    m_lastError.clear();

    // Создаем pipeline для приема RTP/JPEG потока
    QString pipelineStr = QString(
                              "udpsrc port=%1 ! "
                              "application/x-rtp,encoding-name=JPEG,payload=26 ! "
                              "rtpjpegdepay ! "
                              "jpegdec ! "
                              "videoconvert ! "
                              "videoscale ! "
                              "video/x-raw,format=RGB,width=640,height=480 ! "
                              "appsink name=qt_sink emit-signals=true max-buffers=1 drop=true"
                              ).arg(Config::VIDEO_PORT);

    GError *error = nullptr;
    m_pipeline = gst_parse_launch(pipelineStr.toUtf8().constData(), &error);

    if (!m_pipeline) {
        m_lastError = QString("Failed to create pipeline: %1").arg(error->message);
        g_error_free(error);
        emit errorOccurred(m_lastError);
        return;
    }

    // Получаем appsink элемент
    m_appsink = gst_bin_get_by_name(GST_BIN(m_pipeline), "qt_sink");
    if (!m_appsink) {
        m_lastError = "Failed to get appsink element";
        emit errorOccurred(m_lastError);
        gst_object_unref(m_pipeline);
        return;
    }

    // Настраиваем appsink
    g_object_set(m_appsink, "emit-signals", TRUE, "sync", FALSE, nullptr);

    // Подключаем callback для новых кадров
    g_signal_connect(m_appsink, "new-sample", G_CALLBACK(new_sample_cb), this);

    // Получаем bus для сообщений
    GstBus *bus = gst_element_get_bus(GST_ELEMENT(m_pipeline));
    guint bus_watch_id = gst_bus_add_watch(bus, bus_callback, this);
    gst_object_unref(bus);

    // Запускаем pipeline
    gst_element_set_state(GST_ELEMENT(m_pipeline), GST_STATE_PLAYING);

    qDebug() << "GStreamer pipeline started, waiting for video stream on port" << Config::VIDEO_PORT;

    // Главный цикл потока
    while (m_running && !m_lastError.isEmpty() == false) {
        QThread::msleep(10);
    }

    // Останавливаем pipeline
    gst_element_set_state(GST_ELEMENT(m_pipeline), GST_STATE_NULL);

    // Очищаем ресурсы
    g_source_remove(bus_watch_id);
    gst_object_unref(m_appsink);
    gst_object_unref(m_pipeline);

    qDebug() << "GStreamer thread finished";
}

// ================== VideoStreamer Implementation ==================

VideoStreamer::VideoStreamer(QObject *parent)
    : QObject(parent)
    , m_thread(new GStreamerThread(this))
    , m_displayLabel(nullptr)
    , m_displayTimer(new QTimer(this))
    , m_running(false)
{
    // Подключаем сигналы от потока
    connect(m_thread, &GStreamerThread::frameUpdated,
            this, &VideoStreamer::onFrameUpdated);
    connect(m_thread, &GStreamerThread::errorOccurred,
            this, &VideoStreamer::onThreadError);
    connect(m_thread, &GStreamerThread::streamingStarted,
            this, &VideoStreamer::onStreamingStarted);

    // Таймер для обновления дисплея
    connect(m_displayTimer, &QTimer::timeout,
            this, &VideoStreamer::updateDisplay);
    m_displayTimer->setInterval(33); // ~30 fps
}

VideoStreamer::~VideoStreamer()
{
    stop();
}

QString VideoStreamer::generateFilename() const
{
    QString basePath = QStandardPaths::writableLocation(
        QStandardPaths::DocumentsLocation);

    QDir dir(basePath + "/RobotRecordings");
    if (!dir.exists())
        dir.mkpath(".");

    QString fileName =
        "recorded_video_" +
        QDateTime::currentDateTime()
            .toString("yyyy-MM-dd_HH-mm-ss") +
        ".mp4";

    return dir.filePath(fileName);
}

void VideoStreamer::start(QLabel *videoLabel)
{
    if (m_running) {
        qDebug() << "Video streamer already running";
        return;
    }

    if (!videoLabel) {
        emit errorOccurred("No display label provided");
        return;
    }

    m_displayLabel = videoLabel;
    m_currentFile = generateFilename();

    // Показываем заглушку с сообщением о запуске
    QImage placeholder(640, 480, QImage::Format_RGB888);
    placeholder.fill(Qt::black);

    QPainter painter(&placeholder);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 14));
    painter.drawText(placeholder.rect(), Qt::AlignCenter,
                     "Запуск видео потока...\n"
                     "Порт: " + QString::number(Config::VIDEO_PORT) + "\n"
                                                                 "Ожидание данных от робота...");
    painter.end();

    QPixmap pixmap = QPixmap::fromImage(placeholder);
    m_displayLabel->setPixmap(pixmap.scaled(m_displayLabel->size(),
                                            Qt::KeepAspectRatio,
                                            Qt::SmoothTransformation));

    // Запускаем поток GStreamer
    m_thread->start();

    m_running = true;
    m_displayTimer->start();

    emit recordingStarted(m_currentFile);
}

void VideoStreamer::stop()
{
    if (!m_running) {
        return;
    }

    qDebug() << "Stopping video stream...";

    m_displayTimer->stop();
    m_thread->stop();
    m_thread->wait(3000); // Ждем завершения потока

    m_running = false;

    // Очищаем дисплей
    if (m_displayLabel) {
        QImage placeholder(640, 480, QImage::Format_RGB888);
        placeholder.fill(Qt::black);

        QPainter painter(&placeholder);
        painter.setPen(Qt::white);
        painter.drawText(placeholder.rect(), Qt::AlignCenter,
                         "Видео поток остановлен");
        painter.end();

        QPixmap pixmap = QPixmap::fromImage(placeholder);
        m_displayLabel->setPixmap(pixmap.scaled(m_displayLabel->size(),
                                                Qt::KeepAspectRatio,
                                                Qt::SmoothTransformation));
    }

    emit stopped();
}

bool VideoStreamer::isRunning() const
{
    return m_running;
}

void VideoStreamer::onFrameUpdated()
{
    // Новый кадр получен, обновление произойдет по таймеру
}

void VideoStreamer::onThreadError(QString error)
{
    qDebug() << "Thread error:" << error;
    emit errorOccurred(error);

    // Показываем ошибку на дисплее
    if (m_displayLabel) {
        QImage placeholder(640, 480, QImage::Format_RGB888);
        placeholder.fill(Qt::black);

        QPainter painter(&placeholder);
        painter.setPen(Qt::red);
        painter.setFont(QFont("Arial", 12));
        painter.drawText(placeholder.rect(), Qt::AlignCenter,
                         "Ошибка видео:\n" + error);
        painter.end();

        QPixmap pixmap = QPixmap::fromImage(placeholder);
        m_displayLabel->setPixmap(pixmap.scaled(m_displayLabel->size(),
                                                Qt::KeepAspectRatio,
                                                Qt::SmoothTransformation));
    }
}

void VideoStreamer::onStreamingStarted()
{
    qDebug() << "Video streaming started";
    emit started();
}

void VideoStreamer::updateDisplay()
{
    if (!m_running || !m_displayLabel || !m_thread) {
        return;
    }

    // Получаем новый кадр если есть
    if (m_thread->isFrameNew()) {
        QImage frame = m_thread->getCurrentFrame();

        if (!frame.isNull()) {
            // Добавляем информацию о времени на кадр
            QPainter painter(&frame);
            painter.setPen(Qt::green);
            painter.setFont(QFont("Arial", 10));

            QString timestamp = QDateTime::currentDateTime()
                                    .toString("hh:mm:ss.zzz");
            painter.drawText(10, 20, timestamp);

            painter.drawText(frame.width() - 150, 20,
                             QString("640x480"));

            painter.end();

            // Отображаем кадр
            QPixmap pixmap = QPixmap::fromImage(frame);
            m_displayLabel->setPixmap(pixmap.scaled(
                m_displayLabel->size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                ));
        }
    }
}
