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
typedef enum MatrixExceptionLevel MatrixExceptionLevel;

// Обработчик ошибок
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

// Создание матрицы
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

// Освобождение памяти
void matrix_free(Matrix* mat) {
    if (mat == NULL || mat->data == NULL) {
        return;
    }
    free(mat->data);
    mat->data = NULL;
    mat->rows = 0;
    mat->cols = 0;
}

// Превращение в единичную матрицу
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

// Умножение матрицы на скаляр
Matrix matrix_scalar_multiply(Matrix mat, double scalar) {
    if (mat.data == NULL) {
        matrix_handle_exception(MATRIX_WARNING, "Matrix is empty");
        return (Matrix) { 0, 0, NULL };
    }

    Matrix result = matrix_create(mat.rows, mat.cols);
    
    for (size_t idx = 0; idx < result.rows * result.cols; ++idx) {
        result.data[idx] = mat.data[idx] * scalar;
    }
    return result;
}

// Нахождение определителя матрицы
double matrix_determinant(Matrix mat) {
    if (mat.rows != mat.cols) {
        matrix_handle_exception(MATRIX_ERROR, "Matrix must be square to calculate determinant");
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
    matrix_handle_exception(MATRIX_WARNING, "Determinant is not implemented for matrices larger than 3x3");
    return NAN;
}

// Сложение матриц
Matrix matrix_add(Matrix A, Matrix B) {
    if (A.rows != B.rows || A.cols != B.cols) {
        matrix_handle_exception(MATRIX_ERROR, "Matrix dimensions must match for addition");
        return (Matrix) { 0, 0, NULL };
    }
    Matrix result = matrix_create(A.rows, A.cols);
    for (size_t idx = 0; idx < result.rows * result.cols; ++idx) {
        result.data[idx] = A.data[idx] + B.data[idx];
    }
    return result;
}

// Умножение матриц
Matrix matrix_multiply(Matrix A, Matrix B) {
    if (A.cols != B.rows) {
        matrix_handle_exception(MATRIX_ERROR, "Matrix dimensions are invalid for multiplication");
        return (Matrix) { 0, 0, NULL };
    }
    Matrix result = matrix_create(A.rows, B.cols);
    for (size_t idx = 0; idx < A.rows; ++idx) {
        for (size_t idy = 0; idy < B.cols; ++idy) {
            double sum = 0.0;
            for (size_t k = 0; k < A.cols; ++k) {
                sum += A.data[idx * A.cols + k] * B.data[k * B.cols + idy];
            }
            result.data[idx * B.cols + idy] = sum;
        }
    }
    return result;
}

// Нахождение транспонированной матрицы
Matrix matrix_transpose(Matrix mat) {
    Matrix result = matrix_create(mat.cols, mat.rows);
    for (size_t idx = 0; idx < mat.rows; ++idx) {
        for (size_t idy = 0; idy < mat.cols; ++idy) {
            result.data[idy * mat.rows + idx] = mat.data[idx * mat.cols + idy];
        }
    }
    return result;
}

// Нахождение экспоненты матрицы
Matrix matrix_exponent(Matrix mat) {
    if (mat.rows != mat.cols) {
        matrix_handle_exception(MATRIX_ERROR, "Matrix must be square for exponentiation");
        return (Matrix) { 0, 0, NULL };
    }

    Matrix result = matrix_create(mat.rows, mat.cols);
    matrix_identity(result);

    Matrix term = matrix_create(mat.rows, mat.cols);
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
        matrix_scalar_multiply(term, 1.0 / k);
        Matrix new_result = matrix_add(result, term);
        matrix_free(&result);
        result = new_result;
    }
    matrix_free(&term);
    return result;
}

// Инициализация матрицы
void matrix_set(Matrix mat, const double* values) {
    if (mat.data == NULL || values == NULL) {
        matrix_handle_exception(MATRIX_WARNING, "Invalid pointer in matrix_set");
        return;
    }
    memcpy(mat.data, values, mat.rows * mat.cols * sizeof(double));
}

// Универсальный вывод результата
void matrix_print(const char* title, Matrix mat, double scalar_result) {
    if (mat.data != NULL) {
        printf("%s:\n", title);
        for (size_t idx = 0; idx < mat.rows; ++idx) {
            for (size_t idy = 0; idy < mat.cols; ++idy) {
                printf("%6.2f ", mat.data[idx * mat.cols + idy]);
            }
            printf("\n");
        }
    }
    else {
        printf("%s: %.2f\n", title, scalar_result);
    }
}

// Главная функция
int main() {
    Matrix A = matrix_create(2, 2);
    matrix_set(A, (double[]) {
        1.0, 2.0,
            3.0, 4.0
    });

    Matrix B = matrix_create(2, 2);
    matrix_set(B, (double[]) {
        5.0, 6.0,
            7.0, 8.0
    });
    
    Matrix productAscalar = matrix_scalar_multiply(A, 2.0);
    matrix_print("Matrix A multiplied by scalar 2", productAscalar, NAN);

    double detA = matrix_determinant(A);
    matrix_print("Determinant of A", (Matrix) { 0, 0, NULL }, detA);

    Matrix sumAB = matrix_add(A, B);
    matrix_print("Sum of A and B", sumAB, NAN);

    Matrix productAB = matrix_multiply(A, B);
    matrix_print("Product of A and B", productAB, NAN);

    Matrix expA = matrix_exponent(A);
    matrix_print("Matrix exponent (e^A)", expA, NAN);

    Matrix transposeA = matrix_transpose(A);
    matrix_print("Transpose of A", transposeA, NAN);

    matrix_free(&A);
    matrix_free(&B);
    matrix_free(&productAscalar);
    matrix_free(&sumAB);
    matrix_free(&productAB);
    matrix_free(&expA);
    matrix_free(&transposeA);

    return 0;
}
