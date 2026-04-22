#include "command_sender.h"
#include <QDebug>

CommandSender::CommandSender(const QString &host, quint16 port, QObject *parent)
    : QObject(parent), m_host(host), m_port(port)
{
    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::connected, this, &CommandSender::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &CommandSender::onDisconnected);
    connect(m_socket, &QAbstractSocket::errorOccurred, this, &CommandSender::onError);

    m_reconnectTimer = new QTimer(this);
    m_reconnectTimer->setInterval(3000);
    connect(m_reconnectTimer, &QTimer::timeout, this, &CommandSender::tryReconnect);

    tryReconnect();
}

void CommandSender::tryReconnect()
{
    if (m_socket->state() == QAbstractSocket::UnconnectedState)
        m_socket->connectToHost(m_host, m_port);
}

void CommandSender::onConnected()
{
    m_reconnectTimer->stop();
    emit connected();
    emit statusChanged("Подключён к роботу");
}

void CommandSender::onDisconnected()
{
    emit disconnected();
    emit statusChanged("Отключён от робота");
    m_reconnectTimer->start();
}

void CommandSender::onError(QAbstractSocket::SocketError)
{
    emit statusChanged("Ошибка: " + m_socket->errorString());
    if (!m_reconnectTimer->isActive())
        m_reconnectTimer->start();
}

bool CommandSender::isConnected() const
{
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

void CommandSender::sendCommand(const QString &command)
{
    if (!isConnected())
        return;
    char cmd = mapCommand(command);
    if (cmd != '\0')
    {
        m_socket->write(&cmd, 1);
        m_socket->flush();
    }
}

void CommandSender::sendBatch(const QString &batchStr)
{
    if (!isConnected())
        return;
    QByteArray data = batchStr.toUtf8() + "\n";
    m_socket->write(data);
    m_socket->flush();
}

char CommandSender::mapCommand(const QString &command) const
{
    if (command == "Forward")
        return 'W';
    if (command == "Back")
        return 'S';
    if (command == "Left")
        return 'A';
    if (command == "Right")
        return 'D';
    if (command == "Stop")
        return 'X';
    if (command == "Lost")
        return 'L';
    if (command == "Quit")
        return 'Q';
    return '\0';
}
