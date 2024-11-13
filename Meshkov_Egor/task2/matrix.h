#pragma once
#include <stddef.h>


typedef struct {
    double *ptr;
    size_t rows;
    size_t cols;
} Matrix;


typedef enum {
    MAT_OK = 0,
    MAT_NULL_POINTER_ERROR,
    MAT_OVERFLOW_ERROR,
    MAT_ALLOCATION_ERROR,
    MAT_RANGE_RANDOM_NUMBERS_ERR,
    MAT_SIZE_ERROR,
    MAT_EMPTY_MATRIX_ERROR,
    MAT_ZERO_DETERMINANT_REVERSE_MATRIX_ERROR, 
} MatrixExceptions;


typedef enum {
    LOG_ERROR,
    LOG_WARNING,
    LOG_NOTE,
    LOG_NONE,
} LogLevel;


#define MATRIX_LOG_ENABLE


extern Matrix EMPTY;


void matrix_set_log_level(LogLevel level);


MatrixExceptions matrix_alloc(Matrix *M, const size_t rows, const size_t cols);


MatrixExceptions matrix_create_zero(Matrix *M, const size_t rows, const size_t cols);


MatrixExceptions matrix_create_unit(Matrix *M, const size_t rows, const size_t cols);


MatrixExceptions matrix_create_random(Matrix *M, const size_t rows, const size_t cols, const double min, const double max, const unsigned int accuracy);


void matrix_free(Matrix *M);


MatrixExceptions matrix_copy(const Matrix dest, const Matrix src);


MatrixExceptions matrix_print(const Matrix M, const int accuracy);


MatrixExceptions matrix_add(const Matrix C, const Matrix A, const Matrix B);


MatrixExceptions matrix_sub(const Matrix C, const Matrix A, const Matrix B);


MatrixExceptions matrix_multi(const Matrix C, const Matrix A, const Matrix B);


MatrixExceptions matrix_transpoze(const Matrix res_matrix, const Matrix src_matrix);


MatrixExceptions matrix_multiply_by_scalar(const Matrix res_matrix, const Matrix src_matrix, const double scalar);


MatrixExceptions matrix_determinant_gauss_method(double *det, const Matrix matrix);


MatrixExceptions matrix_calculate_reverse(const Matrix reverse_matrix, const Matrix matrix);


MatrixExceptions matrix_solve_equation(const Matrix X, const Matrix A, const Matrix B);


MatrixExceptions matrix_power(const Matrix matrix, const unsigned int degree);


MatrixExceptions matrix_exponent(Matrix *EXPmatrix, const Matrix matrix, const int accuracy);
