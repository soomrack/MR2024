#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <cmath>
#include <cstring>
#include <sstream>

class MatrixException : public std::exception {
private:
    std::string message;
public:
    explicit MatrixException(const std::string& msg);
    virtual const char* what() const noexcept override;
};

MatrixException::MatrixException(const std::string& msg) : message(msg) {}

const char* MatrixException::what() const noexcept {
    return message.c_str();
}

class Matrix {
private:
    size_t rows;
    size_t cols;
    double* data;

    void allocateMemory();

public:
    Matrix();
    Matrix(size_t rows, size_t cols);
    Matrix(const Matrix& other);
    Matrix(Matrix&& other) noexcept;

    ~Matrix();

    Matrix& assign(const Matrix& other);
    Matrix& moveAssign(Matrix&& other) noexcept;

    double& operator()(size_t row, size_t col);
    double operator()(size_t row, size_t col) const;

    void set(const double* values);
    void identity();
    void scalarMultiply(double scalar);
    double determinant() const;
    Matrix sum(const Matrix& other) const;
    Matrix multiple(const Matrix& other) const;
    Matrix transpose() const;
    Matrix exponent() const;
    void print(const std::string& title, double scalar_result = NAN) const;// Вывод матрицы (или скалярного результата)

    size_t getRows() const;
    size_t getCols() const;
};


void Matrix::allocateMemory() {
    if (rows == 0 || cols == 0)
        throw MatrixException("Matrix dimensions cannot be zero");
    if (rows > 0 && cols > 0 && rows > SIZE_MAX / cols)
        throw MatrixException("Matrix dimensions are too large");
    data = new double[rows * cols];
    for (size_t i = 0; i < rows * cols; ++i)
        data[i] = 0.0;
}

Matrix::Matrix() : rows(0), cols(0), data(nullptr) {}

Matrix::Matrix(size_t r, size_t c) : rows(r), cols(c), data(nullptr) {
    allocateMemory();
}

Matrix::Matrix(const Matrix& other) : rows(other.rows), cols(other.cols), data(nullptr) {
    if (other.data == nullptr || rows * cols == 0)
        return;
    allocateMemory();
    std::memcpy(data, other.data, rows * cols * sizeof(double));
}

Matrix::Matrix(Matrix&& other) noexcept : rows(other.rows), cols(other.cols), data(other.data) {
    other.rows = 0;
    other.cols = 0;
    other.data = nullptr;
}

Matrix::~Matrix() {
    delete[] data;
}

Matrix& Matrix::assign(const Matrix& other) {
    if (this == &other)
        return *this;
    delete[] data;
    rows = other.rows;
    cols = other.cols;
    data = nullptr;
    if (other.data == nullptr || rows * cols == 0)
        return *this;
    allocateMemory();
    std::memcpy(data, other.data, rows * cols * sizeof(double));
    return *this;
}

Matrix& Matrix::moveAssign(Matrix&& other) noexcept {
    if (this == &other)
        return *this;
    delete[] data;
    rows = other.rows;
    cols = other.cols;
    data = other.data;
    other.rows = 0;
    other.cols = 0;
    other.data = nullptr;
    return *this;
}

double& Matrix::operator()(size_t row, size_t col) {
    if (row >= rows || col >= cols)
        throw MatrixException("Index out of bounds");
    return data[row * cols + col];
}

double Matrix::operator()(size_t row, size_t col) const {
    if (row >= rows || col >= cols)
        throw MatrixException("Index out of bounds");
    return data[row * cols + col];
}

void Matrix::set(const double* values) {
    if (values == nullptr || rows * cols == 0)
        return;
    std::memcpy(data, values, rows * cols * sizeof(double));
}

void Matrix::identity() {
    if (rows != cols) {
        throw MatrixException("Matrix must be square for identity operation");
    }
    for (size_t i = 0; i < rows * cols; ++i)
        data[i] = 0.0;
    for (size_t i = 0; i < rows; ++i)
        (*this)(i, i) = 1.0;
}

void Matrix::scalarMultiply(double scalar) {
    if (data == nullptr)
        return;
    for (size_t i = 0; i < rows * cols; ++i)
        data[i] *= scalar;
}

double Matrix::determinant() const {
    if (rows != cols)
        throw MatrixException("Matrix must be square to calculate determinant");
    if (rows == 1)
        return data[0];
    if (rows == 2)
        return (*this)(0, 0) * (*this)(1, 1) - (*this)(0, 1) * (*this)(1, 0);
    if (rows == 3)
        return (*this)(0, 0) * ((*this)(1, 1) * (*this)(2, 2) - (*this)(1, 2) * (*this)(2, 1)) -
        (*this)(0, 1) * ((*this)(1, 0) * (*this)(2, 2) - (*this)(1, 2) * (*this)(2, 0)) +
        (*this)(0, 2) * ((*this)(1, 0) * (*this)(2, 1) - (*this)(1, 1) * (*this)(2, 0));
    throw MatrixException("Determinant is not implemented for matrices larger than 3x3");
}

Matrix Matrix::sum(const Matrix& other) const {
    if (rows != other.rows || cols != other.cols)
        throw MatrixException("Matrix dimensions must match for addition");
    Matrix result(rows, cols);
    for (size_t i = 0; i < rows * cols; ++i)
        result.data[i] = data[i] + other.data[i];
    return result;
}

Matrix Matrix::multiple(const Matrix& other) const {
    if (cols != other.rows)
        throw MatrixException("Matrix dimensions are invalid for multiplication");
    Matrix result(rows, other.cols);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < other.cols; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < cols; ++k)
                sum += (*this)(i, k) * other(k, j);
            result(i, j) = sum;
        }
    }
    return result;
}

Matrix Matrix::transpose() const {
    Matrix result(cols, rows);
    for (size_t i = 0; i < rows; ++i)
        for (size_t j = 0; j < cols; ++j)
            result(j, i) = (*this)(i, j);
    return result;
}

Matrix Matrix::exponent() const {
    if (rows != cols)
        throw MatrixException("Matrix must be square for exponentiation");

    Matrix result(rows, cols);
    result.identity();
    Matrix term(result);

    for (size_t k = 1; k <= 10; ++k) {
        Matrix temp = term.multiple(*this);
        term.moveAssign(std::move(temp));
        term.scalarMultiply(1.0 / static_cast<double>(k));
        Matrix newResult = result.sum(term);
        result.assign(newResult);
    }
    return result;
}

void Matrix::print(const std::string& title, double scalar_result) const {
    std::cout << title << ":\n";
    if (data != nullptr) {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j)
                std::cout << std::setw(6) << (*this)(i, j) << " ";
            std::cout << "\n";
        }
    }
    else {
        std::cout << scalar_result << "\n";
    }
    std::cout << "\n";
}

size_t Matrix::getRows() const { return rows; }
size_t Matrix::getCols() const { return cols; }

int main() {
    try {
        Matrix A(2, 2);
        double valuesA[] = { 1.0, 2.0, 3.0, 4.0 };
        A.set(valuesA);

        Matrix B(2, 2);
        double valuesB[] = { 5.0, 6.0, 7.0, 8.0 };
        B.set(valuesB);

        Matrix A_scaled(A);  
        A_scaled.scalarMultiply(2.0);
        A_scaled.print("Matrix A multiplied by scalar 2");

        double detA = A_scaled.determinant();
        std::cout << "Determinant of A_scaled: " << detA << "\n\n";

        Matrix sumAB = A_scaled.sum(B);
        sumAB.print("Sum of A_scaled and B");

        Matrix productAB = A_scaled.multiple(B);
        productAB.print("Product of A_scaled and B");

        Matrix expA = A_scaled.exponent();
        expA.print("Exponent of A_scaled (e^A)");

        Matrix transposeA = A_scaled.transpose();
        transposeA.print("Transpose of A_scaled");
    }
    catch (const MatrixException& e) {
        std::cerr << "MatrixException: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    catch (const std::exception& e) {
        std::cerr << "Standard exception: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}