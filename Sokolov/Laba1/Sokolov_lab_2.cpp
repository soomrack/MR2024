#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

struct {
    size_t rows;
    size_t cols;
    double* data;
} typedef Matrix;


enum {
    MATRIX_ERROR,
    MATRIX_WARNING,
    MATRIX_INFO
} typedef MatrixExceptionLevel;


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


Matrix matrix_duplicate(const Matrix mat) {
    if (mat.rows == 0 || mat.cols == 0) {
        matrix_handle_exception(MATRIX_WARNING, "Cannot duplicate an empty matrix");
        return (Matrix) { 0, 0, NULL };
    }

    Matrix copy = matrix_create(mat.rows, mat.cols);
    memcpy(copy.data, mat.data, mat.rows * mat.cols * sizeof(double));
    return copy;
}


void matrix_zero(Matrix mat) {
    memset(mat.data, 0, mat.rows * mat.cols * sizeof(double));
}


void matrix_identity(Matrix mat) {
    if (mat.rows != mat.cols) {
        matrix_handle_exception(MATRIX_ERROR, "Matrix must be square for identity operation");
        return;
    }
    matrix_zero(mat);
    for (size_t rows = 0; rows < mat.rows; ++rows) {
        mat.data[rows * mat.cols + rows] = 1.0;
    }
}


Matrix matrix_add(Matrix A, Matrix B) {
    if (A.rows != B.rows || A.cols != B.cols) {
        matrix_handle_exception(MATRIX_ERROR, "Matrix dimensions must match for addition");
        return (Matrix) { 0, 0, NULL };
    }
    Matrix result = matrix_create(A.rows, A.cols);
    for (size_t i = 0; i < result.rows * result.cols; ++i) {
        result.data[i] = A.data[i] + B.data[i];
    }
    return result;
}


Matrix matrix_subtract(Matrix A, Matrix B) {
    if (A.rows != B.rows || A.cols != B.cols) {
        matrix_handle_exception(MATRIX_ERROR, "Matrix dimensions must match for subtraction");
        return (Matrix) { 0, 0, NULL };
    }
    Matrix result = matrix_create(A.rows, A.cols);
    for (size_t rows = 0; rows < result.rows * result.cols; ++rows) {
        result.data[rows] = A.data[rows] - B.data[rows];
    }
    return result;
}


Matrix matrix_transpose(Matrix mat) {
    Matrix result = matrix_create(mat.cols, mat.rows);
    for (size_t rows = 0; rows < mat.rows; ++rows) {
        for (size_t cols = 0; cols < mat.cols; ++cols) {
            result.data[cols * result.cols + rows] = mat.data[rows * mat.cols + cols];
        }
    }
    return result;
}


double matrix_determinant(Matrix mat) {
    if (mat.rows != mat.cols) {
        matrix_handle_exception(MATRIX_ERROR, "Matrix must be square for determinant");
        return NAN;
    }
    if (mat.rows == 1) {
        return mat.data[0];
    }
    else if (mat.rows == 2) {
        return mat.data[0] * mat.data[3] - mat.data[1] * mat.data[2];
    }
    else if (mat.rows == 3) {
        return mat.data[0] * (mat.data[4] * mat.data[8] - mat.data[5] * mat.data[7])
            - mat.data[1] * (mat.data[3] * mat.data[8] - mat.data[5] * mat.data[6])
            + mat.data[2] * (mat.data[3] * mat.data[7] - mat.data[4] * mat.data[6]);
    }
    matrix_handle_exception(MATRIX_WARNING, "Determinant calculation is not implemented for matrices larger than 3x3");
    return NAN;
}


void matrix_set(Matrix mat, const double* values) {
    if (mat.data == NULL || values == NULL) {
        matrix_handle_exception(MATRIX_WARNING, "Invalid pointer in matrix_set");
        return;
    }
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

    printf("Determinant of A: %f\n", matrix_determinant(A));

    matrix_free(&A);
    matrix_free(&B);

    return EXIT_SUCCESS;
}
