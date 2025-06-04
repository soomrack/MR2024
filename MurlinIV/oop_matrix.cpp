#include <iostream>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <stdexcept>

class Matrix {
public:
    Matrix(size_t cols, size_t rows);
    Matrix(const Matrix& other);
    Matrix(Matrix&& other) noexcept;
    ~Matrix();

    // Основные методы
    void random();
    void print() const;
    double deter() const;
    void Free();

    // Матричные операции
    Matrix transpose() const;
    static Matrix identity(size_t size);
    Matrix invert() const;
    Matrix exp(int accuracy) const;
    Matrix degree(unsigned int degree) const; 

    // Операторы
    Matrix operator+(const Matrix& rhs) const;
    Matrix operator-(const Matrix& rhs) const;
    Matrix& operator=(const Matrix& rhs);
    Matrix& operator=(Matrix&& rhs) noexcept;
    Matrix operator*(const Matrix& rhs) const;
    Matrix operator*(double scalar) const;

private:
    size_t cols;
    size_t rows;
    double* data;

    void checkSquare() const;
    void checkDimensionsMatch(const Matrix& other) const;
};

class MatrixException : public std::runtime_error {
public:
    explicit MatrixException(const std::string& message)
        : std::runtime_error(message) {}
};

Matrix::Matrix(size_t cols, size_t rows)
    : cols(cols), rows(rows), data(new double[cols * rows]) {}

Matrix::Matrix(const Matrix& other)
    : cols(other.cols), rows(other.rows), data(new double[other.cols * other.rows]) {
    std::memcpy(data, other.data, sizeof(double) * cols * rows);
}

Matrix::Matrix(Matrix&& other) noexcept
    : cols(other.cols), rows(other.rows), data(other.data) {
    other.data = nullptr;
    other.cols = 0;
    other.rows = 0;
}

Matrix::~Matrix() {
    delete[] data;
}

void Matrix::random() {
    for (size_t i = 0; i < cols * rows; i++) {
        data[i] = i + 1;
    }
}

void Matrix::print() const {
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            printf("%.2f ", data[i * cols + j]);
        }
        printf("\n");
    }
    printf("\n");
}

double Matrix::deter() const {
    checkSquare();

    if (cols == 1) {
        return data[0];
    }

    double result = 0;
    for (size_t row = 0; row < rows; ++row) {
        Matrix sub(cols - 1, rows - 1);
        size_t subIndex = 0;

        for (size_t i = 0; i < rows; i++) {
            if (i == row) continue;
            for (size_t j = 1; j < cols; j++) {
                sub.data[subIndex++] = data[i * cols + j];
            }
        }
        result += pow(-1, row) * data[row * cols] * sub.deter();
    }
    return result;
}

void Matrix::Free() {
    delete[] data;
    data = nullptr;
    cols = 0;
    rows = 0;
}

Matrix Matrix::operator+(const Matrix& rhs) const {
    checkDimensionsMatch(rhs);
    Matrix result(*this);

    for (size_t i = 0; i < cols * rows; i++) {
        result.data[i] += rhs.data[i];
    }
    return result;
}

Matrix Matrix::operator-(const Matrix& rhs) const {
    checkDimensionsMatch(rhs);
    Matrix result(*this);

    for (size_t i = 0; i < cols * rows; i++) {
        result.data[i] -= rhs.data[i];
    }
    return result;
}

Matrix& Matrix::operator=(const Matrix& rhs) {
    if (this != &rhs) {
        delete[] data;
        cols = rhs.cols;
        rows = rhs.rows;
        data = new double[cols * rows];
        std::memcpy(data, rhs.data, sizeof(double) * cols * rows);
    }
    return *this;
}

Matrix& Matrix::operator=(Matrix&& rhs) noexcept {
    if (this != &rhs) {
        delete[] data;
        cols = rhs.cols;
        rows = rhs.rows;
        data = rhs.data;
        rhs.data = nullptr;
        rhs.cols = 0;
        rhs.rows = 0;
    }
    return *this;
}

Matrix Matrix::operator*(const Matrix& rhs) const {
    if (cols != rhs.rows) {
        throw MatrixException("Invalid dimensions for matrix multiplication");
    }

    Matrix result(rhs.cols, rows);
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < rhs.cols; j++) {
            double sum = 0;
            for (size_t k = 0; k < cols; k++) {
                sum += data[i * cols + k] * rhs.data[k * rhs.cols + j];
            }
            result.data[i * rhs.cols + j] = sum;
        }
    }
    return result;
}

Matrix Matrix::operator*(double scalar) const {
    Matrix result(*this);
    for (size_t i = 0; i < cols * rows; i++) {
        result.data[i] *= scalar;
    }
    return result;
}

Matrix Matrix::transpose() const {
    Matrix result(rows, cols);
    for (size_t i = 0; i < result.rows; ++i) {
        for (size_t j = 0; j < result.cols; ++j) {
            result.data[i * result.cols + j] = data[j * result.rows + i];
        }
    }
    return result;
}

Matrix Matrix::identity(size_t size) {
    Matrix result(size, size);
    std::memset(result.data, 0, sizeof(double) * size * size);
    for (size_t i = 0; i < size; i++) {
        result.data[i * result.cols + i] = 1;
    }
    return result;
}

Matrix Matrix::invert() const {
    checkSquare();
    double det = deter();
    if (std::abs(det) < 1e-10) {
        throw MatrixException("Matrix is not invertible (determinant is zero)");
    }

    Matrix adjugate(cols, rows);
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            Matrix sub(cols - 1, rows - 1);
            size_t subIndex = 0;

            for (size_t k = 0; k < rows; k++) {
                if (k == i) continue;
                for (size_t l = 0; l < cols; l++) {
                    if (l == j) continue;
                    sub.data[subIndex++] = data[k * cols + l];
                }
            }
            adjugate.data[j * cols + i] = pow(-1, i + j) * sub.deter();
        }
    }
    return adjugate * (1.0 / det);
}

Matrix Matrix::exp(int accuracy) const {
    checkSquare();
    Matrix result = identity(cols);
    Matrix term = identity(cols);
    double factorial = 1.0;

    for (int n = 1; n <= accuracy; n++) {
        factorial *= n;
        term = term * *this;
        result = result + term * (1.0 / factorial);
    }
    return result;
}

Matrix Matrix::degree(unsigned int degree) const {
    checkSquare();
    Matrix result = *this;

    for (unsigned int i = 1; i < degree; i++) {
        result = result * *this;  
    }

    return result;
}

void Matrix::checkSquare() const {
    if (cols != rows) {
        throw MatrixException("Matrix must be square for this operation");
    }
}

void Matrix::checkDimensionsMatch(const Matrix& other) const {
    if (rows != other.rows || cols != other.cols) {
        throw MatrixException("Matrix dimensions do not match");
    }
}

int main() {
    try {
        Matrix A(2, 2);
        Matrix B(2, 2);

        A.random();
        B.random();

        std::cout << "Matrix A:\n";
        A.print();

        std::cout << "Matrix B:\n";
        B.print();

        std::cout << "A^3:\n";
        Matrix A_cubed = A.degree(3);
        A_cubed.print();
        A_cubed.Free();

    }
    catch (const MatrixException& e) {
        std::cerr << "Matrix error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}