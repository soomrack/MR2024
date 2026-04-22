#include "sensor_receiver.h"
#include <QDebug>

SensorReceiver::SensorReceiver(quint16 port, QObject *parent)
    : QObject(parent)
{
    m_socket = new QUdpSocket(this);
    m_socket->bind(QHostAddress::AnyIPv4, port);
    connect(m_socket, &QUdpSocket::readyRead, this, &SensorReceiver::onReadyRead);
    qDebug() << "Distance sensor log: Listening on UDP port" << port;
}

void SensorReceiver::onReadyRead()
{
    while (m_socket->hasPendingDatagrams())
    {
        QByteArray data;
        data.resize(m_socket->pendingDatagramSize());
        m_socket->readDatagram(data.data(), data.size());

        QString str = QString::fromUtf8(data).trimmed();
        for (const QString &line : str.split('\n', Qt::SkipEmptyParts))
            parseLine(line.trimmed());
    }
}

void SensorReceiver::parseLine(const QString &line)
{
    emit rawData(line);

    if (line.contains("Distance:"))
    {
        int start = line.indexOf("Distance:") + 9;
        int end = line.indexOf("cm", start);
        if (end > start)
        {
            bool ok;
            float dist = line.mid(start, end - start).toFloat(&ok);
            if (ok)
            {
                bool blocked = line.contains("Blocked");
                emit distanceUpdated(dist, blocked);
            }
        }
    }
}
