#include "data_receiver.h"

DataReceiver::DataReceiver(QObject *parent)
    : QObject(parent),
    m_socket(new QUdpSocket(this)),
    m_filterByAddress(false)
{
    connect(m_socket, &QUdpSocket::readyRead,
            this, &DataReceiver::onReadyRead);
}

bool DataReceiver::start(quint16 port)
{
    return m_socket->bind(QHostAddress::Any, port);
}

void DataReceiver::setAllowedAddress(const QString& address)
{
    if (!address.isEmpty()) {
        m_allowedAddress = QHostAddress(address);
        m_filterByAddress = true;
    } else {
        m_filterByAddress = false;
    }
}

void DataReceiver::onReadyRead()
{
    while (m_socket->hasPendingDatagrams())
    {
        QByteArray datagram;
        QHostAddress senderAddress;
        quint16 senderPort;

        datagram.resize(m_socket->pendingDatagramSize());

        m_socket->readDatagram(datagram.data(), datagram.size(),
                               &senderAddress, &senderPort);

        // Проверяем, если включена фильтрация по IP
        if (m_filterByAddress && senderAddress != m_allowedAddress) {
            // Игнорируем пакеты с других адресов
            continue;
        }

        emit sensorDataReceived(QString::fromUtf8(datagram).trimmed());
    }
}
