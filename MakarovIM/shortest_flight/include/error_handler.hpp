#pragma once

#include <stdexcept>
#include <string>

class InternalError : public std::runtime_error {
public:
    explicit InternalError(const std::string& msg)
        : std::runtime_error(msg) {}
};

class DataError : public InternalError {
public:
    explicit DataError(const std::string& msg)
        : InternalError("DataError: " + msg) {}
};

class FileError : public InternalError {
public:
    explicit FileError(const std::string& msg)
        : InternalError("FileError: " + msg) {}
};

class GraphError : public InternalError {
public:
    explicit GraphError(const std::string& msg)
        : InternalError("GraphError: " + msg) {}
};

class CSVParsingError : public InternalError {
public:
    explicit CSVParsingError(const std::string& msg)
        : InternalError("CSVParsingError: " + msg) {}
};