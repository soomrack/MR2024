#ifndef DUAL_VIDEO_WIDGET_H
#define DUAL_VIDEO_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "yolo_detector.h"

class DualVideoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DualVideoWidget(QWidget *parent = nullptr);

    QLabel* getLeftVideoLabel() { return m_leftVideoLabel; }
    QLabel* getRightVideoLabel() { return m_rightVideoLabel; }
    YOLODetector* getDetector() { return &m_detector; }

    void setLeftFrame(const QImage &frame);
    void setRightFrame(const QImage &frame);

public slots:
    void onDetectionCompleted(QVector<Detection> detections, qint64 elapsedMs);

private:
    QLabel *m_leftVideoLabel;   // Прямой эфир (слева)
    QLabel *m_rightVideoLabel;   // Обработанное видео (справа)
    QLabel *m_statsLabel;        // Информация о детекции
    QLabel *m_infoLeftLabel;     // Заголовок для левого видео
    QLabel *m_infoRightLabel;    // Заголовок для правого видео

    YOLODetector m_detector;
    QImage m_lastRawFrame;
};

#endif // DUAL_VIDEO_WIDGET_H
