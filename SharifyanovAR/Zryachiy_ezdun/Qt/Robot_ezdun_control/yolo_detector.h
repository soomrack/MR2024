#ifndef YOLO_DETECTOR_H
#define YOLO_DETECTOR_H

#include <QObject>
#include <QImage>
#include <QMutex>
#include <QString>
#include <QVector>
#include <QRectF>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

// Структура для хранения результата детекции
struct Detection {
    int classId;
    float confidence;
    QRectF rect;      // Нормализованные координаты (0-1)
    QString className;
};

class YOLODetector : public QObject
{
    Q_OBJECT

public:
    explicit YOLODetector(QObject *parent = nullptr);
    ~YOLODetector();

    bool loadModel(const QString &modelPath, const QString &classesPath);
    bool isLoaded() const { return m_loaded; }

    // Детекция одного кадра
    QVector<Detection> detect(const QImage &image, float confThreshold = 0.5, float nmsThreshold = 0.4);

    // Вспомогательная функция для отрисовки результатов
    QImage drawDetections(const QImage &image, const QVector<Detection> &detections);

signals:
    void modelLoaded(bool success);
    void detectionCompleted(QVector<Detection> detections, qint64 elapsedMs);

private:
    cv::dnn::Net m_net;
    QStringList m_classes;
    bool m_loaded;
    QMutex m_mutex;

    // Параметры модели
    const int m_inputWidth = 640;
    const int m_inputHeight = 640;

    // Внутренние функции для обработки выхода YOLO
    QVector<Detection> postprocess(const cv::Mat &frame, const std::vector<cv::Mat> &outputs,
                                   float confThreshold, float nmsThreshold);
};

#endif // YOLO_DETECTOR_H
