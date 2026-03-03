#include "batch_command_sender.h"
#include "command_logger.h"
#include <QtCore>
#include <QDebug>

BatchCommandSender::BatchCommandSender(QObject *parent)
    : QObject(parent),
    socket(new QTcpSocket(this)),
    currentBatchIndex(0),
    batchCount(0),
    lastResetTime(0)
{
    connect(socket, &QTcpSocket::connected,
            this, &BatchCommandSender::onConnected);

    connect(socket, &QTcpSocket::disconnected,
            this, &BatchCommandSender::onDisconnected);

    connect(socket, &QTcpSocket::errorOccurred,
            this, &BatchCommandSender::onError);

    batchTimer = new QTimer(this);
    batchTimer->setInterval(BATCH_EXECUTION_DELAY_MS);
    batchTimer->setSingleShot(true);
    connect(batchTimer, &QTimer::timeout,
            this, &BatchCommandSender::executeNextBatchCommand);

    batchTimerControl.start();
    lastResetTime = QDateTime::currentMSecsSinceEpoch();
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
        socket->disconnectFromHost();
    }

    batchQueue.clear();
    currentBatchIndex = 0;
    batchCount = 0;
}

bool BatchCommandSender::isConnected() const
{
    return socket->state() == QAbstractSocket::ConnectedState;
}

void BatchCommandSender::sendBatchCommand(const QString& batchCommand)
{
    if (!isConnected()) {
        qDebug() << "[BATCH] Cannot send - not connected!";
        emit errorOccurred("Not connected to robot");
        return;
    }

    if (!checkBatchRate()) {
        qDebug() << "[BATCH] Rate limit exceeded";
        return;
    }

    qDebug() << "[BATCH] Queueing:" << batchCommand;

    // Добавляем в очередь
    batchQueue.append(batchCommand);

    // Если очередь пуста была - запускаем выполнение
    if (batchQueue.size() == 1) {
        batchTimer->start();
    }

    emit batchCommandSent(batchCommand);
    //CommandLogger::log(QString("BATCH: %1").arg(batchCommand).toStdString().c_str());
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
    if (batchQueue.isEmpty() || currentBatchIndex >= batchQueue.size()) {
        qDebug() << "[BATCH] Queue completed";
        currentBatchIndex = 0;
        emit batchCompleted();
        return;
    }

    QString batchCommand = batchQueue[currentBatchIndex];

    qDebug() << "[BATCH] Executing:" << batchCommand
             << "(" << (currentBatchIndex + 1) << "/" << batchQueue.size() << ")";

    QByteArray data = batchCommand.toUtf8() + "\n";
    socket->write(data);

    // Переходим к следующей команде
    currentBatchIndex++;

    // Если есть еще команды - продолжаем
    if (currentBatchIndex < batchQueue.size()) {
        batchTimer->start();
    } else {
        // Очередь завершена
        currentBatchIndex = 0;
        emit batchCompleted();
        qDebug() << "[BATCH] All commands executed!";
    }
}

bool BatchCommandSender::checkBatchRate()
{
    qint64 now = QDateTime::currentMSecsSinceEpoch();

    if (now - lastResetTime > 1000) {
        batchCount = 0;
        lastResetTime = now;
    }

    batchCount++;

    if (batchCount > MAX_BATCH_COMMANDS_PER_SECOND) {
        qDebug() << "[BATCH] Rate limit:" << batchCount << "commands/sec";
        return false;
    }

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
    emit batchCompleted();
}

void BatchCommandSender::onConnected()
{
    qDebug() << "[BATCH] Connected to robot!";
    emit connected();

    // Очищаем очередь при подключении
    batchQueue.clear();
    currentBatchIndex = 0;
    resetBatchRate();
}

void BatchCommandSender::onDisconnected()
{
    qDebug() << "[BATCH] Disconnected from robot";
    batchTimer->stop();
    batchQueue.clear();
    currentBatchIndex = 0;
    emit disconnected();
}

void BatchCommandSender::onError(QAbstractSocket::SocketError)
{
    QString errorString = socket->errorString();
    qDebug() << "[BATCH] Socket error:" << errorString;
    emit errorOccurred(errorString);
    batchQueue.clear();
    currentBatchIndex = 0;
}
