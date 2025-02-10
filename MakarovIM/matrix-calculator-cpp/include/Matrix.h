#include <cstddef>
#include <iostream>
#include <stdexcept>

enum class MatrixLogLevel {
    WARNING,
    INFO,
    ERROR,
};

class Matrix {
private:
    size_t rows;
    size_t cols;
    double* data;

    // Функция для вычисления индекса в одномерном массиве
    size_t index(size_t row, size_t col) const { return row * cols + col; }

public:
    Matrix();                            
    Matrix(size_t rows, size_t cols);    
    Matrix(const Matrix& other);          
    ~Matrix();

    Matrix& operator=(const Matrix& other);

    // Геттеры для размеров
    size_t getRows() const { return rows; }
    size_t getCols() const { return cols; }

    // Методы проверки
    bool isEmpty() const { return data == nullptr; }
    bool isSquare() const { return rows == cols; }

    // Доступ к элементам
    void set(size_t row, size_t col, double value);
    double get(size_t row, size_t col) const;

    // Вывод матрицы в консоль
    void print() const;

    // Функция логирования (аналог matrix_log)
    static void log(MatrixLogLevel level, const char* location, const char* msg);

    // Перегрузка арифметических операторов
    Matrix operator+(const Matrix& other) const;   // Сложение
    Matrix operator-(const Matrix& other) const;   // Вычитание
    Matrix operator*(const Matrix& other) const;   // Умножение матриц
    Matrix operator*(double scalar) const;         // Умножение на число

    // Другие операции
    Matrix transpose() const;
    Matrix power(int exponent) const;  // Возведение в целую степень (только для квадратных матриц)
    double determinant() const;        // Вычисление определителя (для 1x1, 2x2, 3x3)

    static Matrix identity(size_t size);
};

// g++ -Wall -Wextra -std=c++17 -Iinclude -o bin/matrix_calculator src/main.cpp src/Matrix.cpp