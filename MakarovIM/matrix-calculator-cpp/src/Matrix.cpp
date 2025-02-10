#include "Matrix.h"
#include <iomanip>
#include <cmath>
#include <cstring>

Matrix::Matrix() : rows(0), cols(0), data(nullptr) {}

Matrix::Matrix(size_t rows, size_t cols) : rows(rows), cols(cols) 
{
    if (rows == 0 || cols == 0) {
        data = nullptr;
    } else {
        data = new double[rows * cols]();
    }
}

Matrix::Matrix(const Matrix& other) : rows(other.rows), cols(other.cols)
{
    if (other.data) {
        data = new double[rows * cols];
        std::memcpy(data, other.data, rows * cols * sizeof(double));
    } else {
        data = nullptr;
    }
}

// Деструктор
Matrix::~Matrix() 
{
    delete[] data;
}

// Оператор копирующего присваивания
Matrix& Matrix::operator=(const Matrix& other) 
{       // this это указатель на текущий объект, 
    if (this != &other) { // проверяем не присваиваем ли объект самому себе
        delete[] data;
        rows = other.rows;
        cols = other.cols;
        if (other.data) {
            data = new double[rows * cols];
            std::memcpy(data, other.data, rows * cols * sizeof(double));
        } else {
            data = nullptr;
        }
    }
    return *this;
}

// Установка значения элемента
void Matrix::set(size_t row, size_t col, double value) 
{
    if (row >= rows || col >= cols) {
        log(MatrixLogLevel::ERROR, __func__, "Индекс вышел за допустимые границы");
        throw std::out_of_range("Индекс вышел за допустимые границы");
    }
    data[index(row, col)] = value;
}

// Получение значения элемента
double Matrix::get(size_t row, size_t col) const 
{
    if (row >= rows || col >= cols) {
        log(MatrixLogLevel::ERROR, __func__, "Индекс вышел за допустимые границы");
        throw std::out_of_range("Индекс вышел за допустимые границы");
    }
    return data[index(row, col)];
}

// Вывод матрицы в консоль
void Matrix::print() const 
{
    if (isEmpty()) {
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

// Логирование
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

// Сложение матриц
Matrix Matrix::operator+(const Matrix& other) const 
{
    if (rows != other.rows || cols != other.cols) {
        log(MatrixLogLevel::ERROR, __func__, "Измерения матриц должны быть равны для сложения");
        throw std::invalid_argument("Измерения матриц не совпадают");
    }
    Matrix result(rows, cols);
    for (size_t row = 0; row < rows; ++row)
        for (size_t col = 0; col < cols; ++col)
            result.data[index(row, col)] = get(row, col) + other.get(row, col);
    return result;
}

// Вычитание матриц
Matrix Matrix::operator-(const Matrix& other) const 
{
    if (rows != other.rows || cols != other.cols) {
        log(MatrixLogLevel::ERROR, __func__, "Измерения матриц должны быть равны для вычитания");
        throw std::invalid_argument("Измерения матриц не совпадают");
    }
    Matrix result(rows, cols);
    for (size_t row = 0; row < rows; ++row)
        for (size_t col = 0; col < cols; ++col)
            result.data[index(row, col)] = get(row, col) - other.get(row, col);
    return result;
}

// Умножение матриц
Matrix Matrix::operator*(const Matrix& other) const 
{
    if (cols != other.rows) {
        log(MatrixLogLevel::ERROR, __func__, "Измерения матриц некорректны для умножения");
        throw std::invalid_argument("Некорректные измерения для умножения");
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

// Умножение матрицы на число
Matrix Matrix::operator*(double scalar) const 
{
    Matrix result(rows, cols);
    for (size_t row = 0; row < rows; ++row)
        for (size_t col = 0; col < cols; ++col)
            result.data[index(row, col)] = get(row, col) * scalar;
    return result;
}

// Транспонирование матрицы
Matrix Matrix::transpose() const 
{
    Matrix result(cols, rows);
    for (size_t row = 0; row < rows; ++row)
        for (size_t col = 0; col < cols; ++col)
            result.data[col * rows + row] = get(row, col);
    return result;
}

// Возведение матрицы в целую степень
Matrix Matrix::power(int exponent) const 
{
    if (!isSquare()) {
        log(MatrixLogLevel::ERROR, __func__, "Только квадратные матрицы могут быть возведены в степень");
        throw std::invalid_argument("Матрица должна быть квадратной");
    }
    if (exponent < 0) {
        log(MatrixLogLevel::ERROR, __func__, "Возведение в отрицательную степень не поддерживается программой");
        throw std::invalid_argument("Отрицательная степень не поддерживается");
    }
    Matrix result = Matrix::identity(rows);
    if (exponent == 0)
        return result;
    Matrix base(*this);
    for (int idx = 0; idx < exponent; ++idx)
        result = result * base;
    return result;
}

// Вычисление определителя для матриц 1x1, 2x2, 3x3
double Matrix::determinant() const 
{
    log(MatrixLogLevel::WARNING, __func__, "Определить вычисляется только для матриц 1x1, 2x2, 3x3");
    if (!isSquare()) {
        log(MatrixLogLevel::ERROR, __func__, "Определитель может быть вычислен только для квадратных матриц");
        throw std::invalid_argument("Матрица должна быть квадратной");
    }
    if (rows == 0)
        return NAN;
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
    return NAN;
}

// Вычисление факториала
// ф-ия статик доступна только внутри файла
static unsigned long long factorial(unsigned int n) 
{
    if (n == 0 || n == 1)
        return 1;
    unsigned long long result = 1;
    for (unsigned int idx = 2; idx <= n; ++idx)
        result *= idx;
    return result;
}

// Создание единичной матрицы
Matrix Matrix::identity(size_t size) {
    Matrix I(size, size);
    for (size_t idx = 0; idx < size; ++idx)
        I.set(idx, idx, 1.0);
    return I;
}
