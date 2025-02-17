#include <iostream>
#include <vector>
#include <cstring>
#include <stdexcept>
#include <cmath>

class MatrixException : public std::exception {
private:
    std::string message;
public:
    MatrixException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};

MatrixException ZERO_ROWS_OR_COLUMNS("Zero rows or columns");
MatrixException MEMORY_OVERFLOW("Overflow of allocated memory");
MatrixException ELEMENTS_OVERFLOW("Overflow of the number of elements");
MatrixException SIZE_MISMATCH("Size mismatch");
MatrixException MULTIPLICATION_SIZE_MISMATCH("Incorrect matrix sizes for multiplication");
MatrixException MATRIX_NOT_SQUARE("Matrix is not square");
MatrixException DETERMINANT_ERROR("It is impossible to calculate the determinant for size > 3");

class Matrix {
private:
    size_t rows;
    size_t cols;
    double* data;

public:
    Matrix(size_t r, size_t c);
    Matrix(size_t r, size_t c, const std::vector<double>& arr);
    Matrix(const Matrix& other);
    Matrix(Matrix&& other) noexcept;
    ~Matrix();

    Matrix& operator=(const Matrix& other);
    Matrix& operator=(Matrix&& other) noexcept;

    Matrix operator+(const Matrix& other) const;
    Matrix operator-(const Matrix& other) const;
    Matrix operator*(const Matrix& other) const;
    Matrix operator*(double scalar) const;

    Matrix transpose() const;
    Matrix power(size_t n) const;
    double determinant() const;
    void set_identity();
    void print() const;
    Matrix exp(size_t steps) const;
};


Matrix::Matrix(size_t r, size_t c) : rows(r), cols(c) {
    if (rows == 0 || cols == 0) {
        throw ZERO_ROWS_OR_COLUMNS;
    }
    if (rows > SIZE_MAX / cols) {
        throw MEMORY_OVERFLOW;
    }
    data = new double[rows * cols];
}


Matrix::Matrix(size_t r, size_t c, const std::vector<double>& arr) : rows(r), cols(c) {
    if (rows == 0 || cols == 0) {
        throw ZERO_ROWS_OR_COLUMNS;
    }
    if (rows > SIZE_MAX / cols) {
        throw MEMORY_OVERFLOW;
    }
    if (arr.size() != rows * cols) {
        throw ELEMENTS_OVERFLOW;
    }
    data = new double[rows * cols];
    std::memcpy(data, arr.data(), rows * cols * sizeof(double));
}


Matrix::Matrix(const Matrix& other) : rows(other.rows), cols(other.cols) {
    data = new double[rows * cols];
    std::memcpy(data, other.data, rows * cols * sizeof(double));
}


Matrix::Matrix(Matrix&& other) noexcept 
    : rows(other.rows), cols(other.cols), data(other.data) {
    other.rows = 0;
    other.cols = 0;
    other.data = nullptr;
}


Matrix::~Matrix() {
    delete[] data;
}


Matrix& Matrix::operator=(const Matrix& other) {
    if (this == &other) {
        return *this;
    }

    if (rows == other.rows && cols == other.cols) {
        std::memcpy(data, other.data, rows * cols * sizeof(double)); 
        return *this;
    } 

    if (rows * cols == other.rows * other.cols){
        rows = other.cols;
        cols = other.rows;
        std::memcpy(data, other.data, rows * cols * sizeof(double));
        return *this;
    }

    delete[] data;
    rows = other.rows;
    cols = other.cols;
    data = new double[rows * cols];
    std::memcpy(data, other.data, rows * cols * sizeof(double));
    printf("Srabotal operator kopirovaniya\n");
    
    return *this;
}


Matrix& Matrix::operator=(Matrix&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    rows = other.rows;
    cols = other.cols;
    data = other.data;
    other.rows = 0;
    other.cols = 0;
    other.data = nullptr;
    printf("Srabotal operator perenosa\n");

    return *this;
}


Matrix Matrix::operator+(const Matrix& other) const {
    if (rows != other.rows || cols != other.cols) {
        throw SIZE_MISMATCH;
    }
    Matrix result(rows, cols);
    for (size_t i = 0; i < rows * cols; ++i) {
        result.data[i] = data[i] + other.data[i];
    }
    printf("Srabotal operator +\n");
    return result;
}


Matrix Matrix::operator-(const Matrix& other) const {
    if (rows != other.rows || cols != other.cols) {
        throw SIZE_MISMATCH;
    }
    Matrix result(rows, cols);
    for (size_t i = 0; i < rows * cols; ++i) {
        result.data[i] = data[i] - other.data[i];
    }
    return result;
}


Matrix Matrix::operator*(const Matrix& other) const {
    if (cols != other.rows) {
        throw MULTIPLICATION_SIZE_MISMATCH;
    }
    Matrix result(rows, other.cols);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < other.cols; ++j) {
            double sum = 0;
            for (size_t k = 0; k < cols; ++k) {
                sum += data[i * cols + k] * other.data[k * other.cols + j];
            }
            result.data[i * result.cols + j] = sum;
        }
    }
    printf("Srabotal operator *\n");
    return result;
}


Matrix Matrix::operator*(double scalar) const {
    Matrix result(rows, cols);
    for (size_t i = 0; i < rows * cols; ++i) {
        result.data[i] = data[i] * scalar;
    }
    return result;
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


Matrix Matrix::power(size_t n) const {
    if (rows != cols) {
        throw MATRIX_NOT_SQUARE;
    }
    Matrix result(rows, cols);
    result.set_identity();
    Matrix base = *this;
    while (n > 0) {
        if (n % 2 == 1) {
            result = result * base;
        }
        base = base * base;
        n /= 2;
    }
    return result;
}


double Matrix::determinant() const {
    if (rows != cols) {
        throw MATRIX_NOT_SQUARE;
    }
    if (rows == 1) return data[0];
    if (rows == 2) return data[0] * data[3] - data[1] * data[2];
    if (rows == 3) {
        return data[0]*(data[4]*data[8] - data[5]*data[7]) -
               data[1]*(data[3]*data[8] - data[5]*data[6]) +
               data[2]*(data[3]*data[7] - data[4]*data[6]);
    }
    throw DETERMINANT_ERROR;
}


void Matrix::set_identity() {
    if (rows != cols) {
        throw MATRIX_NOT_SQUARE;
    }
    std::memset(data, 0, rows * cols * sizeof(double));
    for (size_t i = 0; i < rows; ++i) {
        data[i * cols + i] = 1.0;
    }
}


void Matrix::print() const {
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            printf("%8.2f ", data[i * cols + j]);
        }
        printf("\n");
    }
}


Matrix Matrix::exp(size_t steps) const {
    if (rows != cols) {
        throw MATRIX_NOT_SQUARE;
    }
    Matrix result(rows, cols);
    result.set_identity();
    Matrix term = result;
    for (size_t k = 1; k <= steps; ++k) {
        term = (term * (*this)) * (1.0 / k);
        result = result + term;
    }
    return result;
}


int main() {
    try {
        std::vector<double> A = {3,1,7,0,5,7,2,5,8};
        std::vector<double> B = {5,0,8,1,9,6,3,2,1};
        std::vector<double> C = {2,4,2,7,8,3,6,0,4};
        std::vector<double> R = {2,4,2,7,8,3,6,0,4};

        Matrix matA(3, 3, A);
        Matrix matB(3, 3, B);
        Matrix matC(3, 3, C);
        Matrix matR(3, 3, R);

        printf("Matrix A:\n");
        matA.print();
        printf("\nDeterminant of A: %.2f\n", matA.determinant());
        printf("\nMatrix B:\n");
        matB.print();

        printf("\nA + B:\n");
        (matA + matB).print();
        printf("\nA - B:\n");
        (matA - matB).print();
        printf("\nA * B:\n");
        (matA * matB).print();
        printf("\nA * 5:\n");
        (matA * 5).print();
        printf("\nTransposed A:\n");
        matA.transpose().print();
        printf("\nA^3:\n");
        matA.power(3).print();
        printf("\nMatrix exponential of A:\n"); 
        matA.exp(3).print();

        matR = matA * matB + matC;

    } catch (const MatrixException& e) {
        std::cerr << "Matrix Error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
