#pragma once
#ifndef VIDEO_STREAMER_H
#define VIDEO_STREAMER_H

#include <QObject>
#include <QProcess>
#include <QString>
#include <QLabel>
#include <QTimer>
#include <QImage>
#include <QPainter>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QElapsedTimer>

#include "config.h"
#include "dual_video_widget.h"  // Добавлено

// Предварительное объявление структур GStreamer
struct _GstElement;
struct _GstBus;
struct _GstMessage;
typedef struct _GstElement GstElement;
typedef struct _GstBus GstBus;
typedef struct _GstMessage GstMessage;

// Объявление callback функций
extern "C" {
#include <gst/gst.h>
#include <gst/app/gstappsink.h>

GstFlowReturn new_sample_cb(GstElement *sink, gpointer user_data);
gboolean bus_callback(GstBus *bus, GstMessage *msg, gpointer user_data);
}

// Класс для работы с GStreamer в отдельном потоке
class GStreamerThread : public QThread
{
    Q_OBJECT

public:
    explicit GStreamerThread(QObject *parent = nullptr);
    ~GStreamerThread();

    void run() override;
    void stop();
    QImage getCurrentFrame();
    bool isFrameNew();
    QString getLastError();

    // Публичные методы для доступа к данным из callback-функций
    void lockMutex() { m_mutex.lock(); }
    void unlockMutex() { m_mutex.unlock(); }
    void setCurrentFrame(const QImage &frame) { m_currentFrame = frame; }
    void setFrameNew(bool isNew) { m_frameNew = isNew; }
    void setLastError(const QString &error) { m_lastError = error; }
    void* getPipeline() { return m_pipeline; }

signals:
    void frameUpdated();
    void errorOccurred(QString error);
    void streamingStarted();

private:
    volatile bool m_running;
    QMutex m_mutex;
    QImage m_currentFrame;
    bool m_frameNew;
    QString m_lastError;

    // Указатели GStreamer
    void *m_pipeline;
    void *m_appsink;

    // Дружественные callback функции
    friend GstFlowReturn new_sample_cb(GstElement *sink, gpointer user_data);
    friend gboolean bus_callback(GstBus *bus, GstMessage *msg, gpointer user_data);
};

class VideoStreamer : public QObject
{
    Q_OBJECT

public:
    explicit VideoStreamer(QObject *parent = nullptr);
    ~VideoStreamer();

    void start(QLabel *videoLabel);
    void stop();
    bool isRunning() const;

    // Новые методы для работы с DualVideoWidget
    void setDualWidget(DualVideoWidget *widget) { m_dualWidget = widget; }
    DualVideoWidget* getDualWidget() const { return m_dualWidget; }

signals:
    void started();
    void stopped();
    void errorOccurred(QString error);
    void recordingStarted(QString filePath);
    void frameProcessed(QImage frame, QVector<Detection> detections); // Сигнал для обработанного кадра

private slots:
    void onFrameUpdated();
    void onThreadError(QString error);
    void onStreamingStarted();
    void updateDisplay();
    void processFrameForDetection(); // Новый слот для обработки кадра

private:
    QString generateFilename() const;

    GStreamerThread *m_thread;
    QLabel *m_displayLabel;
    QTimer *m_displayTimer;
    bool m_running;
    QString m_currentFile;

    // Новые члены класса
    DualVideoWidget *m_dualWidget;
    int m_frameCounter;
    static constexpr int DETECTION_FRAME_SKIP = 5; // Каждый 5-й кадр
    QElapsedTimer m_detectionTimer;
};

#endif // VIDEO_STREAMER_H
