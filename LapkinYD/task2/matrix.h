#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>

typedef struct Matrix {
    size_t rows;
    size_t cols;
    double* data;
} Matrix;

typedef enum MatrixExceptionLevels{
    OK,
    EXC_NULL,
    EXC_MEMORY,
    EXC_SIZE,
    EXC_NOT_SQUARE,
    EXC_DIMENSION,
    EXC_MULTIPLY,
    EXC_GETTING_MINOR
} MATRIX_EXCEPTION;

typedef enum matrix_type { 
    ORDINARY_MATRIX, 
    VECTOR, 
    SQUARE_MATRIX 
} MATRIX_TYPE;

//extern Matrix MATRIX_NULL;

MATRIX_TYPE get_matrix_type(const Matrix *matrix);

MATRIX_EXCEPTION malloc_matrix(Matrix *matrix, size_t rows, size_t cols);

MATRIX_EXCEPTION set_data(Matrix *mtx, const double *data, size_t data_bite_size);

MATRIX_EXCEPTION print_matrix(const Matrix *matrix);

MATRIX_EXCEPTION get_identity_matrix(Matrix *retn, size_t cols, size_t rows);

MATRIX_EXCEPTION get_determinant(const Matrix *mtx, double *determinant);

MATRIX_EXCEPTION invert_matrix(Matrix *inverse, const Matrix *basic);

MATRIX_EXCEPTION copy_matrix(Matrix *retn, const Matrix *src);

void add(const Matrix *basic, const Matrix *adding);

MATRIX_EXCEPTION summarize(Matrix *resl_mtx, const Matrix *mtx1, const Matrix *mtx2);

MATRIX_EXCEPTION subtract(Matrix *resl_mtx, const Matrix *mtx, const Matrix *sub_mtx);

void multiply_by_number(Matrix *matrix, double number);

MATRIX_EXCEPTION multiply_matrices(Matrix *resl_mtx, const Matrix *base, const Matrix *multiplier);

MATRIX_EXCEPTION rase_to_power(Matrix *resl_mtx, const Matrix *mtx, signed int power);

MATRIX_EXCEPTION get_exp(Matrix *resl_mtx, const Matrix *mtx);

MATRIX_EXCEPTION transpose(Matrix *result_matrix, const Matrix *matrix);

#endif // MATRIX_H