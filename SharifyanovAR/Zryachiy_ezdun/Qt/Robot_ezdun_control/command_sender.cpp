#include "command_sender.h"
#include "command_logger.h"
#include <QtCore>
#include <QDebug>

CommandSender::CommandSender(QObject *parent)
    : QObject(parent),
    socket(new QTcpSocket(this)),
    lastCommand('\0'),
    pendingCommand('\0'),
    currentKey(0),
    movementTimer(new QTimer(this)),
    stopDelayTimer(new QTimer(this)),
    lastSentTime(0),
    commandCount(0),
    lastResetTime(0),
    lastPressTime(0)
{
    connect(socket, &QTcpSocket::connected,
            this, &CommandSender::onConnected);

    connect(socket, &QTcpSocket::disconnected,
            this, &CommandSender::onDisconnected);

    connect(socket, &QTcpSocket::errorOccurred,
            this, &CommandSender::onError);

    movementTimer->setInterval(MOVEMENT_INTERVAL_MS);
    connect(movementTimer, &QTimer::timeout,
            this, &CommandSender::processMovement);

    stopDelayTimer->setSingleShot(true);
    connect(stopDelayTimer, &QTimer::timeout,
            this, &CommandSender::sendStopCommand);

    commandTimer.start();
    lastResetTime = QDateTime::currentMSecsSinceEpoch();
}

CommandSender::~CommandSender()
{
    disconnectFromRobot();
}

void CommandSender::connectToRobot()
{
    if (socket->state() == QAbstractSocket::ConnectedState)
        return;

    socket->connectToHost(
        Config::RASPBERRY_IP,
        Config::COMMAND_PORT
        );
}

void CommandSender::disconnectFromRobot()
{
    if (socket->state() == QAbstractSocket::ConnectedState) {
        stopRobot();
        socket->disconnectFromHost();
    }
    movementTimer->stop();
    stopDelayTimer->stop();
    currentKey = 0;
    lastCommand = '\0';
    pendingCommand = '\0';
}

bool CommandSender::isConnected() const
{
    return socket->state() == QAbstractSocket::ConnectedState;
}

bool CommandSender::checkCommandRate()
{
    qint64 now = QDateTime::currentMSecsSinceEpoch();

    if (now - lastResetTime > 1000) {
        commandCount = 0;
        lastResetTime = now;
    }

    commandCount++;

    if (commandCount > MAX_COMMANDS_PER_SECOND) {
        qDebug() << "Command rate limit exceeded:" << commandCount << "commands in last second";
        return false;
    }

    return true;
}

void CommandSender::resetCommandRate()
{
    commandCount = 0;
    lastResetTime = QDateTime::currentMSecsSinceEpoch();
}

void CommandSender::sendCommand(char command)
{
    if (!isConnected()) {
        qDebug() << "Cannot send command - not connected!";
        return;
    }

    if (!checkCommandRate()) {
        qDebug() << "Command rejected - rate limit";
        return;
    }

    qint64 now = QDateTime::currentMSecsSinceEpoch();

    if (command == lastCommand) {
        if (now - lastSentTime < REPEAT_COMMAND_INTERVAL_MS) {
            qDebug() << "Skipping repeat command" << command << "- too soon";
            return;
        }
    }

    qint64 elapsed = commandTimer.elapsed();
    if (elapsed < COMMAND_DELAY_MS && command != lastCommand && command != ' ') {
        qDebug() << "Delaying command" << command << "for" << (COMMAND_DELAY_MS - elapsed) << "ms";
        if (pendingCommand == '\0') {
            pendingCommand = command;
            stopDelayTimer->start(COMMAND_DELAY_MS - elapsed);
        }
        return;
    }

    lastCommand = command;
    pendingCommand = '\0';
    lastSentTime = now;

    QByteArray data;
    data.append(command);

    qDebug() << "Sending command:" << command << "at" << QTime::currentTime().toString("hh:mm:ss.zzz");

    socket->write(data);

    CommandLogger::log(command);
    emit commandSent(command);

    commandTimer.restart();
}

void CommandSender::stopRobot()
{
    sendCommand(' ');
}

void CommandSender::onKeyPressed(int key)
{
    qint64 now = QDateTime::currentMSecsSinceEpoch();

    if (now - lastPressTime < 100) {
        qDebug() << "Ignoring rapid key press";
        return;
    }
    lastPressTime = now;

    if (key == currentKey) {
        qDebug() << "Key" << key << "already pressed";
        return;
    }

    qDebug() << "Key pressed:" << key;

    currentKey = key;

    stopDelayTimer->stop();

    char command = ' ';

    switch (key) {
    case Qt::Key_W:
        command = 'w';
        break;
    case Qt::Key_S:
        command = 's';
        break;
    case Qt::Key_A:
        command = 'a';
        break;
    case Qt::Key_D:
        command = 'd';
        break;
    default:
        qDebug() << "Unhandled key:" << key;
        return;
    }

    qDebug() << "Sending command for key" << key << "->" << command;
    sendCommand(command);

    if (!movementTimer->isActive()) {
        qDebug() << "Starting movement timer";
        movementTimer->start();
    }
}

void CommandSender::onKeyReleased(int key)
{
    qDebug() << "Key released:" << key;

    if (key != currentKey) {
        qDebug() << "Ignoring release - not current key";
        return;
    }

    currentKey = 0;

    stopDelayTimer->stop();

    qint64 elapsed = commandTimer.elapsed();

    if (elapsed < COMMAND_DELAY_MS) {
        qDebug() << "Scheduling stop after" << (COMMAND_DELAY_MS - elapsed) << "ms";
        stopDelayTimer->start(COMMAND_DELAY_MS - elapsed);
    } else {
        qDebug() << "Sending stop immediately";
        sendStopCommand();
    }
}

void CommandSender::sendStopCommand()
{
    qDebug() << "Sending stop command";
    sendCommand(' ');

    if (movementTimer->isActive()) {
        qDebug() << "Stopping movement timer";
        movementTimer->stop();
    }
}

void CommandSender::processMovement()
{
    if (!isConnected()) {
        qDebug() << "Movement timer: not connected";
        return;
    }

    if (pendingCommand != '\0') {
        qDebug() << "Movement timer: sending pending command" << pendingCommand;
        sendCommand(pendingCommand);
        return;
    }

    if (currentKey != 0) {
        char command = ' ';
        switch (currentKey) {
        case Qt::Key_W:
            command = 'w';
            break;
        case Qt::Key_S:
            command = 's';
            break;
        case Qt::Key_A:
            command = 'a';
            break;
        case Qt::Key_D:
            command = 'd';
            break;
        default:
            return;
        }

        qDebug() << "Movement timer: sending command for key" << currentKey;
        sendCommand(command);
    }
}

void CommandSender::onConnected()
{
    qDebug() << "Connected to robot!";
    emit connected();
    currentKey = 0;
    lastCommand = '\0';
    pendingCommand = '\0';
    commandTimer.restart();
    resetCommandRate();

    QTimer::singleShot(1000, this, [=]() {
        if (isConnected()) {
            qDebug() << "Sending test command (lights)";
            QByteArray data;
            data.append('l');
            socket->write(data);
        }
    });
}

void CommandSender::onDisconnected()
{
    qDebug() << "Disconnected from robot";
    movementTimer->stop();
    stopDelayTimer->stop();
    currentKey = 0;
    lastCommand = '\0';
    pendingCommand = '\0';
    emit disconnected();
}

void CommandSender::onError(QAbstractSocket::SocketError)
{
    QString errorString = socket->errorString();
    qDebug() << "Socket error:" << errorString;
    emit errorOccurred(errorString);
}
