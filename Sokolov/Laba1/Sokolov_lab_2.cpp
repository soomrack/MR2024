#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct {
    size_t rows;
    size_t cols;
    double* data;
} Matrix;

typedef enum {
    MATRIX_ERROR,
    MATRIX_WARNING,
    MATRIX_INFO
} MatrixExceptionLevel;


void matrix_handle_exception(MatrixExceptionLevel level, const char* message) {
    switch (level) {
    case MATRIX_ERROR:
        fprintf(stderr, "ERROR: %s\n", message);
        exit(EXIT_FAILURE);
    case MATRIX_WARNING:
        fprintf(stderr, "WARNING: %s\n", message);
        break;
    case MATRIX_INFO:
        printf("INFO: %s\n", message);
        break;
    }
}


Matrix matrix_create(size_t rows, size_t cols) {
    Matrix mat = { rows, cols, NULL };
    mat.data = (double*)malloc(rows * cols * sizeof(double));
    if (!mat.data) {
        matrix_handle_exception(MATRIX_ERROR, "Failed to allocate memory for matrix");
    }
    return mat;
}


void matrix_free(Matrix* mat) {
    if (mat->data) {
        free(mat->data);
        mat->data = NULL;
    }
}


Matrix matrix_duplicate(const Matrix* mat) {
    Matrix copy = matrix_create(mat->rows, mat->cols);
    memcpy(copy.data, mat->data, mat->rows * mat->cols * sizeof(double));
    return copy;
}


void matrix_zero(Matrix* mat) {
    memset(mat->data, 0, mat->rows * mat->cols * sizeof(double));
}


void matrix_identity(Matrix* mat) {
    if (mat->rows != mat->cols) {
        matrix_handle_exception(MATRIX_ERROR, "Matrix must be square for identity operation");
    }
    matrix_zero(mat);
    for (size_t i = 0; i < mat->rows; ++i) {
        mat->data[i * mat->cols + i] = 1.0;
    }
}


void matrix_scalar_multiply(Matrix* mat, double scalar) {
    for (size_t i = 0; i < mat->rows * mat->cols; ++i) {
        mat->data[i] *= scalar;
    }
}


Matrix matrix_add(const Matrix* A, const Matrix* B) {
    if (A->rows != B->rows || A->cols != B->cols) {
        matrix_handle_exception(MATRIX_ERROR, "Matrix dimensions must match for addition");
    }
    Matrix result = matrix_create(A->rows, A->cols);
    for (size_t i = 0; i < A->rows * A->cols; ++i) {
        result.data[i] = A->data[i] + B->data[i];
    }
    return result;
}


Matrix matrix_subtract(const Matrix* A, const Matrix* B) {
    if (A->rows != B->rows || A->cols != B->cols) {
        matrix_handle_exception(MATRIX_ERROR, "Matrix dimensions must match for subtraction");
    }
    Matrix result = matrix_create(A->rows, A->cols);
    for (size_t i = 0; i < A->rows * A->cols; ++i) {
        result.data[i] = A->data[i] - B->data[i];
    }
    return result;
}


Matrix matrix_multiply(const Matrix* A, const Matrix* B) {
    if (A->cols != B->rows) {
        matrix_handle_exception(MATRIX_ERROR, "Matrix dimensions are invalid for multiplication");
    }
    Matrix result = matrix_create(A->rows, B->cols);
    for (size_t i = 0; i < A->rows; ++i) {
        for (size_t j = 0; j < B->cols; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < A->cols; ++k) {
                sum += A->data[i * A->cols + k] * B->data[k * B->cols + j];
            }
            result.data[i * B->cols + j] = sum;
        }
    }
    return result;
}


Matrix matrix_transpose(const Matrix* mat) {
    Matrix result = matrix_create(mat->cols, mat->rows);
    for (size_t i = 0; i < mat->rows; ++i) {
        for (size_t j = 0; j < mat->cols; ++j) {
            result.data[j * mat->rows + i] = mat->data[i * mat->cols + j];
        }
    }
    return result;
}


double matrix_determinant_3x3(const Matrix* A) {
    if (A->rows != 3 || A->cols != 3) {
        matrix_handle_exception(MATRIX_ERROR, "Determinant calculation is only implemented for 3x3 matrices");
        return 0.0;
    }

    double det = A->data[0] * (A->data[4] * A->data[8] - A->data[5] * A->data[7]) -
        A->data[1] * (A->data[3] * A->data[8] - A->data[5] * A->data[6]) +
        A->data[2] * (A->data[3] * A->data[7] - A->data[4] * A->data[6]);

    return det;
}


Matrix matrix_exponent(const Matrix* mat) {
    if (mat->rows != mat->cols) {
        matrix_handle_exception(MATRIX_ERROR, "Matrix must be square for exponentiation");
    }

    Matrix result = matrix_create(mat->rows, mat->cols);
    matrix_identity(&result);

    Matrix term = matrix_duplicate(&result);
    for (size_t k = 1; k <= 10; ++k) {
        Matrix temp = matrix_multiply(&term, mat);
        matrix_free(&term);
        term = temp;
        matrix_scalar_multiply(&term, 1.0 / k);

        Matrix new_result = matrix_add(&result, &term);
        matrix_free(&result);
        result = new_result;
    }
    matrix_free(&term);
    return result;
}


void matrix_set(Matrix mat, const double* values) {
    memcpy(mat.data, values, mat.rows * mat.cols * sizeof(double));
}


int main() {
    Matrix A = matrix_create(2, 2);
    Matrix B = matrix_create(2, 2);

    matrix_set(A, (double[]) {
        1.0, 2.0,
            3.0, 4.0
    });
    matrix_set(B, (double[]) {
        1.0, 0.0,
            0.0, 1.0
    });

    matrix_free(&A);
    matrix_free(&B);

    return EXIT_SUCCESS;
}
