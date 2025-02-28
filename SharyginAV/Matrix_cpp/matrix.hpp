#pragma once

#include <iostream>
#include <vector>
#include <exception>

using namespace std;

class MatrixException : public std::exception
{
public:
    enum Exceptions {
        PARAMS_ERR,
        SIZE_ERR
    };

    MatrixException(const enum Exceptions code) : m_code(code) {}

    const char* what() const noexcept override
    {
        switch(m_code)
        {
            case PARAMS_ERR:
                return "Input parametrs error";
                break;
            case SIZE_ERR:
                return "Matrix wrong size";
                break;
            default:
                return nullptr;
        }
    }

    enum Exceptions get_code() const noexcept
    {
        return m_code;
    }

private:
    enum Exceptions m_code;
};

class Matrix
{
public:
enum LogLevel {
    LOG_NONE,
    LOG_ERR,
    LOG_WARN,
    LOG_INFO
};

    Matrix(size_t rows, size_t cols);

    Matrix();

    Matrix(const Matrix &M);  // конструктор копирования

    ~Matrix();

    double& operator()(const size_t row_num,const size_t col_num);

    const double& operator()(const size_t row_num, const size_t col_num) const;

    Matrix operator+(const Matrix &A) const;

    Matrix operator-(const Matrix &A) const;

    Matrix& operator+=(const Matrix &A);

    Matrix& operator-=(const Matrix &A);

    Matrix& operator=(const Matrix &A);

    Matrix operator*(const double num);
    
    Matrix operator*(const Matrix &A);

    Matrix& operator*=(const double num);

    Matrix& operator*=(const Matrix &A);

    void print() const noexcept;

    bool equal_size(const Matrix &A) const;

    bool is_square() const;

    void set_identity();

    Matrix transpos();

    Matrix pow(const unsigned int pow) const;

    Matrix exp(const unsigned int iretations) const;

    void get_size();

    size_t find_non_zero_in_col(const size_t idx_start) const;

    void swap_rows(const size_t row_1, const size_t row_2);

    void sub_row(const size_t row, const size_t row_base, const double ratio);

    double det() const;

    static void set_log_level(LogLevel level) noexcept;

private:
    static LogLevel s_current_level;
    void print_log(LogLevel level, const char* format, ...) const noexcept;

    size_t rows;
    size_t cols;
    std::vector<double> data;
};
