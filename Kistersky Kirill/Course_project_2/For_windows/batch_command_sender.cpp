#include "batch_command_sender.h"
#include "command_logger.h"
#include <QtCore>
#include <QDebug>
#include <QThread>
#include <QRegularExpression>

BatchCommandSender::BatchCommandSender(QObject *parent)
    : QObject(parent),
    socket(new QTcpSocket(this)),
    batchCount(0),
    lastResetTime(0)
{
    connect(socket, &QTcpSocket::connected,
            this, &BatchCommandSender::onConnected);

    connect(socket, &QTcpSocket::disconnected,
            this, &BatchCommandSender::onDisconnected);

    connect(socket, &QTcpSocket::errorOccurred,
            this, &BatchCommandSender::onError);

    // Таймер для отправки следующей команды
    batchTimer = new QTimer(this);
    batchTimer->setInterval(BATCH_EXECUTION_DELAY_MS);
    batchTimer->setSingleShot(true);
    connect(batchTimer, &QTimer::timeout,
            this, &BatchCommandSender::executeNextBatchCommand);

    // Таймер для проверки завершения текущей команды
    completionTimer = new QTimer(this);
    completionTimer->setInterval(COMPLETION_CHECK_INTERVAL_MS);
    connect(completionTimer, &QTimer::timeout,
            this, &BatchCommandSender::checkCommandCompletion);

    batchTimerControl.start();
    lastResetTime = QDateTime::currentMSecsSinceEpoch();

    currentCommand.duration = 0;
}

BatchCommandSender::~BatchCommandSender()
{
    disconnectFromRobot();
}

void BatchCommandSender::connectToRobot()
{
    if (socket->state() == QAbstractSocket::ConnectedState)
        return;

    qDebug() << "[BATCH] Connecting to robot...";
    socket->connectToHost(
        Config::RASPBERRY_IP,
        Config::COMMAND_PORT
        );
}

void BatchCommandSender::disconnectFromRobot()
{
    if (socket->state() == QAbstractSocket::ConnectedState) {
        batchTimer->stop();
        completionTimer->stop();
        socket->disconnectFromHost();
    }

    batchQueue.clear();
    currentCommand.command.clear();
    currentCommand.duration = 0;
    batchCount = 0;
}

bool BatchCommandSender::isConnected() const
{
    return socket->state() == QAbstractSocket::ConnectedState;
}

int BatchCommandSender::extractDuration(const QString& command)
{
    QRegularExpression re(":(\\d+)s");
    QRegularExpressionMatch match = re.match(command);
    if (match.hasMatch()) {
        return match.captured(1).toInt();
    }
    return 1;
}

void BatchCommandSender::sendBatchCommand(const QString& batchCommand)
{
    if (!isConnected()) {
        qDebug() << "[BATCH] Cannot send - not connected!";
        emit errorOccurred("Not connected to robot");
        return;
    }

    if (!checkBatchRate()) {
        qDebug() << "[BATCH] Rate limit exceeded, waiting...";
        // Вместо отбрасывания, добавляем в очередь с задержкой
        QTimer::singleShot(1000, [this, batchCommand]() {
            sendBatchCommand(batchCommand);
        });
        return;
    }

    qDebug() << "[BATCH] Queueing:" << batchCommand;

    QueuedCommand cmd;
    cmd.command = batchCommand;
    cmd.duration = extractDuration(batchCommand);

    batchQueue.enqueue(cmd);

    if (batchQueue.size() == 1 && currentCommand.command.isEmpty()) {
        qDebug() << "[BATCH] Starting queue execution";
        executeNextBatchCommand();
    }

    emit batchCommandSent(batchCommand);
}

void BatchCommandSender::sendSimpleBatch(const QString& direction, int seconds)
{
    QString batchCmd = formatBatchCommand(direction, seconds);
    sendBatchCommand(batchCmd);
}

QString BatchCommandSender::formatBatchCommand(const QString& direction, int seconds)
{
    return QString("%1:%2s").arg(direction).arg(seconds);
}

void BatchCommandSender::executeNextBatchCommand()
{
    completionTimer->stop();

    if (batchQueue.isEmpty()) {
        qDebug() << "[BATCH] Queue empty";
        currentCommand.command.clear();
        currentCommand.duration = 0;
        emit batchCompleted();
        return;
    }

    currentCommand = batchQueue.dequeue();

    qDebug() << "[BATCH] Executing:" << currentCommand.command
             << "(" << currentCommand.duration << "s)";

    // Отправляем команду
    QByteArray data = currentCommand.command.toUtf8() + "\n";
    socket->write(data);
    socket->flush();
    qDebug() << "[BATCH] Command sent, waiting" << currentCommand.duration << "seconds";

    executionTimer.start();
    completionTimer->start();
}

void BatchCommandSender::checkCommandCompletion()
{
    if (currentCommand.command.isEmpty()) {
        completionTimer->stop();
        return;
    }

    if (executionTimer.elapsed() >= currentCommand.duration * 1000) {
        qDebug() << "[BATCH] Command completed after" << executionTimer.elapsed() << "ms";

        completionTimer->stop();

        // Отправляем команду остановки
        qDebug() << "[BATCH] Sending stop command";
        socket->write(" \n");  // Пробел - команда остановки
        socket->flush();

        QThread::msleep(100);

        if (!batchQueue.isEmpty()) {
            qDebug() << "[BATCH] Next command in queue, executing after delay";
            batchTimer->start();
        } else {
            qDebug() << "[BATCH] All commands completed";
            currentCommand.command.clear();
            currentCommand.duration = 0;
            emit batchCompleted();
        }
    }
}

bool BatchCommandSender::checkBatchRate()
{
    qint64 now = QDateTime::currentMSecsSinceEpoch();

    if (now - lastResetTime > 1000) {
        batchCount = 0;
        lastResetTime = now;
    }

    if (batchCount >= MAX_BATCH_COMMANDS_PER_SECOND) {
        qDebug() << "[BATCH] Rate limit:" << batchCount << "commands/sec";
        return false;
    }

    batchCount++;
    return true;
}

void BatchCommandSender::resetBatchRate()
{
    batchCount = 0;
    lastResetTime = QDateTime::currentMSecsSinceEpoch();
}

void BatchCommandSender::onBatchComplete()
{
    qDebug() << "[BATCH] Manual batch completion signal";

    if (!currentCommand.command.isEmpty()) {
        socket->write(" \n");
        socket->flush();
        currentCommand.command.clear();
        currentCommand.duration = 0;
    }

    batchQueue.clear();
    batchTimer->stop();
    completionTimer->stop();

    emit batchCompleted();
}

void BatchCommandSender::onConnected()
{
    qDebug() << "[BATCH] Connected to robot!";
    emit connected();

    batchQueue.clear();
    currentCommand.command.clear();
    currentCommand.duration = 0;
    resetBatchRate();
}

void BatchCommandSender::onDisconnected()
{
    qDebug() << "[BATCH] Disconnected from robot";
    batchTimer->stop();
    completionTimer->stop();
    batchQueue.clear();
    currentCommand.command.clear();
    currentCommand.duration = 0;
    emit disconnected();
}

void BatchCommandSender::onError(QAbstractSocket::SocketError)
{
    QString errorString = socket->errorString();
    qDebug() << "[BATCH] Socket error:" << errorString;
    emit errorOccurred(errorString);

    batchTimer->stop();
    completionTimer->stop();
    batchQueue.clear();
    currentCommand.command.clear();
    currentCommand.duration = 0;
}
