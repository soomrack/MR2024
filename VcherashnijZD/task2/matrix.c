#include "matrix.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

Matrix EMPTY = {0, 0, NULL};

STATUS matrix_alloc(const size_t rows, const size_t cols, Matrix** ret) {
    Matrix* matrix = malloc(sizeof(Matrix));
    if (matrix == NULL) {
        return ERR_MALLOC;
    }
    matrix->rows = rows;
    matrix->cols = cols;
    matrix->values = malloc(sizeof(double) * rows * cols);
    if (matrix->values == NULL) {
        free(matrix);
        return ERR_MALLOC;
    }
    *ret = matrix;
    return OK;
}

STATUS matrix_identity(Matrix* matrix) {
    if (matrix->rows != matrix->cols) {
        return ERR_SIZE;
    }
    for (size_t i = 0; i < matrix->rows; ++i) {
        for (size_t j = 0; j < matrix->cols; ++j) {
            if (i == j) {
                matrix->values[i * matrix->cols + j] = 1.0;
            } else {
                matrix->values[i * matrix->cols + j] = 0.0;
            }
        }
    }
    return OK;
}

STATUS matrix_fill_val(Matrix *matrix, const double* value) {
    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        matrix->values[i] = value[i];
    }
    return OK;
}

STATUS matrix_clone(Matrix* matrix, Matrix** ret) {
    Matrix* clone = NULL;
    STATUS status = matrix_alloc(matrix->rows, matrix->cols, &clone);
    if (status != OK) {
        return status;
    }

    // Use memcpy to copy the 'values' array directly
    memcpy(clone->values, matrix->values, matrix->rows * matrix->cols * sizeof(double));

    *ret = clone;
    return OK;
}

STATUS matrix_add(Matrix* matA, Matrix* matB) {
    if (matA->rows != matB->rows || matA->cols != matB->cols) {
        return ERR_SIZE;
    }
    for (size_t i = 0; i < matA->rows * matA->cols; ++i) {
        matA->values[i] += matB->values[i];
    }
    return OK;
}

STATUS matrix_subt(Matrix* matA, Matrix* matB) {
    if (matA->rows != matB->rows || matA->cols != matB->cols) {
        return ERR_SIZE;
    }
    for (size_t i = 0; i < matA->rows * matA->cols; ++i) {
        matA->values[i] -= matB->values[i];
    }
    return OK;
}

STATUS matrix_mult(Matrix* matA, Matrix* matB, Matrix** ret) {
    if (matA->cols != matB->rows) {
        return ERR_SIZE;
    }
    Matrix* result = NULL;
    STATUS status = matrix_alloc(matA->rows, matB->cols, &result);
    if (status != OK) {
        return status;
    }
    for (size_t i = 0; i < matA->rows; ++i) {
        for (size_t j = 0; j < matB->cols; ++j) {
            for (size_t k = 0; k < matA->cols; ++k) {
                result->values[i * matB->cols + j] += matA->values[i * matA->cols + k] * matB->values[k * matB->cols + j];
            }
        }
    }
    *ret = result;
    return OK;
}

STATUS matrix_mult_by_num(const double a, Matrix* matrix) {
    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        matrix->values[i] *= a;
    }
    return OK;
}

STATUS matrix_change_rows(Matrix *matrix, const size_t rowA, const size_t rowB) {
    if (rowA >= matrix->rows || rowB >= matrix->rows) {
        return ERR_SIZE;
    }
    for (size_t j = 0; j < matrix->cols; ++j) {
        double temp = matrix->values[rowA * matrix->cols + j];
        matrix->values[rowA * matrix->cols + j] = matrix->values[rowB * matrix->cols + j];
        matrix->values[rowB * matrix->cols + j] = temp;
    }
    return OK;
}

STATUS matrix_det(Matrix* matrix, double* ret) {
    if (matrix->rows != matrix->cols) {
        return ERR_SIZE;
    }
    if (matrix->rows == 1) {
        *ret = matrix->values[0];
        return OK;
    }
    if (matrix->rows == 2) {
        *ret = matrix->values[0] * matrix->values[3] - matrix->values[1] * matrix->values[2];
        return OK;
    }
    double det = 0.0;
    for (size_t i = 0; i < matrix->cols; ++i) {
        Matrix* submatrix = NULL;
        STATUS status = matrix_alloc(matrix->rows - 1, matrix->cols - 1, &submatrix);
        if (status != OK) {
            return status;
        }
        size_t sub_row = 0;
        for (size_t row = 1; row < matrix->rows; ++row) {
            size_t sub_col = 0;
            for (size_t col = 0; col < matrix->cols; ++col) {
                if (col != i) {
                    submatrix->values[sub_row * (matrix->cols - 1) + sub_col] = matrix->values[row * matrix->cols + col];
                    sub_col++;
                }
            }
            sub_row++;
        }
        double sub_det;
        status = matrix_det(submatrix, &sub_det);
        if (status != OK) {
            matrix_free(submatrix);
            return status;
        }
        det += pow(-1, i) * matrix->values[i] * sub_det;
        matrix_free(submatrix);
    }
    *ret = det;
    return OK;
}

STATUS matrix_pow(Matrix* matrix, const int power, Matrix** ret) {
    if (power < 0) {
        return ERR_PWR;
    }
    if (power == 0) {
        Matrix* result = NULL;
        STATUS status = matrix_alloc(matrix->rows, matrix->cols, &result);
        if (status != OK) {
            return status;
        }
        status = matrix_identity(result);
        if (status != OK) {
            matrix_free(result);
            return status;
        }
        *ret = result;
        return OK;
    }
    Matrix* result = NULL;
    STATUS status = matrix_clone(matrix, &result);
    if (status != OK) {
        return status;
    }
    for (int i = 1; i < power; ++i) {
        Matrix* temp = NULL;
        status = matrix_mult(result, matrix, &temp);
        if (status != OK) {
            matrix_free(result);
            return status;
        }
        matrix_free(result);
        result = temp;
    }
    *ret = result;
    return OK;
}

STATUS matrix_check_max_diff(Matrix* matA, Matrix* matB, double* ret) {
    if (matA->rows != matB->rows || matA->cols != matB->cols) {
        return ERR_SIZE;
    }
    double max_diff = 0.0;
    for (size_t i = 0; i < matA->rows * matA->cols; ++i) {
        double diff = fabs(matA->values[i] - matB->values[i]);
        if (diff > max_diff) {
            max_diff = diff;
        }
    }
    *ret = max_diff;
    return OK;
}

#define EXPONENT_STEPS 100
#define EXPONENT_ACCURACY 1e-6

STATUS matrix_exp(Matrix* matrix, Matrix** ret) {
    if (matrix->rows != matrix->cols) {
        return ERR_SIZE;
    }
    Matrix* result = NULL;
    STATUS status = matrix_alloc(matrix->rows, matrix->cols, &result);
    if (status != OK) {
        return status;
    }
    status = matrix_identity(result);
    if (status != OK) {
        matrix_free(result);
        return status;
    }
    Matrix* prev = NULL;
    status = matrix_alloc(matrix->rows, matrix->cols, &prev);
    if (status != OK) {
        matrix_free(result);
        return status;
    }
    for (int m = 1; m <= EXPONENT_STEPS; ++m) {
        matrix_free(prev);
        status = matrix_clone(result, &prev);
        if (status != OK) {
            matrix_free(result);
            return status;
        }
        Matrix* mult = NULL;
        status = matrix_alloc(matrix->rows, matrix->cols, &mult);
        if (status != OK) {
            matrix_free(result);
            matrix_free(prev);
            return status;
        }
        status = matrix_mult_by_num(1.0 / m, matrix);
        if (status != OK) {
            matrix_free(result);
            matrix_free(prev);
            matrix_free(mult);
            return status;
        }
        Matrix* n1_member = NULL;
        status = matrix_mult(prev, matrix, &n1_member);
        matrix_free(mult);
        if (status != OK) {
            matrix_free(result);
            matrix_free(prev);
            return status;
        }
        status = matrix_add(result, n1_member);
        matrix_free(n1_member);
        if (status != OK) {
            matrix_free(result);
            matrix_free(prev);
            return status;
        }
        double max_diff;
        status = matrix_check_max_diff(result, prev, &max_diff);
        if (status != OK) {
            matrix_free(result);
            matrix_free(prev);
            return status;
        }
        if (max_diff < EXPONENT_ACCURACY) {
            matrix_free(prev);
            *ret = result;
            return OK;
        }
    }
    matrix_free(prev);
    *ret = result;
    return OK;
}

STATUS matrix_equals(Matrix* matA, Matrix* matB, const double accuracy, int* res) {
    if (matA->rows != matB->rows || matA->cols != matB->cols) {
        return ERR_SIZE;
    }
    for (size_t i = 0; i < matA->rows * matA->cols; ++i) {
        if (fabs(matA->values[i] - matB->values[i]) > accuracy) {
            *res = 0;
            return OK;
        }
    }
    *res = 1;
    return OK;
}


void matrix_free(Matrix* matrix) {
    if (matrix != NULL) {
        free(matrix->values);
        free(matrix);
    }
}

void matrix_print(Matrix* matrix) {
    for (size_t i = 0; i < matrix->rows; ++i) {
        for (size_t j = 0; j < matrix->cols; ++j) {
            printf("%f ", matrix->values[i * matrix->cols + j]);
        }
        printf("\n");
    }
}

