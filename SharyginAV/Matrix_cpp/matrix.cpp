#include "matrix.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdarg>


Matrix::LogLevel Matrix::s_current_level = Matrix::LOG_INFO;

MatrixException::MatrixException(const enum Exceptions code) : m_code(code) {}

const char* MatrixException::what() const noexcept
{
    switch(m_code)
    {
        case PARAMS_ERR:
            return "Input parametrs error";
        case SIZE_ERR:
            return "Matrix wrong size";
        default:
            return nullptr;
    }
}

enum MatrixException::Exceptions MatrixException::get_code() const noexcept
{
    return m_code;
}

void Matrix::set_log_level(LogLevel level) noexcept
{
    s_current_level = level;
}


void Matrix::print_log(LogLevel level, const char* format, ...) const noexcept
{
    if(level <= s_current_level) {
        switch(level)
        {
        case LOG_ERR:
            printf("\033[0;31mERROR: \033[0m");
            break;
        case LOG_WARN:
            printf("\033[1;33mWARNING: \033[0m");
            break;
        case LOG_INFO:
            printf("INFO: ");
        default:
            break;
        }
        va_list list;
        va_start(list, format);
        vprintf(format, list);
        va_end(list);
    }
}


Matrix::Matrix(size_t rows, size_t cols) : rows(rows), cols(cols), data(rows * cols, 0.0)
{
    print_log(LOG_INFO, "create matrix %ld x %ld\n", rows, cols);
}


Matrix::Matrix() : rows(0), cols(0)
{
    print_log(LOG_INFO, "create empty matrix\n");
}


Matrix::Matrix(const Matrix &M) : rows(M.rows), cols(M.cols), data(M.data)
{
    print_log(LOG_INFO, "copy matrix\n");
}


Matrix& Matrix::operator=(Matrix&& other) noexcept {
    if(this == &other) return *this;
    rows = other.rows;
    cols = other.cols;
    data = std::move(other.data);
    other.rows = 0;
    other.cols = 0;
    other.data.clear();
    return *this;
}


Matrix::~Matrix()
{
    print_log(LOG_INFO, "matrix delete\n");
}


double& Matrix::operator()(const size_t row_num,const size_t col_num) {
    if(row_num >= rows || col_num >= cols) {
        print_log(LOG_ERR, "index out of range\n");
        throw MatrixException(MatrixException::PARAMS_ERR);
    }
    return data[row_num * cols + col_num];
}


const double& Matrix::operator()(const size_t row_num, const size_t col_num) const {
    if(row_num >= rows || col_num >= cols) {
        print_log(LOG_ERR, "index out of range\n");
        throw MatrixException(MatrixException::PARAMS_ERR);
    }
    return data[row_num * cols + col_num];
}


Matrix Matrix::operator+(const Matrix &A) const
{
    if(!A.equal_size(*this)) {
        print_log(LOG_ERR, "matrices is not equal\n");
        throw MatrixException(MatrixException::SIZE_ERR);
    }

    Matrix result(A.rows, A.cols);
    for(size_t idx = 0; idx < rows * cols; ++idx) {
        result.data[idx] = data[idx] + A.data[idx];
    }

    return result;
}


Matrix Matrix::operator-(const Matrix &A) const
{
    if(!A.equal_size(*this)) {
        print_log(LOG_ERR, "matrices is not equal\n");
        throw MatrixException(MatrixException::SIZE_ERR);
    }

    Matrix result(A.rows, A.cols);
    for(size_t idx = 0; idx < rows * cols; ++idx) {
        result.data[idx] = data[idx] - A.data[idx];
    }

    return result;
}


Matrix& Matrix::operator+=(const Matrix &A)
{
    if(!A.equal_size(*this)) {
        print_log(LOG_ERR, "matrices is not equal\n");
        throw MatrixException(MatrixException::SIZE_ERR);
    }

    for(size_t idx = 0; idx < rows * cols; ++idx) {
        data[idx] += A.data[idx];
    }

    return *this;
}


Matrix& Matrix::operator-=(const Matrix &A)
{
    if(!A.equal_size(*this)) {
        print_log(LOG_ERR, "matrices is not equal\n");
        throw MatrixException(MatrixException::SIZE_ERR);
    }

    for(size_t idx = 0; idx < rows * cols; ++idx) {
        data[idx] -= A.data[idx];
    }

    return *this;
}


Matrix& Matrix::operator=(const Matrix &A)
{
    if(data == A.data) {
        return *this;
    }

    rows = A.rows;
    cols = A.cols;
    data = A.data;

    return *this;
}


Matrix Matrix::operator*(const double num)
{
    Matrix res(*this);

    for(size_t idx = 0; idx < rows * cols; ++idx) {
        res.data[idx] *= num;
    }

    return res;
}


Matrix Matrix::operator*(const Matrix &A)
{
    if(cols != A.rows) {
        print_log(LOG_ERR, "number of columns of first matrix is ​​not equal to number of rows of second matrix\n");
        throw MatrixException(MatrixException::SIZE_ERR);
    }
    Matrix res(rows, A.cols);

    for(size_t row_idx = 0; row_idx < rows; ++row_idx) {
        for(size_t col_idx = 0; col_idx < A.cols; ++col_idx) {
            for(size_t idx = 0; idx < A.rows; ++idx) {
                res(row_idx, col_idx) += (*this)(row_idx, idx) * A(idx, col_idx);
            }
        }
    }

    return res;
}


Matrix& Matrix::operator*=(const double num)
{
    for(size_t idx = 0; idx < rows * cols; ++idx) {
        data[idx] *= num;
    }

    return *this;
}


Matrix& Matrix::operator*=(const Matrix &A)
{
    if(cols != A.rows) {
        print_log(LOG_ERR, "number of columns of first matrix is ​​not equal to number of rows of second matrix\n");
        throw MatrixException(MatrixException::SIZE_ERR);
    }

    *this = (*this) * A;

    return *this;
}


void Matrix::print() const noexcept
{
    for(size_t row_idx = 0; row_idx < rows; ++row_idx) {
        for(size_t col_idx = 0; col_idx < cols; ++col_idx) {
            std::cout << (*this)(row_idx, col_idx);
            col_idx == cols - 1 ? std::cout << '\n' : std::cout << '\t';
        }
    }
}


bool Matrix::equal_size(const Matrix &A) const
{
    return (rows == A.rows) && (cols == A.cols);
}


bool Matrix::is_square() const
{
    return rows == cols;
}


void Matrix::set_identity()
{
    if(!(*this).is_square()) {
        print_log(LOG_ERR, "matrix is not square\n");
        throw MatrixException(MatrixException::SIZE_ERR);
    }

    data.assign(rows * cols, 0.0);

    for(size_t idx = 0; idx < rows; ++idx) {
        (*this)(idx, idx) = 1;
    }
}


Matrix Matrix::transpos()
{
    Matrix res(cols, rows);

    for(size_t row_idx = 0; row_idx < rows; ++row_idx) {
        for(size_t col_idx = 0; col_idx < cols; ++col_idx) {
            res(col_idx, row_idx) = (*this)(row_idx, col_idx);
        }            
    }

    return res;
}


Matrix Matrix::pow(const unsigned int pow) const
{
    if(!(*this).is_square()) {
        print_log(LOG_ERR, "matrix is not square\n");
        throw MatrixException(MatrixException::SIZE_ERR);
    }
    
    Matrix res(rows, cols);
    res.set_identity();

    for(unsigned int idx = 0; idx < pow; ++idx) {
        res *= (*this);
    }

    return res;
}


Matrix Matrix::exp(const unsigned int iterations) const
{
    if(!this->is_square()) {
        print_log(LOG_ERR, "matrix is not square\n");
        throw MatrixException(MatrixException::SIZE_ERR);
    }

    Matrix res(rows, cols);
    res.set_identity();

    Matrix term(rows, cols);
    term.set_identity();

    for(unsigned int idx = 1; idx < iterations; ++idx) {
        term *= (*this);
        term *= (1 / (double)idx);
        res += term;
    }

    return res;
}


void Matrix::get_size()
{
    std::cout << "Количество строк: " << rows << "\tКоличество столбцов: " << cols << '\n';
}


size_t Matrix::find_non_zero_in_col(const size_t idx_start) const
{   
    if(idx_start >= rows) {
        print_log(LOG_ERR, "out of range\n");
        throw MatrixException(MatrixException::PARAMS_ERR);
    }

    for(size_t row_idx = idx_start + 1; row_idx < rows; ++row_idx) {
        if(fabs((*this)(row_idx, row_idx)) >= 0.00001) {
            return row_idx;
        }
    }

    return 0;
}

void Matrix::swap_rows(const size_t row_1, const size_t row_2)
{
    if(row_1 >= rows || row_2 >= rows) {
        print_log(LOG_ERR, "out of range\n");
        throw MatrixException(MatrixException::PARAMS_ERR);
    }

    double tmp = 0.0;

    for(size_t col_idx = 0; col_idx < cols; ++col_idx) {
        tmp = (*this)(row_1, col_idx);
        (*this)(row_1, col_idx) = (*this)(row_2, col_idx);
        (*this)(row_2, col_idx) = tmp;
    }
}


void Matrix::sub_row(const size_t row, const size_t row_base, const double ratio)
{
    if(row >= rows || row_base >= rows) {
        print_log(LOG_ERR, "out of range\n");
        throw MatrixException(MatrixException::PARAMS_ERR);
    }

    for(size_t col_idx = 0; col_idx < cols; ++col_idx) {
        (*this)(row, col_idx) -= ratio * (*this)(row_base, col_idx);
    }
}


double Matrix::det() const
{
    if(!this->is_square()) {
        print_log(LOG_ERR, "Matrix is not square\n");
        throw MatrixException(MatrixException::SIZE_ERR);
    }

    Matrix tmp(*this);

    double det = 1.0;
    for(size_t row_idx = 0; row_idx < rows - 1; ++row_idx) {
        if(fabs(tmp(row_idx, row_idx)) < 0.00001) {
            size_t idx_non_zero = tmp.find_non_zero_in_col(row_idx);
            if(idx_non_zero == 0) {
                return 0.0;
            }

            tmp.swap_rows(row_idx, idx_non_zero);
            det *= -1.0;
        }

        for(size_t row = row_idx + 1; row < rows; ++row) {
            double mult = tmp(row, row_idx) / tmp(row_idx, row_idx);
            tmp.sub_row(row, row_idx, mult);
        }
    }

    for(size_t idx = 0; idx < rows; ++idx) {
        det *= tmp(idx, idx);
    }

    return det;
}

