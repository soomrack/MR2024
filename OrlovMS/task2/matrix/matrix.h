#pragma once
#include <stddef.h>


typedef struct {
    size_t rows;
    size_t cols;
    double* data;
} Matrix;


typedef enum {
    MAT_OK = 0,
    MAT_ALLOC_ERR,
    MAT_EMPTY_ERR,
    MAT_SIZE_ERR
} MStatus_t;


MStatus_t matrix_init(Matrix* m, const size_t rows, const size_t cols);


void matrix_free(Matrix* m);


void matrix_set(Matrix m, const size_t row, const size_t col, const double number);


double matrix_get(const Matrix m, const size_t row, const size_t col);


unsigned char matrix_is_empty(const Matrix m);


unsigned char matrix_is_square(const Matrix m);


unsigned char matrix_equal_size(const Matrix m1, const Matrix m2);


void matrix_print(const Matrix m);


MStatus_t matrix_zeros(Matrix m);


MStatus_t matrix_ones(Matrix m);


MStatus_t matrix_copy(const Matrix src, Matrix* dest);


MStatus_t matrix_sum(const Matrix m1, const Matrix m2, Matrix* result);


MStatus_t matrix_sub(const Matrix m1, const Matrix m2, Matrix* result);


MStatus_t matrix_mul_num(const Matrix m, const double number, Matrix* result);


MStatus_t matrix_mul(const Matrix m1, const Matrix m2, Matrix* result);


MStatus_t matrix_pow(const Matrix m, const unsigned exp, Matrix* result);


MStatus_t matrix_exp(const Matrix m, Matrix* result);


MStatus_t matrix_transp(Matrix m);


MStatus_t matrix_det(const Matrix m, double* det);
