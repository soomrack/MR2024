#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stddef.h>
#include <iostream>

typedef double MatrixItem;


class Matrix {
private:
    size_t rows;
    size_t cols;
    double* data;
public:
    Matrix() : rows(0), cols(0), data(nullptr) { }
    Matrix(const size_t rows, const size_t cols);
    Matrix(const size_t rows, const size_t cols, double* data);
    Matrix(const Matrix& A);
    Matrix(Matrix&& A) noexcept;
    ~Matrix() { rows = 0; cols = 0; delete[] data; };
public:
    Matrix& operator=(const Matrix& A);
    Matrix& operator=(Matrix&& A) noexcept;
    Matrix& operator+=(const Matrix& A);
    Matrix& operator-=(const Matrix& A);
    Matrix& operator*=(const Matrix& A);
    Matrix operator+(const Matrix& A);
    Matrix operator-(const Matrix& A);
    Matrix operator*(const Matrix& A);
    Matrix operator*(double number) const;
public:
    void set_zero();
    void set_one();
    double determinant() const;
    Matrix transpose() const;
    Matrix minor(size_t exclude_row, size_t exclude_col) const;
    Matrix matrix_exponent(const unsigned long long int n);
    Matrix inverse() const;
    void print(const std::string& operation_name = "") const;
};


class MatrixException : public std::domain_error {
public:
    MatrixException(const std::string message) : std::domain_error{ message } {};
};


Matrix::Matrix(const size_t rows, const size_t cols, double* value)
    : rows(rows), cols(cols), data(value)
{
    if (cols == 0 || rows == 0) {
        throw MatrixException("Initialized matrix with 0 rows / cols");
    }

    if (sizeof(MatrixItem) * rows * cols >= SIZE_MAX / rows * cols) {
        throw MatrixException("Memory allocation failed");
    };

    data = new MatrixItem[rows * cols];
    memcpy(data, value, sizeof(MatrixItem) * rows * cols);
}


Matrix::Matrix(const size_t rows, const size_t cols)
    : rows(rows), cols(cols)
{
    if (cols == 0 || rows == 0) {
        throw MatrixException("Initialized matrix with 0 rows / cols");
    }

    if (rows > SIZE_MAX / cols / sizeof(MatrixItem)) {
        throw MatrixException("Memory allocation failed");
    };

    data = new MatrixItem[rows * cols];
}


Matrix::Matrix(const Matrix& A)
{
    rows = A.rows;
    cols = A.cols;
    data = new MatrixItem[rows * cols];
    memcpy(data, A.data, sizeof(MatrixItem) * rows * cols);
}


Matrix::Matrix(Matrix&& A) noexcept : rows(A.rows), cols(A.cols), data(A.data)
{
    A.rows = 0;
    A.cols = 0;
    A.data = nullptr;
}


Matrix& Matrix::operator=(const Matrix& A)
{
    if (this == &A) { return *this; }

    if (cols == A.cols && rows == A.rows) {
        memcpy(data, A.data, sizeof(MatrixItem) * rows * cols);
        return *this;
    }

    delete[] data;
    rows = A.rows;
    cols = A.cols;
    data = new MatrixItem[rows * cols];
    memcpy(data, A.data, sizeof(MatrixItem) * rows * cols);
    return *this;
}


Matrix& Matrix::operator=(Matrix&& A) noexcept
{
    delete[] data;
    rows = A.rows;
    cols = A.cols;
    data = A.data;
    A.rows = 0;
    A.cols = 0;
    A.data = nullptr;
    return *this;
}


Matrix& Matrix::operator+=(const Matrix& A)
{
    if (rows != A.rows || cols != A.cols) {
        throw MatrixException("Matrices of different sizes");
    }

    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] += A.data[idx];
    }
    return *this;
}


Matrix& Matrix::operator-=(const Matrix& A)
{
    if (rows != A.rows || cols != A.cols) {
        throw MatrixException("Matrices of different sizes");
    }
    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] -= A.data[idx];
    }
    return *this;
}


Matrix& Matrix::operator*=(const Matrix& A)
{
    if (rows != A.rows || cols != A.cols) {
        throw MatrixException("Matrices of different sizes");
    }

    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] *= A.data[idx];
    }
    return *this;
}


Matrix Matrix::operator+(const Matrix& A)
{
    Matrix result(*this);
    result += A;
    return result;
}


Matrix Matrix::operator-(const Matrix& A)
{
    Matrix result(*this);
    result -= A;
    return result;
}


Matrix Matrix::operator*(const Matrix& A)
{
    if (cols != A.rows) {
        throw MatrixException("Matrices dimensions do not match for multiplication");
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


Matrix Matrix::operator*(double number) const
{
    Matrix result(rows, cols);
    for (size_t i = 0; i < rows * cols; i++) {
        result.data[i] = data[i] * number;
    }
    return result;
}


void Matrix::set_zero()
{
    if (data == nullptr) return;
    memset(data, 0, sizeof(MatrixItem) * cols * rows);
}


void Matrix::set_one()
{
    set_zero();
    for (size_t idx = 0; idx < rows * cols; idx += cols + 1)
        data[idx] = 1.;
}


Matrix Matrix::transpose() const
{
    Matrix result(cols, rows);
    for (size_t row = 0; row < rows; ++row)
        for (size_t col = 0; col < cols; ++col)
            result.data[col * rows + row] = data[row * cols + col];
    return result;
}


Matrix Matrix::minor(size_t exclude_row, size_t exclude_col) const
{
    if (rows <= 1 || cols <= 1) {
        throw MatrixException("Cannot calculate minor of a matrix with size less than 2");
    }

    Matrix Minor(rows - 1, cols - 1);
    size_t minor_row = 0;
    size_t minor_col = 0;

    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            if (row == exclude_row || col == exclude_col) {
                continue;
            }
            Minor.data[minor_row * Minor.cols + minor_col] = data[row * cols + col];
            minor_col++;
            if (minor_col == Minor.cols) {
                minor_col = 0;
                minor_row++;
            }
        }
    }
    return Minor;
}


Matrix Matrix::inverse() const
{
    if (cols != rows) {
        throw MatrixException("Matrix dimensions are not equal");
    }

    double det = this->determinant();
    if (det == 0) {
        throw MatrixException("Matrix is singular and can't be inverted");
    }

    Matrix Inv(rows, cols);
    for (size_t row = 0; row < Inv.rows; row++) {
        for (size_t col = 0; col < Inv.cols; col++) {
            Matrix minor_matrix = this->minor(row, col);
            Inv.data[col * rows + row] = pow(-1, row + col) * minor_matrix.determinant() / det;
        }
    }
    return Inv;
}


Matrix Matrix::matrix_exponent(const unsigned long long int n)
{
    if (cols != rows) {
        throw MatrixException("Matrix dimensions are not equal");
    }

    Matrix matrix_exponent_term(*this);
    Matrix matrix_exponent_result(rows, cols);
    matrix_exponent_result.set_one();

    if (n == 0) {
        return matrix_exponent_result;
    }

    double factorial = 1.0;

    for (unsigned long long int idx = 1; idx <= n; idx++) {
        factorial *= idx;
        matrix_exponent_term = matrix_exponent_term * (*this) * (1.0 / factorial);
        matrix_exponent_result += matrix_exponent_term;
    }
    return matrix_exponent_result;
}


double Matrix::determinant() const
{
    if (cols != rows) {
        throw MatrixException("Matrix dimensions are not equal");
    }

    if (rows == 1) return data[0];

    if (rows == 2) return (data[0] * data[3] - data[1] * data[2]);

    double det = 0.0;
    int sign = 1;

    for (size_t col = 0; col < cols; col++) {
        Matrix temp_matrix = this->minor(0, col);
        det += sign * data[col] * temp_matrix.determinant();
        sign = -sign;
    }
    return det;
}


void Matrix::print(const std::string& operation_name) const
{
    if (!operation_name.empty()) {
        std::cout << operation_name << ":\n";
    }

    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++)
            printf("%.2f ", data[row * cols + col]);
        printf("\n");
    }
    printf("\n");
}


int main() {
    double src_data_A[4] = { 1., 3., 5., 7., };
    double src_data_B[4] = { 1., 2., 3., 4., };
    double src_data_C[4] = { 9., 8., 7., 6., };

    Matrix A(2, 2, src_data_A);
    Matrix B(2, 2, src_data_B);
    Matrix C(2, 2, src_data_C);

    Matrix sum = A + B;
    Matrix subtract = A - B;
    Matrix mult = A * B;
    Matrix mult_by_number = A * 10;
    Matrix transposed = A.transpose();
    Matrix inversed = A.inverse();
    Matrix exponent = A.matrix_exponent(3);
    double det = A.determinant();

    A.print("Matrix A");
    B.print("Matrix B");
    C.print("Matrix C");
    sum.print("A + B");
    subtract.print("A - B");
    mult.print("A * B");
    mult_by_number.print("A * 3");
    transposed.print("A Transposed");
    inversed.print("A Inversed");
    exponent.print("A^3");
    std::cout << "Determinant of A: " << det << std::endl;
}
