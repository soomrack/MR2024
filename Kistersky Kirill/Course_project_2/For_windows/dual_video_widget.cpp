#include "dual_video_widget.h"
#include <QDateTime>
#include <QPainter>
#include <QDebug>
#include <QCoreApplication>

DualVideoWidget::DualVideoWidget(QWidget *parent)
    : QWidget(parent)
    , m_leftVideoLabel(new QLabel(this))
    , m_rightVideoLabel(new QLabel(this))
    , m_statsLabel(new QLabel(this))
    , m_infoLeftLabel(new QLabel("Прямой эфир", this))
    , m_infoRightLabel(new QLabel("С детекцией YOLO", this))
{
    // Настраиваем левый видео лейбл (прямой эфир)
    m_leftVideoLabel->setMinimumSize(320, 240);
    m_leftVideoLabel->setAlignment(Qt::AlignCenter);
    m_leftVideoLabel->setStyleSheet("border: 2px solid #444; background-color: #1a1a1a;");
    m_leftVideoLabel->setText("Ожидание видео...");

    // Настраиваем правый видео лейбл (с детекцией)
    m_rightVideoLabel->setMinimumSize(320, 240);
    m_rightVideoLabel->setAlignment(Qt::AlignCenter);
    m_rightVideoLabel->setStyleSheet("border: 2px solid #444; background-color: #1a1a1a;");
    m_rightVideoLabel->setText("YOLO детекция\n(ожидание)");

    // Настраиваем информационные лейблы
    m_infoLeftLabel->setAlignment(Qt::AlignCenter);
    m_infoLeftLabel->setStyleSheet("color: #aaa; font: bold 10pt;");

    m_infoRightLabel->setAlignment(Qt::AlignCenter);
    m_infoRightLabel->setStyleSheet("color: #aaa; font: bold 10pt;");

    // Настраиваем статистику
    m_statsLabel->setAlignment(Qt::AlignCenter);
    m_statsLabel->setStyleSheet("color: #4CAF50; font: bold 10pt; padding: 5px;");
    m_statsLabel->setText("YOLO: загрузка модели...");

    // Создаем layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Горизонтальный layout для видео
    QHBoxLayout *videoLayout = new QHBoxLayout();

    // Левая колонка
    QVBoxLayout *leftColumn = new QVBoxLayout();
    leftColumn->addWidget(m_infoLeftLabel);
    leftColumn->addWidget(m_leftVideoLabel);

    // Правая колонка
    QVBoxLayout *rightColumn = new QVBoxLayout();
    rightColumn->addWidget(m_infoRightLabel);
    rightColumn->addWidget(m_rightVideoLabel);

    videoLayout->addLayout(leftColumn);
    videoLayout->addLayout(rightColumn);

    // Добавляем всё в главный layout
    mainLayout->addLayout(videoLayout);
    mainLayout->addWidget(m_statsLabel);

    setLayout(mainLayout);

    // Подключаем сигнал детекции
    connect(&m_detector, &YOLODetector::detectionCompleted,
            this, &DualVideoWidget::onDetectionCompleted);

    // Загружаем модель (пути нужно будет проверить)
    QString appDir = QCoreApplication::applicationDirPath();
    bool modelLoaded = m_detector.loadModel(appDir + "/models/yolo11n.onnx", appDir + "/models/coco.names");
    if (modelLoaded) {
        m_statsLabel->setText("YOLO: модель загружена, ожидание видео...");
    } else {
        m_statsLabel->setText("YOLO: ОШИБКА загрузки модели!");
    }
}

void DualVideoWidget::setLeftFrame(const QImage &frame)
{
    if (!frame.isNull()) {
        m_lastRawFrame = frame;

        QPixmap pixmap = QPixmap::fromImage(frame);
        m_leftVideoLabel->setPixmap(pixmap.scaled(
            m_leftVideoLabel->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            ));
    }
}

void DualVideoWidget::setRightFrame(const QImage &frame)
{
    if (!frame.isNull()) {
        QPixmap pixmap = QPixmap::fromImage(frame);
        m_rightVideoLabel->setPixmap(pixmap.scaled(
            m_rightVideoLabel->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            ));
    }
}

void DualVideoWidget::onDetectionCompleted(QVector<Detection> detections, qint64 elapsedMs)
{
    QString stats = QString("YOLO: найдено %1 объектов | Время обработки: %2 мс | Кадров в секунду: %3")
                        .arg(detections.size())
                        .arg(elapsedMs)
                        .arg(elapsedMs > 0 ? QString::number(1000.0 / elapsedMs, 'f', 1) : "0");

    m_statsLabel->setText(stats);
}
