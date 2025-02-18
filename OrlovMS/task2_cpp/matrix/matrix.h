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
    static LogLevel s_current_level;

    size_t m_rows;
    size_t m_cols;
    double* m_data;
public:
    Matrix();
    Matrix(const size_t rows, const size_t cols);
    Matrix(const Matrix &mat);
    Matrix(Matrix &&mat);
    ~Matrix();

    Matrix& operator=(const Matrix &mat);
    Matrix& operator=(Matrix &&mat);
    Matrix operator-() const;
    Matrix operator+(const Matrix &second) const;
    Matrix& operator+=(const Matrix &mat);
    Matrix operator-(const Matrix &second) const;
    Matrix& operator-=(const Matrix &mat);
    Matrix operator*(const double num) const noexcept;
    Matrix& operator*=(const double num) noexcept;
    Matrix operator*(const Matrix &second) const;
    Matrix& operator*=(const Matrix &second);

    void resize(const size_t rows, const size_t cols);

    void print() const noexcept;

    size_t get_rows() const noexcept;
    size_t get_cols() const noexcept;

    void set(const size_t row, const size_t col, const double num);
    double get(const size_t row, const size_t col) const;

    bool is_empty() const noexcept;
    bool is_square() const noexcept;
    bool equal_size(const Matrix& second) const noexcept;

    void set_zeros() noexcept;
    void set_identity();

    Matrix pow(const unsigned int pow) const;
    Matrix exp(const unsigned int iterations = 10) const;
    Matrix transp() const;
    double det() const;

    static void set_log_level(LogLevel level) noexcept;
private:
    void print_log(LogLevel level, const char *format, ...) const noexcept;
    void alloc(const size_t rows, const size_t cols);
    void free();
    size_t find_non_zero_in_col(const size_t idx_start) const noexcept;
    void swap_rows(const size_t first, const size_t second) noexcept;
    void sub_row(const size_t row, const size_t row_base, const double ratio) noexcept;
};
