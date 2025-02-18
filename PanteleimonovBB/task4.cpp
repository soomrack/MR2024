#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <cstdint>
#include <limits>
#include <algorithm>


class Matrix {
private:
    size_t rows, cols;
    double* data;

public:
    Matrix();
    public:
    Matrix(size_t n); 
    Matrix(size_t r, size_t c);
    Matrix(const Matrix& M);
    Matrix(Matrix&& M) noexcept;
    ~Matrix();
     static uint64_t factorial(unsigned int f);
public:
    Matrix& operator=(const Matrix& M);
    Matrix& operator=(Matrix&& M) noexcept;
    Matrix operator+(const Matrix& B) const;
    Matrix operator-(const Matrix& B) const;
    Matrix operator*(double scalar) const;
    Matrix operator*(const Matrix& B) const;

    void input();
    void print() const;
    static Matrix identity(size_t n);
    Matrix power(size_t exp) const;
    Matrix transpose() const;
    Matrix exponent(size_t order) const;
    double determinant() const;

    friend std::ostream& operator<<(std::ostream& os, const Matrix& matrix);
};

Matrix::Matrix() : rows(0), cols(0), data(nullptr) {
}

 uint64_t Matrix::factorial(unsigned int f) {
    std::uint64_t res = 1;
    for (unsigned int idx = 1; idx <= f; idx++) {
        if (res > UINT64_MAX / idx) {
            throw std::overflow_error("Переполнение при вычислении факториала!");
        }
        res *= idx;
    }
    return res;
}

Matrix::Matrix(size_t n) : rows(n), cols(n) {
    if (n == 0) {
        data = nullptr;
        return;
    }

    if (n > std::numeric_limits<size_t>::max() / n) {     
        throw std::overflow_error("Размер матрицы слишком велик для выделения памяти.");
    }
    data = new double[n * n];
}


Matrix::Matrix(const size_t row, const size_t col) : rows(row), cols(col) {
    if (row == 0 || col == 0) {
        data = nullptr;
        return;
    }

    if (row > std::numeric_limits<size_t>::max() / col) {     
        throw std::overflow_error("Размер матрицы слишком велик для выделения памяти.");
    }
    data = new double[row * col];
}


Matrix::Matrix(const Matrix& M) : rows(M.rows), cols(M.cols) {
    data = new double[rows * cols];

   std::copy(M.data, M.data + rows * cols, data);
}


Matrix::Matrix(Matrix&& M) noexcept : rows(M.rows), cols(M.cols), data(M.data) {
    M.rows = 0;
    M.cols = 0;
    M.data = nullptr;
}


Matrix::~Matrix() {
    rows = 0;
    cols = 0;
    delete[] data;
}


void Matrix::input() {
    std::cout << "Введите элементы матрицы (" << rows << "x" << cols << "):\n";
    for (size_t idx = 0; idx < rows * cols; idx++) {
        std::cin >> data[idx];
    }
}

void Matrix::print() const {
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            std::cout << data[i * cols + j] << " ";
        }
        std::cout << std::endl;
    }
}

Matrix& Matrix::operator=(const Matrix& M) {
    if (this == &M) return *this;
    delete[] data;
    rows = M.rows;
    cols = M.cols;
    data = new double[rows * cols];
    std::copy(M.data, M.data + rows * cols, data);
    return *this;
}

Matrix& Matrix::operator=(Matrix&& M) noexcept {
    if (this == &M) return *this;
    delete[] data;
    rows = M.rows;
    cols = M.cols;
    data = M.data;
    M.rows = 0;
    M.cols = 0;
    M.data = nullptr;
    return *this;
}


Matrix Matrix::operator+(const Matrix& B) const {
    if (rows != B.rows || cols != B.cols) throw std::runtime_error("Размеры матриц не совпадают.");
    Matrix result(rows, cols);
    for (size_t idx = 0; idx < rows * cols; idx++) {
        result.data[idx] = data[idx] + B.data[idx];
    }
    return result;
}

Matrix Matrix::operator-(const Matrix& B) const {
    if (rows != B.rows || cols != B.cols) throw std::runtime_error("Размеры матриц не совпадают.");
    Matrix result(rows, cols);
    for (size_t idx = 0; idx < rows * cols; idx++) {
        result.data[idx] = data[idx] - B.data[idx];
    }
    return result;
}

Matrix Matrix::operator*(double scalar) const {
    Matrix result(rows, cols);
    for (size_t idx = 0; idx < rows * cols; idx++) {
        result.data[idx] = data[idx] * scalar;
    }
    return result;
}

Matrix Matrix::operator*(const Matrix& B) const {
    if (cols != B.rows) throw std::runtime_error("Нельзя умножить: число столбцов первой не равно числу строк второй.");
    Matrix result(rows, B.cols);
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < B.cols; j++) {
            for (size_t k = 0; k < cols; k++) {
                result.data[i * B.cols + j] += data[i * cols + k] * B.data[k * B.cols + j];
            }
        }
    }
    return result;
}


Matrix Matrix::transpose() const {
    Matrix result(cols, rows);
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            result.data[j * rows + i] = data[i * cols + j];
        }
    }
    return result;
}

Matrix Matrix::power(size_t exp) const {
    if (rows != cols) throw std::runtime_error("Только квадратные матрицы могут возводиться в степень.");
    Matrix result = identity(rows);
    Matrix base = *this;
    while (exp) {
        if (exp % 2) result = result * base;
        base = base * base;
        exp /= 2;
    }
    return result;
}

double Matrix::determinant() const {
    if (rows != cols) throw std::runtime_error("Определитель вычисляется только для квадратных матриц.");
    if (rows == 1) return data[0];

    double det = 0.0;
    for (size_t i = 0; i < cols; i++) {
        det += (i % 2 == 0 ? 1 : -1) * data[i] * determinant();
    }
    return det;
}

Matrix Matrix::exponent(size_t order) const {
    if (rows != cols) throw std::runtime_error("Экспонента определена только для квадратных матриц.");
    Matrix result = identity(rows);
    Matrix term = identity(rows);
    for (size_t n = 1; n <= order; n++) {
        term = term * (*this) * (1.0 / n);
        result = result + term;
    }
    return result;
}

Matrix Matrix::identity(size_t n) {
    Matrix id(n, n);
    for (size_t i = 0; i < n; i++) id.data[i * n + i] = 1.0;
    return id;
}

std::ostream& operator<<(std::ostream& os, const Matrix& matrix) {
    matrix.print();
    return os;
}


int main() {
    try {
        size_t choice;
        std::cout << "Выберите операцию:\n";
        std::cout << "1. Сложение матриц\n";
        std::cout << "2. Разность матриц\n";
        std::cout << "3. Умножение матриц\n";
        std::cout << "4. Транспонирование\n";
        std::cout << "5. Возведение в степень\n";
        std::cout << "6. Определитель матрицы\n";
        std::cout << "7. Вычисление экспоненты\n";
        std::cout << "8. Выход\n";
        std::cout << "Ваш выбор: ";
        std::cin >> choice;

        if (choice == 8) {
            std::cout << "Выход.\n";
            return 0;
        }

        size_t rows, cols;
        std::cout << "Введите размер матрицы A (строки столбцы): ";
        std::cin >> rows >> cols;
        Matrix A(rows, cols);
        A.input();

        if (choice == 1 || choice == 2 || choice == 3) {
            std::cout << "Введите размер матрицы B (строки столбцы): ";
            std::cin >> rows >> cols;
            Matrix B(rows, cols);
            B.input();

            if (choice == 1) {
                Matrix result = A + B;
                std::cout << "Результат сложения:\n" << result;
            } else if (choice == 2) {
                Matrix result = A - B;
                std::cout << "Результат вычитания:\n" << result;
            } else {
                Matrix result = A * B;
                std::cout << "Результат умножения:\n" << result;
            }
        } else if (choice == 4) {
            Matrix result = A.transpose();
            std::cout << "Транспонированная матрица:\n" << result;
        } else if (choice == 5) {
            size_t exp;
            std::cout << "Введите степень: ";
            std::cin >> exp;
            Matrix result = A.power(exp);
            std::cout << "Матрица в степени " << exp << ":\n" << result;
        } else if (choice == 6) {
            std::cout << "Определитель: " << A.determinant() << std::endl;
        } else if (choice == 7) {
            size_t order;
            std::cout << "Введите порядок разложения экспоненты: ";
            std::cin >> order;
            Matrix result = A.exponent(order);
            std::cout << "Экспонента матрицы:\n" << result;
        }
    } catch (const std::exception &e) {
        std::cout << "Ошибка: " << e.what() << std::endl;
    }

    return 0;
}
