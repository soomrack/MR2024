#pragma once
#ifndef COMMAND_LOGGER_H
#define COMMAND_LOGGER_H

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>
#include <QStandardPaths>
#include <QMutex>

class CommandLogger
{
public:
    static void log(char command)
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);

        QFile& file = getLogFile();
        if (!file.isOpen())
            return;

        QTextStream out(&file);

        QString timestamp = QDateTime::currentDateTime()
                                .toString("yyyy-MM-dd HH:mm:ss");

        out << timestamp << "," << command << "\n";
        out.flush();
    }

private:
    static QFile& getLogFile()
    {
        static QFile logFile(createFilePath());

        if (!logFile.isOpen())
        {
            logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
        }

        return logFile;
    }

    static QString createFilePath()
    {
        QString basePath = QStandardPaths::writableLocation(
            QStandardPaths::DocumentsLocation);

        QDir dir(basePath + "/RobotLogs");

        if (!dir.exists())
            dir.mkpath(".");

        QString fileName =
            "command_log_" +
            QDateTime::currentDateTime()
                .toString("yyyy-MM-dd_HH-mm-ss") +
            ".csv";

        return dir.filePath(fileName);
    }
};

#endif // COMMAND_LOGGER_H

