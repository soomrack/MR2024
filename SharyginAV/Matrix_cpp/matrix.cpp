#include "matrix.hpp"
#include <iostream>
#include <vector>


Matrix::Matrix(size_t rows, size_t cols) : rows(rows), cols(cols), data(rows * cols, 0.0) {}


Matrix::Matrix() : rows(0), cols(0) {}


Matrix::Matrix(const Matrix &M) : rows(M.rows), cols(M.cols), data(M.data) {}  // конструктор копирования


Matrix::~Matrix() = default;


double& Matrix::operator()(const size_t row_num,const size_t col_num) {
    if(row_num >= rows || col_num >= cols) {
        throw "Out of range";
    }
    return data[row_num * cols + col_num];
}


const double& Matrix::operator()(const size_t row_num, const size_t col_num) const {
    if(row_num >= rows || col_num >= cols) {
        throw "Out of range";
    }
    return data[row_num * cols + col_num];
}


Matrix Matrix::operator+(const Matrix &A) const
{
    if(!A.equal_size(*this)) {
        throw "Matrices of unequal sizes";
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
        throw "Matrices of unequal sizes";
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
        throw "Matrices of unequal sizes";
    }

    for(size_t idx = 0; idx < rows * cols; ++idx) {
        data[idx] += A.data[idx];
    }

    return *this;
}


Matrix& Matrix::operator-=(const Matrix &A)
{
    if(!A.equal_size(*this)) {
        throw "Matrices of unequal sizes";
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
    // должна быть проверка размеров
    Matrix res(rows, A.cols);

    for(size_t row_idx = 0; row_idx < rows; ++row_idx) {
        for(size_t col_idx = 0; col_idx < cols; ++col_idx) {
            for(size_t idx = 0; idx < A.rows; ++idx) {
                res(row_idx, col_idx) += 
            }
        }
    }
}


Matrix& Matrix::operator*=(const double num)
{
    for(size_t idx = 0; idx < rows * cols; ++idx) {
        data[idx] *= num;
    }

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


void Matrix::get_size()
{
    cout << "Количество строк: " << rows << "\tКоличество столбцов: " << cols << '\n';
}
