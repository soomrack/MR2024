#include "mainwindow.h"
#include "data_logger.h"
#include "data_receiver.h"
#include "batch_command_sender.h"
#include "dual_video_widget.h"
#include "yolo_detector.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QDateTime>
#include <QResizeEvent>
#include <QFocusEvent>
#include <QTime>
#include <QDebug>
#include <QFrame>
#include <QSpacerItem>
#include <QFontDatabase>
#include <QRegularExpression>
#include <QLineEdit>
#include <QElapsedTimer>

QLabel *yoloObjectsLabel;
QLabel *yoloTimeLabel;
QLabel *yoloFpsLabel;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    sender(new CommandSender(this)),
    streamer(new VideoStreamer(this)),
    logger(new DataLogger(this)),
    receiver(new DataReceiver(this)),
    batchSender(new BatchCommandSender(this)),
    videoDisplay(nullptr)
{
    setupUI();
    setupConnections();

    connect(logger, &DataLogger::newLogMessage,
            logBox, &QTextEdit::append);

    if (receiver->start(5601)) {
        logger->logSystem("UDP 5601 запущен (датчик расстояния)");
    } else {
        logger->logSystem("Ошибка запуска UDP 5601");
    }

    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    logger->logSystem("MainWindow initialized");
}

MainWindow::~MainWindow()
{
    streamer->stop();
    sender->disconnectFromRobot();
    if (batchSender) {
        batchSender->disconnectFromRobot();
    }
}

void MainWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    setStyleSheet(
        "QMainWindow, QWidget {"
        "   background-color: #1a1a2e;"
        "   font-family: Arial;"
        "}"
        "QPushButton {"
        "   background-color: #0f3460;"
        "   color: #e0e0e0;"
        "   border: 1px solid #16213e;"
        "   padding: 6px 16px;"
        "   border-radius: 6px;"
        "   font-weight: bold;"
        "   font-size: 12px;"
        "   min-height: 30px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #1a4a7a;"
        "   border: 1px solid #4fc3f7;"
        "}"
        "QPushButton:disabled {"
        "   background-color: #1a1a2e;"
        "   color: #444;"
        "   border: 1px solid #222;"
        "}"
        "QTextEdit {"
        "   border: 1px solid #0f3460;"
        "   border-radius: 4px;"
        "   background-color: #0d0d1a;"
        "   color: #a0c4ff;"
        "   font-family: 'Courier New';"
        "   font-size: 11px;"
        "   padding: 4px;"
        "}"
        "QLabel {"
        "   font-family: Arial;"
        "   color: #c0c0d0;"
        "}"
        "QLineEdit {"
        "   border: 1px solid #0f3460;"
        "   border-radius: 4px;"
        "   padding: 6px;"
        "   background-color: #0d0d1a;"
        "   color: #e0e0e0;"
        "   font-size: 12px;"
        "}"
        "QFrame {"
        "   background-color: #16213e;"
        "   border-radius: 8px;"
        "}"
        );

    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setSpacing(6);
    mainLayout->setContentsMargins(10, 8, 10, 8);

    // ВЕРХНИЙ СТАТУС-БАР
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->setSpacing(8);

    connectionStatus = new QLabel("● Отключен");
    connectionStatus->setStyleSheet(
        "color: #f44336; font-weight: bold; font-size: 13px;"
        "padding: 3px 8px; background-color: transparent;"
        );
    connectionStatus->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    videoStatus = new QLabel("● Видео не активно");
    videoStatus->setStyleSheet(
        "color: #607d8b; font-weight: bold; font-size: 13px;"
        "padding: 3px 8px; background-color: transparent;"
        );
    videoStatus->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    topLayout->addWidget(connectionStatus);
    topLayout->addStretch();
    topLayout->addWidget(videoStatus);
    mainLayout->addLayout(topLayout);

    // БЛОК УПРАВЛЕНИЯ
    QFrame *controlCard = new QFrame();
    controlCard->setStyleSheet(
        "QFrame { background-color: #16213e; border-radius: 8px;"
        "border: 1px solid #0f3460; padding: 6px; }"
        );

    QVBoxLayout *ctrlLayout = new QVBoxLayout(controlCard);
    ctrlLayout->setSpacing(4);
    ctrlLayout->setContentsMargins(10, 6, 10, 6);

    QLabel *ctrlTitle = new QLabel("УПРАВЛЕНИЕ РОБОТОМ");
    ctrlTitle->setStyleSheet(
        "font-size: 13px; font-weight: bold; color: #4fc3f7;"
        "background-color: transparent; letter-spacing: 1px;"
        );
    ctrlTitle->setAlignment(Qt::AlignCenter);
    ctrlLayout->addWidget(ctrlTitle);

    QLabel *instructions = new QLabel(
        "W — Вперед    |    S — Назад    |    A — Влево    |    D — Вправо    |    ПРОБЕЛ — Стоп    |    L — Return to home"
        );
    instructions->setStyleSheet(
        "background-color: #0f1a2e; border: 1px solid #0f3460; border-radius: 5px;"
        "font-size: 11px; color: #90a4ae; padding: 6px;"
        );
    instructions->setAlignment(Qt::AlignCenter);
    instructions->setWordWrap(true);
    ctrlLayout->addWidget(instructions);

    mainLayout->addWidget(controlCard);

    // БЛОК ПАКЕТНЫХ КОМАНД
    QFrame *batchCard = new QFrame();
    batchCard->setStyleSheet(
        "QFrame { background-color: #16213e; border-radius: 8px;"
        "border: 1px solid #0f3460; padding: 6px; }"
        );

    QHBoxLayout *batchLayout = new QHBoxLayout(batchCard);
    batchLayout->setSpacing(8);
    batchLayout->setContentsMargins(10, 6, 10, 6);

    QLabel *batchLabel = new QLabel("📦 Пакет:");
    batchLabel->setStyleSheet(
        "font-weight: bold; font-size: 12px; color: #ce93d8; background-color: transparent;"
        );

    batchCommandEdit = new QLineEdit();
    batchCommandEdit->setPlaceholderText("Например: Forward:2s; Back:1s; Left:1s");
    batchCommandEdit->setMinimumHeight(30);

    sendBatchButton = new QPushButton("Отправить пакет");
    sendBatchButton->setStyleSheet(
        "QPushButton { background-color: #6a1b9a; color: white;"
        "border: 1px solid #9c27b0; border-radius: 6px; padding: 6px 16px;"
        "font-weight: bold; font-size: 12px; }"
        "QPushButton:hover { background-color: #7b1fa2; border: 1px solid #ce93d8; }"
        );

    batchLayout->addWidget(batchLabel);
    batchLayout->addWidget(batchCommandEdit, 1);
    batchLayout->addWidget(sendBatchButton);
    mainLayout->addWidget(batchCard);

    // ВИДЕОПОТОК — занимает всё свободное место
    QFrame *videoCard = new QFrame();
    videoCard->setStyleSheet(
        "QFrame { background-color: #16213e; border-radius: 8px;"
        "border: 1px solid #0f3460; padding: 6px; }"
        );

    QVBoxLayout *videoCardLayout = new QVBoxLayout(videoCard);
    videoCardLayout->setSpacing(4);
    videoCardLayout->setContentsMargins(8, 6, 8, 6);

    QHBoxLayout *videoTitleLayout = new QHBoxLayout();

    QLabel *videoTitle = new QLabel("ВИДЕОПОТОК");
    videoTitle->setStyleSheet(
        "font-size: 13px; font-weight: bold; color: #4fc3f7;"
        "background-color: transparent; letter-spacing: 1px;"
        );

    yoloObjectsLabel = new QLabel("📊 0");
    yoloObjectsLabel->setStyleSheet(
        "color: #66bb6a; font-size: 12px; font-weight: bold; background-color: transparent;"
        );
    yoloTimeLabel = new QLabel("⏱ 0мс");
    yoloTimeLabel->setStyleSheet(
        "color: #42a5f5; font-size: 12px; font-weight: bold; background-color: transparent;"
        );
    yoloFpsLabel = new QLabel("⚡ 0fps");
    yoloFpsLabel->setStyleSheet(
        "color: #ffa726; font-size: 12px; font-weight: bold; background-color: transparent;"
        );

    videoTitleLayout->addWidget(videoTitle);
    videoTitleLayout->addStretch();
    videoTitleLayout->addWidget(yoloObjectsLabel);
    videoTitleLayout->addSpacing(10);
    videoTitleLayout->addWidget(yoloTimeLabel);
    videoTitleLayout->addSpacing(10);
    videoTitleLayout->addWidget(yoloFpsLabel);
    videoCardLayout->addLayout(videoTitleLayout);

    videoDisplay = new DualVideoWidget();
    videoDisplay->setMinimumHeight(300);
    videoDisplay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    videoCardLayout->addWidget(videoDisplay, 1);

    mainLayout->addWidget(videoCard, 1);

    // НИЖНИЙ БЛОК: ДАТЧИК + КОМАНДЫ
    QFrame *bottomCard = new QFrame();
    bottomCard->setStyleSheet(
        "QFrame { background-color: #16213e; border-radius: 8px;"
        "border: 1px solid #0f3460; padding: 6px; }"
        );

    QHBoxLayout *bottomLayout = new QHBoxLayout(bottomCard);
    bottomLayout->setSpacing(8);
    bottomLayout->setContentsMargins(8, 6, 8, 6);

    // Датчик расстояния
    QFrame *sensorFrame = new QFrame();
    sensorFrame->setStyleSheet(
        "QFrame { background-color: #0f1a2e; border: 1px solid #0f3460;"
        "border-radius: 6px; padding: 6px; }"
        );

    QVBoxLayout *sensorLayout = new QVBoxLayout(sensorFrame);
    sensorLayout->setSpacing(3);
    sensorLayout->setContentsMargins(8, 6, 8, 6);

    QLabel *sensorTitle = new QLabel("ДАТЧИК РАССТОЯНИЯ");
    sensorTitle->setStyleSheet(
        "font-size: 11px; font-weight: bold; color: #4fc3f7;"
        "background-color: transparent; letter-spacing: 1px;"
        );
    sensorLayout->addWidget(sensorTitle);

    distanceLabel = new QLabel("---");
    distanceLabel->setStyleSheet(
        "background-color: #050d1a; color: #00e676;"
        "font-size: 32px; font-weight: bold; padding: 10px;"
        "border-radius: 6px; border: 1px solid #00695c;"
        );
    distanceLabel->setAlignment(Qt::AlignCenter);
    distanceLabel->setMinimumHeight(65);
    distanceLabel->setMaximumHeight(65);
    sensorLayout->addWidget(distanceLabel);

    QLabel *distanceUnit = new QLabel("сантиметров");
    distanceUnit->setStyleSheet(
        "color: #546e7a; font-size: 11px; background-color: transparent;"
        );
    distanceUnit->setAlignment(Qt::AlignCenter);
    sensorLayout->addWidget(distanceUnit);

    // Последние команды
    QFrame *commandsFrame = new QFrame();
    commandsFrame->setStyleSheet(
        "QFrame { background-color: #0f1a2e; border: 1px solid #0f3460;"
        "border-radius: 6px; padding: 6px; }"
        );

    QVBoxLayout *commandsLayout = new QVBoxLayout(commandsFrame);
    commandsLayout->setSpacing(3);
    commandsLayout->setContentsMargins(8, 6, 8, 6);

    QLabel *commandsTitle = new QLabel("ПОСЛЕДНИЕ КОМАНДЫ");
    commandsTitle->setStyleSheet(
        "font-size: 11px; font-weight: bold; color: #4fc3f7;"
        "background-color: transparent; letter-spacing: 1px;"
        );
    commandsLayout->addWidget(commandsTitle);

    commandsLog = new QTextEdit();
    commandsLog->setReadOnly(true);
    commandsLog->setMaximumHeight(90);
    commandsLog->setMinimumHeight(65);
    commandsLayout->addWidget(commandsLog);

    bottomLayout->addWidget(sensorFrame, 1);
    bottomLayout->addWidget(commandsFrame, 2);
    mainLayout->addWidget(bottomCard);

    // СИСТЕМНЫЙ ЛОГ
    QFrame *logCard = new QFrame();
    logCard->setStyleSheet(
        "QFrame { background-color: #16213e; border-radius: 8px;"
        "border: 1px solid #0f3460; padding: 6px; }"
        );

    QVBoxLayout *logLayout = new QVBoxLayout(logCard);
    logLayout->setSpacing(3);
    logLayout->setContentsMargins(8, 6, 8, 6);

    QLabel *logTitle = new QLabel("СИСТЕМНЫЙ ЛОГ");
    logTitle->setStyleSheet(
        "font-size: 11px; font-weight: bold; color: #4fc3f7;"
        "background-color: transparent; letter-spacing: 1px;"
        );
    logLayout->addWidget(logTitle);

    logBox = new QTextEdit();
    logBox->setReadOnly(true);
    logBox->setMaximumHeight(80);
    logBox->setMinimumHeight(55);
    logLayout->addWidget(logBox);

    mainLayout->addWidget(logCard);

    // КНОПКИ УПРАВЛЕНИЯ
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(8);

    connectButton    = new QPushButton("🔧 Подключиться");
    startVideoButton = new QPushButton("▶ Старт видео");
    stopVideoButton  = new QPushButton("⏹ Стоп видео");

    stopVideoButton->setEnabled(false);

    connectButton->setStyleSheet(
        "QPushButton { background-color: #0f3460; color: #e0e0e0;"
        "border: 1px solid #1565c0; border-radius: 6px; padding: 7px 18px;"
        "font-weight: bold; font-size: 12px; min-height: 32px; }"
        "QPushButton:hover { background-color: #1565c0; border: 1px solid #4fc3f7; }"
        "QPushButton:disabled { background-color: #1a1a2e; color: #444; border: 1px solid #222; }"
        );
    startVideoButton->setStyleSheet(
        "QPushButton { background-color: #1b5e20; color: #e0e0e0;"
        "border: 1px solid #2e7d32; border-radius: 6px; padding: 7px 18px;"
        "font-weight: bold; font-size: 12px; min-height: 32px; }"
        "QPushButton:hover { background-color: #2e7d32; border: 1px solid #66bb6a; }"
        "QPushButton:disabled { background-color: #1a1a2e; color: #444; border: 1px solid #222; }"
        );
    stopVideoButton->setStyleSheet(
        "QPushButton { background-color: #37474f; color: #e0e0e0;"
        "border: 1px solid #455a64; border-radius: 6px; padding: 7px 18px;"
        "font-weight: bold; font-size: 12px; min-height: 32px; }"
        "QPushButton:hover { background-color: #455a64; border: 1px solid #90a4ae; }"
        "QPushButton:disabled { background-color: #1a1a2e; color: #444; border: 1px solid #222; }"
        );

    buttonLayout->addWidget(connectButton);
    buttonLayout->addWidget(startVideoButton);
    buttonLayout->addWidget(stopVideoButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
    mainLayout->setAlignment(Qt::AlignTop);
}

void MainWindow::setupConnections()
{
    // Подключение к роботу
    connect(connectButton, &QPushButton::clicked, this, [=]() {
        if (sender->isConnected() || (batchSender && batchSender->isConnected())) {
            sender->disconnectFromRobot();
            if (batchSender) batchSender->disconnectFromRobot();
        } else {
            sender->connectToRobot();
            if (batchSender) batchSender->connectToRobot();
        }
    });

    connect(startVideoButton, &QPushButton::clicked, this, [=]() {
        if (videoDisplay) {
            streamer->start(videoDisplay->getLeftVideoLabel());
            streamer->setDualWidget(videoDisplay);
        }
    });

    connect(stopVideoButton, &QPushButton::clicked, this, [=]() {
        streamer->stop();
    });

    // CommandSender
    connect(sender, &CommandSender::connected, this, [=]() {
        connectionStatus->setText("● Подключен");
        connectionStatus->setStyleSheet(
            "color: #66bb6a; font-weight: bold; font-size: 13px;"
            "padding: 3px 8px; background-color: transparent;"
            );
        connectButton->setText("🔧 Отключиться");
        logger->logSystem("Подключен к роботу");
    });

    connect(sender, &CommandSender::disconnected, this, [=]() {
        connectionStatus->setText("● Отключен");
        connectionStatus->setStyleSheet(
            "color: #f44336; font-weight: bold; font-size: 13px;"
            "padding: 3px 8px; background-color: transparent;"
            );
        connectButton->setText("🔧 Подключиться");
        logger->logSystem("Отключен от робота");
    });

    connect(sender, &CommandSender::commandSent, this, [=](char cmd) {
        QString commandStr;
        QString color;

        switch(cmd) {
        case 'w': commandStr = "↑ Вперед";     color = "#66bb6a"; break;
        case 's': commandStr = "↓ Назад";       color = "#ef5350"; break;
        case 'a': commandStr = "← Влево";       color = "#ffa726"; break;
        case 'd': commandStr = "→ Вправо";      color = "#ffa726"; break;
        case 'l': commandStr = "⌂ Return home"; color = "#ce93d8"; break;
        case ' ': commandStr = "⏹ Стоп";        color = "#90a4ae"; break;
        default:  commandStr = QString("'%1'").arg(cmd); color = "#555";
        }

        QString timeStr = QTime::currentTime().toString("hh:mm:ss");
        commandsLog->append(
            QString("<span style='color: %1; font-weight: bold;'>[%2] %3</span>")
                .arg(color).arg(timeStr).arg(commandStr)
            );
        logger->logCommand(commandStr);
    });

    connect(sender, &CommandSender::errorOccurred, this, [=](const QString &error) {
        logger->logSystem("Ошибка: " + error);
    });

    // Пакетные команды
    connect(batchSender, &BatchCommandSender::connected, this, [=]() {
        logger->logSystem("Пакетный отправитель подключен");
    });

    connect(batchSender, &BatchCommandSender::disconnected, this, [=]() {
        logger->logSystem("Пакетный отправитель отключен");
    });

    connect(batchSender, &BatchCommandSender::errorOccurred, this, [=](QString error) {
        logger->logSystem("Ошибка пакетного отправителя: " + error);
    });

    connect(batchSender, &BatchCommandSender::batchCommandSent, this, [=](QString cmd) {
        QString timeStr = QTime::currentTime().toString("hh:mm:ss");
        commandsLog->append(
            QString("<span style='color: #ce93d8; font-weight: bold;'>[%1] 📦 Пакет: %2</span>")
                .arg(timeStr).arg(cmd)
            );
        logger->logCommand("Пакет: " + cmd);
    });

    connect(batchSender, &BatchCommandSender::batchCompleted, this, [=]() {
        QString timeStr = QTime::currentTime().toString("hh:mm:ss");
        commandsLog->append(
            QString("<span style='color: #66bb6a; font-weight: bold;'>[%1] ✅ Пакет выполнен</span>")
                .arg(timeStr)
            );
        logger->logSystem("Пакетная команда выполнена");
    });

    connect(sendBatchButton, &QPushButton::clicked, this, [=]() {
        QString command = batchCommandEdit->text().trimmed();
        if (!command.isEmpty()) {
            sendBatchCommand(command);
            batchCommandEdit->clear();
        }
    });

    connect(batchCommandEdit, &QLineEdit::returnPressed, this, [=]() {
        QString command = batchCommandEdit->text().trimmed();
        if (!command.isEmpty()) {
            sendBatchCommand(command);
            batchCommandEdit->clear();
        }
    });

    // YOLO статистика
    if (videoDisplay && videoDisplay->getDetector()) {
        YOLODetector *detector = videoDisplay->getDetector();

        connect(detector, &YOLODetector::detectionCompleted,
                this, [=](QVector<Detection> detections, qint64 elapsedMs) {
                    yoloObjectsLabel->setText(QString("📊 %1").arg(detections.size()));
                    yoloTimeLabel->setText(QString("⏱ %1мс").arg(elapsedMs));

                    static QElapsedTimer fpsTimer;
                    static int frameCount = 0;
                    static float fps = 0.0f;
                    if (!fpsTimer.isValid()) fpsTimer.start();
                    frameCount++;
                    qint64 elapsed = fpsTimer.elapsed();
                    if (elapsed >= 1000) {
                        fps = frameCount * 1000.0f / elapsed;
                        frameCount = 0;
                        fpsTimer.restart();
                    }
                    yoloFpsLabel->setText(QString("⚡ %1fps").arg(fps, 0, 'f', 1));
                });

        connect(detector, &YOLODetector::modelLoaded, this, [=](bool success) {
            if (success) {
                logger->logSystem("✅ YOLO модель загружена");
            } else {
                logger->logSystem("❌ Ошибка загрузки YOLO");
            }
        });
    }

    // VideoStreamer
    connect(streamer, &VideoStreamer::started, this, [=]() {
        startVideoButton->setEnabled(false);
        stopVideoButton->setEnabled(true);
        videoStatus->setText("● Видео активно");
        videoStatus->setStyleSheet(
            "color: #66bb6a; font-weight: bold; font-size: 13px;"
            "padding: 3px 8px; background-color: transparent;"
            );
        logger->logSystem("Видео поток запущен");
    });

    connect(streamer, &VideoStreamer::stopped, this, [=]() {
        startVideoButton->setEnabled(true);
        stopVideoButton->setEnabled(false);
        videoStatus->setText("● Видео не активно");
        videoStatus->setStyleSheet(
            "color: #607d8b; font-weight: bold; font-size: 13px;"
            "padding: 3px 8px; background-color: transparent;"
            );
        logger->logSystem("Видео поток остановлен");
    });

    connect(streamer, &VideoStreamer::errorOccurred, this, [=](const QString &error) {
        logger->logSystem("Ошибка видео: " + error);
        startVideoButton->setEnabled(true);
        stopVideoButton->setEnabled(false);
        videoStatus->setText("● Видео ошибка");
        videoStatus->setStyleSheet(
            "color: #f44336; font-weight: bold; font-size: 13px;"
            "padding: 3px 8px; background-color: transparent;"
            );
    });

    connect(streamer, &VideoStreamer::recordingStarted, this, [=](const QString &file) {
        logger->logSystem("Запись видео: " + file);
    });

    // Датчик расстояния
    connect(receiver, &DataReceiver::sensorDataReceived,
            this, [=](const QString& data)
            {
                distanceLabel->setText(data);
                logger->logSystem("ДАТЧИК: " + data);

                QString clean = data.simplified();
                QRegularExpression re(R"([\d.,]+)");
                QRegularExpressionMatch match = re.match(clean);
                if (!match.hasMatch()) return;

                QString numStr = match.captured(0).replace(",", ".");
                bool ok;
                float distance = numStr.toFloat(&ok);
                if (!ok) return;

                if (distance < 20.0) {
                    distanceLabel->setStyleSheet(
                        "background-color: #050d1a; color: #ff1744;"
                        "font-size: 32px; font-weight: bold; padding: 10px;"
                        "border-radius: 6px; border: 1px solid #b71c1c;"
                        );
                } else {
                    distanceLabel->setStyleSheet(
                        "background-color: #050d1a; color: #00e676;"
                        "font-size: 32px; font-weight: bold; padding: 10px;"
                        "border-radius: 6px; border: 1px solid #00695c;"
                        );
                }
            });
}

void MainWindow::sendBatchCommand(const QString& command)
{
    if (!batchSender) {
        logger->logSystem("Ошибка: batchSender не инициализирован");
        return;
    }

    if (!batchSender->isConnected()) {
        logger->logSystem("Ошибка: не подключен к роботу");
        return;
    }

    batchSender->sendBatchCommand(command);
    logger->logSystem("Отправлен пакет: " + command);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!sender->isConnected()) return;
    if (event->isAutoRepeat()) return;

    switch (event->key())
    {
    case Qt::Key_W:
    case Qt::Key_A:
    case Qt::Key_S:
    case Qt::Key_D:
        sender->onKeyPressed(event->key());
        break;
    case Qt::Key_Space:
        sender->stopRobot();
        break;
    case Qt::Key_L:
        sender->sendCommand('l');
        break;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (!sender->isConnected()) return;
    if (event->isAutoRepeat()) return;

    switch (event->key())
    {
    case Qt::Key_W:
    case Qt::Key_A:
    case Qt::Key_S:
    case Qt::Key_D:
        sender->onKeyReleased(event->key());
        break;
    }
}

void MainWindow::focusInEvent(QFocusEvent *event)
{
    QMainWindow::focusInEvent(event);
    setFocus();
}

void MainWindow::focusOutEvent(QFocusEvent *event)
{
    if (sender && sender->isConnected()) {
        sender->stopRobot();
        logger->logCommand("Авто-стоп (потеря фокуса)");
    }
    QMainWindow::focusOutEvent(event);
    logger->logSystem("Фокус потерян (focusOutEvent)");
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
}

void MainWindow::onBatchCompleted()
{
    logger->logSystem("Пакетная команда завершена");
}

void MainWindow::updateConnectionStatus(bool connected)
{
    if (connected) {
        connectionStatus->setText("● Подключен");
        connectionStatus->setStyleSheet(
            "color: #66bb6a; font-weight: bold; font-size: 13px;"
            "padding: 3px 8px; background-color: transparent;"
            );
    } else {
        connectionStatus->setText("● Отключен");
        connectionStatus->setStyleSheet(
            "color: #f44336; font-weight: bold; font-size: 13px;"
            "padding: 3px 8px; background-color: transparent;"
            );
    }
}

void MainWindow::updateVideoStatus(bool streaming)
{
    if (streaming) {
        videoStatus->setText("● Видео активно");
        videoStatus->setStyleSheet(
            "color: #66bb6a; font-weight: bold; font-size: 13px;"
            "padding: 3px 8px; background-color: transparent;"
            );
    } else {
        videoStatus->setText("● Видео не активно");
        videoStatus->setStyleSheet(
            "color: #607d8b; font-weight: bold; font-size: 13px;"
            "padding: 3px 8px; background-color: transparent;"
            );
    }
}

void MainWindow::logMessage(const QString& message)
{
    logger->logSystem(message);
}

void MainWindow::onDetectionCompleted(QVector<Detection> detections, qint64 elapsedMs)
{
    yoloObjectsLabel->setText(QString("📊 %1").arg(detections.size()));
    yoloTimeLabel->setText(QString("⏱ %1мс").arg(elapsedMs));

    static QElapsedTimer fpsTimer;
    static int frameCount = 0;
    static float fps = 0.0f;
    if (!fpsTimer.isValid()) fpsTimer.start();
    frameCount++;
    qint64 elapsed = fpsTimer.elapsed();
    if (elapsed >= 1000) {
        fps = frameCount * 1000.0f / elapsed;
        frameCount = 0;
        fpsTimer.restart();
    }
    yoloFpsLabel->setText(QString("⚡ %1fps").arg(fps, 0, 'f', 1));

    if (!detections.isEmpty()) {
        QStringList objectList;
        for (const Detection& det : detections) {
            objectList << QString("%1(%2%)").arg(det.className).arg((int)(det.confidence * 100));
        }
        if (objectList.size() > 3) {
            objectList = objectList.mid(0, 3);
            objectList << "...";
        }
        logger->logSystem(QString("YOLO: найдено %1 объектов [%2]")
                              .arg(detections.size())
                              .arg(objectList.join(", ")));
    }
}

void MainWindow::updateYoloStats(const QString& stats)
{
    logger->logSystem("YOLO stats: " + stats);
}
