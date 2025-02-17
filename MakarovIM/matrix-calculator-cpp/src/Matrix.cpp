#include "Matrix.h"
#include <iomanip>
#include <cmath>
#include <cstring>

Matrix::Matrix() : rows(0), cols(0), data(nullptr) {
    log(MatrixLogLevel::INFO, __func__, "Инициализирована нулевая матрица");
}


Matrix::Matrix(size_t rows, size_t cols) : rows(rows), cols(cols) {
    if (rows == 0 || cols == 0) {
        data = nullptr;
        return;
    
    if(__SIZE_MAX__ / rows / cols / sizeof(double) == 0) {
        rows = 0;
        cols = 0;
        log(MatrixLogLevel::ERROR, __func__, "Размер матрицы слишком велик");
        throw MatrixException(MatrixErrorType::INTERNAL_ERROR, "Размер матрицы слишком велик");
        return;
    }
    data = new double[rows * cols];
    }
}

Matrix::Matrix(const Matrix& other) : rows(other.rows), cols(other.cols)
{
    if (!other.data) {
        data = nullptr;
        return;
    }

    data = new double[rows * cols];
    std::memcpy(data, other.data, rows * cols * sizeof(double));
}


Matrix::~Matrix() 
{
    delete[] data;
}


Matrix& Matrix::operator=(const Matrix& other) 
{  
    if (!other.data) {
        data = nullptr;
        log(MatrixLogLevel::WARNING, __func__, "Матрица пуста");
        return;
    }

    if (this != &other) {
        delete[] data;
        rows = other.rows;
        cols = other.cols;
    }

    data = new double[rows * cols];
    std::memcpy(data, other.data, rows * cols * sizeof(double));

    return *this;
}


Matrix& Matrix::operator=(Matrix&& other) 
{
    if (this != &other) {
        delete[] data;
        rows = other.rows;
        cols = other.cols;
        data = other.data;
        other.data = nullptr;
        other.rows = 0;
        other.cols = 0;
    }
    return *this;
}


void Matrix::set(size_t row, size_t col, double value) 
{
    if (row >= rows || col >= cols) {
        log(MatrixLogLevel::ERROR, __func__, "Индекс вышел за допустимые границы");
        throw MatrixException(MatrixErrorType::OUT_OF_BOUND_ERROR, "Индекс вышел за допустимые границы");
    }
    data[index(row, col)] = value;
}


double Matrix::get(size_t row, size_t col) const 
{
    if (row >= rows || col >= cols) {
        log(MatrixLogLevel::ERROR, __func__, "Индекс вышел за допустимые границы");
        throw MatrixException(MatrixErrorType::OUT_OF_BOUND_ERROR, "Индекс вышел за допустимые границы");
    }
    return data[index(row, col)];
}


void Matrix::print() const 
{
    if (is_empty()) {
        std::cout << "Матрица не инициализирована" << std::endl;
        return;
    }
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            std::cout << std::fixed << std::setprecision(2) << get(col, col) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}


void Matrix::log(MatrixLogLevel level, const char* location, const char* msg) 
{
    const char* levelStr = "";
    switch (level) {
        case MatrixLogLevel::ERROR:   levelStr = "ERROR";   break;
        case MatrixLogLevel::WARNING: levelStr = "WARNING"; break;
        case MatrixLogLevel::INFO:    levelStr = "INFO";    break;
    }
    std::cerr << levelStr << ": " << msg << "\nLocation: " << location << "\n" << std::endl;
}


Matrix Matrix::operator+(const Matrix& other) const 
{
    if (rows != other.rows || cols != other.cols) {
        log(MatrixLogLevel::ERROR, __func__, "Измерения матриц должны быть равны для сложения");
        throw MatrixException(MatrixErrorType::DIMENSION_MISMATCH_ERROR, "Измерения матриц не равны");
    }
    Matrix result(rows, cols);
    for (size_t idx = 0; idx < rows * cols; ++idx) {
        result.data[idx] = data[idx] + other.data[idx];
    }
    return result;
}


Matrix Matrix::operator-(const Matrix& other) const 
{
    if (rows != other.rows || cols != other.cols) {
        log(MatrixLogLevel::ERROR, __func__, "Измерения матриц должны быть равны для вычитания");
        throw MatrixException(MatrixErrorType::DIMENSION_MISMATCH_ERROR, "Измерения матриц не равны");
    }
    Matrix result(rows, cols);
    for (size_t idx = 0; idx < rows * cols; ++idx) {
        result.data[idx] = data[idx] - other.data[idx];
    }
    return result;
}


Matrix Matrix::operator*(const Matrix& other) const 
{
    if (cols != other.rows) {
        log(MatrixLogLevel::ERROR, __func__, "Измерения матриц некорректны для умножения");
        throw MatrixException(MatrixErrorType::DIMENSION_ERROR, "Измерения матриц не корректны");
    }
    Matrix result(rows, other.cols);
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < other.cols; ++col) {
            double sum = 0;
            for (size_t k = 0; k < cols; ++k)
                sum += get(row, k) * other.get(k, col);
            result.data[row * other.cols + col] = sum;
        }
    }
    return result;
}


Matrix Matrix::operator*(double scalar) const 
{
    Matrix result(rows, cols);
    for (size_t idx = 0; idx < rows * cols; ++idx)
        result.data[idx] = data[idx] * scalar;
    return result;
}


Matrix Matrix::transpose() const 
{
    Matrix result(cols, rows);
    for (size_t row = 0; row < rows; ++row)
        for (size_t col = 0; col < cols; ++col)
            result.data[col * rows + row] = get(row, col);
    return result;
}


Matrix Matrix::power(int exponent) const 
{
    if (!is_square()) {
        log(MatrixLogLevel::ERROR, __func__, "Только квадратные матрицы могут быть возведены в степень");
        throw MatrixException(MatrixErrorType::NOT_SQUARE_ERROR, "Матрица должна быть квадратной");
    }
    if (exponent < 0) {
        log(MatrixLogLevel::ERROR, __func__, "Возведение в отрицательную степень не поддерживается программой");
        throw MatrixException(MatrixErrorType::INTERNAL_ERROR, "Возведение в отрицательную степень не поддерживается");
    }
    Matrix result = Matrix::identity(rows);
    if (exponent == 0)
        return result;
    for (int idx = 0; idx < exponent; ++idx)
        result = result * (*this);
    return result;
}


double Matrix::determinant() const 
{
    log(MatrixLogLevel::WARNING, __func__, "Определить вычисляется только для матриц 1x1, 2x2, 3x3");
    if (!is_square()) {
        log(MatrixLogLevel::ERROR, __func__, "Определитель может быть вычислен только для квадратных матриц");
        throw MatrixException(MatrixErrorType::NOT_SQUARE_ERROR, "Определитель может быть вычислен только для квадратных матриц");
    }
    if (rows == 0)
        throw MatrixException(MatrixErrorType::INTERNAL_ERROR, "Определитель не может быть вычислен для пустой матрицы");
    if (rows == 1)
        return data[0];
    if (rows == 2)
        return get(0, 0) * get(1, 1) - get(0, 1) * get(1, 0);
    if (rows == 3) {
        double det = get(0, 0) * get(1, 1) * get(2, 2)
                   + get(0, 1) * get(1, 2) * get(2, 0)
                   + get(0, 2) * get(1, 0) * get(2, 1)
                   - get(0, 2) * get(1, 1) * get(2, 0)
                   - get(0, 0) * get(1, 2) * get(2, 1)
                   - get(0, 1) * get(1, 0) * get(2, 2);
        return det;
    }
    throw MatrixException(MatrixErrorType::INTERNAL_ERROR, "Определить вычисляется только для матриц 1x1, 2x2, 3x3");
}


static unsigned long long factorial(unsigned int n) 
{
    if (n == 0 || n == 1)
        return 1;
    unsigned long long result = 1;
    for (unsigned int idx = 2; idx <= n; ++idx)
        result *= idx;
    return result;
}


Matrix Matrix::exponent(unsigned int num_terms) const 
{
    if (!is_square()) {
        log(MatrixLogLevel::ERROR, __func__, "Экспонента матрицы определена только для квадратных матриц");
        throw MatrixException(MatrixErrorType::NOT_SQUARE_ERROR, "Экспонента матрицы определена только для квадратных матриц");
    }
    Matrix result = Matrix::identity(rows);
    Matrix term = Matrix::identity(rows); 
    for (unsigned int k = 1; k < num_terms; ++k) {
        term = (term * (*this)) * (1.0 / static_cast<double>(k));
        result = result + term;
    }
    return result;
}


Matrix Matrix::identity(size_t size) 
{
    Matrix I(size, size);
    for (size_t idx = 0; idx < size; ++idx)
        I.set(idx, idx, 1.0);
    return I;
}
