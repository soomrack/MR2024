#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

struct Matrix {
    size_t rows;
    size_t cols;
    double* data;
};


const struct Matrix NULL_MATRIX = { .rows = 0, .cols = 0, .data = NULL };


//Создает матрицу с указанными строками и столбцами и проверяет ошибки
struct Matrix matrix_create(const size_t rows, const size_t cols)
{
    struct Matrix A = { .rows = rows, .cols = cols, .data = NULL };

    if (rows == 0 || cols == 0 || rows >= SIZE_MAX / sizeof(double) / cols)
        return NULL_MATRIX;

    A.data = (double*)malloc(sizeof(double) * A.rows * A.cols);
    if (A.data == NULL) return NULL_MATRIX;
    return A;
}

//Освобождает память, выделенную для матрицы A
void matrix_delete(struct Matrix* A)
{
    if (A->data != NULL) {
        free(A->data);
        A->data = NULL;
    }
    A->rows = 0;
    A->cols = 0;
}


//устанавливает все элементы матрицы A в 0
void matrix_set_zero(struct Matrix A)
{
    if (A.data != NULL) {
        memset(A.data, 0, sizeof(double) * A.rows * A.cols);
    }
}


//заполняет матрицу A последовательными числами
void matrix_set_onetonum(struct Matrix A)
{
    if (A.data == NULL) return;
    for (size_t idx = 0; idx < A.rows * A.cols; idx += 1) {
        A.data[idx] = idx + 1;
    }
}


void matrix_set_numtoone(struct Matrix A)
{
    if (A.data == NULL) return;
    for (size_t idx = 0; idx < A.rows * A.cols; idx += 1) {
        A.data[idx] = A.rows * A.cols - idx;
    }
}


//диагональные элементы равны 1, а все остальные 0
void matrix_set_one(struct Matrix A)
{
    if (A.data == NULL) return;
    matrix_set_zero(A);
    for (size_t idx = 0; idx < A.rows && idx < A.cols; idx += 1) {
        A.data[idx * A.cols + idx] = 1.0;
    }
}


//Выводит матрицу A
void matrix_print(const struct Matrix A)
{
    for (size_t row = 0; row < A.rows; ++row) {
        printf("[ ");
        for (size_t col = 0; col < A.cols; ++col) {
            printf("%4.2f ", A.data[row * A.cols + col]);
        }
        printf("]\n");
    }
    printf("\n");
}


// C = A + B
struct Matrix matrix_summa(struct Matrix A, struct Matrix B)
{
    if (A.cols != B.cols || A.rows != B.rows) return NULL_MATRIX;
    struct Matrix C = matrix_create(A.rows, A.cols);
    if (C.data == NULL) return NULL_MATRIX;

    for (size_t idx = 0; idx < C.rows * C.cols; idx += 1) {
        C.data[idx] = A.data[idx] + B.data[idx];
    }
    return C;
}


// C = A - B
struct Matrix matrix_sub(struct Matrix A, struct Matrix B)
{
    if (A.cols != B.cols || A.rows != B.rows) return NULL_MATRIX;
    struct Matrix C = matrix_create(A.rows, A.cols);
    if (C.data == NULL) return NULL_MATRIX;

    for (size_t idx = 0; idx < C.rows * C.cols; idx += 1) {
        C.data[idx] = A.data[idx] - B.data[idx];
    }
    return C;
}


int main()
{
    struct Matrix A = matrix_create(3, 3);
    if (A.data == NULL) {
        printf("Не удалось создать матрицу A\n");
        return -1;
    }

    matrix_set_onetonum(A);
    printf("Matrix A:\n");
    matrix_print(A);

    struct Matrix B = matrix_create(3, 3);
    if (B.data == NULL) {
        printf("Не удалось создать матрицу B\n");
        matrix_delete(&A);
        return -1;
    }

    matrix_set_numtoone(B);
    printf("Matrix B:\n");
    matrix_print(B);

    struct Matrix C = matrix_summa(A, B);
    if (C.data != NULL) {
        printf("Matrix A + B:\n");
        matrix_print(C);
    }
    else {
        printf("Сложение матриц не удалось\n");
    }

    struct Matrix D = matrix_sub(A, B);
    if (D.data != NULL) {
        printf("Matrix A - B:\n");
        matrix_print(D);
    }
    else {
        printf("Вычитание матрицы не удалось\n");
    }

    // Освобождение выделенной памяти
    matrix_delete(&A);
    matrix_delete(&B);
    matrix_delete(&C);
    matrix_delete(&D);

    return 0;
}