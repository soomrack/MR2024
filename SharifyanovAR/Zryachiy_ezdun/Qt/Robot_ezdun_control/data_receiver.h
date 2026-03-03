#pragma once

#include <QObject>
#include <QUdpSocket>
#include "config.h"

class DataReceiver : public QObject
{
    Q_OBJECT

public:
    explicit DataReceiver(QObject *parent = nullptr);
    bool start(quint16 port = Config::DATA_PORT_UDP);
    void setAllowedAddress(const QString& address);

signals:
    void sensorDataReceived(const QString& data);

private slots:
    void onReadyRead();

private:
    QUdpSocket* m_socket;
    QHostAddress m_allowedAddress;
    bool m_filterByAddress;
};
