#ifndef COMMAND_SENDER_H
#define COMMAND_SENDER_H

#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QString>
#include <QSet>
#include <QElapsedTimer>
#include <QDateTime>

#include "config.h"
#include "command_logger.h"

class CommandSender : public QObject
{
    Q_OBJECT

public:
    explicit CommandSender(QObject *parent = nullptr);
    ~CommandSender();

    void connectToRobot();
    void disconnectFromRobot();
    void sendCommand(char command);
    void stopRobot();

    bool isConnected() const;

public slots:
    void onKeyPressed(int key);
    void onKeyReleased(int key);

signals:
    void connected();
    void disconnected();
    void errorOccurred(QString error);
    void commandSent(char command);

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);
    void processMovement();
    void sendStopCommand();

private:
    bool checkCommandRate();
    void resetCommandRate();

    QTcpSocket *socket;
    char lastCommand;
    char pendingCommand;
    int currentKey;

    QTimer *movementTimer;
    QTimer *stopDelayTimer;
    QElapsedTimer commandTimer;

    qint64 lastSentTime;
    int commandCount;
    qint64 lastResetTime;
    qint64 lastPressTime;

    static const int COMMAND_DELAY_MS = 200;
    static const int MOVEMENT_INTERVAL_MS = 100;
    static const int REPEAT_COMMAND_INTERVAL_MS = 250;
    static const int MAX_COMMANDS_PER_SECOND = 15;
};

#endif // COMMAND_SENDER_H
