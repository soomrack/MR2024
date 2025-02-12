#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <stddef.h>
#include <iostream>

using namespace std;
typedef double MatrixItem;


class Matrix
{
private:
    size_t cols;
    size_t rows;
    double* data;

public:
    Matrix() : rows(0), cols(0), data(nullptr) {};
    Matrix(const size_t rows, const size_t cols);
    Matrix(const size_t rows, const size_t cols, double* data);
    Matrix(const Matrix& A);
    Matrix(Matrix&& A) noexcept;
    ~Matrix() { rows = 0; cols = 0; delete[] data; cout << ("destructor\n"); };

public:
    Matrix& operator = (const Matrix& A);
    Matrix& operator = (Matrix&& A) noexcept;
    Matrix& operator += (const Matrix& A);
    Matrix& operator -= (const Matrix& A);
    Matrix& operator *= (const Matrix& A);

    Matrix& operator + (const Matrix& A);
    Matrix& operator - (const Matrix& A);
    Matrix& operator * (const Matrix& A);



public:
    void print();
    void set_zero();
    void set_one();

};


class MatrixException : public domain_error
{
public:
    MatrixException(const string message) : domain_error{ message } {};
};


// exceptions
MatrixException MEMORY_ERROR("Memory allocation failed\n");
MatrixException DATA_ERROR("Matrix data is nullptr\n");
MatrixException SIZE_ERROR("Matrixes of different sizes\n");


Matrix::Matrix(const size_t rows, const size_t cols, double* value)
    :rows(rows), cols(cols), data(value)
{
    if (cols == 0 || rows == 0) {
        printf("Initialized matrix with 0 rows / cols\n");
        return;
    };

    if (sizeof(MatrixItem) * rows * cols >= SIZE_MAX) {
        throw MEMORY_ERROR;
    };

    data = new MatrixItem[rows * cols];
    memcpy(data, value, sizeof(MatrixItem) * rows * cols);
}


Matrix::Matrix(const size_t rows, const size_t cols)
    : rows(rows), cols(cols)
{
    if (cols == 0 || rows == 0) {
        printf("Initialized matrix with 0 rows / cols\n");
        return;
    };

    if (sizeof(MatrixItem) * rows * cols >= SIZE_MAX) {
        throw MEMORY_ERROR;
    };

    data = new MatrixItem[rows * cols];
}


Matrix::Matrix(const Matrix& A)
{
    rows = A.rows;
    cols = A.cols;
    data = new MatrixItem[rows * cols];
    memcpy(data, A.data, sizeof(MatrixItem));
}


Matrix::Matrix(Matrix&& A) noexcept : rows(A.rows), cols(A.cols), data(A.data)
{
    A.rows = 0;
    A.cols = 0;
    A.data = nullptr;
}


Matrix& Matrix::operator=(const Matrix& A)
{
    if (this == &A) { return *this; };

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


Matrix& Matrix::operator = (Matrix&& A) noexcept
{
    delete[] data;
    rows = A.rows;
    cols = A.cols;
    data = A.data;
    A.rows = 0;
    A.cols = 0;
    A.data = nullptr;
    return *this;
};


Matrix& Matrix::operator+=(const Matrix& A)
{
    if (rows != A.rows || cols != A.cols) {
        throw SIZE_ERROR;
    }

    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] += A.data[idx]; 
    }

    return *this;
}


Matrix& Matrix::operator -= (const Matrix& A)
{
    if (rows != A.rows || cols != A.cols) {
        throw SIZE_ERROR;
    }
    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] -= A.data[idx];
    }
    return *this;
}


Matrix& Matrix::operator *= (const Matrix& A)
{
    if (rows != A.rows || cols != A.cols) {
        throw SIZE_ERROR;
    }

    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] *= A.data[idx];
    }

    return *this;
}


Matrix& Matrix::operator + (const Matrix& A)
{
    if (A.cols != this->cols || A.rows != this->rows)
        throw SIZE_ERROR;

    Matrix* C = new Matrix(A.cols, A.rows);

    for (size_t idx = 0; idx < A.cols * A.rows; ++idx)
        C->data[idx] = A.data[idx] + this->data[idx];
    return *C;
}


Matrix& Matrix::operator - (const Matrix& A)
{
    if (A.cols != this->cols || A.rows != this->rows)
        throw SIZE_ERROR;

    Matrix* C = new Matrix(A.cols, A.rows);

    for (size_t idx = 0; idx < A.cols * A.rows; ++idx)
        C->data[idx] = A.data[idx] - this->data[idx];
    return *C;
}


Matrix& Matrix::operator*(const Matrix& A)
{
    if (A.cols != this->rows)
        throw SIZE_ERROR;

    Matrix* C = new Matrix(this->rows, A.cols);

    for (size_t C_col = 0; C_col < this->rows; ++C_col) {
        for (size_t C_row = 0; C_row < A.cols; ++C_row) {
            C->data[C_row + C_col * A.cols] = 0;
            for (size_t idx = 0; idx < this->cols; ++idx) {
                C->data[C_row + C_col * A.cols] += this->data[idx + (C_col * this->cols)]
                    * A.data[idx * A.cols + C_row];
            };
        };
    };
    return *C;
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
    if (data == nullptr) return;
    memset(data, 0, sizeof(MatrixItem) * cols * rows);
}


void Matrix::set_one()
{
    set_zero();
    for (size_t idx = 0; idx < rows * cols; idx += cols + 1)
        data[idx] = 1.;
}




/*

//TRANSPOSITION = A ^ T
void matrix_trans(struct Matrix A, struct Matrix T)
{
    for (size_t row = 0; row < A.rows; row++) {
        for (size_t col = 0; col < A.cols; col++) {
            T.data[col * A.rows + row] = A.data[row * A.cols + col];
        }
    }
}


// MINOR
struct Matrix Minor(const struct Matrix A, const size_t row, const size_t col)
{
    struct Matrix Minor = matrix_create(A.rows - 1, A.cols - 1);
    size_t idx = 0;

    for (size_t rowA = 0; rowA < Minor.rows; ++rowA) {
        if (rowA == row) continue;
        for (size_t colA = 0; colA < Minor.cols; ++colA) {
            if (colA == col) continue;
            Minor.data[idx] = A.data[rowA * A.cols + colA];
            ++idx;
        }
    }

    return Minor;
}


// DET A
double matrix_determinant(const struct Matrix A)
{
    if ((A.cols != A.rows) || A.rows == 0 || A.cols == 0) {
        return NAN;
    }

    double det = 0;
    int sign = 1;

    if (A.rows == 0) return NAN;
    if (A.rows == 1) return A.data[0];
    if (A.rows == 2) return (A.data[0] * A.data[3] - A.data[2] * A.data[1]);

    for (size_t idx = 0; idx < A.rows; idx++) {
        struct Matrix temp = Minor(A, 0, idx);
        det += sign * A.data[idx] * matrix_determinant(temp);
        sign = -sign;
        matrix_delete(&temp);
    }
    return det;
}


void determinant_print(struct Matrix A, const char* text)
{
    printf("%s\n", text);
    printf("%f\n", matrix_determinant(A));
    printf("\n");
}




// A = A * n
struct Matrix matrix_mult_number(const struct Matrix A, const double n) {
    if (A.data == NULL)  return MATRIX_NULL;

    struct Matrix Num = matrix_create(A.cols, A.rows);

    if (Num.data == NULL) return MATRIX_NULL;

    for (size_t idx = 0; idx < Num.cols * Num.rows; ++idx) {
        Num.data[idx] = A.data[idx] * n;
    }

    return Num;
}


// A = A^(-1)
struct Matrix matrix_inverse(const struct Matrix B) {
    if (B.cols != B.rows) {
        matrix_error(ERROR, "The matrix is ​​not square.\n");
        return MATRIX_NULL;
    }

    double det = matrix_determinant(B);
    if (det == 0) {
        printf("Error: Matrix is singular and cannot be inverted.\n");
        return MATRIX_NULL;
    }

    struct Matrix Inv = matrix_create(B.rows, B.cols);
    if (Inv.data == NULL) return MATRIX_NULL;

    for (size_t row = 0; row < Inv.rows; row++) {
        for (size_t col = 0; col < Inv.cols; col++) {
            struct Matrix MinorMatrix = Minor(B, row, col);
            Inv.data[col * B.rows + row] = pow(-1, row + col) * matrix_determinant(MinorMatrix) / det;
            matrix_delete(&MinorMatrix);
        }
    }
    return Inv;
}


struct Matrix matrix_exponent(const struct Matrix A, const unsigned long long int n) {
    if (A.cols != A.rows) {
        matrix_error(ERROR, "The matrix is ​​not square.\n");
        return MATRIX_NULL;
    }

    struct Matrix matrix_exponent_term = matrix_create(A.rows, A.cols);
    matrix_set_one(matrix_exponent_term);

    struct Matrix matrix_exponent_result = matrix_create(A.rows, A.cols);
    matrix_set_zero(matrix_exponent_result);

    if (n == 0) {
        matrix_delete(&matrix_exponent_result);
        return matrix_exponent_term;
    }

    double factorial = 1.0;

    for (unsigned long long int idx = 1; idx <= n; idx++) {
        struct Matrix temp = matrix_create(A.rows, A.cols);
        matrix_mult(temp, matrix_exponent_term, A);
        matrix_delete(&matrix_exponent_term);

        matrix_exponent_term = temp;
        factorial *= idx;

        matrix_mult_number(matrix_exponent_term, 1.0 / factorial);

        struct Matrix added = matrix_create(A.rows, A.cols);
        matrix_sum(added, matrix_exponent_result, matrix_exponent_term);

        matrix_delete(&matrix_exponent_result);
        matrix_exponent_result = added;
    }

    matrix_delete(&matrix_exponent_term);
    return matrix_exponent_result;
}

*/

int main()
{
    double src_data[9] = { 0., 1., 2., 3., 4., 5., 6., 7., 8. };
    double src_data1[9] = { 8., 7., 6., 5., 4., 3., 2., 1., 0. };

    Matrix A(3, 3, src_data);
    Matrix B(3, 3, src_data1);
    Matrix C(3,3);
    C = A + B;


    C.print();
    B.print();
    A.print();


}
