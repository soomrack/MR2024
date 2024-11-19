#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <stddef.h>



typedef double MatrixItem;
struct Matrix
{
    size_t cols;
    size_t rows;
    MatrixItem* data;
};


const struct Matrix MATRIX_NULL = { 0,0,NULL };


struct Matrix matrix_create(const size_t cols, const size_t rows)
{
    if (cols == 0 || rows == 0) {
        struct Matrix A = { cols,rows, NULL };
        return A;
    }
    if (rows >= SIZE_MAX / sizeof(MatrixItem) / cols) return MATRIX_NULL;
    struct Matrix A = { cols,rows, NULL };
    size_t total = sizeof(MatrixItem) * A.cols * A.rows;
    A.data = (MatrixItem*)malloc(total);
    if (A.data == NULL) return MATRIX_NULL;
    return A;
}


void matrix_set_zero(struct Matrix A)
{
    if (A.data == NULL) return;
    memset(A.data, 0, sizeof(MatrixItem) * A.cols * A.rows);
}


void matrix_set_one(struct Matrix A)
{
    matrix_set_zero(A);
    for (size_t idx = 0; idx < A.cols * A.rows; idx += A.cols + 1)
        A.data[idx] = 1;
}


void matrix_error()
{
    printf("Error: different number of columns or rows\n");
}


void matrix_delete(struct Matrix* A)
{
    A->rows = 0;
    A->cols = 0;
    free(A->data);
    A->data = NULL;
}


void matrix_fill(struct Matrix A, const MatrixItem values[])
{
    for (size_t row = 0; row < A.rows; row++) {
        for (size_t col = 0; col < A.cols; col++) {
            A.data[col * A.rows + row] = values[row * A.cols + col];
        }
    }
}


// A = B + C
void matrix_sum(const struct Matrix A, const struct Matrix B, const struct Matrix C)
{
    if (B.cols != C.cols || B.rows != C.rows || B.cols != A.cols || B.rows != A.rows) {
        matrix_error();
        return;
    }
    for (size_t idx = 0; idx < B.cols * B.rows; ++idx) {
        A.data[idx] = B.data[idx] + C.data[idx];
    }
}

// A = B - C
void matrix_substraction(const struct Matrix A, const struct Matrix B, const struct Matrix C)
{
    if (B.cols != C.cols || B.rows != C.rows || B.cols != A.cols || B.rows != A.rows) {
        matrix_error();
        return;
    }
    for (size_t idx = 0; idx < B.cols * B.rows; ++idx) {
        A.data[idx] = B.data[idx] - C.data[idx];
    }
}


// A = B * C
void matrix_mult(const struct Matrix A, const struct Matrix B, const struct Matrix C)
{
    if (B.cols != C.rows || B.rows != A.rows || A.rows != C.cols)
    {
        matrix_error();
    }
    struct Matrix Mult = matrix_create(A.rows, A.cols);
    MatrixItem data_old;
    for (size_t row = 0; row < B.rows; row++) {
        for (size_t col = 0; col < B.cols; col++) {
            data_old = 0;
            for (size_t idx = 0; idx < C.rows; idx++) {
                data_old += B.data[idx * B.rows + col] * C.data[row * C.cols + idx];
            }
            Mult.data[row * B.cols + col] = data_old;
        }
    }
    memcpy(A.data, Mult.data, sizeof(MatrixItem) * Mult.cols * Mult.rows);
    matrix_delete(&Mult);
}


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
    for (size_t rowA = 0; rowA < A.rows; ++rowA) {
        if (rowA == row) continue;
        for (size_t colA = 0; colA < A.cols; ++colA) {
            if (colA == col) continue;
            Minor.data[idx] = A.data[rowA * A.cols + colA];
            ++idx;
        }
    }
    return Minor;
}


// DET A
double matrix_determinant(const struct Matrix A) {
    if (A.rows != A.cols) {
        matrix_error();
        return NAN;
    }
    double det = 0;
    int k = 1;
    if (A.rows == 0) return NAN;
    if (A.rows == 1) return A.data[0];
    if (A.rows == 2) return (A.data[0] * A.data[3] - A.data[2] * A.data[1]);
    for (size_t idx = 0; idx < A.rows; idx++) {
        struct Matrix temp = Minor(A, 0, idx);
        det += k * A.data[idx] * matrix_determinant(temp);
        k = -k;
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


// A += B
int matrix_add(struct Matrix A, struct Matrix B)
{
    if (A.cols != B.cols || A.rows != B.rows) return 1;

    for (size_t idx = 0; idx < A.cols * A.rows; ++idx)
        A.data[idx] += B.data[idx];
    return 0;
}


// A = A * n
void matrix_mult_number(struct Matrix A, const double n) {
    if (A.data == NULL) {
        matrix_error();
        return;
    }
    for (size_t idx = 0; idx < A.cols * A.rows; ++idx) {
        A.data[idx] *= n;
    }
}


// A = A*(-1)
struct Matrix matrix_inverse(const struct Matrix B) {
    if (B.rows != B.cols) {
        matrix_error();
        return MATRIX_NULL;
    }
    double det = matrix_determinant(B);
    if (det == 0) {
        printf("Error: Matrix is singular and cannot be inverted.\n");
        return MATRIX_NULL;
    }

    struct Matrix Inv = matrix_create(B.rows, B.cols);
    if (Inv.data == NULL) return MATRIX_NULL;

    for (size_t row = 0; row < B.rows; row++) {
        for (size_t col = 0; col < B.cols; col++) {
            struct Matrix MinorMatrix = Minor(B, row, col);
            Inv.data[col * B.rows + row] = pow(-1, row + col) * matrix_determinant(MinorMatrix) / det;
            matrix_delete(&MinorMatrix);
        }
    }
    return Inv;
}


struct Matrix matrix_exponent(const struct Matrix A, const unsigned long long int n) {
    if (A.rows != A.cols || n < 0) {
        matrix_error();
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

        struct Matrix added = matrix_create(A.rows, A.cols);
        matrix_mult_number(matrix_exponent_term, 1.0 / factorial);

        matrix_sum(added, matrix_exponent_result, matrix_exponent_term);

        matrix_delete(&matrix_exponent_result);
        matrix_exponent_result = added;
    }

    matrix_delete(&matrix_exponent_term);
    return matrix_exponent_result;
}


void matrix_print(const struct Matrix A, const char* text)
{
    printf("%s\n", text);
    for (size_t col = 0; col < A.cols; ++col) {
        printf("[");
        for (size_t row = 0; row < A.rows; ++row) {
            printf("%4.2f ", A.data[A.cols * row + col]);
        }
        printf("]\n");
    }
    printf("\n");
}


void calculation()
{
    MatrixItem values_A[] =
    { 1., 2.,
      3., 4.,
    };

    MatrixItem values_B[] =
    { 1., 1.,
      2., 2.,
    };

    struct Matrix A = matrix_create(2, 2);
    matrix_set_one(A);

    struct Matrix B = matrix_create(2, 2);
    matrix_set_one(B);

    struct Matrix E = matrix_create(2, 2);
    matrix_set_one(E);

    struct Matrix C = matrix_create(2, 2);
    struct Matrix D = matrix_create(2, 2);
    struct Matrix P = matrix_create(2, 2);
    struct Matrix T = matrix_create(2, 2);
    struct Matrix K = matrix_create(2, 2);
    struct Matrix InvA = matrix_inverse(A);
    struct Matrix Exp = matrix_exponent(A, 3);


    matrix_fill(A, values_A);
    matrix_fill(B, values_B);
    matrix_fill(K, values_A);

    matrix_sum(C, A, B);

    matrix_substraction(D, A, B);

    matrix_mult(P, A, B);

    matrix_trans(A, T);

    matrix_determinant(A);

    matrix_mult_number(K, 2.0);


    matrix_print(A, "Matrix A");
    matrix_print(B, "Matrix B");
    matrix_print(C, "C = A + B");
    matrix_print(D, "D = A - B");
    matrix_print(P, "P = A * B");
    matrix_print(T, "T = A^T");
    matrix_print(K, "K = 2 * A");
    matrix_print(E, "Matrix E");
    determinant_print(A, "det A = ");
    matrix_print(Exp, "Matrix Exp^A=");
    matrix_print(InvA, "Inverse of A");


    matrix_delete(&A);
    matrix_delete(&B);
    matrix_delete(&C);
    matrix_delete(&D);
    matrix_delete(&P);
    matrix_delete(&T);
    matrix_delete(&E);
    matrix_delete(&K);
    matrix_delete(&InvA);
    matrix_delete(&Exp);
}


int main()
{
    calculation();
    return 0;
}
