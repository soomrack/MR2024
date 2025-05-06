#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <stdexcept>
#include <iostream>

using namespace std;
typedef double MatrixItem;

class Matrix 
{
private:
    size_t rows;
    size_t cols;
    double *data;
public:
    Matrix() : rows(0), cols(0), data(nullptr){ cout << ("constructor\n"); };
    Matrix(const size_t rows, const size_t cols); 
    Matrix(const size_t rows, const size_t cols, const double *value);
    Matrix(const Matrix& M);
    Matrix(Matrix&& M) noexcept;
    ~Matrix();
public:
    Matrix& operator=(const Matrix& M);
    Matrix& operator=(Matrix&& M) noexcept;
    Matrix& operator+=(const Matrix& M);
    Matrix& operator-=(const Matrix& M);
    Matrix& operator*=(const Matrix& M);
    Matrix& operator*=(const double number);
    Matrix operator+(const Matrix& M) const;
    Matrix operator-(const Matrix& M) const;
    Matrix operator*(const Matrix& M) const;
    Matrix operator*(const double number) const;
public:
    void print();
    void set_zero();
    void set_one();
    double determinant() const;
    Matrix transpose() const;
    Matrix minor(size_t exclude_row, size_t exclude_col) const;
    Matrix matrix_exponent(const unsigned long long int n);
    Matrix inverse() const;
};


class MatrixException : public domain_error
{
public:
    MatrixException(const string message) : domain_error{ message } {};
};


Matrix::Matrix(const size_t rows, const size_t cols) : rows(rows), cols(cols) 
{
    cout << ("constructor_r_c\n");
    if (cols == 0 || rows == 0) {
        throw MatrixException("Initialized matrix with 0 rows / cols\n");
        return;
    };

    if (rows > SIZE_MAX / cols / sizeof(MatrixItem)) {
        throw MatrixException("Memory allocation failed\n");
    };

    data = new MatrixItem[cols * rows];
}


Matrix::Matrix(const size_t rows, const size_t cols, const double *value) : rows(rows), cols(cols), data(nullptr) 
{
    cout << ("constructor data\n");
    if (cols == 0 || rows == 0) {
        throw MatrixException("Initialized matrix with 0 rows / cols\n");
        return;
    };

    if (rows > SIZE_MAX / cols / sizeof(MatrixItem)) {
        throw MatrixException("Memory allocation failed\n");
    };

    data = new MatrixItem[cols * rows];
    memcpy(data, value, sizeof(MatrixItem) * rows * cols);
}


Matrix::Matrix(const Matrix& M)
{
    cout << ("copy\n");
    rows = M.rows;
    cols = M.cols;
    data = new double[rows * cols];
    memcpy(data, M.data, sizeof(MatrixItem) * rows * cols);
}


Matrix::Matrix(Matrix&& M) noexcept
{
    cout << ("move\n");
    rows = M.rows;
    cols = M.cols;
    data = M.data;

    M.rows = 0;
    M.cols = 0;
    M.data = nullptr;
}


Matrix::~Matrix()
{
    rows = 0;
    cols = 0;
    delete[] data;
    data = nullptr;
    cout << ("destructor\n");
}


Matrix& Matrix::operator=(const Matrix& M)
{
    if (this == &M) {
        return *this;
    }
    if (cols == M.cols && rows == M.rows) {
       memcpy(data, M.data, cols * rows * sizeof(double));
       return *this;
    }

    delete[] data;
    rows = M.rows;
    cols = M.cols;
    data = new double[rows * cols];
    memcpy(data, M.data,cols * rows * sizeof(double));
    return *this;
}


Matrix& Matrix::operator=(Matrix&& M) noexcept
{
    cout << ("move\n");
    if (this == &M) {
        return *this;
    }
    delete[] data;
    rows = M.rows;
    cols = M.cols;
    data = M.data;

    M.rows = 0;
    M.cols = 0;
    M.data = nullptr;
    return *this;
}


Matrix& Matrix::operator+=(const Matrix& M)
{
    if (rows != M.rows || cols != M.cols) {
        throw MatrixException("Matrixes of different sizes\n");
    }

    for (size_t idx=0; idx < rows * cols; idx++) {
        data[idx] += M.data[idx];
    }
    return *this;
}


Matrix& Matrix::operator-=(const Matrix& M)
{

    if (rows != M.rows || cols != M.cols) {
        throw MatrixException("Matrixes of different sizes\n");
    }

    for (size_t idx=0; idx < rows * cols; idx++) {
        data[idx] -= M.data[idx];
    }
    return *this;
}


Matrix& Matrix::operator*=(const Matrix& M)
{
    if (cols != M.rows) {
        throw MatrixException("Incompatible matrix sizes for multiplication\n");
    }

    Matrix mult(rows, M.cols);
    for(size_t row = 0; row < mult.rows; row++ ) {
        for(size_t col = 0; col < mult.cols; col++) {
            for (size_t idx = 0; idx < cols; idx++) {
                mult.data[row * mult.cols + col] += data[row * cols + idx] * M.data[idx * M.cols + col];
            }
        }
    }
    *this = mult;
    return *this;
}


Matrix& Matrix::operator*=(const double number)
{
    for(size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] *= number;
    }
    return *this;
}


Matrix Matrix::operator+(const Matrix& M) const 
{
    Matrix sum(*this);
    sum += M;
    return sum;
}


Matrix Matrix::operator-(const Matrix& M) const 
{
    Matrix sub(*this);
    sub -= M;
    return sub;
}


Matrix Matrix::operator*(const Matrix& M) const
{
    if (cols != M.rows) {
        throw MatrixException("Incompatible matrix sizes for multiplication\n");
    }
    Matrix mult(*this);
    mult *= M;
    return mult;
}


Matrix Matrix::operator*(const double number) const
{
    Matrix mult(*this);
    mult *= number;
    return mult;
}


void Matrix::print() 
{
    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++)
            printf("%.f ", data[row * cols + col]);
        printf("\n");
    }
    printf("\n");
}


void Matrix::set_zero()
{
    memset(data, 0, sizeof(MatrixItem) * cols * rows);
}


void Matrix::set_one()
{
    set_zero();
    for (size_t idx=0; idx < rows * cols; idx += cols +1)
        data[idx] = 1;
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
        throw MatrixException("Cannot compute minor of a matrix with size <= 1");
    }

    Matrix Minor(rows -1, cols -1);
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


double Matrix::determinant() const
{
    if (cols != rows) {
        throw MatrixException("Matrix is not square\n");
    }

    if (rows == 1) return data[0];

    if (rows == 2 && cols == 2) return (data[0] * data[3] - data[1] * data[2]);

    double det = 0.0;
    int sign = 1;

    for (size_t col = 0; col < cols; col++) {
        Matrix temp = this->minor(0, col);
        det += sign * data[col] * temp.determinant();
        sign = -sign;
    }
    return det;
}


Matrix Matrix::matrix_exponent(const unsigned long long int n)
{
    if (cols != rows) {
        throw MatrixException("Matrix is not square\n");
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


Matrix Matrix::inverse() const
{
    if (cols != rows) {
        throw MatrixException("Matrix is not square\n");
    }

    Matrix Inv(rows, cols);
    double det = this->determinant();
    for (size_t row = 0; row < Inv.rows; row++) {
        for (size_t col = 0; col < Inv.cols; col++) {
            Matrix MinorMatrix = this->minor(row, col);
            Inv.data[col * rows + row] = pow(-1, row + col) * MinorMatrix.determinant() / det;
        }
    }
    return Inv;
}


int main()
{
    double src_data[9] = { 0., 1., 2., 3., 9., 5., 6., 7., 8. };
    double src_data1[9] = { 8., 7., 6., 5., 4., 3., 2., 1., 0. };
    double src_data2[9] = { 3., 5., 8., 6., 4., 6., 9., 2., 1. };

    Matrix A(3, 3, src_data);
    Matrix B(3, 3, src_data1);
    Matrix C(3, 3, src_data2);
    cout << "Creating SUM\n";
    Matrix SUM = A + B;
    cout << "Creating SUB\n";
    Matrix SUB = A - B;
    cout << "Creating MULT\n";
    Matrix MULT = A * B;
    cout << "Creating MULT_SCAL\n";
    Matrix MULT_SCAL = A * 3;
    cout << "Creating TRAN\n";
    Matrix TRAN = A.transpose();
    cout << "Creating MIN\n";
    Matrix MIN = A.minor(2, 2);
    cout << "Creating INV\n";
    Matrix INV = A.inverse();
    cout << "Creating EXP\n";
    Matrix EXP = A.matrix_exponent(3);
    cout << "Creating DET\n";
    double DET = A.determinant();
   
    A.print();
    B.print();
    C.print();
    SUM.print();
    SUB.print();
    MULT.print();
    MULT_SCAL.print();
    TRAN.print();
    MIN.print();
    INV.print();
    EXP.print();
}