#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


struct Matrix {
    size_t rows;
    size_t cols;
    double *data;
};
typedef struct Matrix Matrix;


enum MatrixExceptionLevel {ERROR, WARNING, INFO, DEBUG};


void matrix_exception(const enum MatrixExceptionLrvel level, char* msg)
{
    if (level == ERROR) {
        printf("ERROR: %s", msg);
    }
    if (level == WARNING) {
        printf("WARNING: %s", msg);
    }
    if (level == INFO) {
        printf("INFO: %s", msg);
    }
    if (level == DEBUG) {
        printf("DEBUG: %s", msg);
    }
}


Matrix matrix_allocate(const size_t rows, const size_t cols)
{
    Matrix A = { rows, cols, NULL };

    if (rows == 0 || cols == 0) {
        matrix_exception(INFO, "no matrix");
        return A;
    }
    if (SIZE_MAX / cols / rows / sizeof(double) < 1) {
        matrix_exception(ERROR, "overflow");
        return A;
    }

    A.data = malloc(rows * cols * sizeof(double));

    if (A.data == NULL) {
        matrix_exception(ERROR, "Allocation memory fail");
        return A;
    }
    
    A.rows = rows;
    A.cols = cols;
    return A;
}

void matrix_free(Matrix* A)
{
    free(A->data);
    *A = (Matrix){0, 0, NULL};
}

void matrix_set(const Matrix A, const double *values)
{
    memcpy(A.data, values, A.rows * A.cols * sizeof(double));
}

void matrix_print(const Matrix A)
{
    for (size_t row = 0; row < A.rows; row++) {
        for (size_t col = 0; col < A.cols; col++) {
            printf(" %.2f", A.data[row * A.cols + col]);
        }
        printf("\n");
    }
    printf("\n");
}


// B := A
void matrix_copy(const Matrix B, const Matrix A)
{
    if ((A.cols != B.cols) || (A.rows != B.rows)) {
        matrix_exception(ERROR, "different sizes");
        return;
    }
    if (B.data == NULL) {
        matrix_exception(ERROR, "memory is  not allocated");
        return;
    }

    memcpy(B.data, A.data, A.cols * A.rows * sizeof(double));
}

// A += B
void matrix_add(const Matrix A, const Matrix B)
{
    if ((A.cols != B.cols) || (A.rows != B.rows)) {
        matrix_exception(ERROR, "different sizes");
        return;
    }

    for (size_t idx = 0; idx < A.cols * A.rows; idx++) {
        A.data[idx] += B.data[idx];
    }
}

// A -= B
void matrix_negative_add(const Matrix A, const Matrix B)
{
    if ((A.cols != B.cols) || (A.rows != B.rows)) {
        matrix_exception(ERROR, "different sizes");
        return;
    }

    for (size_t idx = 0; idx < A.cols * A.rows; idx++) {
        A.data[idx] -= B.data[idx];
    }
}

// C = A + B
void matrix_sum(const Matrix C, const Matrix A, const Matrix B)
{
    if ((A.cols != B.cols) || (A.rows != B.rows) || (A.cols != C.cols) || (A.rows != C.rows) ||
        (C.cols != B.cols) || (C.rows != B.rows)) {
        matrix_exception(ERROR, "different sizes");
        return;
    }

    for (size_t idx = 0; idx < A.cols * A.rows; idx++) {
        C.data[idx] = A.data[idx] + B.data[idx];
    }
}

// C = A - B
void matrix_sub(const Matrix C, const Matrix A, const Matrix B)
{
    if ((A.cols != B.cols) || (A.rows != B.rows) || (A.cols != C.cols) || (A.rows != C.rows) ||
        (C.cols != B.cols) || (C.rows != B.rows)) {
        matrix_exception(ERROR, "different sizes");
        return;
    }

    for (size_t idx = 0; idx < A.cols * A.rows; idx++) {
        C.data[idx] = A.data[idx] - B.data[idx];
    }
}

// A *= k
void matrix_multiplication_k(const Matrix A, const double k)
{
    for (size_t idx = 0; idx < A.cols * A.rows; idx++) {
        A.data[idx] *= k;
    }
}

// C = A * B
void matrix_multiplication(const Matrix C, const Matrix A, const Matrix B)
{
    if ((A.cols != B.rows) || (C.cols != B.cols) || (C.rows != A.rows)) {
        matrix_exception(ERROR, "incorrect sizes");
        return;
    }

    for (size_t row = 0; row < C.rows; row++) {
        for (size_t col = 0; col < C.cols; col++) {
            C.data[row * C.cols + col] = 0;
            for (size_t idx = 0; idx < A.cols; idx++) {
                C.data[row * C.cols + col] += A.data[row * A.cols + idx] * B.data[idx * B.cols + col];
            }
        }
    }
}

// B = A ^ T
void matrix_transp(const Matrix B, const Matrix A)
{
    if ((A.cols != B.cols) || (A.rows != B.rows)) {
        matrix_exception(ERROR, "different sizes");
        return;
    }

    for (size_t row = 0; row < B.rows; row++) {
        for (size_t col = 0; col < B.cols; col++) {
            B.data[row + A.rows * col] = A.data[col + A.cols * row];
        }
    }
}

double matrix_determinant(const Matrix A)
{
    double det;
    if (A.cols != A.rows) {
        matrix_exception(ERROR, "not square");
        return;
    }
    
    if (A.rows == 2) {
        det = A.data[0] * A.data[3] - A.data[1] * A.data[2];
        return det;
    }
    if (A.rows == 3) {
        det = A.data[0] * A.data[4] * A.data[8]
            + A.data[1] * A.data[5] * A.data[6]
            + A.data[3] * A.data[7] * A.data[2]
            - A.data[2] * A.data[4] * A.data[6]
            - A.data[3] * A.data[1] * A.data[8]
            - A.data[0] * A.data[5] * A.data[7];
        return det;
    }
}

// B = A ^ n
int matrix_pow(const Matrix B, const Matrix A, const unsigned int n)
{
    if (A.rows != A.cols) {
        matrix_exception(WARNING, "not square");
        return 1;
    }

    if ((A.cols != B.cols) || (A.rows != B.rows)) {
        matrix_exception(ERROR, "different sizes");
        return 1;
    }

    if (n == 0) {
        matrix_copy(B, A);
        return 1;
    }

    Matrix tmp = matrix_allocate(A.rows, A.cols);
    if (tmp.data == NULL) {
        matrix_exception(ERROR, "no memory allocated");
        return 1;
    }

    matrix_copy(B, A);

    for (unsigned int pow = 1; pow < n; ++pow) {
        matrix_multiplication(tmp, B, A);
        matrix_copy(B, tmp);
    }

    matrix_free(&tmp);
}


int main()
{
    Matrix A, B, C;
    A = matrix_allocate(2, 2);
    B = matrix_allocate(2, 2);
    C = matrix_allocate(2, 2);
    
    matrix_set(A, (double[]) {
        1., 2.,
        3., 4
    });
    matrix_set(B, (double[]) {
        1., 2.,
        2., 1.
    });
    matrix_print(A);
    matrix_print(B);


    matrix_add(A, B);
    matrix_print(A);

    matrix_negative_add(A, B);
    matrix_print(A);

    matrix_sum(C, A, B);
    matrix_print(C);

    matrix_sub(C, A, B);
    matrix_print(C);

    matrix_multiplication_k(A, 5.78);
    matrix_print(A);

    matrix_multiplication(C, A, B);
    matrix_print(C);

    matrix_transp(C, A);
    matrix_print(C);
    
    double det = matrix_determinant(A);
    printf("%f\n\n", det);

    matrix_pow(C, A, 5);
    matrix_print(C);

    matrix_free(&A);
    matrix_free(&B);
    matrix_free(&C);
    return 0;
}