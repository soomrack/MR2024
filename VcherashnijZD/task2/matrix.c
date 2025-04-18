#include "matrix.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>


Matrix EMPTY = {0, 0, NULL};


MatrixStatus matrix_alloc(Matrix* ret, const size_t rows, const size_t cols) {
    if (ret == NULL) return ERR_NULL;
    if (!rows && !cols) {
        *ret = EMPTY;
        ret->rows = rows;
        ret->cols = cols;
        return OK;
    }

    size_t size = rows * cols;
    if (rows != 0 && size / rows != cols)
        return ERR_OVERFLOW;
    // Double size is platform-dependent. Cannot use bitflip easily
    size_t size_in_bytes = size * sizeof(double);
    if (size_in_bytes / sizeof(double) != size)
        return ERR_OVERFLOW;

    ret->values = (double*) malloc(size_in_bytes);
    if (ret->values == NULL) {
        ret->rows = 0;
        ret->cols = 0;
        return ERR_MALLOC;
    }

    ret->rows = rows;
    ret->cols = cols;
    return OK;
}


void matrix_zero(Matrix mat) {
    if (mat.values != NULL)
        memset(mat.values, 0, sizeof(double) * mat.rows * mat.cols);
}


MatrixStatus matrix_identity(Matrix matrix) {
    if (matrix.values == NULL)
        return (matrix.rows == matrix.cols || matrix.cols == 0) ? OK : ERR_MALLOC;

    matrix_zero(matrix);  // It is safe if matrix initialized
    for (size_t idx = 0; idx < matrix.rows*matrix.cols; idx += matrix.cols + 1)
        matrix.values[idx] = 1.0;
    return OK;
}


void matrix_fill_val(Matrix matrix, const double* value) {
    memcpy(matrix.values, value, matrix.rows * matrix.cols * sizeof(double));
}


MatrixStatus matrix_clone(Matrix* ret, Matrix src) {
    if (ret == NULL) return ERR_NULL;
    MatrixStatus status = matrix_alloc(ret, src.rows, src.cols);
    if (status != OK)
        return status;
    matrix_fill_val(*ret, src.values);
    return OK;
}


MatrixStatus matrix_equals(int* res, const Matrix matA, const Matrix matB, const double accuracy) {
    if (matA.rows != matB.rows || matA.cols != matB.cols)
        return ERR_SIZE;

    for (size_t idx = 0; idx < matA.rows * matA.cols; ++idx) {
        if (fabs(matA.values[idx] - matB.values[idx]) > accuracy) {
            *res = 0;
            return OK;
        }
    }
    *res = 1;
    return OK;
}


void matrix_free(Matrix* matrix) {
    if (matrix == NULL) return;
    free(matrix->values);
    *matrix = EMPTY;
}


void matrix_print(const Matrix matrix) {
    for (size_t row = 0; row < matrix.rows; ++row) {
        for (size_t col = 0; col < matrix.cols; ++col)
            printf("%lf\t", matrix.values[row * matrix.cols + col]);
        printf("\n");
    }
}


MatrixStatus matrix_transp(Matrix matrix)
{
    if (matrix.cols != matrix.rows)
        return ERR_SIZE;

    for (size_t row = 1; row < matrix.rows; row++) {
        for (size_t col = 0; col < row; col++) {
            double tmp = matrix.values[row * matrix.cols + col];
            matrix.values[row * matrix.cols + col] = matrix.values[col * matrix.cols + row];
            matrix.values[col * matrix.cols + row] = tmp;
        }
    }

    return OK;
}


// A += B
MatrixStatus matrix_add(Matrix matA, const Matrix matB) {
    if (matA.rows != matB.rows || matA.cols != matB.cols)
        return ERR_SIZE;
    for (size_t idx = 0; idx < matA.rows * matA.cols; ++idx)
        matA.values[idx] += matB.values[idx];
    return OK;
}


// A -= B
MatrixStatus matrix_subt(Matrix matA, const Matrix matB) {
    if (matA.rows != matB.rows || matA.cols != matB.cols)
        return ERR_SIZE;
    for (size_t idx = 0; idx < matA.rows * matA.cols; ++idx)
        matA.values[idx] -= matB.values[idx];
    return OK;
}


// ret c = matA * matB
MatrixStatus matrix_mult_in_place(Matrix ret, const Matrix matA, const Matrix matB) {
    if (ret.values == matA.values || ret.values == matB.values)
        return ERR_OVERFLOW;
    if (matA.cols != matB.rows)
        return ERR_SIZE;
    if (ret.rows != matA.rows || ret.cols != matB.cols)
        return ERR_SIZE;

    matrix_zero(ret);  // It is safe if matrix initialized
    for (size_t row = 0; row < ret.rows; row++)
        for (size_t col = 0; col < ret.cols; col++)
            for (size_t a_col = 0; a_col < matA.cols; a_col++)
                ret.values[row * ret.cols + col] += matA.values[row * matA.cols + a_col] * matB.values[a_col * matB.cols + col];

    return OK;
}


MatrixStatus matrix_mult(Matrix* ret, const Matrix matA, const Matrix matB) {
    if (ret == NULL) return ERR_NULL;
    if (matA.cols != matB.rows)
        return ERR_SIZE;

    MatrixStatus status = matrix_alloc(ret, matA.rows, matB.cols);
    if (status != OK)
        return status;

    status = matrix_mult_in_place(*ret, matA, matB);
    if (status != OK)
        matrix_free(ret);
    return status;
}


MatrixStatus matrix_mult_by_num(Matrix matrix, const double val) {
    for (size_t idx = 0; idx < matrix.rows * matrix.cols; ++idx)
        matrix.values[idx] *= val;
    return OK;
}


MatrixStatus matrix_swap_rows(Matrix matrix, const size_t rowA, const size_t rowB) {
    if (rowA >= matrix.rows || rowB >= matrix.rows) {
        return ERR_SIZE;
    }

    // Calculate the starting addresses of the rows
    double* row_a_ptr = matrix.values + rowA * matrix.cols;
    double* row_b_ptr = matrix.values + rowB * matrix.cols;

    // Use a temporary array to avoid overwriting
    double temp[matrix.cols]; 
    memcpy(temp, row_a_ptr, matrix.cols * sizeof(double));

    memcpy(row_a_ptr, row_b_ptr, matrix.cols * sizeof(double)); 
    memcpy(row_b_ptr, temp, matrix.cols * sizeof(double)); 

    return OK;
}


MatrixStatus matrix_det(double* ret, const Matrix matrix) {
    if (ret == NULL) return ERR_NULL;
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

    Matrix submatrix;
    MatrixStatus status = matrix_alloc(&submatrix, matrix.rows - 1, matrix.cols - 1);
    if (status != OK) {
        return status;
    }
    double det = 0.0;
    for (size_t base_col = 0; base_col < matrix.cols; ++base_col) {
        size_t sub_row = 0;

        // Step 1: minor
        for (size_t row = 1; row < matrix.rows; ++row) {
            size_t sub_col = 0;
            for (size_t col = 0; col < matrix.cols; ++col) {
                if (col != base_col) {
                    submatrix.values[sub_row * (matrix.cols - 1) + sub_col] = matrix.values[row * matrix.cols + col];
                    sub_col++;
                }
            }
            sub_row++;
        }

        // Step 2: get determinant of minor
        double sub_det;
        status = matrix_det(&sub_det, submatrix);
        if (status != OK) {
            matrix_free(&submatrix);
            return status;
        }

        // Step 3: update determinant
        det += (base_col % 2 == 0 ? 1 : -1) * matrix.values[base_col] * sub_det;
    }
    matrix_free(&submatrix);
    *ret = det;
    return OK;
}

MatrixStatus matrix_pow(Matrix* ret, const Matrix matrix, unsigned int power) {
    if (ret == NULL) return ERR_NULL;

    // matA, matB swap in place, no more allocation
    Matrix matA, matB;
    MatrixStatus status = matrix_alloc(&matB, matrix.rows, matrix.cols);
    if (status != OK) {
        return status;
    }

    if (power == 0) {
        status = matrix_identity(matB);
        if (status != OK) {
            return status;
        }
        *ret = matB;
        return OK;
    }

    status = matrix_clone(&matA, matrix);
    if (status != OK) {
        matrix_free(&matB);
        return status;
    }
    
    // Optimized power algorithm
    while (power > 1) {
        if (power % 2 == 0) {
            // Even power: square temp
            status = matrix_mult_in_place(matB, matA, matA); 
            if (status != OK) {
                matrix_free(&matB);
                matrix_free(&matA);
                return status;
            }
            power /= 2;
        } else {
            // Odd power: multiply temp with matrix
            status = matrix_mult_in_place(matB, matA, matrix); 
            if (status != OK) {
                matrix_free(&matB);
                matrix_free(&matA);
                return status;
            }
            power--;
        }
        // Swap for the next iteration
        Matrix tmp = matA;
        matA = matB;
        matB = tmp;
    }
    *ret = matA;
    matrix_free(&matB);
    return OK;
}


MatrixStatus matrix_check_max_diff(double* ret, const Matrix matA, const Matrix matB) {
    if (matA.rows != matB.rows || matA.cols != matB.cols)
        return ERR_SIZE;
    double max_diff = 0.0;
    for (size_t idx = 0; idx < matA.rows * matA.cols; ++idx) {
        double diff = fabs(matA.values[idx] - matB.values[idx]);
        if (diff > max_diff)
            max_diff = diff;
    }
    *ret = max_diff;
    return OK;
}


#define STEPS 100	
#define ACCURACY 1e-6

MatrixStatus matrix_exp(Matrix* ret, const Matrix matrix) {
    if (ret == NULL) return ERR_NULL;
    if (matrix.rows != matrix.cols)
        return ERR_SIZE;
    Matrix result;
    MatrixStatus status = matrix_alloc(&result, matrix.rows, matrix.cols);
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
    Matrix n1_member;
    status = matrix_alloc(&n1_member, matrix.rows, matrix.cols);
    if (status != OK) {
        matrix_free(&result);
        matrix_free(&prev);
        return status;
    }
    double max_diff;
    for (int m = 1; m <= STEPS; ++m) {
        matrix_fill_val(prev, result.values);
        status = matrix_mult_by_num(matrix, 1.0 / m);
        if (status != OK) {
            matrix_free(&result);
            matrix_free(&prev);
            matrix_free(&n1_member);
            return status;
        }
        status = matrix_mult_in_place(n1_member, prev, matrix);
        if (status != OK) {
            matrix_free(&result);
            matrix_free(&prev);
            matrix_free(&n1_member);
            return status;
        }
        status = matrix_add(result, n1_member);
        if (status != OK) {
            matrix_free(&result);
            matrix_free(&n1_member);
            matrix_free(&prev);
            return status;
        }
        status = matrix_check_max_diff(&max_diff, result, prev);
        if (status != OK) {
            matrix_free(&n1_member);
            matrix_free(&result);
            matrix_free(&prev);
            return status;
        }
        if (max_diff < ACCURACY) {
            matrix_free(&prev);
            matrix_free(&n1_member);
            *ret = result;
            return OK;
        }
    }
    matrix_free(&prev);
    matrix_free(&n1_member);
    *ret = result;
    return OK;
}


// Lsolve based on gauss method
MatrixStatus matrix_lsolve(Matrix* ret, const Matrix matA, const Matrix matB) {
    if (ret == NULL) return ERR_NULL;
    if (matA.rows != matA.cols || matA.rows != matB.rows)
        return ERR_SIZE;

    double det_a;
    MatrixStatus status = matrix_det(&det_a, matA);
    if (status != OK || fabs(det_a) < 1e-6)
        return ERR_DET;

    status = matrix_alloc(ret, matA.rows, 1);
    if (status != OK)
        return status;
    Matrix submatrix;
    status = matrix_alloc(&submatrix, matA.rows, matA.cols);

    // Iterate over columns
    for (size_t col = 0; col < matA.rows; ++col) {
        memcpy(submatrix.values, matA.values, matA.rows * matA.cols * sizeof(double));
        for (size_t row = 0; row < matA.rows; ++row) {
            submatrix.values[row * matA.cols + col] = matB.values[row];
        }

        // Calc determinant of swapped row matrix
        double det_bi;
        status = matrix_det(&det_bi, submatrix);
        if (status != OK) {
            matrix_free(&submatrix);
            matrix_free(ret);
            return status;
        }

        ret->values[col] = - det_bi / det_a;
    }
    matrix_free(&submatrix);

    return OK;
}


static double matrix_dot_product(const Matrix a, const Matrix b) {
    if (a.rows != b.rows || a.cols != b.cols) {
        return NAN;  // Error!
    }
    double dot_product = 0.0;
    for (size_t idx = 0; idx < a.rows * a.cols; idx++) {
        dot_product += a.values[idx] * b.values[idx];
    }
    return dot_product;
}


int matrix_is_symmetric(const Matrix mat) {
    for (size_t col = 0; col < mat.rows; col++) {
        for (size_t row = col + 1; row < mat.cols; row++) {
            if ((mat.values[col * mat.cols + row] - mat.values[row * mat.cols + col]) < ACCURACY) {
                return 0;
            }
        }
    }
    return 1;
}

// Iterative solve 
MatrixStatus matrix_lsolve_cg(Matrix* ret, const Matrix matA, const Matrix matB) {
    if (ret == NULL) return ERR_NULL;
    if (matA.cols != matB.rows)
        return ERR_SIZE;
    if (!matrix_is_symmetric(matA))
        return ERR_ITER;

    MatrixStatus status = matrix_alloc(ret, matA.rows, matB.cols);
    if (status != OK)
         goto check;

    Matrix r, p, Ap, x;
    status = matrix_alloc(&r, ret->rows, ret->cols);
    if (status != OK)
         goto check;

    status = matrix_alloc(&Ap, ret->rows, ret->cols);
    if (status != OK)
         goto check;

    status = matrix_alloc(&x, ret->rows, ret->cols);
    if (status != OK)
         goto check;

    // Initialize x with zeros (initial guess)
    matrix_zero(x);

    // Calculate initial residual r = b - Ax
    status = matrix_mult_in_place(r, matA, x);
    if (status != OK)
         goto check;
    status = matrix_subt(r, matB);
    if (status != OK)
         goto check;

    // Set initial search direction p = r
    status = matrix_clone(&p, r);
    if (status != OK)
         goto check;

    // Main CG iteration loop
    double r_dot_r, p_dot_Ap, alpha;
    for (int k = 0; k < STEPS; k++) {
        r_dot_r = matrix_dot_product(r, r); 
        if (r_dot_r < ACCURACY)
            break;

        // Calculate Ap = A * p
        status = matrix_mult_in_place(Ap, matA, p); 
        if (status != OK)
            break;

        p_dot_Ap = matrix_dot_product(p, Ap); 
        alpha = r_dot_r / p_dot_Ap;

        // Update solution x = x + alpha * p
        status = matrix_mult_by_num(p, alpha);
        if (status != OK)
            break;

        status = matrix_add(x, p);
        if (status != OK)
            break;

        // Update residual r = r - alpha * Ap
        status = matrix_mult_by_num(Ap, alpha);
        if (status != OK)
            break;

        status = matrix_subt(r, Ap);
        if (status != OK)
            break;

        // Update search direction p = r + beta * p
        // Calculate beta = (r_new dot r_new) / (r_old dot r_old)
        double r_new_dot_r_new = matrix_dot_product(r, r);
        double beta = r_new_dot_r_new / r_dot_r; 
        status = matrix_mult_by_num(p, beta);
        if (status != OK)
            break;

        status = matrix_add(p, r);
        if (status != OK)
            break;

    }
check:
    if (status != OK) {
        matrix_free(ret);
        matrix_free(&r);
        matrix_free(&p);
        matrix_free(&Ap);
        matrix_free(&x);
        return status;
    }
    // Copy the solution from x to ret
    matrix_fill_val(*ret, x.values);

    matrix_free(&r);
    matrix_free(&p);
    matrix_free(&Ap);
    matrix_free(&x);

    return OK;
}

