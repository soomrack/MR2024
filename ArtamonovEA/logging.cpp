#pragma once

#include <iostream>


enum LogLevel {
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};


// ----------------- Exceptions ----------------- //


class MatrixException : public std::exception {
    private:
        std::string message;
    public:
        MatrixException(const std::string &message): message(message) {}
        const char* what() const noexcept override {
            return message.c_str();
        }
};

class SizeMismatchException : public MatrixException {
    public:
        SizeMismatchException(): MatrixException("Matrices of unequal sizes.") {}
        SizeMismatchException(const std::string &message): MatrixException(message) {}
};

class DivisionByZeroException : public MatrixException {
    public:
        DivisionByZeroException(): MatrixException("Encountered division by zero.") {}
};

class UndefinedOperationException : public MatrixException {
    public:
        UndefinedOperationException(): MatrixException("Undefined operation.") {}
};


// ------------------ Logger ------------------- //


class Logger {
    private:
        enum LogLevel level;
    public:
        Logger(): level(INFO) {}
        Logger(enum LogLevel level): level(level) {}

        void set_level(enum LogLevel new_level) { level = new_level; }
        LogLevel get_level() const { return level; }
        bool match_level(const enum LogLevel &message_level) const noexcept;

        void log(const std::string &message, const enum LogLevel &level) const noexcept;

        void info(const std::string &message) const noexcept { log(message, INFO); }
        void warning(const std::string &message) const noexcept{ log(message, WARNING); }
        void error(const std::string &message) const noexcept { log(message, ERROR); }
};


bool Logger::match_level(const enum LogLevel &message_level) const noexcept
{
    return message_level >= level;
}


void Logger::log(const std::string &message, const enum LogLevel &level) const noexcept
{
    if (!match_level(level)) {
        return;
    }
    switch (level) {
        case INFO:
            std::cout << "[INFO] " << message << std::endl;
            break;
        case WARNING:
            std::cout << "[WARNING] " << message << std::endl;
            break;
        case ERROR:
            std::cerr << "[ERROR] " << message << std::endl;
            break;
        case CRITICAL:
            std::cerr << "[CRITICAL] " << message << std::endl;
            break;
    }
}


Logger get_logger() { return Logger(); }
