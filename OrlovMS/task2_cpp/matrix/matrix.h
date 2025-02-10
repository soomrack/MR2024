#pragma once
#include <cstring>
#include <ostream>

// uncomment line below to enable print_log calls
#define MATRIX_LOG_ENABLE

class Matrix
{
public:
    enum LogLevel {
        LOG_NONE,
        LOG_ERR,
        LOG_WARN,
        LOG_INFO
    };
private:
    static LogLevel current_level;

    size_t rows;
    size_t cols;
    double* data;

    void print_log(LogLevel level, const char *format, ...);

    void alloc(const size_t rows, const size_t cols);
    void free();
public:
    static void set_log_level(LogLevel level);

    Matrix();
    Matrix(const size_t rows, const size_t cols);
    Matrix(const Matrix &mat);
    ~Matrix();

    size_t get_rows();
    size_t get_cols();

    void set(const size_t row, const size_t col, const double num);
    double get(const size_t row, const size_t col);

    bool is_empty();
    bool is_square();

    void set_zeros();
    void set_identity();
};

std::ostream& operator<<(std::ostream& stream, Matrix &mat);
