#pragma once
#include <cstring>
#include <ostream>

// uncomment line below to enable print_log calls
#define MATRIX_LOG_ENABLE

class Matrix
{
public:
    enum Exceptions {
        ALLOC_ERR,
        PARAMS_ERR,
        SIZE_ERR
    };

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
private:
    void print_log(LogLevel level, const char *format, ...) const noexcept;

    void alloc(const size_t rows, const size_t cols);
    void free();
public:
    static void set_log_level(LogLevel level) noexcept;

    Matrix();
    Matrix(const size_t rows, const size_t cols);
    Matrix(const Matrix &mat);
    ~Matrix();

    void print() const noexcept;

    size_t get_rows() const noexcept;
    size_t get_cols() const noexcept;

    void set(const size_t row, const size_t col, const double num);
    double get(const size_t row, const size_t col) const;

    bool is_empty() const noexcept;
    bool is_square() const noexcept;

    void set_zeros() noexcept;
    void set_identity();
};
