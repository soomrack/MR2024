#ifndef BATCH_COMMAND_SENDER_H
#define BATCH_COMMAND_SENDER_H

#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QString>
#include <QElapsedTimer>
#include <QDateTime>
#include <QQueue>

#include "config.h"
#include "command_logger.h"

class BatchCommandSender : public QObject
{
    Q_OBJECT

public:
    explicit BatchCommandSender(QObject *parent = nullptr);
    ~BatchCommandSender();

    void connectToRobot();
    void disconnectFromRobot();
    bool isConnected() const;

    // ПАКЕТНЫЕ КОМАНДЫ
    void sendBatchCommand(const QString& batchCommand);
    void sendSimpleBatch(const QString& direction, int seconds);
    void sendForward(int seconds) { sendSimpleBatch("Forward", seconds); }
    void sendBackward(int seconds) { sendSimpleBatch("Backward", seconds); }
    void sendLeft(int seconds) { sendSimpleBatch("Left", seconds); }
    void sendRight(int seconds) { sendSimpleBatch("Right", seconds); }
    void sendStop(int seconds) { sendSimpleBatch("Stop", seconds); }

public slots:
    void onBatchComplete();

signals:
    void connected();
    void disconnected();
    void errorOccurred(QString error);
    void batchCommandSent(QString command);
    void batchCompleted();

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);
    void executeNextBatchCommand();
    void checkCommandCompletion();

private:
    struct QueuedCommand {
        QString command;
        int duration;  // в секундах
    };

    QString formatBatchCommand(const QString& direction, int seconds);
    int extractDuration(const QString& command);
    bool checkBatchRate();
    void resetBatchRate();

    QTcpSocket *socket;

    // Очередь пакетных команд
    QQueue<QueuedCommand> batchQueue;
    QueuedCommand currentCommand;
    QTimer *batchTimer;
    QTimer *completionTimer;
    QElapsedTimer executionTimer;

    // Rate limiting
    QElapsedTimer batchTimerControl;
    int batchCount;
    qint64 lastResetTime;

    static const int BATCH_RATE_LIMIT_MS = 1000;
    static const int MAX_BATCH_COMMANDS_PER_SECOND = 2;
    static const int BATCH_EXECUTION_DELAY_MS = 500;
    static const int COMPLETION_CHECK_INTERVAL_MS = 100;
};

#endif // BATCH_COMMAND_SENDER_H
