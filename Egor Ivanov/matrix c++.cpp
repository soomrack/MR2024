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
    Matrix(const size_t col,const size_t row);
    Matrix(const Matrix& A);
    Matrix(Matrix&& A);
    ~Matrix();

public:
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

public:
    void print();
    void matrix_random();
    Matrix exp(const int x);
    void transp();
    void matrix_identity();
    void matrix_zero();
    double det(Matrix B);
};


Matrix::Matrix() {
    rows = 0;
    cols = 0;
    data = nullptr;
}


Matrix::Matrix(const size_t row, const size_t col)
{
    cols = col;
    rows = row;

    if (cols == 0 || rows == 0) {
       // сообщение об ошибке
        return;
    };

    if (sizeof(double) * rows * cols >= SIZE_MAX) {
        // сообщение об ошибке память переполнена
    };

    data = new double[rows * cols];
}


Matrix::Matrix(const Matrix& A)
{
    rows = A.rows;
    cols = A.cols;
    data = new double[rows * cols];
    memcpy(data, A.data, sizeof(double) * rows * cols);
}


Matrix::Matrix(Matrix&& A) {
    cols = A.cols;
    rows = A.rows;
    data = A.data;
    A.data = nullptr;
    A.cols = 0;
    A.rows = 0;
}


Matrix::~Matrix() 
{
    delete[] data;
    cols = 0;
    rows = 0;
    data = nullptr;
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
        // ошибка памяти
    }

    memset(data, 0, cols * rows * sizeof(double));
}


void Matrix::matrix_identity()
{
    if (rows != cols) {
        // ошибка размера матриц
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

}


int main() {

}