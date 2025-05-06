#ifndef SENSORS_AND_LEDS_HANDLER_H
#define SENSORS_AND_LEDS_HANDLER_H

#include <QObject>
#include <QSerialPort>
#include <QByteArray>

class Sensors_and_leds_handler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int time READ get_time NOTIFY sensorValuesChanged)
    Q_PROPERTY(int temperature READ get_temperature NOTIFY sensorValuesChanged)
    Q_PROPERTY(int humidity READ get_humidity NOTIFY sensorValuesChanged)
    Q_PROPERTY(int humidity_dirt READ get_humidity_dirt NOTIFY sensorValuesChanged)
public:
    explicit Sensors_and_leds_handler(QObject *parent = nullptr);
    ~Sensors_and_leds_handler();

    int get_time() const;
    int get_temperature() const;
    int get_humidity() const;
    int get_humidity_dirt() const;

    Q_INVOKABLE void startReading(const QString &portName);
    Q_INVOKABLE void sendCommand(const QString &command);

signals:
    void sensorValuesChanged();

private slots:
    void readData();

private:
    QSerialPort *serial;
    QByteArray buffer;
    int m_time;
    int m_temperature;
    int m_humidity;
    int m_humidity_dirt;
};

#endif // SENSORS_AND_LEDS_HANDLER_H
