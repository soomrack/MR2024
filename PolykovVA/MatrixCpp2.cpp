#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <cstring>


class Matrix
{
private:
    size_t cols;
    size_t rows;
    double* data;

public:
    Matrix();
    Matrix(const size_t rows, const size_t cols);
    Matrix(const size_t rows, const size_t cols, double* data2);
    Matrix(const Matrix& A);
    Matrix(Matrix&& A);
    ~Matrix();


    Matrix& operator=(const Matrix& A);
    Matrix& operator=(Matrix&& A);
    Matrix operator+(const Matrix& A);
    Matrix& operator+=(const Matrix& A);
    Matrix operator-(const Matrix& A);
    Matrix operator*(const Matrix& A);
    Matrix operator*(const double x);
    Matrix& operator*=(const Matrix& A);
    Matrix& operator*=(const double x);
    Matrix& operator-=(const Matrix& A);


    void print();
    void matrix_random();
    Matrix exp(const int x);
    Matrix transp();
    void matrix_identity();
    void matrix_zero();
    double det();

    // power -- возведение в степень
};


class MatrixException : public std::domain_error
{
public:
    MatrixException(const std::string message) : domain_error{ message } { };
};


MatrixException MEMORY_ERROR("Memory allocation failed\n");
MatrixException DATA_ERROR("Matrix.data is nullptr\n");
MatrixException SIZE_ERROR("Matrixes of different sizes\n");


Matrix::Matrix() {

    printf("\nContructor\n");

    rows = 0;
    cols = 0;
    data = nullptr;
}


Matrix::Matrix(const size_t rows, const size_t cols) :
    rows(rows), cols(cols), data(nullptr)
{

    if (cols == 0 || rows == 0) {
        printf("ERROR cols=0 or rows=0");
        return;
    };

    if (sizeof(double) * rows * cols >= SIZE_MAX) {  // BUG: nonsense
        throw MEMORY_ERROR;
    };

    data = new double[rows * cols];
}


Matrix::Matrix(const Matrix& A)
{
    printf("\nCopy constructor\n");

    rows = A.rows;
    cols = A.cols;
    data = new double[rows * cols];
    memcpy(data, A.data, sizeof(double) * rows * cols);
}


Matrix::Matrix(Matrix&& A) :
    rows(A.rows), cols(A.cols), data(A.data)
{

    printf("\nMove constructor\n");

    A.data = nullptr;
    A.cols = 0;
    A.rows = 0;
}


Matrix::~Matrix()
{
    printf("\nDestructor\n");

    delete[] data;
    cols = 0;
    rows = 0;
}



Matrix::Matrix(const size_t rows, const size_t cols, double* data2) :
    rows(rows), cols(cols)
{
    printf("\nCreation constructor\n");

    if (cols == 0 || rows == 0) {
        printf("ERROR cols=0 or rows=0");
        return;
    };

    if (sizeof(double) * rows * cols >= SIZE_MAX / rows * cols) {   // BUG:
        throw MEMORY_ERROR;
    };

    data = new double[rows * cols];
    memcpy(data, data2, sizeof(double) * rows * cols);
}


Matrix& Matrix::operator=(const Matrix& A)
{
    printf("\nCopy assignment\n");

    if (this == &A) { return *this; };

    if (cols == A.cols && rows == A.rows) {
        memcpy(data, A.data, sizeof(double) * rows * cols);
        return *this;
    }

    delete[] data;
    rows = A.rows;
    cols = A.cols;
    data = new double[rows * cols];
    memcpy(data, A.data, sizeof(double) * rows * cols);
    return *this;
}


Matrix& Matrix::operator=(Matrix&& A)
{
    printf("\nMove assignment\n");

    rows = A.rows;
    cols = A.cols;
    data = A.data;
    A.data = nullptr;
    A.rows = 0;
    A.cols = 0;
    return *this;
}


void Matrix::print()
{
    for (size_t col = 0; col < cols; ++col) {
        printf("| ");
        for (size_t row = 0; row < rows; ++row) {
            printf("%lf ", data[row * cols + col]);
        }
        printf("|\n");
    }
}


void Matrix::matrix_zero()
{
    if (data == nullptr) {
        throw DATA_ERROR;
    }

    memset(data, 0, cols * rows * sizeof(double));
}


void Matrix::matrix_identity()
{
    if (rows != cols) {
        throw SIZE_ERROR;
    }

    matrix_zero();

    for (size_t idx = 0; idx < rows * cols; idx += rows + 1) {
        data[idx] = 1.0;
    }
}


void Matrix::matrix_random()
{
    for (size_t idx = 0; idx < cols * rows; idx++) {
        data[idx] = rand() % 10;
    }
}



Matrix Matrix::exp(int x)
{
    if (cols != rows) {
        throw SIZE_ERROR;
    }

    Matrix matrix_term(*this);
    Matrix matrix_exponent_result(rows, cols);
    matrix_exponent_result.matrix_identity();

    if (x == 0) {
        return matrix_exponent_result;
    }

    for (size_t n = 1; n <= x; n++) {   // BUG: size_t or int ?
        matrix_term = matrix_term * (*this) * (1.0 / n);
        matrix_exponent_result += matrix_term;
    }
    return matrix_exponent_result;
}


Matrix Matrix::transp()
{
    Matrix transp(cols, rows);
    for (size_t row = 0; row < rows; ++row)
        for (size_t col = 0; col < cols; ++col)
            transp.data[col * rows + row] = data[row * cols + col];
    return transp;
}


double Matrix::det()
{
    if (cols != rows) {
        throw SIZE_ERROR;
    }

    if (rows == 1) {
        return data[0];
    }
    else if (rows == 2) {
        return data[0] * data[3] - data[1] * data[2];
    }
    else if (rows == 3) {
        return data[0] * (data[4] * data[8] - data[5] * data[7]) -
            data[1] * (data[3] * data[8] - data[5] * data[6]) +
            data[2] * (data[3] * data[7] - data[4] * data[6]);
    }

    // BUG: return what?
}


Matrix& Matrix::operator+=(const Matrix& A) // A = A + B
{
    printf("\n+=\n");

    if (rows != A.rows || cols != A.cols) {
        throw SIZE_ERROR;
    }

    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] += A.data[idx];
    }

    return *this;
}


Matrix& Matrix::operator-=(const Matrix& A) // A = A - B
{
    printf("\n-=\n");

    if (rows != A.rows || cols != A.cols) {
        throw SIZE_ERROR;
    }

    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] -= A.data[idx];
    }

    return *this;
}


Matrix& Matrix::operator*=(const Matrix& A) // A = A * B
{
    printf("\n*=\n");

    if (cols != A.rows) {
        throw SIZE_ERROR;
    }

    Matrix multi(rows, A.cols);
    for (size_t row = 0; row < multi.rows; row++) {
        for (size_t col = 0; col < multi.cols; col++) {
            for (size_t idx = 0; idx < A.cols; idx++) {
                multi.data[row * multi.cols + col] += data[row * cols + idx] * A.data[idx * A.cols + col];
            }  // BUG: initial value? 
        }
    }
    *this = multi;
    return *this;
}


Matrix& Matrix::operator*=(const double x) // A = A * const 
{
    printf("\n*=x\n");
    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] *= x;
    }

    return *this;
}


Matrix Matrix::operator+(const Matrix& A) // C = A + B
{
    printf("\n+\n");

    Matrix sum(*this);
    sum += A;
    return sum;
}


Matrix Matrix::operator-(const Matrix& A) // C = A - B
{
    printf("\n-\n");

    Matrix sub(*this);
    sub -= A;
    return sub;
}


Matrix Matrix::operator*(const Matrix& A) // C = A * B
{
    printf("\n*\n");

    Matrix multi(*this);
    multi *= A;
    return multi;
}


Matrix Matrix::operator*(const double x) // C = A * const
{
    printf("\n*x\n");

    Matrix multi(*this);
    multi *= x;
    return multi;
}


int main() {

    double data[9] = { 0., 1., 2., 3., 4., 5., 6., 7., 8. };
    Matrix A(3, 3, data);

    A.print();
    Matrix B = A * 10;
}
