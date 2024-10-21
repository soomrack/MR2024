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
} MatrixStatus;


#define MATRIX_LOG_ENABLE


typedef enum {
    LOG_NONE,
    LOG_ERR,
    LOG_WARN,
    LOG_INFO
} LogLevel;


void matrix_set_log_level(LogLevel level);


MatrixStatus matrix_alloc(Matrix* M, const size_t rows, const size_t cols);


void matrix_free(Matrix* M);


void matrix_set(const Matrix M, const size_t row, const size_t col, const double number);


double matrix_get(const Matrix M, const size_t row, const size_t col);


unsigned char matrix_is_empty(const Matrix M);


unsigned char matrix_is_square(const Matrix M);


unsigned char matrix_equal_size(const Matrix A, const Matrix B);


void matrix_print(const Matrix M);


MatrixStatus matrix_set_zeros(const Matrix M);


MatrixStatus matrix_set_identity(const Matrix M);


MatrixStatus matrix_copy(Matrix* dest, const Matrix src);


MatrixStatus matrix_clone(Matrix* dest, const Matrix src);


MatrixStatus matrix_sum(Matrix* result, const Matrix A, const Matrix B);


MatrixStatus matrix_sub(Matrix* result, const Matrix A, const Matrix B);


MatrixStatus matrix_mul_num(Matrix* result, const Matrix M, const double number);


MatrixStatus matrix_mul(Matrix* result, const Matrix A, const Matrix B);


MatrixStatus matrix_pow(Matrix* result, const Matrix M, const unsigned int pow);


MatrixStatus matrix_exp(Matrix* result, const Matrix M);


MatrixStatus matrix_transp(const Matrix M);


MatrixStatus matrix_det(double* det, const Matrix M);
