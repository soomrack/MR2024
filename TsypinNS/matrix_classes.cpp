#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <limits>
#include <string>

typedef double MatrixItem;

class MatrixException : public std::domain_error
{
public:
    MatrixException(const std::string message) : std::domain_error{ message } {};
};

class Matrix {
private:
    size_t rows;
    size_t cols;
    MatrixItem* data;

    void check_size(size_t value) const {
        if (value > std::numeric_limits<size_t>::max() / sizeof(MatrixItem)) {
            throw MatrixException("Размер матрицы слишком большой");
        }
    }

    void check_matrix_size(size_t rows, size_t cols) const {
        if (cols > std::numeric_limits<size_t>::max() / rows) {
            throw MatrixException("Переполнение размера матрицы при операции");
        }
    }

public:
    // Конструкторы и деструктор
    Matrix();
    Matrix(size_t rows, size_t cols);
    Matrix(size_t rows, size_t cols, MatrixItem* values);
    Matrix(const Matrix& other);
    Matrix(Matrix&& other) noexcept;
    ~Matrix();

    // Операторы присваивания
    Matrix& operator=(const Matrix& other);
    Matrix& operator=(Matrix&& other) noexcept;

    // Арифметические операторы
    Matrix& operator+=(const Matrix& other);
    Matrix& operator-=(const Matrix& other);
    Matrix& operator*=(const Matrix& other);
    Matrix operator+(const Matrix& other) const;
    Matrix operator-(const Matrix& other) const;
    Matrix operator*(const Matrix& other) const;
    Matrix operator*(MatrixItem scalar) const;

    // Основные методы матрицы
    void print() const;
    void set_zero();
    void set_one();
    MatrixItem determinant() const;
    Matrix transpose() const;
    Matrix minor(size_t excluded_row, size_t excluded_col) const;
    Matrix matrix_exponent(unsigned iterations) const;
    Matrix inverse() const;
};


Matrix::Matrix() : rows(0), cols(0), data(nullptr) {
    std::cout << "Базовый конструктор\n";
}

Matrix::Matrix(size_t rows, size_t cols) : rows(rows), cols(cols) {
    std::cout << "Параметризованный конструктор\n";
    check_size(rows);
    check_size(cols);
    check_matrix_size(rows, cols);
    if (rows == 0 || cols == 0) {
        throw MatrixException("Строки и столбцы не могут быть равны нулю");
    }
    data = new MatrixItem[rows * cols];
    set_zero();
}

Matrix::Matrix(size_t rows, size_t cols, MatrixItem* values) : rows(rows), cols(cols) {
    std::cout << "Конструктор из массива\n";
    check_size(cols);
    check_size(rows);
    check_matrix_size(rows, cols);
    if (rows == 0 || cols == 0) {
        throw MatrixException("Строки и столбцы не могут быть равны нулю");
    }
    data = new MatrixItem[rows * cols];
    std::memcpy(data, values, sizeof(MatrixItem) * rows * cols);
}

Matrix::Matrix(const Matrix& other) : rows(other.rows), cols(other.cols) {
    std::cout << "Конструктор копирования\n";
    data = new MatrixItem[rows * cols];
    std::memcpy(data, other.data, sizeof(MatrixItem) * rows * cols);
}

Matrix::Matrix(Matrix&& other) noexcept : rows(other.rows), cols(other.cols), data(other.data) {
    std::cout << "Конструктор перемещения\n";
    other.rows = 0;
    other.cols = 0;
    other.data = nullptr;
}

Matrix::~Matrix() {
    delete[] data;
    std::cout << "Деструктор\n";
}

Matrix& Matrix::operator=(const Matrix& other) {
    std::cout << "Копирующее присваивание\n";
    if (this == &other) return *this;
    
    delete[] data;
    rows = other.rows;
    cols = other.cols;
    check_matrix_size(rows, cols);
    data = new MatrixItem[rows * cols];
    std::memcpy(data, other.data, sizeof(MatrixItem) * rows * cols);
    return *this;
}

Matrix& Matrix::operator=(Matrix&& other) noexcept {
    std::cout << "Перемещающее присваивание\n";
    if (this == &other) return *this;
    
    delete[] data;
    rows = other.rows;
    cols = other.cols;
    data = other.data;
    
    other.rows = 0;
    other.cols = 0;
    other.data = nullptr;
    return *this;
}

Matrix& Matrix::operator+=(const Matrix& other) {
    if (rows != other.rows || cols != other.cols) {
        throw MatrixException("У матриц должно быть одинаковое количество столбцов и строк для сложения");
    }
    
    for (size_t i = 0; i < rows * cols; ++i) {
        data[i] += other.data[i];
    }
    return *this;
}

Matrix& Matrix::operator-=(const Matrix& other) {
    if (rows != other.rows || cols != other.cols) {
        throw MatrixException("У матриц должно быть одинаковое количество столбцов и строк для вычитания");
    }
    
    for (size_t i = 0; i < rows * cols; ++i) {
        data[i] -= other.data[i];
    }
    return *this;
}

Matrix& Matrix::operator*=(const Matrix& other) {
    if (cols != other.rows) {
        throw MatrixException("Количество столбцов в первой матрице должно совпадать с количеством строк во второй для умножения");
    }
    
    Matrix result(rows, other.cols);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < other.cols; ++j) {
            result.data[i * other.cols + j] = 0;
            for (size_t k = 0; k < cols; ++k) {
                result.data[i * other.cols + j] += 
                    data[i * cols + k] * other.data[k * other.cols + j];
            }
        }
    }
    *this = std::move(result);
    return *this;
}

Matrix Matrix::operator+(const Matrix& other) const {
    Matrix result(*this);
    result += other;
    return result;
}

Matrix Matrix::operator-(const Matrix& other) const {
    Matrix result(*this);
    result -= other;
    return result;
}

Matrix Matrix::operator*(const Matrix& other) const {
    Matrix result(*this);
    result *= other;
    return result;
}

Matrix Matrix::operator*(MatrixItem scalar) const {
    Matrix result(rows, cols);
    for (size_t i = 0; i < rows * cols; ++i) {
        result.data[i] = data[i] * scalar;
    }
    return result;
}

void Matrix::print() const {
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            std::cout << data[i * cols + j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Matrix::set_zero() {
    std::memset(data, 0, sizeof(MatrixItem) * rows * cols);
}

void Matrix::set_one() {
    set_zero();
    for (size_t i = 0; i < (rows < cols ? rows : cols); ++i) {
        data[i * cols + i] = 1.0;
    }
}

MatrixItem Matrix::determinant() const {
    if (rows != cols) {
        throw MatrixException("Матрица должна быть квадратной для вычисления определителя");
    }
    if (rows == 0 || cols == 0) {
        throw MatrixException("Строки и столбцы не могут быть равны нулю");
    }
    if (rows == 1) return data[0];
    if (rows == 2) return data[0] * data[3] - data[1] * data[2];
    
    MatrixItem det = 0;
    int sign = 1;
    
    for (size_t col = 0; col < cols; ++col) {
        Matrix minor = this->minor(0, col);
        det += sign * data[col] * minor.determinant();
        sign = -sign;
    }
    return det;
}

Matrix Matrix::transpose() const {
    Matrix result(cols, rows);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            result.data[j * rows + i] = data[i * cols + j];
        }
    }
    return result;
}

Matrix Matrix::minor(size_t excluded_row, size_t excluded_col) const {
    if (rows <= 1 || cols <= 1) {
        throw MatrixException("Нельзя сосчитать минор для матрицы с количеством строк или столбцов <= 1");
    }
    
    Matrix result(rows - 1, cols - 1);
    size_t index = 0;
    
    for (size_t i = 0; i < rows; ++i) {
        if (i == excluded_row) continue;
        
        for (size_t j = 0; j < cols; ++j) {
            if (j == excluded_col) continue;
            
            result.data[index++] = data[i * cols + j];
        }
    }
    return result;
}

Matrix Matrix::matrix_exponent(unsigned iterations) const {
    if (rows != cols) {
        throw MatrixException("Матрица должна быть квадратной для возведения в степень");
    }
    
    Matrix result(rows, cols);
    result.set_one();
    
    Matrix term(rows, cols);
    term.set_one();
    
    double factorial = 1.0;
    
    for (unsigned n = 1; n <= iterations; ++n) {
        factorial *= n;
        term = term * (*this) * (1.0 / factorial);
        result += term;
    }
    return result;
}

Matrix Matrix::inverse() const {
    MatrixItem det = determinant();
    if (std::fabs(det) < 1e-10) {
        throw MatrixException("Определитель равен нулю");
    }
    
    Matrix inv(rows, cols);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            Matrix minor = this->minor(i, j);
            inv.data[j * rows + i] = std::pow(-1, i + j) * minor.determinant() / det;
        }
    }
    return inv;
}

int main() {
    try {
        MatrixItem data1[9] = {0., 1., 2., 3., 9., 5., 6., 7., 8.};
        MatrixItem data2[9] = {8., 7., 6., 5., 4., 3., 2., 1., 0.};
        MatrixItem data3[9] = {3., 5., 8., 6., 4., 6., 9., 2., 1.};

        Matrix A(3, 3, data1);
        Matrix B(3, 3, data2);
        Matrix C(3, 3, data3);

        std::cout << "Матрица A:" << std::endl;
        A.print();
        
        std::cout << "Матрица B:" << std::endl;
        B.print();
        
        std::cout << "Матрица C:" << std::endl;
        C.print();

        Matrix sum = A + B;
        std::cout << "A + B:" << std::endl;
        sum.print();

        Matrix diff = A - B;
        std::cout << "A - B:" << std::endl;
        diff.print();

        Matrix product = A * B;
        std::cout << "A * B:" << std::endl;
        product.print();

        Matrix scaled = A * 3.0;
        std::cout << "A * 3:" << std::endl;
        scaled.print();

        Matrix transposed = A.transpose();
        std::cout << "A транспонированное:" << std::endl;
        transposed.print();

        Matrix minor = A.minor(1, 1);
        std::cout << "Минор A (1,1):" << std::endl;
        minor.print();

        Matrix inverse = A.inverse();
        std::cout << "Обратная A:" << std::endl;
        inverse.print();

        Matrix exponent = A.matrix_exponent(5);
        std::cout << "Возведение в степень A (5 степень):" << std::endl;
        exponent.print();

        std::cout << "Определитель A: " << A.determinant() << std::endl << std::endl;

        Matrix result = A * B + C;
        std::cout << "A * B + C:" << std::endl;
        result.print();

    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}