#include "matrix.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

// If only one cycle in function i as index!
Matrix EMPTY = {0, 0, NULL};


STATUS matrix_alloc(Matrix* ret, const size_t rows, const size_t cols) {
    size_t array = rows * cols;
    if (rows != 0 && array / rows != cols)
        return ERR_OVERFLOW;
    // Double size is platform-dependent. Cannot use bitflip easily
    size_t size = array*sizeof(double);
    if (size / sizeof(double) != array)
        return ERR_OVERFLOW;
    ret->values = malloc(size);
    if (ret->values == NULL)
        return ERR_MALLOC;
    ret->rows = rows;
    ret->cols = cols;
    return OK;
}


STATUS matrix_identity(Matrix matrix) {
    if (matrix.rows != matrix.cols)
        return ERR_SIZE;
    if (matrix.values == NULL)
        return (matrix.rows == matrix.cols && matrix.cols == 0) ? OK : ERR_MALLOC;
    memset(matrix.values, 0, sizeof(double) * matrix.rows * matrix.cols); // It is safe if matrix initialized
    for (size_t i = 0; i < matrix.rows; ++i)
        matrix.values[i * matrix.cols + i] = 1.0;
    return OK;
}


STATUS matrix_fill_val(Matrix matrix, const double* value) {
    memcpy(matrix.values, value, matrix.rows * matrix.cols * sizeof(double));
    return OK;
}


STATUS matrix_clone(Matrix* ret, Matrix src) {
    STATUS status = matrix_alloc(ret, src.rows, src.cols);
    if (status != OK)
        return status;
    matrix_fill_val(*ret, src.values);
    return OK;
}


STATUS matrix_equals(int* res, const Matrix matA, const Matrix matB, const double accuracy) {
    if (matA.rows != matB.rows || matA.cols != matB.cols)
        return ERR_SIZE;
    for (size_t i = 0; i < matA.rows * matA.cols; ++i)
        if (fabs(matA.values[i] - matB.values[i]) > accuracy) {
            *res = 0;
            return OK;
        }
    *res = 1;
    return OK;
}


void matrix_free(Matrix* matrix) {
    if (matrix->values != NULL) {
        free(matrix->values);
        matrix->values = NULL;
    }
    matrix->rows = 0;
    matrix->cols = 0;
}


void matrix_print(const Matrix matrix) {
    for (size_t row = 0; row < matrix.rows; ++row) {
        for (size_t col = 0; col < matrix.cols; ++col)
            printf("%f ", matrix.values[row * matrix.cols + col]);
        printf("\n");
    }
}


STATUS matrix_add(Matrix matA, const Matrix matB) {
    if (matA.rows != matB.rows || matA.cols != matB.cols)
        return ERR_SIZE;
    for (size_t i = 0; i < matA.rows * matA.cols; ++i)
        matA.values[i] += matB.values[i];
    return OK;
}


STATUS matrix_subt(Matrix matA, const Matrix matB) {
    if (matA.rows != matB.rows || matA.cols != matB.cols)
        return ERR_SIZE;
    for (size_t i = 0; i < matA.rows * matA.cols; ++i)
        matA.values[i] -= matB.values[i];
    return OK;
}


STATUS matrix_mult(Matrix* ret, Matrix matA, Matrix matB) {
    if (matA.cols != matB.rows)
        return ERR_SIZE;
    STATUS status = matrix_alloc(ret, matA.rows, matB.cols);
    if (status != OK)
        return status;
    for (size_t rowA = 0; rowA < matA.rows; ++rowA)
        for (size_t colB = 0; colB < matB.cols; ++colB)
            for (size_t colA = 0; colA < matA.cols; ++colA)
                ret->values[rowA * matB.cols + colB] +=
                    matA.values[rowA * matA.cols + colA]
                    * matB.values[colA * matB.cols + colB];
    return OK;
}


STATUS matrix_mult_by_num(Matrix matrix, const double a) {
    for (size_t i = 0; i < matrix.rows * matrix.cols; ++i)
        matrix.values[i] *= a;
    return OK;
}


STATUS matrix_change_rows(Matrix matrix, const size_t rowA, const size_t rowB) {
    if (rowA >= matrix.rows || rowB >= matrix.rows) {
        return ERR_SIZE;
    }

    // Calculate the starting addresses of the rows
    double* row_a_ptr = matrix.values + rowA * matrix.cols;
    double* row_b_ptr = matrix.values + rowB * matrix.cols;

    memcpy(row_a_ptr, row_b_ptr, matrix.cols * sizeof(double));
    memcpy(row_b_ptr, row_a_ptr, matrix.cols * sizeof(double)); 

    return OK;
}


STATUS matrix_det(double* ret, Matrix matrix) {
    if (matrix.rows != matrix.cols) {
        return ERR_SIZE;
    }

    if (matrix.rows == 1) {
        *ret = matrix.values[0];
        return OK;
    }

    if (matrix.rows == 2) {
        *ret = matrix.values[0] * matrix.values[3] - matrix.values[1] * matrix.values[2];
        return OK;
    }

    double det = 0.0;
    for (size_t i = 0; i < matrix.cols; ++i) {
        Matrix submatrix;
        STATUS status = matrix_alloc(&submatrix, matrix.rows - 1, matrix.cols - 1);
        if (status != OK) {
            return status;
        }

        size_t sub_row = 0;
        for (size_t row = 1; row < matrix.rows; ++row) {
            size_t sub_col = 0;
            for (size_t col = 0; col < matrix.cols; ++col) {
                if (col != i) {
                    submatrix.values[sub_row * (matrix.cols - 1) + sub_col] = matrix.values[row * matrix.cols + col];
                    sub_col++;
                }
            }
            sub_row++;
        }

        double sub_det;
        status = matrix_det(&sub_det, submatrix);
        if (status != OK) {
            matrix_free(&submatrix);
            return status;
        }

        det += (i % 2 == 0 ? 1 : -1) * matrix.values[i] * sub_det;
        matrix_free(&submatrix);
    }

    *ret = det;
    return OK;
}


STATUS matrix_pow(Matrix* ret, Matrix matrix, const int power) {
    if (power < 0) {
        return ERR_PWR;
    }

    if (power == 0) {
        STATUS status = matrix_alloc(ret, matrix.rows, matrix.cols);
        if (status != OK) {
            return status;
        }
        status = matrix_identity(*ret);
        if (status != OK) {
            return status;
        }
        return OK;
    }

    STATUS status = matrix_clone(ret, matrix);
    if (status != OK) {
        return status;
    }

    // Optimized loop using binary exponentiation
    for (int i = 1; i < power; i <<= 1) {
        if (power & i) { // If the current bit is set in 'power'
            Matrix temp;
            status = matrix_mult(&temp, *ret, matrix);
            if (status != OK) {
                matrix_free(ret);
                return status;
            }
            matrix_free(ret);
            *ret = temp;
        }
        Matrix temp;
        status = matrix_mult(&temp, *ret, *ret); // Square
        if (status != OK) {
            matrix_free(ret);
            return status;
        }
        matrix_free(ret);
        *ret = temp;
    }

    return OK;
}


STATUS matrix_check_max_diff(double* ret, const Matrix matA, const Matrix matB) {
    if (matA.rows != matB.rows || matA.cols != matB.cols)
        return ERR_SIZE;
    double max_diff = 0.0;
    for (size_t i = 0; i < matA.rows * matA.cols; ++i) {
        double diff = fabs(matA.values[i] - matB.values[i]);
        if (diff > max_diff)
            max_diff = diff;
    }
    *ret = max_diff;
    return OK;
}


#define EXPONENT_STEPS 100
#define EXPONENT_ACCURACY 1e-6

STATUS matrix_exp(Matrix* ret, const Matrix matrix) {
    if (matrix.rows != matrix.cols)
        return ERR_SIZE;
    Matrix result;
    STATUS status = matrix_alloc(&result, matrix.rows, matrix.cols);
    if (status != OK)
        return status;
    status = matrix_identity(result);
    if (status != OK) {
        matrix_free(&result);
        return status;
    }
    Matrix prev;
    status = matrix_alloc(&prev, matrix.rows, matrix.cols);
    if (status != OK) {
        matrix_free(&result);
        return status;
    }
    for (int m = 1; m <= EXPONENT_STEPS; ++m) {
        status = matrix_fill_val(prev, result.values);
        if (status != OK) {
            matrix_free(&result);
            return status;
        }
        status = matrix_mult_by_num(matrix, 1.0 / m);
        if (status != OK) {
            matrix_free(&result);
            matrix_free(&prev);
            return status;
        }
        Matrix n1_member;
        status = matrix_mult(&n1_member, prev, matrix);
        if (status != OK) {
            matrix_free(&result);
            matrix_free(&prev);
            return status;
        }
        status = matrix_add(result, n1_member);
        matrix_free(&n1_member);
        if (status != OK) {
            matrix_free(&result);
            matrix_free(&prev);
            return status;
        }
        double max_diff;
        status = matrix_check_max_diff(&max_diff, result, prev);
        if (status != OK) {
            matrix_free(&result);
            matrix_free(&prev);
            return status;
        }
        if (max_diff < EXPONENT_ACCURACY) {
            matrix_free(&prev);
            *ret = result;
            return OK;
        }
    }
    matrix_free(&prev);
    *ret = result;
    return OK;
}


STATUS matrix_lsolve(Matrix* ret, const Matrix matA, const Matrix matB) {
    if (matA.rows != matA.cols || matA.rows != matB.rows)
        return ERR_SIZE;

    double det_a;
    STATUS status = matrix_det(&det_a, matA);
    if (status != OK || fabs(det_a) < 1e-6)
        return ERR_DET;

    status = matrix_alloc(ret, matA.rows, 1);
    if (status != OK)
        return status;

    for (size_t col = 0; col < matA.rows; ++col) {
        Matrix submatrix;
        status = matrix_alloc(&submatrix, matA.rows, matA.cols);
        if (status != OK) {
            matrix_free(ret);
            return status;
        }

        memcpy(submatrix.values, matA.values, matA.rows * matA.cols * sizeof(double));
        for (size_t row = 0; row < matA.rows; ++row) {
            submatrix.values[row * matA.cols + col] = matB.values[row];
        }

        double det_bi;
        status = matrix_det(&det_bi, submatrix);
        if (status != OK) {
            matrix_free(&submatrix);
            matrix_free(ret);
            return status;
        }

        ret->values[col] = -det_bi / det_a;
        matrix_free(&submatrix);
    }

    return OK;
}


static double dot_product(const double* v1, const double* v2, size_t size) {
    double sum = 0.0;
    for (size_t i = 0; i < size; ++i) {
        sum += v1[i] * v2[i];
    }
    return sum;
}


STATUS matrix_lsolve_cg(Matrix* ret, const Matrix matA, const Matrix matB) {
    if (matA.rows != matA.cols || matA.rows != matB.rows) {
        return ERR_SIZE;
    }

    Matrix x;
    STATUS status = matrix_alloc(&x, matA.rows, 1);
    if (status != OK) {
        return status;
    }
    memset(x.values, 0, sizeof(double) * x.rows);

    Matrix r;
    status = matrix_alloc(&r, matA.rows, 1);
    if (status != OK) {
        matrix_free(&x);
        return status;
    }

    Matrix r_prev;
    status = matrix_alloc(&r_prev, matA.rows, 1);
    if (status != OK) {
        matrix_free(&x);
        matrix_free(&r);
        return status;
    }

    Matrix p;
    status = matrix_alloc(&p, matA.rows, 1);
    if (status != OK) {
        matrix_free(&x);
        matrix_free(&r);
        matrix_free(&r_prev);
        return status;
    }

    status = matrix_mult(&r, matA, x);
    if (status != OK) {
        matrix_free(&x);
        matrix_free(&r);
        matrix_free(&r_prev);
        matrix_free(&p);
        return status;
    }
    status = matrix_subt(r, matB);
    if (status != OK) {
        matrix_free(&x);
        matrix_free(&r);
        matrix_free(&r_prev);
        matrix_free(&p);
        return status;
    }

    memcpy(p.values, r.values, matA.rows * sizeof(double));

    double tolerance = 1e-6;
    int max_iterations = 100; 
    int iteration = 0;

    while (iteration < max_iterations) {
        memcpy(r_prev.values, r.values, matA.rows * sizeof(double));

        double alpha = 0.0;
        status = matrix_mult(&r_prev, matA, p);
        if (status != OK) {
            matrix_free(&x);
            matrix_free(&r);
            matrix_free(&r_prev);
            matrix_free(&p);
            return status;
        }
        alpha = dot_product(r.values, r.values, matA.rows) / dot_product(r_prev.values, p.values, matA.rows);

        for (size_t i = 0; i < matA.rows; ++i) {
            x.values[i] += alpha * p.values[i];
            r.values[i] -= alpha * r_prev.values[i];
        }

        if (dot_product(r.values, r.values, matA.rows) < tolerance) {
            break;
        }

        double beta = dot_product(r.values, r.values, matA.rows) / dot_product(r_prev.values, r_prev.values, matA.rows);

        for (size_t i = 0; i < matA.rows; ++i) {
            p.values[i] = r.values[i] + beta * p.values[i];
        }

        iteration++;
    }

    STATUS ret_status = dot_product(r.values, r.values, matA.rows) >= tolerance ? ERR_ITER : OK;
    status = matrix_clone(ret, x);
    if (status != OK) {
        matrix_free(&x);
        matrix_free(&r);
        matrix_free(&r_prev);
        matrix_free(&p);
        return status;
    }

    matrix_free(&x);
    matrix_free(&r);
    matrix_free(&r_prev);
    matrix_free(&p);
    return ret_status;
}

