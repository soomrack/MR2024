#pragma once
#include <fstream>
#include <iomanip>
#include <sstream>
#include <windows.h>
#include <string>
#include <iostream>

class DataLogger {
public:

    bool initialize() {

        SYSTEMTIME st;
        GetLocalTime(&st);

        std::stringstream ss;

        ss << "C:\\Video_streamer\\Video_streamer\\sensor_logs\\sensor_log_"
            << st.wYear << "-"
            << std::setw(2) << std::setfill('0') << st.wMonth << "-"
            << std::setw(2) << st.wDay << "_"
            << std::setw(2) << st.wHour << "-"
            << std::setw(2) << st.wMinute << "-"
            << std::setw(2) << st.wSecond
            << ".csv";

        m_fullPath = ss.str();

        m_file.open(m_fullPath, std::ios::out);

        if (!m_file.is_open()) {
            std::cerr << "Logger: cannot create file:\n"
                << m_fullPath << "\n";
            return false;
        }

        std::cout << "-------------------------------------\n";
        std::cout << "Logger initialized\n";
        std::cout << "File: " << m_fullPath << "\n";
        std::cout << "-------------------------------------\n";

        m_file << "Date,Time,Milliseconds,Data\n";

        return true;
    }

    void log(const char* data) {

        if (!m_file.is_open() || !data)
            return;

        SYSTEMTIME st;
        GetLocalTime(&st);

        m_file
            << st.wYear << "-"
            << std::setw(2) << std::setfill('0') << st.wMonth << "-"
            << std::setw(2) << st.wDay << ","
            << std::setw(2) << st.wHour << ":"
            << std::setw(2) << st.wMinute << ":"
            << std::setw(2) << st.wSecond << ","
            << st.wMilliseconds << ","
            << data;

        if (data[strlen(data) - 1] != '\n')
            m_file << "\n";
    }

    void close() {

        if (m_file.is_open()) {
            m_file.flush();
            m_file.close();

            std::cout << "Logger closed\n";
            std::cout << "Saved to: " << m_fullPath << "\n";
        }
    }

private:
    std::ofstream m_file;
    std::string   m_fullPath;
};
