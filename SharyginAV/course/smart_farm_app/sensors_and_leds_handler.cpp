#include "sensors_and_leds_handler.h"
#include <QDebug>

Sensors_and_leds_handler::Sensors_and_leds_handler(QObject *parent)
    : QObject(parent), serial(new QSerialPort(this)), m_temperature(0), m_humidity(0), m_humidity_dirt(0), m_time(0)
{
    connect(serial, &QSerialPort::readyRead, this, &Sensors_and_leds_handler::readData);
}


Sensors_and_leds_handler::~Sensors_and_leds_handler()
{
    if(serial->isOpen()) {
        serial->close();
    }
}


int Sensors_and_leds_handler::get_temperature() const
{
    return m_temperature;
}


int Sensors_and_leds_handler::get_humidity() const
{
    return m_humidity;
}


int Sensors_and_leds_handler::get_humidity_dirt() const
{
    return m_humidity_dirt;
}


int Sensors_and_leds_handler::get_time() const
{
    return m_time;
}


void Sensors_and_leds_handler::startReading(const QString &portName)
{
    serial->setPortName(portName);
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (serial->open(QIODevice::ReadWrite)) {
        qDebug() << "Порт успешно открыт!";
    } else {
        qDebug() << "Ошибка открытия порта:" << serial->errorString();
    }
}


void Sensors_and_leds_handler::sendCommand(const QString &command)
{
    if(serial->isOpen()) {
        QByteArray data = command.toUtf8() + "\n";
        serial->write(data);
        qDebug() << "Отправлена команда" << command;
    }
    else {
        qDebug() << "Порт не открыт, команда не отправлена.";
    }
}


void Sensors_and_leds_handler::readData()
{
    buffer += serial->readAll();

    int pos = buffer.indexOf('\n');
    if(pos != -1) {
        QByteArray line = buffer.left(pos).trimmed();
        buffer = buffer.mid(pos + 1);

        qDebug() << "Полученная строка: " << line;

        QList<QByteArray> values = line.split(',');
        if(values.size() == 4) {
            m_time = values[0].toInt();
            m_temperature = values[1].toInt();
            m_humidity = values[2].toInt();
            m_humidity_dirt = values[3].toInt();
            emit sensorValuesChanged();
        }
        else {
            qDebug() << "Ошибка: получено неверное количество значений.";
        }
    }
}
