#ifndef BATCH_COMMAND_SENDER_H
#define BATCH_COMMAND_SENDER_H

#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QString>
#include <QElapsedTimer>
#include <QDateTime>

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

    // НОВЫЕ МЕТОДЫ ДЛЯ ПАКЕТНЫХ КОМАНД
    void sendBatchCommand(const QString& batchCommand);
    void sendSimpleBatch(const QString& direction, int seconds);
    void sendForward(int seconds) { sendSimpleBatch("Forward", seconds); }
    void sendBackward(int seconds) { sendSimpleBatch("Backward", seconds); }
    void sendLeft(int seconds) { sendSimpleBatch("Left", seconds); }
    void sendRight(int seconds) { sendSimpleBatch("Right", seconds); }
    void sendStop(int seconds) { sendSimpleBatch("Stop", seconds); }

public slots:
    void onBatchComplete();  // Вызывается когда пакет завершился

signals:
    void connected();
    void disconnected();
    void errorOccurred(QString error);
    void batchCommandSent(QString command);
    void batchCompleted();  // Сигнал завершения пакетной команды

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);
    void executeNextBatchCommand();

private:
    QString formatBatchCommand(const QString& direction, int seconds);
    bool checkBatchRate();
    void resetBatchRate();

    QTcpSocket *socket;

    // Очередь пакетных команд
    QStringList batchQueue;
    int currentBatchIndex;
    QTimer *batchTimer;

    // Rate limiting
    QElapsedTimer batchTimerControl;
    int batchCount;
    qint64 lastResetTime;

    static const int BATCH_RATE_LIMIT_MS = 1000;  // 1 команда в секунду
    static const int MAX_BATCH_COMMANDS_PER_SECOND = 1;
    static const int BATCH_EXECUTION_DELAY_MS = 200;
};

#endif // BATCH_COMMAND_SENDER_H
