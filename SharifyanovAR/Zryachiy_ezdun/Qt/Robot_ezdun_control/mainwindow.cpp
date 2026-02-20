#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QDateTime>
#include <QResizeEvent>
#include <QFocusEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    sender(new CommandSender(this)),
    streamer(new VideoStreamer(this))
{
    setupUI();
    setupConnections();

    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    qDebug() << "MainWindow initialized";
}

MainWindow::~MainWindow()
{
    streamer->stop();
    sender->disconnectFromRobot();
}

void MainWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    // Статус подключения к роботу
    connectionStatus = new QLabel("Отключен от робота");
    connectionStatus->setStyleSheet("color: red; font-weight: bold;");
    mainLayout->addWidget(connectionStatus);

    // Инструкция по управлению
    instructions = new QLabel(
        "УПРАВЛЕНИЕ РОБОТОМ:\n"
        "W - Вперед | S - Назад | A - Влево | D - Вправо\n"
        "ПРОБЕЛ - Остановка | L - Return to home"
        );
    instructions->setStyleSheet("background-color: #f0f0f0; padding: 8px;");
    instructions->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(instructions);

    // Статус видео
    videoStatus = new QLabel("Видео: не активно");
    videoStatus->setStyleSheet("color: gray;");
    mainLayout->addWidget(videoStatus);

    // Видео дисплей
    videoDisplay = new QLabel();
    videoDisplay->setMinimumHeight(360);
    videoDisplay->setMaximumHeight(480);
    videoDisplay->setStyleSheet("border: 2px solid #cccccc; background-color: black;");
    videoDisplay->setAlignment(Qt::AlignCenter);

    // Показываем заглушку
    QPixmap placeholder(640, 480);
    placeholder.fill(Qt::black);
    videoDisplay->setPixmap(placeholder);

    mainLayout->addWidget(videoDisplay);

    // Лог
    logBox = new QTextEdit();
    logBox->setReadOnly(true);
    logBox->setMaximumHeight(120);
    mainLayout->addWidget(logBox);

    // Кнопки
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    connectButton = new QPushButton("Подключиться к роботу");
    startVideoButton = new QPushButton("Старт видео");
    stopVideoButton = new QPushButton("Стоп видео");

    stopVideoButton->setEnabled(false);

    buttonLayout->addWidget(connectButton);
    buttonLayout->addWidget(startVideoButton);
    buttonLayout->addWidget(stopVideoButton);

    mainLayout->addLayout(buttonLayout);
}

void MainWindow::setupConnections()
{
    // Подключение к роботу
    connect(connectButton, &QPushButton::clicked, this, [=]() {
        if (sender->isConnected()) {
            sender->disconnectFromRobot();
        } else {
            sender->connectToRobot();
        }
    });

    // Управление видео
    connect(startVideoButton, &QPushButton::clicked, this, [=]() {
        streamer->start(videoDisplay);
    });

    connect(stopVideoButton, &QPushButton::clicked, this, [=]() {
        streamer->stop();
    });

    // Сигналы от CommandSender
    connect(sender, &CommandSender::connected, this, [=]() {
        connectionStatus->setText("Подключен к роботу");
        connectionStatus->setStyleSheet("color: green; font-weight: bold;");
        connectButton->setText("Отключиться от робота");
        logBox->append("[" + QTime::currentTime().toString() + "] Подключен к роботу");
    });

    connect(sender, &CommandSender::disconnected, this, [=]() {
        connectionStatus->setText("Отключен от робота");
        connectionStatus->setStyleSheet("color: red; font-weight: bold;");
        connectButton->setText("Подключиться к роботу");
        logBox->append("[" + QTime::currentTime().toString() + "] Отключен от робота");
    });

    connect(sender, &CommandSender::commandSent, this,
            [=](char cmd) {
                QString commandStr;
                switch(cmd) {
                case 'w': commandStr = "Вперед"; break;
                case 's': commandStr = "Назад"; break;
                case 'a': commandStr = "Влево"; break;
                case 'd': commandStr = "Вправо"; break;
                case 'l': commandStr = "Return home"; break;
                case ' ': commandStr = "Стоп"; break;
                default: commandStr = QString("'%1'").arg(cmd);
                }
                logBox->append("[" + QTime::currentTime().toString() + "] Команда: " + commandStr);
            });

    connect(sender, &CommandSender::errorOccurred, this,
            [=](QString error) {
                logBox->append("[" + QTime::currentTime().toString() + "] Ошибка: " + error);
            });

    // Сигналы от VideoStreamer
    connect(streamer, &VideoStreamer::started, this, [=]() {
        startVideoButton->setEnabled(false);
        stopVideoButton->setEnabled(true);
        videoStatus->setText("Видео: активно");
        videoStatus->setStyleSheet("color: green;");
        logBox->append("[" + QTime::currentTime().toString() + "] Видео поток запущен");
    });

    connect(streamer, &VideoStreamer::stopped, this, [=]() {
        startVideoButton->setEnabled(true);
        stopVideoButton->setEnabled(false);
        videoStatus->setText("Видео: не активно");
        videoStatus->setStyleSheet("color: gray;");
        logBox->append("[" + QTime::currentTime().toString() + "] Видео поток остановлен");
    });

    connect(streamer, &VideoStreamer::errorOccurred, this,
            [=](QString error) {
                logBox->append("[" + QTime::currentTime().toString() + "] Ошибка видео: " + error);
                startVideoButton->setEnabled(true);
                stopVideoButton->setEnabled(false);
                videoStatus->setText("Видео: ошибка");
                videoStatus->setStyleSheet("color: red;");
            });

    connect(streamer, &VideoStreamer::recordingStarted, this,
            [=](QString file) {
                logBox->append("[" + QTime::currentTime().toString() + "] Запись: " + file);
            });
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
        logBox->append("[" + QTime::currentTime().toString() + "] Остановка");
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
    }
    QMainWindow::focusOutEvent(event);
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
