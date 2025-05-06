#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>
#include <stdint.h>
#include <math.h>

// Структура для матрицы
struct Matrix {
    size_t cols;
    size_t rows;
    double *data;
};

enum MatrixExceptionLevel {
    ERROR,
    DEBUG
};

// Прототипы функций
struct Matrix matrix_allocate(const size_t cols, const size_t rows);
void matrix_free(struct Matrix *A);
void matrix_fill_random(struct Matrix A);
void matrix_print(const struct Matrix A);
struct Matrix matrix_add(struct Matrix A, struct Matrix B);
struct Matrix matrix_subtract(struct Matrix A, struct Matrix B);
struct Matrix matrix_copy(struct Matrix A);
struct Matrix matrix_multiply_const(struct Matrix A, double constant);
struct Matrix matrix_multiply(struct Matrix A, struct Matrix B);
struct Matrix matrix_pow(struct Matrix A, const size_t degree);
struct Matrix matrix_transpon(struct Matrix A);
struct Matrix matrix_exp(struct Matrix A);
double determinant(struct Matrix A);
void matrix_error(const enum MatrixExceptionLevel level, const char *location, const char *msg);

#endif // MATRIX_H
