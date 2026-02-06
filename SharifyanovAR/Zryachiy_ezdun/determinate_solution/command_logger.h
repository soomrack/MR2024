#pragma once
#ifndef COMMAND_LOGGER_H
#define COMMAND_LOGGER_H

#include <fstream>
#include <chrono>
#include <iomanip>
#include <mutex>
#include <ctime>
#include <sstream>
#include <string>

class CommandLogger {
private:
    static std::ofstream& getLogFile() {
        static std::ofstream logFile(createFileName(), std::ios::out);
        return logFile;
    }

    static std::string createFileName() {
        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);

        std::tm localTime{};
        localtime_s(&localTime, &time);

        std::ostringstream name;
        name << "command_log_"
            << std::put_time(&localTime, "%Y-%m-%d_%H-%M-%S")
            << ".csv";

        return name.str();
    }

public:
    static void log(char command) {
        static std::mutex logMutex;
        std::lock_guard<std::mutex> lock(logMutex);

        auto& logFile = getLogFile();
        if (!logFile.is_open())
            return;

        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);

        std::tm localTime{};
        localtime_s(&localTime, &time);

        logFile
            << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S")
            << "," << command << "\n";

        logFile.flush();
    }
};

#endif // COMMAND_LOGGER_H
