#ifndef MATRIX_H
#define MATRIX_H

#include <cstring>
#include <exception>

// Раскомментируйте строку ниже, чтобы включить вызовы print_log
#define MATRIX_LOG_ENABLE

class MatrixException : public std::exception {
public:
    enum Exceptions {
        PARAMS_ERR,
        SIZE_ERR
    };

    MatrixException(const enum Exceptions code) : exception_code(code) {}

    const char* what() const noexcept override {
        switch(exception_code) {
        case PARAMS_ERR:
            return "Input parameters error";
        case SIZE_ERR:
            return "Matrix wrong size";
        default:
            return nullptr;
        }
    }

    enum Exceptions get_code() const noexcept {
        return exception_code;
    }

private:
    enum Exceptions exception_code;
};

class Matrix {
public:
    enum LogLevel {
        LOG_NONE,
        LOG_ERR,
        LOG_WARN,
        LOG_INFO
    };

private:
    static LogLevel current_log_level;
    size_t row_count;
    size_t col_count;
    double* data_array;

public:
    Matrix();
    Matrix(const size_t row_num, const size_t col_num);
    Matrix(const Matrix &source_matrix);
    Matrix(Matrix &&source_matrix);
    ~Matrix();

    Matrix& operator=(const Matrix &source_matrix);
    Matrix& operator=(Matrix &&source_matrix);
    Matrix operator-() const;
    Matrix operator+(const Matrix &second_matrix) const;
    Matrix& operator+=(const Matrix &source_matrix);
    Matrix operator-(const Matrix &second_matrix) const;
    Matrix& operator-=(const Matrix &source_matrix);
    Matrix operator*(const double number) const noexcept;
    Matrix& operator*=(const double number) noexcept;
    Matrix operator*(const Matrix &second_matrix) const;
    Matrix& operator*=(const Matrix &second_matrix);

    void resize(const size_t row_num, const size_t col_num);
    void print() const noexcept;
    size_t get_rows() const noexcept;
    size_t get_cols() const noexcept;
    void set(const size_t row_index, const size_t col_index, const double number);
    double get(const size_t row_index, const size_t col_index) const;
    bool is_empty() const noexcept;
    bool is_square() const noexcept;
    bool equal_size(const Matrix& second_matrix) const noexcept;
    void set_zeros() noexcept;
    void set_identity();
    Matrix pow(const unsigned int power) const;
    Matrix exp(const unsigned int iterations = 10) const;
    Matrix transp() const;
    double det() const;

    static void set_log_level(LogLevel level) noexcept;

private:
    void print_log(LogLevel level, const char *format, ...) const noexcept;
    void alloc(const size_t row_num, const size_t col_num);
    void free();
    size_t find_non_zero_in_col(const size_t start_index) const noexcept;
    void swap_rows(const size_t first_row, const size_t second_row) noexcept;
    void sub_row(const size_t row_index, const size_t base_row_index, const double ratio) noexcept;
};

#endif // MATRIX_H

