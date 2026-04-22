#ifndef CLIENT_H
#define CLIENT_H

#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <QObject>

#include "main_window.h"
#include "yolo_detector.h"
#include "ocr_detector.h"

class GStreamerClient : public QObject
{
    Q_OBJECT
public:
    GStreamerClient(MainWindow *window,
                   const std::string &yoloModel = "yolov8n.onnx");
    ~GStreamerClient();
public slots:
    int run_listen();

private:
    int listen();
    MainWindow   *m_window;
    YoloDetector  m_detector;
    OcrDetector   m_ocr;
};

#endif // CLIENT_H