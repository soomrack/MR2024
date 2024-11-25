#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

struct Matrix {
    size_t rows;
    size_t cols;
    double* data;
};
typedef struct Matrix Matrix;

enum MatrixExceptionLevel {
    MATRIX_ERROR,
    MATRIX_WARNING,
    MATRIX_INFO
};
typedef enum MatrixExceptionLevel;


void matrix_handle_exception(const MatrixExceptionLevel level, const char* message) {
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


Matrix matrix_create(const size_t rows, const size_t cols) {
    if (rows == 0 || cols == 0) {
        matrix_handle_exception(MATRIX_ERROR, "Matrix dimensions cannot be zero");
        return (Matrix) { 0, 0, NULL };
    }

    size_t size = rows * cols;
    if (size / cols != rows) {
        matrix_handle_exception(MATRIX_ERROR, "Matrix dimensions are too large");
        return (Matrix) { 0, 0, NULL };
    }

    Matrix mat = { rows, cols, (double*)malloc(size * sizeof(double)) };
    if (!mat.data) {
        matrix_handle_exception(MATRIX_ERROR, "Failed to allocate memory for matrix");
        return (Matrix) { 0, 0, NULL };
    }
    return mat;
}


void matrix_free(Matrix* mat) {
    if (mat == NULL || mat->data == NULL) {
        return;
    }
    free(mat->data);
    mat->data = NULL;
    mat->rows = 0;
    mat->cols = 0;
}


void matrix_identity(Matrix mat) {
    if (mat.rows != mat.cols) {
        matrix_handle_exception(MATRIX_ERROR, "Matrix must be square for identity operation");
        return;
    }
    memset(mat.data, 0, mat.rows * mat.cols * sizeof(double));
    for (size_t idx = 0; idx < mat.rows; ++idx) {
        mat.data[idx * mat.cols + idx] = 1.0;
    }
}


Matrix matrix_add(Matrix A, Matrix B) {
    if (A.rows != B.rows || A.cols != B.cols) {
        matrix_handle_exception(MATRIX_ERROR, "Matrix dimensions must match for addition");
        return (Matrix) { 0, 0, NULL };
    }
    Matrix result = matrix_create(A.rows, A.cols);
    if (!result.data) {
        matrix_handle_exception(MATRIX_ERROR, "Failed to allocate memory for addition");
        return (Matrix) { 0, 0, NULL };
    }
    for (size_t idx = 0; idx < result.rows * result.cols; ++idx) {
        result.data[idx] = A.data[idx] + B.data[idx];
    }
    return result;
}


Matrix matrix_multiply(Matrix A, Matrix B) {
    if (A.cols != B.rows) {
        matrix_handle_exception(MATRIX_ERROR, "Matrix dimensions are invalid for multiplication");
        return (Matrix) { 0, 0, NULL };
    }
    Matrix result = matrix_create(A.rows, B.cols);
    if (!result.data) {
        matrix_handle_exception(MATRIX_ERROR, "Failed to allocate memory for multiplication");
        return (Matrix) { 0, 0, NULL };
    }
    for (size_t i = 0; i < A.rows; ++i) {
        for (size_t j = 0; j < B.cols; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < A.cols; ++k) {
                sum += A.data[i * A.cols + k] * B.data[k * B.cols + j];
            }
            result.data[i * B.cols + j] = sum;
        }
    }
    return result;
}


Matrix matrix_exponent(Matrix mat) {
    if (mat.rows != mat.cols) {
        matrix_handle_exception(MATRIX_ERROR, "Matrix must be square for exponentiation");
        return (Matrix) { 0, 0, NULL };
    }

    Matrix result = matrix_create(mat.rows, mat.cols);
    if (!result.data) {
        matrix_handle_exception(MATRIX_ERROR, "Failed to allocate memory for exponentiation result");
        return (Matrix) { 0, 0, NULL };
    }
    matrix_identity(result);

    Matrix term = matrix_create(mat.rows, mat.cols);
    if (!term.data) {
        matrix_free(&result);
        matrix_handle_exception(MATRIX_ERROR, "Failed to allocate memory for term");
        return (Matrix) { 0, 0, NULL };
    }
    memcpy(term.data, result.data, mat.rows * mat.cols * sizeof(double));

    for (size_t k = 1; k <= 10; ++k) {
        Matrix temp = matrix_multiply(term, mat);
        matrix_free(&term);
        term = temp;
        if (!term.data) {
            matrix_free(&result);
            matrix_handle_exception(MATRIX_ERROR, "Failed during term calculation");
            return (Matrix) { 0, 0, NULL };
        }
        double factor = 1.0 / k;
        for (size_t i = 0; i < term.rows * term.cols; ++i) {
            term.data[i] *= factor;
        }

        Matrix new_result = matrix_add(result, term);
        matrix_free(&result);
        result = new_result;
        if (!result.data) {
            matrix_free(&term);
            matrix_handle_exception(MATRIX_ERROR, "Failed during result calculation");
            return (Matrix) { 0, 0, NULL };
        }
    }
    matrix_free(&term);
    return result;
}


void matrix_set(Matrix mat, const double* values) {
    if (mat.data == NULL || values == NULL) {
        matrix_handle_exception(MATRIX_WARNING, "Invalid pointer in matrix_set");
        return;
    }
    memcpy(mat.data, values, mat.rows * mat.cols * sizeof(double));
}


void matrix_print(const Matrix mat, const char* title) {
    if (mat.data == NULL) {
        matrix_handle_exception(MATRIX_WARNING, "Matrix is empty");
        return;
    }
    printf("%s:\n", title);
    for (size_t idx = 0; idx < mat.rows; ++idx) {
        for (size_t idy = 0; idy < mat.cols; ++idy) {
            printf("%6.2f ", mat.data[idx * mat.cols + idy]);
        }
        printf("\n");
    }
}


int main() {
    
    Matrix A = matrix_create(2, 2);
    matrix_set(A, (double[]) {
        0.0, 1.0,
            -1.0, 0.0
    });

    
    Matrix expA = matrix_exponent(A);
    if (expA.data) {
        matrix_print(expA, "Matrix exponent (e^A)");
    }

    
    matrix_free(&A);
    matrix_free(&expA);

    return EXIT_SUCCESS;
}
