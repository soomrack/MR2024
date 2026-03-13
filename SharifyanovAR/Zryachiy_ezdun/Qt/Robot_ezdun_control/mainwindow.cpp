#include "mainwindow.h"
#include "data_logger.h"
#include "data_receiver.h"
#include "batch_command_sender.h"
#include "dual_video_widget.h"  // ДОБАВЛЕНО
#include "yolo_detector.h"       // ДОБАВЛЕНО

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

// ДОБАВЛЕНО для статистики YOLO
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
    videoDisplay(nullptr)  // ИЗМЕНЕНО: было QLabel*, стало DualVideoWidget*
{
    setupUI();
    setupConnections();

    // Подключаем логгер к окну
    connect(logger, &DataLogger::newLogMessage,
            logBox, &QTextEdit::append);

    // Запуск UDP датчика
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

    // Общий стиль окна
    setStyleSheet(
        "QMainWindow {"
        "   background-color: #f5f5f5;"
        "   font-family: Arial;"
        "}"
        "QPushButton {"
        "   background-color: #2196F3;"
        "   color: white;"
        "   border: none;"
        "   padding: 6px 14px;"
        "   border-radius: 5px;"
        "   font-weight: bold;"
        "   font-size: 11px;"
        "   min-height: 28px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #1976D2;"
        "}"
        "QPushButton:disabled {"
        "   background-color: #b0b0b0;"
        "   color: #666;"
        "}"
        "QTextEdit {"
        "   border: 1px solid #e0e0e0;"
        "   border-radius: 4px;"
        "   background-color: white;"
        "   font-family: 'Courier New';"
        "   font-size: 11px;"
        "   padding: 4px;"
        "}"
        "QLabel {"
        "   font-family: Arial;"
        "}"
        "QLineEdit {"
        "   border: 1px solid #e0e0e0;"
        "   border-radius: 4px;"
        "   padding: 6px;"
        "   background-color: white;"
        "   font-size: 11px;"
        "}"
        );

    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setSpacing(8);
    mainLayout->setContentsMargins(12, 8, 12, 8);

    // ВЕРХНИЙ СТАТУС‑БАР
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->setSpacing(6);

    connectionStatus = new QLabel("● Отключен");
    connectionStatus->setStyleSheet(
        "color: #f44336;"
        "font-weight: bold;"
        "font-size: 12px;"
        "padding: 3px 6px;"
        );
    connectionStatus->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    videoStatus = new QLabel("● Видео не активно");
    videoStatus->setStyleSheet(
        "color: #9e9e9e;"
        "font-weight: bold;"
        "font-size: 12px;"
        "padding: 3px 6px;"
        );
    videoStatus->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    topLayout->addWidget(connectionStatus);
    topLayout->addStretch();
    topLayout->addWidget(videoStatus);

    mainLayout->addLayout(topLayout);

    // БЛОК УПРАВЛЕНИЯ
    QFrame *controlCard = new QFrame();
    controlCard->setStyleSheet(
        "QFrame {"
        "   background-color: white;"
        "   border-radius: 8px;"
        "   border: 1px solid #e0e0e0;"
        "   padding: 8px;"
        "}"
        );

    QVBoxLayout *ctrlLayout = new QVBoxLayout(controlCard);
    ctrlLayout->setSpacing(4);

    QLabel *ctrlTitle = new QLabel("УПРАВЛЕНИЕ РОБОТОМ");
    ctrlTitle->setStyleSheet(
        "font-size: 14px;"
        "font-weight: bold;"
        "color: #333;"
        "margin-bottom: 4px;"
        );
    ctrlTitle->setAlignment(Qt::AlignCenter);
    ctrlLayout->addWidget(ctrlTitle);

    QLabel *instructions = new QLabel(
        "W — Вперед    |    S — Назад    |    A — Влево    |    D — Вправо   |   ПРОБЕЛ — Остановка    |    L — Return to home"
        );
    instructions->setStyleSheet(
        "background-color: #f8f9fa;"
        "border: 1px solid #e0e0e0;"
        "border-radius: 6px;"
        "font-size: 11px;"
        "color: #555;"
        "padding: 8px;"
        );
    instructions->setAlignment(Qt::AlignCenter);
    instructions->setWordWrap(true);
    ctrlLayout->addWidget(instructions);

    mainLayout->addWidget(controlCard, 0, Qt::AlignTop);

    // БЛОК ПАКЕТНЫХ КОМАНД
    QFrame *batchCard = new QFrame();
    batchCard->setStyleSheet(
        "QFrame {"
        "   background-color: #f0f4f8;"
        "   border-radius: 6px;"
        "   border: 1px solid #c0c0c0;"
        "   padding: 6px;"
        "}"
        );

    QHBoxLayout *batchLayout = new QHBoxLayout(batchCard);
    batchLayout->setSpacing(6);
    batchLayout->setContentsMargins(6, 6, 6, 6);

    QLabel *batchLabel = new QLabel("📦 Пакетная команда:");
    batchLabel->setStyleSheet("font-weight: bold; font-size: 11px; color: #555;");

    batchCommandEdit = new QLineEdit();
    batchCommandEdit->setPlaceholderText("Например: Forward:2s; Back:1s; Left:1s");
    batchCommandEdit->setMinimumHeight(28);

    sendBatchButton = new QPushButton("Отправить пакет");
    sendBatchButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #9C27B0;"
        "}"
        "QPushButton:hover {"
        "   background-color: #7B1FA2;"
        "}"
        );

    batchLayout->addWidget(batchLabel);
    batchLayout->addWidget(batchCommandEdit, 1);
    batchLayout->addWidget(sendBatchButton);

    mainLayout->addWidget(batchCard);

    // ========== ИЗМЕНЕНО: ОБЛАСТЬ ВИДЕО С YOLO ==========
    QFrame *videoCard = new QFrame();
    videoCard->setStyleSheet(
        "QFrame {"
        "   background-color: white;"
        "   border-radius: 6px;"
        "   border: 1px solid #e0e0e0;"
        "   padding: 6px;"
        "}"
        );

    QVBoxLayout *videoCardLayout = new QVBoxLayout(videoCard);
    videoCardLayout->setSpacing(4);

    // Заголовок видео
    QHBoxLayout *videoTitleLayout = new QHBoxLayout();
    QLabel *videoTitle = new QLabel("ВИДЕОПОТОК");
    videoTitle->setStyleSheet(
        "font-size: 12px;"
        "font-weight: bold;"
        "color: #333;"
        "margin-bottom: 2px;"
        );

    // Статистика YOLO (маленькая строка справа от заголовка)
    yoloObjectsLabel = new QLabel("📊 0");
    yoloObjectsLabel->setStyleSheet("color: #4CAF50; font-size: 10px; font-weight: bold;");
    yoloTimeLabel = new QLabel("⏱ 0мс");
    yoloTimeLabel->setStyleSheet("color: #2196F3; font-size: 10px; font-weight: bold;");
    yoloFpsLabel = new QLabel("⚡ 0fps");
    yoloFpsLabel->setStyleSheet("color: #FF9800; font-size: 10px; font-weight: bold;");

    videoTitleLayout->addWidget(videoTitle);
    videoTitleLayout->addStretch();
    videoTitleLayout->addWidget(yoloObjectsLabel);
    videoTitleLayout->addWidget(yoloTimeLabel);
    videoTitleLayout->addWidget(yoloFpsLabel);

    videoCardLayout->addLayout(videoTitleLayout);

    // ИЗМЕНЕНО: videoDisplay теперь DualVideoWidget
    videoDisplay = new DualVideoWidget();
    videoDisplay->setMinimumHeight(220);
    videoDisplay->setMaximumHeight(380);

    videoCardLayout->addWidget(videoDisplay);

    mainLayout->addWidget(videoCard, 1);
    // ========== КОНЕЦ ИЗМЕНЕНИЙ В ВИДЕО ==========

    // НИЖНИЙ БЛОК: ДАТЧИК + КОМАНДЫ (БЕЗ ИЗМЕНЕНИЙ)
    QFrame *bottomCard = new QFrame();
    bottomCard->setStyleSheet(
        "QFrame {"
        "   background-color: white;"
        "   border-radius: 8px;"
        "   border: 1px solid #e0e0e0;"
        "   padding: 6px;"
        "}"
        );

    QHBoxLayout *bottomLayout = new QHBoxLayout(bottomCard);
    bottomLayout->setSpacing(8);
    bottomLayout->setContentsMargins(6, 6, 6, 6);

    // ЛЕВОЕ ОКНО — ДАТЧИК РАССТОЯНИЯ
    QFrame *sensorFrame = new QFrame();
    sensorFrame->setStyleSheet(
        "QFrame {"
        "   background-color: #f8f9fa;"
        "   border: 1px solid #e0e0e0;"
        "   border-radius: 6px;"
        "   padding: 6px;"
        "}"
        );

    QVBoxLayout *sensorLayout = new QVBoxLayout(sensorFrame);
    sensorLayout->setSpacing(3);

    QLabel *sensorTitle = new QLabel("ДАТЧИК РАССТОЯНИЯ");
    sensorTitle->setStyleSheet(
        "font-size: 11px;"
        "font-weight: bold;"
        "color: #666;"
        );
    sensorLayout->addWidget(sensorTitle);

    distanceLabel = new QLabel("---");
    distanceLabel->setStyleSheet(
        "background-color: #1e1e1e;"
        "color: #4caf50;"
        "font-size: 32px;"
        "font-weight: bold;"
        "padding: 12px;"
        "border-radius: 6px;"
        );
    distanceLabel->setAlignment(Qt::AlignCenter);
    distanceLabel->setMinimumHeight(70);
    distanceLabel->setMaximumHeight(70);
    sensorLayout->addWidget(distanceLabel);

    QLabel *distanceUnit = new QLabel("сантиметров");
    distanceUnit->setStyleSheet(
        "color: #999;"
        "font-size: 11px;"
        );
    distanceUnit->setAlignment(Qt::AlignCenter);
    sensorLayout->addWidget(distanceUnit);

    // ПРАВОЕ ОКНО — КОМАНДЫ WASD
    QFrame *commandsFrame = new QFrame();
    commandsFrame->setStyleSheet(
        "QFrame {"
        "   background-color: #f8f9fa;"
        "   border: 1px solid #e0e0e0;"
        "   border-radius: 6px;"
        "   padding: 6px;"
        "}"
        );

    QVBoxLayout *commandsLayout = new QVBoxLayout(commandsFrame);
    commandsLayout->setSpacing(3);

    QLabel *commandsTitle = new QLabel("ПОСЛЕДНИЕ КОМАНДЫ");
    commandsTitle->setStyleSheet(
        "font-size: 11px;"
        "font-weight: bold;"
        "color: #666;"
        );
    commandsLayout->addWidget(commandsTitle);

    commandsLog = new QTextEdit();
    commandsLog->setReadOnly(true);
    commandsLog->setMaximumHeight(90);
    commandsLog->setMinimumHeight(60);
    commandsLog->setStyleSheet(
        "QTextEdit {"
        "   background-color: #ffffff;"
        "   border: 1px solid #e0e0e0;"
        "   border-radius: 4px;"
        "   font-family: 'Courier New';"
        "   font-size: 11px;"
        "   padding: 3px;"
        "}"
        );

    commandsLayout->addWidget(commandsLog);

    bottomLayout->addWidget(sensorFrame, 1);
    bottomLayout->addWidget(commandsFrame, 1);

    mainLayout->addWidget(bottomCard, 0, Qt::AlignTop);

    // СИСТЕМНЫЙ ЛОГ (БЕЗ ИЗМЕНЕНИЙ)
    QFrame *logCard = new QFrame();
    logCard->setStyleSheet(
        "QFrame {"
        "   background-color: white;"
        "   border-radius: 8px;"
        "   border: 1px solid #e0e0e0;"
        "   padding: 6px;"
        "}"
        );

    QVBoxLayout *logLayout = new QVBoxLayout(logCard);
    logLayout->setSpacing(3);

    QLabel *logTitle = new QLabel("СИСТЕМНЫЙ ЛОГ");
    logTitle->setStyleSheet(
        "font-size: 11px;"
        "font-weight: bold;"
        "color: #666;"
        );
    logLayout->addWidget(logTitle);

    logBox = new QTextEdit();
    logBox->setReadOnly(true);
    logBox->setMaximumHeight(90);
    logBox->setMinimumHeight(60);
    logBox->setStyleSheet(
        "QTextEdit {"
        "   background-color: #f8f9fa;"
        "   border: 1px solid #e0e0e0;"
        "   border-radius: 4px;"
        "   font-family: 'Courier New';"
        "   font-size: 11px;"
        "   padding: 3px;"
        "}"
        );

    logLayout->addWidget(logBox);

    mainLayout->addWidget(logCard, 0, Qt::AlignTop);

    // КНОПКИ УПРАВЛЕНИЯ (БЕЗ ИЗМЕНЕНИЙ)
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(6);

    connectButton = new QPushButton("🔧 Подключиться");
    startVideoButton = new QPushButton("▶ Старт видео");
    stopVideoButton = new QPushButton("⏹ Стоп видео");

    stopVideoButton->setEnabled(false);

    QString btnStyle =
        "QPushButton {"
        "   background-color: #2196F3;"
        "   color: white;"
        "   border: none;"
        "   padding: 6px 14px;"
        "   border-radius: 5px;"
        "   font-weight: bold;"
        "   font-size: 11px;"
        "   min-height: 28px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #1976D2;"
        "}"
        "QPushButton:disabled {"
        "   background-color: #b0b0b0;"
        "   color: #666;"
        "}";

    connectButton->setStyleSheet(btnStyle);
    startVideoButton->setStyleSheet(btnStyle);
    stopVideoButton->setStyleSheet(btnStyle);

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

    // ========== ИЗМЕНЕНО: Видео с поддержкой DualVideoWidget ==========
    connect(startVideoButton, &QPushButton::clicked, this, [=]() {
        if (videoDisplay) {
            streamer->start(videoDisplay->getLeftVideoLabel());  // ИЗМЕНЕНО
            // Подключаем VideoStreamer к DualVideoWidget
            streamer->setDualWidget(videoDisplay);  // ДОБАВЛЕНО
        }
    });

    connect(stopVideoButton, &QPushButton::clicked, this, [=]() {
        streamer->stop();
    });
    // ========== КОНЕЦ ИЗМЕНЕНИЙ ==========

    // CommandSender — статусы и команды
    connect(sender, &CommandSender::connected, this, [=]() {
        connectionStatus->setText("● Подключен");
        connectionStatus->setStyleSheet(
            "color: #4caf50;"
            "font-weight: bold;"
            "font-size: 12px;"
            "padding: 3px 6px;"
            );
        connectButton->setText("🔧 Отключиться");
        logger->logSystem("Подключен к роботу");
    });

    connect(sender, &CommandSender::disconnected, this, [=]() {
        connectionStatus->setText("● Отключен");
        connectionStatus->setStyleSheet(
            "color: #f44336;"
            "font-weight: bold;"
            "font-size: 12px;"
            "padding: 3px 6px;"
            );
        connectButton->setText("🔧 Подключиться");
        logger->logSystem("Отключен от робота");
    });

    connect(sender, &CommandSender::commandSent, this, [=](char cmd) {
        QString commandStr;
        QString color;

        switch(cmd) {
        case 'w':
            commandStr = "↑ Вперед";
            color = "#4caf50";
            break;
        case 's':
            commandStr = "↓ Назад";
            color = "#f44336";
            break;
        case 'a':
            commandStr = "← Влево";
            color = "#ff9800";
            break;
        case 'd':
            commandStr = "→ Вправо";
            color = "#ff9800";
            break;
        case 'l':
            commandStr = "⌂ Return home";
            color = "#9c27b0";
            break;
        case ' ':
            commandStr = "⏹ Стоп";
            color = "#607d8b";
            break;
        default:
            commandStr = QString("'%1'").arg(cmd);
            color = "#999";
        }

        QString timeStr = QTime::currentTime().toString("hh:mm:ss");
        commandsLog->append(
            QString("<span style='color: %1; font-weight: bold;'>[%2] %3</span>")
                .arg(color)
                .arg(timeStr)
                .arg(commandStr)
            );

        logger->logCommand(commandStr);
    });

    connect(sender, &CommandSender::errorOccurred, this, [=](const QString &error) {
        logger->logSystem("Ошибка: " + error);
    });

    // ПОДКЛЮЧЕНИЯ ДЛЯ ПАКЕТНЫХ КОМАНД
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
            QString("<span style='color: #9C27B0; font-weight: bold;'>[%1] 📦 Пакет: %2</span>")
                .arg(timeStr)
                .arg(cmd)
            );
        logger->logCommand("Пакет: " + cmd);
    });

    connect(batchSender, &BatchCommandSender::batchCompleted, this, [=]() {
        QString timeStr = QTime::currentTime().toString("hh:mm:ss");
        commandsLog->append(
            QString("<span style='color: #4CAF50; font-weight: bold;'>[%1] ✅ Пакет выполнен</span>")
                .arg(timeStr)
            );
        logger->logSystem("Пакетная команда выполнена");
    });

    // Отправка пакетной команды
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

    // ========== ДОБАВЛЕНО: Подключение сигналов YOLO для статистики ==========
    if (videoDisplay && videoDisplay->getDetector()) {
        YOLODetector *detector = videoDisplay->getDetector();

        connect(detector, &YOLODetector::detectionCompleted,
                this, [=](QVector<Detection> detections, qint64 elapsedMs) {
                    // Обновляем статистику в заголовке
                    yoloObjectsLabel->setText(QString("📊 %1").arg(detections.size()));
                    yoloTimeLabel->setText(QString("⏱ %1мс").arg(elapsedMs));

                    // Расчет FPS
                    static int frameCount = 0;
                    static qint64 lastTime = 0;
                    static float fps = 0;

                    if (lastTime == 0) {
                        lastTime = elapsedMs;
                    } else {
                        frameCount++;
                        if (frameCount >= 10) {
                            fps = 10000.0f / (elapsedMs - lastTime);
                            lastTime = elapsedMs;
                            frameCount = 0;
                        }
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
    // ========== КОНЕЦ ДОБАВЛЕНИЙ ==========

    // VideoStreamer
    connect(streamer, &VideoStreamer::started, this, [=]() {
        startVideoButton->setEnabled(false);
        stopVideoButton->setEnabled(true);
        videoStatus->setText("● Видео активно");
        videoStatus->setStyleSheet(
            "color: #4caf50;"
            "font-weight: bold;"
            "font-size: 12px;"
            "padding: 3px 6px;"
            );
        logger->logSystem("Видео поток запущен");
    });

    connect(streamer, &VideoStreamer::stopped, this, [=]() {
        startVideoButton->setEnabled(true);
        stopVideoButton->setEnabled(false);
        videoStatus->setText("● Видео не активно");
        videoStatus->setStyleSheet(
            "color: #9e9e9e;"
            "font-weight: bold;"
            "font-size: 12px;"
            "padding: 3px 6px;"
            );
        logger->logSystem("Видео поток остановлен");
    });

    connect(streamer, &VideoStreamer::errorOccurred, this, [=](const QString &error) {
        logger->logSystem("Ошибка видео: " + error);
        startVideoButton->setEnabled(true);
        stopVideoButton->setEnabled(false);
        videoStatus->setText("● Видео ошибка");
        videoStatus->setStyleSheet(
            "color: #f44336;"
            "font-weight: bold;"
            "font-size: 12px;"
            "padding: 3px 6px;"
            );
    });

    connect(streamer, &VideoStreamer::recordingStarted, this, [=](const QString &file) {
        logger->logSystem("Запись видео: " + file);
    });

    // ДАТЧИК РАССТОЯНИЯ
    connect(receiver, &DataReceiver::sensorDataReceived,
            this, [=](const QString& data)
            {
                distanceLabel->setText(data);
                logger->logSystem("ДАТЧИК: " + data);

                QString clean = data.simplified();
                QRegularExpression re(R"([\d.,]+)");
                QRegularExpressionMatch match = re.match(clean);
                if (!match.hasMatch()) {
                    return;
                }

                QString numStr = match.captured(0).replace(",", ".");
                bool ok;
                float distance = numStr.toFloat(&ok);
                if (!ok) {
                    return;
                }

                if (distance < 20.0) {
                    distanceLabel->setStyleSheet(
                        "background-color: #1e1e1e;"
                        "color: #f44336;"
                        "font-size: 32px;"
                        "font-weight: bold;"
                        "padding: 12px;"
                        "border-radius: 6px;"
                        );
                } else {
                    distanceLabel->setStyleSheet(
                        "background-color: #1e1e1e;"
                        "color: #4caf50;"
                        "font-size: 32px;"
                        "font-weight: bold;"
                        "padding: 12px;"
                        "border-radius: 6px;"
                        );
                }
            });
}

// Метод для отправки пакетной команды
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
    // Убрал старую логику с pixmap, так как теперь videoDisplay сам обрабатывает resize
}

// ========== РЕАЛИЗАЦИИ НЕДОСТАЮЩИХ МЕТОДОВ ==========

void MainWindow::onBatchCompleted()
{
    logger->logSystem("Пакетная команда завершена");
}

void MainWindow::updateConnectionStatus(bool connected)
{
    if (connected) {
        connectionStatus->setText("● Подключен");
        connectionStatus->setStyleSheet(
            "color: #4caf50;"
            "font-weight: bold;"
            "font-size: 12px;"
            "padding: 3px 6px;"
            );
    } else {
        connectionStatus->setText("● Отключен");
        connectionStatus->setStyleSheet(
            "color: #f44336;"
            "font-weight: bold;"
            "font-size: 12px;"
            "padding: 3px 6px;"
            );
    }
}

void MainWindow::updateVideoStatus(bool streaming)
{
    if (streaming) {
        videoStatus->setText("● Видео активно");
        videoStatus->setStyleSheet(
            "color: #4caf50;"
            "font-weight: bold;"
            "font-size: 12px;"
            "padding: 3px 6px;"
            );
    } else {
        videoStatus->setText("● Видео не активно");
        videoStatus->setStyleSheet(
            "color: #9e9e9e;"
            "font-weight: bold;"
            "font-size: 12px;"
            "padding: 3px 6px;"
            );
    }
}

void MainWindow::logMessage(const QString& message)
{
    logger->logSystem(message);
}

void MainWindow::onDetectionCompleted(QVector<Detection> detections, qint64 elapsedMs)
{
    // Обновляем статистику в заголовке
    yoloObjectsLabel->setText(QString("📊 %1").arg(detections.size()));
    yoloTimeLabel->setText(QString("⏱ %1мс").arg(elapsedMs));

    // Расчет FPS
    static int frameCount = 0;
    static qint64 lastTime = 0;
    static float fps = 0;

    if (lastTime == 0) {
        lastTime = elapsedMs;
    } else {
        frameCount++;
        if (frameCount >= 10) {
            fps = 10000.0f / (elapsedMs - lastTime);
            lastTime = elapsedMs;
            frameCount = 0;
        }
    }
    yoloFpsLabel->setText(QString("⚡ %1fps").arg(fps, 0, 'f', 1));

    // Логируем если найдены объекты
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
    // Можно использовать для обновления дополнительной статистики
    logger->logSystem("YOLO stats: " + stats);
}
