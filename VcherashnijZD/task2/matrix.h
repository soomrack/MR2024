#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>

typedef enum MatrixStatus {
    OK,
    ERR_MALLOC,
    ERR_NULL,
    ERR_OVERFLOW,
    ERR_DET,
    ERR_PWR,
    ERR_SIZE,
    ERR_ITER
} MatrixStatus;

typedef struct Matrix {
    size_t cols;
    size_t rows;
    double* values;
} Matrix;

extern Matrix EMPTY;

MatrixStatus matrix_alloc(Matrix* ret, const size_t rows, const size_t cols);
void         matrix_free(Matrix* matrix);
void         matrix_zero(Matrix mat);
void         matrix_fill_val(Matrix matrix, const double* value);
MatrixStatus matrix_clone(Matrix* ret, const Matrix src);
MatrixStatus matrix_identity(Matrix matrix);
MatrixStatus matrix_equals(int* res, const Matrix matA,
                 const Matrix matB, const double accuracy);
void         matrix_print(const Matrix matrix);


MatrixStatus matrix_add(Matrix matA, const Matrix matB);
MatrixStatus matrix_subt(Matrix matA, const Matrix matB);
MatrixStatus matrix_mult(Matrix* ret, const Matrix matA, const Matrix matB);
MatrixStatus matrix_mult_in_place(Matrix ret, const Matrix matA, const Matrix matB);
MatrixStatus matrix_transp(Matrix matrix);
MatrixStatus matrix_mult_by_num(Matrix matrix, const double a);
MatrixStatus matrix_swap_rows(Matrix matrix, const size_t rowA, const size_t rowB);

MatrixStatus matrix_det(double* ret, const Matrix matrix);
MatrixStatus matrix_pow(Matrix* ret, const Matrix matrix, unsigned int power);
MatrixStatus matrix_check_max_diff(double* ret, const Matrix matA, const Matrix matB);
MatrixStatus matrix_exp(Matrix* ret, const Matrix matrix);

MatrixStatus matrix_lsolve(Matrix* ret, const Matrix matA, const Matrix matB);
MatrixStatus matrix_lsolve_cg(Matrix* ret, const Matrix matA, const Matrix matB);

#endif //MATRIX_H
