#ifndef COMMAND_SENDER_H
#define COMMAND_SENDER_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>

class CommandSender : public QObject
{
    Q_OBJECT
public:
    explicit CommandSender(const QString &host, quint16 port, QObject *parent = nullptr);
    void sendCommand(const QString &command);
    void sendBatch(const QString &batchStr);
    bool isConnected() const;

signals:
    void connected();
    void disconnected();
    void statusChanged(const QString &status);

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);
    void tryReconnect();

private:
    QTcpSocket *m_socket;
    QTimer     *m_reconnectTimer;
    QString     m_host;
    quint16     m_port;
    char mapCommand(const QString &command) const;
};

#endif // COMMAND_SENDER_H
