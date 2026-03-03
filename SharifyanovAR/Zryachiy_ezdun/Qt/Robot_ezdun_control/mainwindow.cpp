#include "mainwindow.h"
#include "data_logger.h"
#include "data_receiver.h"
#include "batch_command_sender.h"

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
#include <QLineEdit>  // Добавляем для batchCommandEdit

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    sender(new CommandSender(this)),
    streamer(new VideoStreamer(this)),
    logger(new DataLogger(this)),
    receiver(new DataReceiver(this)),
    batchSender(new BatchCommandSender(this))  // Добавляем batchSender
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

    // Общий стиль окна - добавляем стили для QLineEdit
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

    // ========== НОВЫЙ БЛОК: ПАКЕТНЫЕ КОМАНДЫ ==========
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

    // ОБЛАСТЬ ВИДЕО (немного уменьшаем высоту для пакетных команд)
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

    QLabel *videoTitle = new QLabel("ВИДЕОПОТОК");
    videoTitle->setStyleSheet(
        "font-size: 12px;"
        "font-weight: bold;"
        "color: #333;"
        "margin-bottom: 2px;"
        );
    videoCardLayout->addWidget(videoTitle);

    videoDisplay = new QLabel();
    videoDisplay->setMinimumHeight(220);   // Немного уменьшаем для пакетных команд
    videoDisplay->setMaximumHeight(380);
    videoDisplay->setAlignment(Qt::AlignCenter);
    videoDisplay->setStyleSheet(
        "border: 1px solid #e0e0e0;"
        "border-radius: 5px;"
        "background-color: #1a1a1a;"
        );

    QPixmap placeholder(640, 480);
    placeholder.fill(Qt::black);
    videoDisplay->setPixmap(placeholder);

    videoCardLayout->addWidget(videoDisplay);

    mainLayout->addWidget(videoCard, 1);

    // НИЖНИЙ БЛОК: ДАТЧИК + КОМАНДЫ
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

    // СИСТЕМНЫЙ ЛОГ
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

    // КНОПКИ УПРАВЛЕНИЯ
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
    // Подключение к роботу - обновляем для batchSender
    connect(connectButton, &QPushButton::clicked, this, [=]() {
        if (sender->isConnected() || (batchSender && batchSender->isConnected())) {
            sender->disconnectFromRobot();
            if (batchSender) batchSender->disconnectFromRobot();
        } else {
            sender->connectToRobot();
            if (batchSender) batchSender->connectToRobot();
        }
    });

    // Видео
    connect(startVideoButton, &QPushButton::clicked, this, [=]() {
        streamer->start(videoDisplay);
    });

    connect(stopVideoButton, &QPushButton::clicked, this, [=]() {
        streamer->stop();
    });

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

    // ========== ПОДКЛЮЧЕНИЯ ДЛЯ ПАКЕТНЫХ КОМАНД ==========

    // Подключение batchSender
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

    // Отправка пакетной команды по кнопке
    connect(sendBatchButton, &QPushButton::clicked, this, [=]() {
        QString command = batchCommandEdit->text().trimmed();
        if (!command.isEmpty()) {
            sendBatchCommand(command);
            batchCommandEdit->clear();
        }
    });

    // Отправка пакетной команды по Enter в поле ввода
    connect(batchCommandEdit, &QLineEdit::returnPressed, this, [=]() {
        QString command = batchCommandEdit->text().trimmed();
        if (!command.isEmpty()) {
            sendBatchCommand(command);
            batchCommandEdit->clear();
        }
    });

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
        // Можно показать предупреждение, но не обязательно
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

    if (videoDisplay && !videoDisplay->pixmap().isNull()) {
        QPixmap scaled = videoDisplay->pixmap().scaled(
            videoDisplay->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            );
        videoDisplay->setPixmap(scaled);
    }
}
