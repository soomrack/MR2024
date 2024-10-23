#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct Matrix {
    size_t rows;
    size_t cols;
    double* data;
};

typedef struct Matrix Matrix;

enum MatrixExceptionLevel { ERROR, INFO, DEBUG };


void matrix_exception(const MatrixExceptionLevel level, const char* msg)
{
    if (level == ERROR) {
        printf("ERROR: %s \n", msg);
    }
}


struct Matrix matrix_allocation(const size_t rows, const size_t cols)  // Выделение памяти
{
    if (cols == 0 || rows == 0)
        return Matrix{ cols, rows, NULL };
    if (SIZE_MAX / cols / rows / sizeof(double) == 0)
        return Matrix{ cols, rows, NULL };

    double* data = (double*)malloc(rows * cols * sizeof(double));

    if (data == NULL) {
        matrix_exception(ERROR, "matrix allocation : allocation overflow");
        return Matrix{ 0, 0, NULL };
    }
    return Matrix{ cols, rows, data };
}


void output(const struct Matrix A) // Блок вывода
{
    for (size_t col = 0; col < A.cols; ++col) {
        printf("| ");
        for (size_t row = 0; row < A.rows; ++row) {
            printf("%lf ", A.data[row * A.cols + col]);
        }
        printf("|\n");
    }
}


void free_matrix(struct Matrix* A) // Освобождение памяти посмотреть!!
{
    if (A == NULL) return;
    free(A->data);
}


void matrix_random(Matrix A)  // Введение случайных значений в матрицу
{
    for (size_t idx = 0; idx < A.cols * A.rows; idx++) {
        A.data[idx] = rand() % 10;
    }
}


void matrix_zero(Matrix A) //Создание нулевой матрицы
{
    if (A.data != NULL) {
        memset(A.data, 0, sizeof(double) * A.rows * A.cols);
    }
}


int matrix_identity(const struct Matrix A)  // Создание единичной матрицы 
{
    if (A.data == NULL) return 1;
    matrix_zero(A);
    for (size_t idx = 0; idx < A.rows * A.cols; idx += A.rows + 1) A.data[idx] = 1.0;
    return 0;
}


// A = A + B
int matrix_add(const struct Matrix A, const struct Matrix B) // Сложение матриц
{
    if (A.rows != B.rows || A.cols != B.cols) {
        matrix_exception(ERROR, "Matrix addition : Incorrect matrix size");
        return 1;
    }
    for (size_t idx = 0; idx < A.rows * A.cols; ++idx) {
        A.data[idx] += B.data[idx];
    }
    return 0;
}


// A = A - B
int matrix_sub(const struct Matrix A, const struct Matrix B) // Вычитание матриц
{
    if (A.rows != B.rows || A.cols != B.cols) {
        matrix_exception(ERROR, "Matrix subtraction: Incorrect matrix size");
        return 1;
    }
    for (size_t idx = 0; idx < A.rows * A.cols; ++idx) {
        A.data[idx] -= B.data[idx];
    }
    return 0;
}


// C = A * B
int matrix_multiplication(const struct Matrix C, const struct Matrix A, const struct Matrix B) // умножение матриц
{
    if (A.cols != B.rows) {
        matrix_exception(ERROR, "Matrix multiplication: The number of columns is not equal to the number of rows");
        return 1;
    }
    for (size_t col = 0; col < C.cols; col++) {
        for (size_t row = 0; row < C.rows; row++) {
            double sum = 0;
            for (size_t idx = 0; idx < A.rows; idx++) {
                sum += A.data[idx * A.cols + col] * B.data[row * B.cols + idx];
            }
            C.data[row * C.cols + col] = sum;
        }
    }
    return 0;
}


// A = A * const
int matrix_multiplication_x(const Matrix A, const double x) // умножение матрицы на число
{
    for (size_t idx = 0; idx < A.cols * A.rows; ++idx) {
        A.data[idx] *= x;
    }
    return 0;
}


int matrix_transposition(const Matrix A, const Matrix D) // транспонирование матрицы 
{
    if (A.rows != D.cols || A.cols != D.rows) return 1;
    for (size_t col = 0; col < D.cols; ++col) {
        for (size_t row = 0; row < D.rows; ++row) {
            D.data[row * D.cols + col] = A.data[col * D.rows + row];
        }
    }
    return 0;
}


/*
int matrix_det(const Matrix A) {
    if (A.cols != A.rows) {
        printf("Error_det\n");
        return 1;
    }
    for (size_t col = 0; col < A.cols; ++col) {
        for (size_t row = 0; row < A.rows; ++row) {

        }
    }
}
*/


/*void matrix_set(const Matrix A, const double* values)
{
    memcpy(A.data, values, A.rows * A.cols * sizeof(double));
}
  */


int main()
{
    struct Matrix A;
    A = matrix_allocation(3, 2);
    /* matrix_set(A, (double*) {
         1., 2.,
         3., 4.,
         5., 6.,
     }); */
    matrix_random(A);
    printf("Matrix A\n");
    output(A);

    struct Matrix B;
    B = matrix_allocation(2, 3);
    matrix_random(B);
    printf("Matrix B\n");
    output(B);

    struct Matrix E;
    int size_matrix_identity = 5; //размер единичной матрицы
    E = matrix_allocation(size_matrix_identity, size_matrix_identity);
    matrix_random(E);

    struct Matrix C;
    C = matrix_allocation(B.rows, A.cols);
    matrix_multiplication(C, A, B);
    printf("Matrix multiplication\n");
    output(C);

    struct Matrix D;
    D = matrix_allocation(A.rows, A.cols);
    matrix_transposition(A, D);
    printf("Matrix transposition\n");
    output(D);

    matrix_multiplication_x(A, 2);
    printf("Matrix * x \n");
    output(A);

    matrix_add(A, B);
    printf("Matrix +\n");
    output(A);

    matrix_sub(A, B);
    printf("Matrix -\n");
    output(A);

    matrix_zero(A);
    printf("Matrix zero\n");
    output(A);

    matrix_identity(E);
    printf("Matrix identity\n");
    output(E);

    // matrix_det(A);
    // printf("Matrix det\n");



    free_matrix(&A);
    free_matrix(&B);
    free_matrix(&C);
    free_matrix(&D);
    free_matrix(&E);
    return 0;
}
