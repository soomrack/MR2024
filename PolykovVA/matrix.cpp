#include <iostream>
#include <stdexcept>
#include <cmath>
#include <cstring>

using namespace std;

typedef double MatrixItem;

class Matrix {
private:
    size_t rows;
    size_t cols;
    MatrixItem* data;

public:
    Matrix() : rows(0), cols(0), data(nullptr) { cout << "Default Constructor\n"; }
    Matrix(size_t rows, size_t cols);
    Matrix(size_t rows, size_t cols, const MatrixItem* values);
    Matrix(const Matrix& A);
    Matrix(Matrix&& A) noexcept;
    ~Matrix();

    Matrix& operator=(const Matrix& A);
    Matrix& operator=(Matrix&& A) noexcept;
    Matrix operator+(const Matrix& A) const;
    Matrix operator-(const Matrix& A) const;
    Matrix operator*(const Matrix& A) const;
    Matrix operator*(double scalar) const;

    void set_zero();
    void set_one();
    double determinant() const;
    Matrix transpose() const;
    Matrix minor(size_t exclude_row, size_t exclude_col) const;
    Matrix inverse() const;
    void print() const;
};

// Custom exception class
class MatrixException : public domain_error {
public:
    explicit MatrixException(const string& message) : domain_error(message) {}
};

// Constructor
Matrix::Matrix(size_t rows, size_t cols) : rows(rows), cols(cols) {
    if (rows == 0 || cols == 0) {
        throw MatrixException("Matrix cannot have zero rows or columns");
    }
    data = new MatrixItem[rows * cols]();
}

// Constructor with data
Matrix::Matrix(size_t rows, size_t cols, const MatrixItem* values) : Matrix(rows, cols) {
    memcpy(data, values, rows * cols * sizeof(MatrixItem));
}

// Destructor
Matrix::~Matrix() {
    delete[] data;
    cout << "Destructor\n";
}

// Copy Assignment
Matrix& Matrix::operator=(const Matrix& A) {
    if (this != &A) {
        delete[] data;
        rows = A.rows;
        cols = A.cols;
        data = new MatrixItem[rows * cols];
        memcpy(data, A.data, rows * cols * sizeof(MatrixItem));
    }
    return *this;
}

// Move Assignment
Matrix& Matrix::operator=(Matrix&& A) noexcept {
    if (this != &A) {
        delete[] data;
        rows = A.rows;
        cols = A.cols;
        data = A.data;
        A.rows = 0;
        A.cols = 0;
        A.data = nullptr;
    }
    return *this;
}

// Addition
Matrix Matrix::operator+(const Matrix& A) const {
    if (rows != A.rows || cols != A.cols) {
        throw MatrixException("Matrix dimensions must match for addition");
    }
    Matrix result(rows, cols);
    for (size_t i = 0; i < rows * cols; ++i) {
        result.data[i] = data[i] + A.data[i];
    }
    return result;
}

// Subtraction
Matrix Matrix::operator-(const Matrix& A) const {
    if (rows != A.rows || cols != A.cols) {
        throw MatrixException("Matrix dimensions must match for subtraction");
    }
    Matrix result(rows, cols);
    for (size_t i = 0; i < rows * cols; ++i) {
        result.data[i] = data[i] - A.data[i];
    }
    return result;
}

// Scalar Multiplication
Matrix Matrix::operator*(double scalar) const {
    Matrix result(rows, cols);
    for (size_t i = 0; i < rows * cols; ++i) {
        result.data[i] = data[i] * scalar;
    }
    return result;
}

// Matrix Multiplication
Matrix Matrix::operator*(const Matrix& A) const {
    if (cols != A.rows) {
        throw MatrixException("Matrix dimensions must match for multiplication");
    }

    Matrix result(rows, A.cols);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < A.cols; ++j) {
            result.data[i * A.cols + j] = 0;
            for (size_t k = 0; k < cols; ++k) {
                result.data[i * A.cols + j] += data[i * cols + k] * A.data[k * A.cols + j];
            }
        }
    }
    return result;
}

// Set to zero matrix
void Matrix::set_zero() {
    memset(data, 0, rows * cols * sizeof(MatrixItem));
}

// Set to identity matrix
void Matrix::set_one() {
    set_zero();
    for (size_t i = 0; i < min(rows, cols); ++i) {
        data[i * cols + i] = 1.0;
    }
}

// Transpose
Matrix Matrix::transpose() const {
    Matrix result(cols, rows);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            result.data[j * rows + i] = data[i * cols + j];
        }
    }
    return result;
}

// Determinant
double Matrix::determinant() const {
    if (rows != cols) {
        throw MatrixException("Determinant requires a square matrix");
    }
    Matrix temp(*this);
    double det = 1.0;

    for (size_t i = 0; i < rows; ++i) {
        if (temp.data[i * cols + i] == 0) return 0;
        for (size_t j = i + 1; j < rows; ++j) {
            double factor = temp.data[j * cols + i] / temp.data[i * cols + i];
            for (size_t k = i; k < cols; ++k) {
                temp.data[j * cols + k] -= factor * temp.data[i * cols + k];
            }
        }
        det *= temp.data[i * cols + i];
    }
    return det;
}

// Print Matrix
void Matrix::print() const {
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            cout << data[i * cols + j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

// MAIN FUNCTION
int main() {
    double src_data[9] = { 0, 1, 2, 3, 9, 5, 6, 7, 8 };
    double src_data1[9] = { 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    double src_data2[9] = { 3, 5, 8, 6, 4, 6, 9, 2, 1 };

    Matrix A(3, 3, src_data);
    Matrix B(3, 3, src_data1);
    Matrix C(3, 3, src_data2);

    Matrix SUM = A + B;
    Matrix SUB = A - B;
    Matrix MULT = A * B;
    Matrix MULT_SCAL = A * 3;
    Matrix TRAN = A.transpose();
    double DET = A.determinant();

    cout << "Matrix A:\n"; A.print();
    cout << "Matrix B:\n"; B.print();
    cout << "Sum:\n"; SUM.print();
    cout << "Subtraction:\n"; SUB.print();
    cout << "Multiplication:\n"; MULT.print();
    cout << "Scalar Multiplication:\n"; MULT_SCAL.print();
    cout << "Transpose of A:\n"; TRAN.print();
    cout << "Determinant of A: " << DET << endl;

    return 0;
}
