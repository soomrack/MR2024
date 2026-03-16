#include "yolo_detector.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QPainter>
#include <QElapsedTimer>
#include <opencv2/imgproc.hpp>

YOLODetector::YOLODetector(QObject *parent)
    : QObject(parent)
    , m_loaded(false)
{
}

YOLODetector::~YOLODetector()
{
}

bool YOLODetector::loadModel(const QString &modelPath, const QString &classesPath)
{
    QMutexLocker locker(&m_mutex);

    // Загружаем классы
    QFile classesFile(classesPath);
    if (!classesFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open classes file:" << classesPath;
        emit modelLoaded(false);
        return false;
    }

    m_classes.clear();
    QTextStream textStream(&classesFile);
    while (!textStream.atEnd()) {
        QString line = textStream.readLine();
        if (!line.isEmpty()) {
            m_classes.append(line);
        }
    }
    classesFile.close();

    qDebug() << "Loaded" << m_classes.size() << "classes";

    // Загружаем модель ONNX
    try {
        m_net = cv::dnn::readNetFromONNX(modelPath.toStdString());

        // Настройка бэкенда (можно включить CUDA если нужно)
        // m_net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
        // m_net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);

        if (m_net.empty()) {
            qDebug() << "Failed to load model:" << modelPath;
            emit modelLoaded(false);
            return false;
        }

        qDebug() << "Model loaded successfully:" << modelPath;
        m_loaded = true;
        emit modelLoaded(true);
        return true;

    } catch (const cv::Exception &e) {
        qDebug() << "OpenCV exception:" << e.what();
        emit modelLoaded(false);
        return false;
    }
}

QVector<Detection> YOLODetector::detect(const QImage &image, float confThreshold, float nmsThreshold)
{
    QVector<Detection> results;

    if (!m_loaded || image.isNull()) {
        return results;
    }

    QMutexLocker locker(&m_mutex);
    QElapsedTimer timer;
    timer.start();

    try {
        // Конвертируем QImage в cv::Mat
        cv::Mat frame;
        if (image.format() == QImage::Format_RGB888) {
            frame = cv::Mat(image.height(), image.width(), CV_8UC3,
                            const_cast<uchar*>(image.bits()), image.bytesPerLine());
        } else {
            // Конвертируем в RGB если нужно
            QImage rgbImage = image.convertToFormat(QImage::Format_RGB888);
            frame = cv::Mat(rgbImage.height(), rgbImage.width(), CV_8UC3,
                            const_cast<uchar*>(rgbImage.bits()), rgbImage.bytesPerLine());
        }

        // Создаем blob для входа в сеть
        cv::Mat blob = cv::dnn::blobFromImage(frame, 1.0/255.0,
                                              cv::Size(m_inputWidth, m_inputHeight),
                                              cv::Scalar(), true, false);

        // Запускаем инференс
        m_net.setInput(blob);
        std::vector<cv::Mat> outputs;
        m_net.forward(outputs, m_net.getUnconnectedOutLayersNames());

        // Постпроцессинг
        results = postprocess(frame, outputs, confThreshold, nmsThreshold);

    } catch (const cv::Exception &e) {
        qDebug() << "Detection error:" << e.what();
    }

    emit detectionCompleted(results, timer.elapsed());
    return results;
}

QVector<Detection> YOLODetector::postprocess(const cv::Mat &frame,
                                             const std::vector<cv::Mat> &outputs,
                                             float confThreshold, float nmsThreshold)
{
    QVector<Detection> results;

    // Выход YOLOv8/v11: тензор [1, 84, 8400]
    // 84 = 4 координаты (cx,cy,w,h) + 80 классов
    // 8400 = количество предсказаний
    const cv::Mat &output = outputs[0];

    // Получаем размеры: [1, 84, 8400]
    int numClasses = output.size[1] - 4;   // 80
    int numPreds   = output.size[2];        // 8400

    // Указатель на данные
    const float *data = (const float*)output.data;

    std::vector<int>       classIds;
    std::vector<float>     confidences;
    std::vector<cv::Rect>  boxes;

    for (int i = 0; i < numPreds; i++) {
        // cx, cy, w, h — нормализованы относительно inputWidth/inputHeight
        float cx = data[0 * numPreds + i];
        float cy = data[1 * numPreds + i];
        float w  = data[2 * numPreds + i];
        float h  = data[3 * numPreds + i];

        // Находим класс с максимальным score
        float maxScore = 0.0f;
        int   maxClass = 0;
        for (int c = 0; c < numClasses; c++) {
            float score = data[(4 + c) * numPreds + i];
            if (score > maxScore) {
                maxScore = score;
                maxClass = c;
            }
        }

        if (maxScore < confThreshold)
            continue;

        // Переводим координаты в пиксели исходного кадра
        float scaleX = (float)frame.cols / m_inputWidth;
        float scaleY = (float)frame.rows / m_inputHeight;

        int left   = (int)((cx - w / 2.0f) * scaleX);
        int top    = (int)((cy - h / 2.0f) * scaleY);
        int width  = (int)(w * scaleX);
        int height = (int)(h * scaleY);

        classIds.push_back(maxClass);
        confidences.push_back(maxScore);
        boxes.push_back(cv::Rect(left, top, width, height));
    }

    // NMS
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);

    for (int idx : indices) {
        Detection det;
        det.classId   = classIds[idx];
        det.confidence = confidences[idx];

        const cv::Rect &box = boxes[idx];
        det.rect = QRectF(
            (float)box.x / frame.cols,
            (float)box.y / frame.rows,
            (float)box.width  / frame.cols,
            (float)box.height / frame.rows
            );

        if (det.classId >= 0 && det.classId < m_classes.size())
            det.className = m_classes[det.classId];
        else
            det.className = QString("class_%1").arg(det.classId);

        results.append(det);
    }

    return results;
}

QImage YOLODetector::drawDetections(const QImage &image, const QVector<Detection> &detections)
{
    if (image.isNull() || detections.isEmpty()) {
        return image;
    }

    QImage result = image.convertToFormat(QImage::Format_RGB888);

    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing);

    QFont font = painter.font();
    font.setPixelSize(14);
    painter.setFont(font);

    for (const Detection &det : detections) {
        // Преобразуем нормализованные координаты в пиксели
        QRect rect(
            det.rect.x() * result.width(),
            det.rect.y() * result.height(),
            det.rect.width() * result.width(),
            det.rect.height() * result.height()
            );

        // Разные цвета для разных классов
        QColor color(255 * (det.classId % 3 == 0),
                     255 * (det.classId % 3 == 1),
                     255 * (det.classId % 3 == 2));

        // Рисуем прямоугольник
        QPen pen(color, 2);
        painter.setPen(pen);
        painter.drawRect(rect);

        // Рисуем подпись
        QString label = QString("%1 (%2%)")
                            .arg(det.className)
                            .arg((int)(det.confidence * 100));

        QRect textRect = painter.boundingRect(rect, Qt::AlignLeft, label);
        textRect.moveTopLeft(rect.topLeft());

        painter.fillRect(textRect, color);
        painter.setPen(Qt::black);
        painter.drawText(textRect, Qt::AlignCenter, label);
    }

    painter.end();
    return result;
}
