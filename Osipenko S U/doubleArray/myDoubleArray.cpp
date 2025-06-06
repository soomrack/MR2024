#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <stdexcept>


enum MatrixExceptionLevel {ERROR, WARNING, INFO, DEBUG};

// Сообщение об ошибке
void matrix_error(const enum MatrixExceptionLevel level, const char* location)
{
    if (level == ERROR) {
        printf("\nERROR\nLoc: %s\n", location);
    }

    if (level == WARNING) {
        printf("\nWARNING\nLoc: %s\n", location);
    }

    if (level == INFO) {
        printf("\nINFO\nLoc: %s\n", location);
    }

    if (level == DEBUG) {
        printf("\nDEBUG\nLoc: %s\n", location);
    }
}


class MatrixException : public std::domain_error { // Конструктор для исключений
public:
    explicit MatrixException(const std::string& message) : domain_error(message) {} 
};

class MatrixError : public std::domainError{
    public:
        explicit MatrixError(const std::string& message) : domainError(message){}
}

class Matrix{
    size_t rows;
    size_t cols;
    double* data;

public:
    Matrix(size_t des_rows, size_t des_cols); // Конструктор
    ~Matrix(); // Деструктор
    Matrix(const Matrix& other); // Конструктор копирования
    Matrix(Matrix&& other); // Конструктор переноса

public:  // Сервисные методы (функции)
    void print();
    void set_zero();
    void set_identity();
    void fill_with_indices();
    void set_value(const unsigned int idx, const double value);
    double determinant();

    Matrix transp();
    Matrix exp(const unsigned int targ_num); // E = e ^ A

public:  // Переопределение операторов
    double& operator[](unsigned int idx); // Определение значения элемента матрицы по индексу
    Matrix& operator=(Matrix&& other); // Оператор присваивания через перенос
    Matrix& operator=(const Matrix& B);
    Matrix& operator+=(const Matrix& B);
    Matrix& operator*=(const double k);
    Matrix operator+(const Matrix& B);
    Matrix operator-(const Matrix& B);
    Matrix operator*(const Matrix& B);
    Matrix operator^(const unsigned int targ_power);
}

Matrix::Matrix(size_t des_rows, size_t des_cols) 
{
    rows = des_rows;
    cols = des_cols;

    if (cols == 0 || rows == 0) {
        matrix_error(INFO, "matrix_alloc");
        throw MatrixException("Matrix is ix0 or 0xj");
    }   

    if (rows > SIZE_MAX / cols) { // Проверка на переполнение cols * rows
        matrix_error(ERROR, "matrix_alloc");
        rows = 0;
        cols = 0;
        data = nullptr;
        throw MatrixException("Cols * rows exceeds SIZE_MAX");
    }

    if (cols * rows > SIZE_MAX / sizeof(double)) { // Проверка на общий объем памяти
        matrix_error(ERROR, "matrix_alloc");
        rows = 0;
        cols = 0;
        data = nullptr;
        throw MatrixException("Memory allocation size exceeds SIZE_MAX");
    }

    data = new double[cols * rows];
}


Matrix::~Matrix() 
{
    delete[] data;
}


Matrix::Matrix(const Matrix& B) : rows(B.rows), cols(B.cols) // Конструктор копирования
{
    printf("\nConstructor copy\n");
    data = new double[rows * cols];
    std::copy(B.data, B.data + rows * cols, data);
}


Matrix::Matrix(Matrix&& other) : rows(other.rows), cols(other.cols), data(other.data) // Конструтор переноса
{
    printf("\nConstructor move\n");
    other.rows = 0;
    other.cols = 0;
    other.data = nullptr;
}


void Matrix::print() // Вывод матрицы
{
    printf("\n_____________________________\n");
    for (size_t row_i = 0; row_i < rows; ++row_i) {
        for (size_t col_j = 0; col_j < cols; ++col_j) {
            printf("%.2f ", data[row_i * cols + col_j]);
        }
        printf("\n");
    }
    printf("\n");
}


void Matrix::set_zero() // Заполнение матрицы нулями
{   
    if (cols == 0 || rows == 0) {
        matrix_error(INFO, "set_zero");
        throw MatrixException("Matrix is ix0 or 0xj");
    }   

    memset(data, 0, cols * rows * sizeof(double));
}


void Matrix::set_identity() // Превращение матрицы в "единичную" по диагонали
{   
    set_zero();
    for (size_t idx = 0; idx < rows * cols; idx += cols + 1) {
        data[idx] = 1.0;
    }
}


void Matrix::fill_with_indices() // Заполнение матрицы индексами элементов
{
    for (size_t row_i = 0; row_i < rows; ++row_i) {
        for (size_t col_j = 0; col_j < cols; ++col_j) {
            data[row_i * cols + col_j] = row_i * cols + col_j; 
        }
    }
}


void Matrix::set_value(const unsigned int idx, const double value) // Заполнение элемента матрицы по индексу
{
    if (idx >= cols * rows) {
        matrix_error(ERROR, "set_value");
        throw MatrixException("The entered index exceeds the maximum for this matrix");
    }

    data[idx] = value;
}


double Matrix::determinant() // Определитель
{
    double det;
    if (rows != cols) {
        printf("A non-square matrix\n");
        return NAN;
    }

    if (rows == 0) {
        return NAN;
    }

    if (rows == 1) {
        det = data[0];
        return det;
    }

    if (rows == 2) {
        det = data[0] * data[3] - data[1] * data[2];
        return det;
    }

    if (rows == 3) {
        det = data[0] * data[4] * data[8] 
        + data[1] * data[5] * data[6] 
        + data[3] * data[7] * data[2] 
        - data[2] * data[4] * data[6] 
        - data[3] * data[1] * data[8] 
        - data[0] * data[5] * data[7];
        return det;
    }

    return NAN;
}


Matrix Matrix::transp() // B = A ^ T
{
    Matrix result(cols, rows);

    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            result.data[row + rows * col] = data[col + cols * row];
        }
    }

    return result;
}


Matrix Matrix::exp(const unsigned int targ_num) // E = e ^ A
{
    if (rows != cols) {
        matrix_error(ERROR, "matrix_pow");
        throw MatrixException("A non-square matrix");
    }

    Matrix E(cols,rows);
    E.set_identity();

    if (targ_num == 1) {
        return E;
    }

    Matrix tmp(E);
    Matrix buf_mult(rows, cols);

    for (size_t num = 1; num < targ_num; ++num) { 

        buf_mult = std::move(tmp) * *this;
        buf_mult *= (1.0/num);

        E += buf_mult;
        tmp = std::move(buf_mult);
    }
    return E;
}


double& Matrix::operator[](unsigned int idx) // Определение значения элемента матрицы по индексу
{
    if (idx >= cols * rows) {
        matrix_error(ERROR, "matrix_idx");
        throw MatrixException("The entered index exceeds the maximum for this matrix");
    }

    return data[idx];
}


Matrix& Matrix::operator=(Matrix&& other) // Оператор присваивания через перенос
{
    printf("\nOperator move\n");
    
    if (this == &other) {
        return *this;
    }

    if ((rows != other.rows) && (cols != other.cols)) {
        matrix_error(ERROR, "matrix_copy");
        throw MatrixException("Matrices of different dimensions");
    }
    
    rows = other.rows;
    cols = other.cols;
    data = other.data;

    other.rows = 0;
    other.cols = 0;
    other.data = nullptr;

    return *this;
}


Matrix& Matrix::operator=(const Matrix& B) // A := B
{
    if (this == &B) {
        matrix_error(INFO, "matrix_copy");
        printf("Equating identical matrices\n");
        return *this;
    }
    
    if ((cols != B.cols) || (rows != B.rows )) { 
        matrix_error(ERROR, "matrix_copy");
        throw MatrixException("Matrices of different dimensions");
    }   

    memcpy(data, B.data, cols * rows * sizeof(double));
    return *this;
}


Matrix& Matrix::operator+=(const Matrix& B) // A += B
{
    if ((cols != B.cols) || (rows != B.rows )) {
        matrix_error(ERROR, "A = A + B");
        throw MatrixException("Matrices of different dimensions");
    }

    for (size_t idx = 0; idx < cols * rows; ++idx) {
        data[idx] += B.data[idx];
    }

    return *this;
}


Matrix& Matrix::operator*=(const double k) // A *= k
{
    for (size_t idx = 0; idx < cols * rows; idx++) {
        data[idx] *= k;
    }
    return *this;
}


Matrix Matrix::operator+(const Matrix& B) // C = A + B
{
    if ((cols != B.cols) || (rows != B.rows))
    {
        matrix_error(ERROR, "C = A + B");
        throw MatrixException("Matrices of different dimensions");
    }

    Matrix result(B);

    result += *this;
    printf("Operator +");
    return result;
}


Matrix Matrix::operator-(const Matrix& B) // C = A - B
{
    if ((cols != B.cols) || (rows != B.rows))
    {
        matrix_error(ERROR, "C = A - B");
        throw MatrixException("Matrices of different dimensions");

    }

    Matrix result(rows, cols);

    for (size_t idx = 0; idx < cols * rows; ++idx) {
        result.data[idx] = data[idx] - B.data[idx];
    }

    return result;
}


Matrix Matrix::operator*(const Matrix& B) // C = A * B
{
    if (cols != B.rows)
    {
        matrix_error(ERROR, "C = A * B");
        throw MatrixException("Matrices of different dimensions");
    }

    Matrix result(rows, B.cols);

    for (size_t row = 0; row < result.rows; ++row) {
        for (size_t col = 0; col < result.cols; ++col) {
            result.data[row * (result.cols) + col] = 0;
                
            for (size_t idx = 0; idx < cols; idx++) {
                result.data[row * result.cols + col] += data[row * cols + idx] * B.data[idx * B.cols + col];
            }
        }
    }

    printf("Operator *");
    return result;
}


Matrix Matrix::operator^(const unsigned int targ_power) // B = A ^ n
{
    if (rows != cols) {
        matrix_error(WARNING, "matrix_pow");
        throw MatrixException("A non-square matrix");
    }

    Matrix result (*this);

    if (targ_power == 0) {
        result.set_identity();
        return result;
    }

    for (unsigned int pow = 1; pow < targ_power; ++pow) {
        result = std::move(result) * (*this);
    }
    return result;
}
int main()
{
    double res;
    Matrix A(3,3);
    A.print();
    Matrix B(3,3);
    B.fill_with_indices();
    A = B;
    A.print();
    A *= 2;
    A.print();
    res = A[2];
    printf("%.2f\n", res);
    A.set_value(0, 2.3);
    A.print();
    printf("%.2f", A.determinant());
    Matrix C = B - A;
    A.print();
    B.print();
    C.print();

    Matrix D(2,3);
    D.fill_with_indices();
    Matrix E(3,4);
    E.fill_with_indices();
    D.print();
    E.print();
    printf("above");
    Matrix F = D * E;
    F.print();
    printf("end");
    Matrix G = F.transp();
    G.print();

    printf("Matrix_pow test\n");
    B.print();
    Matrix J = B ^ 2;
    J.print();
}