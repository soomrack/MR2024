#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>

typedef enum STATUS {
    OK,
    ERR_MALLOC,
    ERR_OVERFLOW,
    ERR_DET,
    ERR_PWR,
    ERR_SIZE
} STATUS;

typedef struct Matrix {
    size_t cols;
    size_t rows;
    double* values;
} Matrix;

extern Matrix EMPTY;

STATUS matrix_alloc(Matrix* ret, const size_t rows, const size_t cols);
void   matrix_free(Matrix* matrix);
STATUS matrix_fill_val(Matrix matrix, const double* value);
STATUS matrix_clone(Matrix* ret, const Matrix src);
STATUS matrix_identity(Matrix matrix);
STATUS matrix_equals(int* res, const Matrix matA, const Matrix matB, const double accuracy);
void   matrix_print(const Matrix matrix);

STATUS matrix_add(Matrix matA, const Matrix matB);
STATUS matrix_subt(Matrix matA, const Matrix matB);
STATUS matrix_mult(Matrix* ret, const Matrix matA, const Matrix matB);
STATUS matrix_mult_by_num(Matrix matrix, const double a);
STATUS matrix_change_rows(Matrix matrix, const size_t rowA, const size_t rowB);

STATUS matrix_det(double* ret, Matrix matrix);
STATUS matrix_pow(Matrix* ret, Matrix matrix, const int power);
STATUS matrix_check_max_diff(double* ret, const Matrix matA, const Matrix matB);
STATUS matrix_exp(Matrix* ret, const Matrix matrix);


#endif //MATRIX_H
