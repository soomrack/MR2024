#pragma once

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QMutex>

class DataLogger : public QObject
{
    Q_OBJECT

public:
    explicit DataLogger(QObject *parent = nullptr);
    ~DataLogger();

    void logCommand(const QString& cmd);
    void logSensor(const QString& data);
    void logSystem(const QString& msg);

signals:
    void newLogMessage(const QString& msg);

private:
    void writeToFile(const QString& msg);

    QFile m_file;
    QTextStream m_stream;
    QMutex m_mutex;
};
