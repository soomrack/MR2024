#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>

// remember NTSTATUS from WinNT and drivers?
typedef enum STATUS {
    OK,
    ERR_MALLOC,
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

STATUS matrix_alloc(const size_t rows, const size_t cols, Matrix** ret);

STATUS matrix_identity(Matrix* matrix);

STATUS matrix_fill_val(Matrix *matrix, const double* value);

STATUS matrix_clone(Matrix* matrix, Matrix** ret);

STATUS matrix_add(Matrix* matA, Matrix* matB);

STATUS matrix_subt(Matrix* matA, Matrix* matB);

STATUS matrix_mult(Matrix* matA, Matrix* matB, Matrix** ret);

STATUS matrix_mult_by_num(const double a, Matrix* matrix);

STATUS matrix_change_rows(Matrix *matrix, const size_t rowA, const size_t rowB);

STATUS matrix_det(Matrix* matrix, double* ret);

STATUS matrix_pow(Matrix* matrix, const int power, Matrix** ret); // only support simple power

STATUS matrix_check_max_diff(Matrix* matA, Matrix* matB, double* ret);

STATUS matrix_exp(Matrix* matrix, Matrix** ret);

STATUS matrix_equals(Matrix* matA, Matrix* matB, const double accuracy, int* res);

void matrix_free(Matrix* matrix);

void matrix_print(Matrix* matrix);

#endif //MATRIX_H
