#ifndef SENSOR_RECEIVER_H
#define SENSOR_RECEIVER_H

#include <QObject>
#include <QUdpSocket>

class SensorReceiver : public QObject
{
    Q_OBJECT
public:
    explicit SensorReceiver(quint16 port, QObject *parent = nullptr);

signals:
    void distanceUpdated(float cm, bool blocked);
    void rawData(const QString &line);

private slots:
    void onReadyRead();

private:
    QUdpSocket *m_socket;
    void parseLine(const QString &line);
};

#endif // SENSOR_RECEIVER_H
