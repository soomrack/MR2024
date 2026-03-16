#include "data_logger.h"
#include <QDateTime>
#include <QMutexLocker>

DataLogger::DataLogger(QObject *parent)
    : QObject(parent),
    m_file("robot_log.txt")
{
    if (m_file.open(QIODevice::Append | QIODevice::Text))
        m_stream.setDevice(&m_file);
}

DataLogger::~DataLogger()
{
    m_file.close();
}

void DataLogger::logCommand(const QString &cmd)
{
    QString msg = QDateTime::currentDateTime()
    .toString("[hh:mm:ss] CMD: ") + cmd;

    writeToFile(msg);
    emit newLogMessage(msg);
}

void DataLogger::logSensor(const QString &data)
{
    QString msg = QDateTime::currentDateTime()
    .toString("[hh:mm:ss] SENSOR: ") + data;

    writeToFile(msg);
    emit newLogMessage(msg);
}

void DataLogger::logSystem(const QString &msg)
{
    QString full = QDateTime::currentDateTime()
    .toString("[hh:mm:ss] SYS: ") + msg;

    writeToFile(full);
    emit newLogMessage(full);
}

void DataLogger::writeToFile(const QString &msg)
{
    QMutexLocker locker(&m_mutex);
    if (m_file.isOpen())
    {
        m_stream << msg << "\n";
        m_stream.flush();
    }
}
