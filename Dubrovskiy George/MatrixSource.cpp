#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <stdexcept>
#include <iostream>

using namespace std;

class Matrix
{
private:
    size_t rows;
    size_t cols;
    double* data;
public:
    Matrix();
    Matrix(const size_t rows, const size_t cols);
    Matrix(const size_t rows, const size_t cols, const double* data);
    Matrix(const Matrix& A);
    Matrix(Matrix&& A) noexcept;
    ~Matrix();
public:
    Matrix& operator=(const Matrix& A);
    Matrix& operator=(Matrix&& A) noexcept;
    Matrix& operator+=(const Matrix& A);
    Matrix operator+(const Matrix& A);
    Matrix& operator-=(const Matrix& A);
    Matrix operator-(const Matrix& A);
    Matrix& operator*=(const Matrix& A);
    Matrix operator*(const Matrix& A);
    Matrix& operator*=(const double number);
    Matrix operator*(const double number);
public:
    void print();
    void set_zero();
    void set_one();
    double determinant(const Matrix& A);
    Matrix transpose(const Matrix& A);
    Matrix minor(size_t exclude_row, size_t exclude_col, const Matrix& A);
    Matrix inverse(const Matrix& A);
};


class MatrixException : public domain_error
{
public:
    MatrixException(const string message) : domain_error{ message } {};
};


Matrix::Matrix() {
    rows = 0;
    cols = 0;
    data = nullptr;
}

Matrix::Matrix(const size_t rows, const size_t cols) : rows(rows), cols(cols), data(nullptr) {
    if (rows == 0 || cols == 0) {
        throw std::runtime_error("Matrix Null\n");
    }
    if (sizeof(double) * rows * cols >= SIZE_MAX) {
        throw std::runtime_error("Size matrix error\n");
    };

    data = new double[rows * cols];
}

Matrix::Matrix(const size_t rows, const size_t cols, const double* value) : rows(rows), cols(cols) {
    if (rows == 0 || cols == 0) {
        throw std::runtime_error("Matrix Null\n");
        if (sizeof(double) * rows * cols >= SIZE_MAX) {
            throw std::runtime_error("Size matrix error\n");
        }

        data = new double[rows * cols];

        std::memcpy(data, value, rows * cols * sizeof(double));
    }

    Matrix::Matrix(const Matrix & A) {
        rows = A.rows;
        cols = A.cols;
        data = new double[rows * cols];
        memcpy(data, A.data, sizeof(double) * rows * cols);
    }

    Matrix::Matrix(Matrix && A) noexcept : rows(A.rows), cols(A.cols), data(A.data)
    {
        A.rows = 0;
        A.cols = 0;
        A.data = nullptr;
    }

    Matrix::~Matrix() {
        rows = 0;
        cols = 0;
        delete[] data;
    }


    Matrix& Matrix::operator=(const Matrix & A)
    {
        if (this == &A) {
            return *this;
        }
        if (cols == A.cols && rows == A.rows) {
            memcpy(data, A.data, cols * rows * sizeof(double));
            return *this;
        }

        delete[] data;
        rows = A.rows;
        cols = A.cols;
        data = new double[rows * cols];
        memcpy(data, A.data, cols * rows * sizeof(double));
        return *this;
    }


    Matrix& Matrix::operator=(Matrix && A) noexcept
    {
        if (this == &A) {
            return *this;
        }
        delete[] data;
        rows = A.rows;
        cols = A.cols;
        data = A.data;

        A.rows = 0;
        A.cols = 0;
        A.data = nullptr;
        return *this;
    }


    void Matrix::set_zero()
    {
        if (data == nullptr) return;
        memset(data, 0, sizeof(double) * cols * rows);
    }


    void Matrix::set_one()
    {
        set_zero();
        for (size_t idx = 0; idx < rows * cols; idx += cols + 1)
            data[idx] = 1.;
    }


    //sum
    Matrix& Matrix::operator+=(const Matrix & A)
    {
        if (rows != A.rows || cols != A.cols) {
            throw MatrixException("Matrices of different sizes\n");
        }

        for (size_t idx = 0; idx < rows * cols; idx++) {
            data[idx] += A.data[idx];
        }
        return *this;
    }

    Matrix Matrix::operator+(const Matrix & A)
    {
        Matrix Sum(*this);
        Sum += A;
        return Sum;
    }


    //substruct
    Matrix& Matrix::operator-=(const Matrix & A)
    {

        if (rows != A.rows || cols != A.cols) {
            throw MatrixException("Matrixes of different sizes\n");
        }

        for (size_t idx = 0; idx < rows * cols; idx++) {
            data[idx] -= A.data[idx];
        }
        return *this;
    }


    Matrix Matrix::operator-(const Matrix & A)
    {
        Matrix Sub(*this);
        Sub -= A;
        return Sub;
    }


    //multiply with number
    Matrix& Matrix::operator*=(const double number)
    {
        for (size_t idx = 0; idx < rows * cols; idx++) {
            data[idx] *= number;
        }
        return *this;
    }

    Matrix Matrix::operator*(const double number)
    {
        Matrix mult(*this);
        mult *= number;
        return mult;
    }


    //mult matrix
    Matrix& Matrix::operator*=(const Matrix & A)
    {
        if (cols != A.rows) {
            throw MatrixException("Matrixes of different sizes\n");
        }

        Matrix mult(rows, A.cols);
        for (size_t row = 0; row < mult.rows; row++) {
            for (size_t col = 0; col < mult.cols; col++) {
                mult.data[row * A.cols + col] = 0;
                for (size_t idx = 0; idx < cols; idx++) {
                    mult.data[row * mult.cols + col] += data[row * cols + idx] * A.data[idx * A.cols + col];
                }
            }
        }
        *this = mult;
        return *this;
    }


    Matrix Matrix::operator*(const Matrix & A)
    {
        if (cols != A.rows) {
            throw MatrixException("Matrixes of different sizes\n");
        }

        Matrix result(rows, A.cols);
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < A.cols; j++) {
                result.data[i * A.cols + j] = 0;
                for (size_t k = 0; k < cols; k++) {
                    result.data[i * A.cols + j] += data[i * cols + k] * A.data[k * A.cols + j];
                }
            }
        }

        return result;
    }

    //transpose
    Matrix Matrix::transpose(const Matrix & A)
    {
        Matrix result(cols, rows);
        for (size_t row = 0; row < rows; ++row)
            for (size_t col = 0; col < cols; ++col)
                result.data[col * rows + row] = data[row * cols + col];
        return result;
    }


    //minor
    Matrix Matrix::minor(size_t exclude_row, size_t exclude_col, const Matrix & A)
    {
        if (rows <= 1 || cols <= 1) {
            throw MatrixException("Cannot calculate. Matrix size less than 2\n");
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


    //determinant
    double Matrix::determinant(const Matrix & A)
    {
        if (rows != cols) {
            throw std::runtime_error("Matrix is not square\n");
        }

        if (rows == 1) {
            return data[0];
        }

        if (rows == 2) {
            return data[0] * data[3] - data[1] * data[2];
        }

        double det = 0.0;
        double sign = 1.0;

        for (size_t j = 0; j < cols; j++) {
            Matrix submatrix(rows - 1, cols - 1);
            size_t sub_i = 0;

            for (size_t i = 1; i < rows; i++) {
                size_t sub_j = 0;
                for (size_t k = 0; k < cols; k++) {
                    if (k == j) continue;
                    submatrix.data[sub_i * (cols - 1) + sub_j] = data[i * cols + k];
                    sub_j++;
                }
                sub_i++;
            }

            det += sign * data[j] * submatrix.determinant(A);
            sign = -sign;
        }

        return det;
    }



    Matrix Matrix::inverse(const Matrix & A)
    {
        if (cols != rows) {
            throw MatrixException("Matrix is not square\n");
        }

        Matrix inv(rows, cols);
        double det = this->determinant(A);
        for (size_t row = 0; row < inv.rows; row++) {
            for (size_t col = 0; col < inv.cols; col++) {
                Matrix MinorMatrix = this->minor(row, col, A);
                inv.data[col * rows + row] = pow(-1, row + col) * MinorMatrix.determinant(A) / det;
            }
        }
        return inv;
    }


    /*void Matrix::matrix_fill(const double* values)
    {
        if (!values) {
            throw std::runtime_error("������� ������������ ������ ��������.");
        }

        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                data[i * cols + j] = values[i * cols + j];
            }
        }
    }
    */

    void Matrix::print()
    {
        for (size_t row = 0; row < rows; row++) {
            for (size_t col = 0; col < cols; col++)
                printf("%.f ", data[row * cols + col]);
            printf("\n");
        }
        printf("\n");
    }


    void calculations()
    {
        double src_data[4] = { 0., 1., 2., 3. };
        double src_data1[4] = { 3., 2., 1., 0. };
        double src_data2[9] = { 1., 2., 3., 4., 5., 6., 7., 8., 9. };

        Matrix A(2, 2, src_data);
        Matrix B(2, 2, src_data1);
        Matrix C(3, 3, src_data2);



        Matrix SUM = A + B;

        Matrix SUB = A - B;

        Matrix MULT = A * B;

        Matrix MULT_SCAL = A * 3;

        Matrix TRAN = A.transpose(A);

        Matrix MIN = C.minor(2, 2, C);

        Matrix INV = A.inverse(A);

        double DET = A.determinant(A);

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

    }


    int main()
    {
        calculations();
        return 0;
    }
