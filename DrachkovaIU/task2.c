/* Type your code here, or load an example. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    size_t rows;
    size_t cols;
    double *data;
} Matrix;

Matrix MATRIX_ZERO = {0, 0, NULL};  // Нулевая матрица

typedef enum {ERROR, WARNING} MatrixException;

// Функция для вывода сообщений об ошибках
void matrix_error(const MatrixException error_level, const char *message) {
    if (error_level == ERROR) {
        fprintf(stderr, "ERROR: %s", message);
    } else if (error_level == WARNING) {
        fprintf(stderr, "WARNING: %s", message);
    }
}

// Функция для выделения памяти под матрицу
Matrix matrix_allocate(const Matrix frame) {
    Matrix matrix = MATRIX_ZERO;
    if (frame.rows == 0 || frame.cols == 0) {
        return matrix;
    }
    matrix.rows = frame.rows;
    matrix.cols = frame.cols;

    matrix.data = calloc(matrix.rows * matrix.cols, sizeof(double));
    if (matrix.data == NULL) {
        matrix_error(ERROR, "Ошибка обращения к памяти.\n");
        return MATRIX_ZERO;
    }
    return matrix;
}

// Функция для освобождения памяти матрицы
void memory_free(Matrix *matrix) {
    if (matrix != NULL && matrix->data != NULL) {
        free(matrix->data);
        matrix->data = NULL;
    }
    matrix->rows = 0;
    matrix->cols = 0;
}

// Функция для передачи массива в матрицу
void matrix_pass_array(Matrix *matrix, double array_name[]) {
    *matrix = matrix_allocate(*matrix);
    memcpy(matrix->data, array_name, matrix->rows * matrix->cols * sizeof(double));
}

// Функция для вывода матрицы
void matrix_print(const Matrix *matrix, const char *message) {
    printf("%s", message);
    if (matrix->data == NULL) {
        printf("Невозможно выполнить операцию.\n\n");
        return;
    }
    for (size_t row = 0; row < matrix->rows; row++) {
        for (size_t col = 0; col < matrix->cols; col++) {
            printf("%.2f ", matrix->data[row * matrix->cols + col]);
        }
        printf("\n");
    }
    printf("\n");
}

// Функция для копирования матрицы
void matrix_copy(Matrix destination, const Matrix source) {
    if (destination.rows != source.rows || destination.cols != source.cols) {
        matrix_error(ERROR, "Невозможно выполнить операцию для матриц разных размеров.\n");
        return;
    }
    memcpy(destination.data, source.data, source.rows * source.cols * sizeof(double));
}

// Функция сложения матриц
Matrix matrix_sum(const Matrix A, const Matrix B) {
    if (A.rows != B.rows || A.cols != B.cols) {
        matrix_error(ERROR, "Невозможно добавить матрицы разных размеров.\n");
        return MATRIX_ZERO;
    }

    Matrix C = matrix_allocate((Matrix){A.rows, A.cols});
    for (size_t idx = 0; idx < A.rows * A.cols; idx++) {
        C.data[idx] = A.data[idx] + B.data[idx];
    }
    return C;
}

// Функция вычитания матриц
Matrix matrix_subtract(const Matrix A, const Matrix B) {
    if (A.rows != B.rows || A.cols != B.cols) {
        matrix_error(ERROR, "Невозможно вычитать матрицы разных размеров.\n");
        return MATRIX_ZERO;
    }

    Matrix C = matrix_allocate((Matrix){A.rows, A.cols});
    for (size_t idx = 0; idx < A.rows * A.cols; idx++) {
        C.data[idx] = A.data[idx] - B.data[idx];
    }
    return C;
}

// Функция умножения матриц
Matrix matrix_multiply(const Matrix A, const Matrix B) {
    if (A.cols != B.rows) {
        matrix_error(ERROR, "Невозможно перемножить матрицы несовместимых размеров.\n");
        return MATRIX_ZERO;
    }

    Matrix C = matrix_allocate((Matrix){A.rows, B.cols});
    for (size_t row = 0; row < C.rows; row++) {
        for (size_t col = 0; col < C.cols; col++) {
            for (size_t k = 0; k < A.cols; k++) {
                C.data[row * C.cols + col] += A.data[row * A.cols + k] * B.data[k * B.cols + col];
            }
        }
    }
    return C;
}

// Функция умножения матрицы на число
Matrix matrix_multi_by_number(const Matrix A, double number) {
    Matrix C = matrix_allocate((Matrix){A.rows, A.cols});
    for (size_t idx = 0; idx < A.rows * A.cols; idx++) {
        C.data[idx] = A.data[idx] * number;
    }
    return C;
}

// Функция транспонирования матрицы
Matrix matrix_transpose(const Matrix A) {
    Matrix T = matrix_allocate((Matrix){A.cols, A.rows});
    for (size_t row = 0; row < A.rows; row++) {
        for (size_t col = 0; col < A.cols; col++) {
            T.data[col * T.rows + row] = A.data[row * A.cols + col];
        }
    }
    return T;
}

// Функция нахождения детерминанта
double matrix_determinant(const Matrix A) {
    if (A.rows != A.cols) return NAN;
    if (A.rows == 1) return A.data[0];
    if (A.rows == 2) return A.data[0] * A.data[3] - A.data[1] * A.data[2];

    double det = 0;
    for (size_t col = 0; col < A.cols; col++) {
        Matrix submatrix = matrix_allocate((Matrix){A.rows - 1, A.cols - 1});
        size_t sub_idx = 0;
        for (size_t i = 1; i < A.rows; i++) {
            for (size_t j = 0; j < A.cols; j++) {
                if (j == col) continue;
                submatrix.data[sub_idx++] = A.data[i * A.cols + j];
            }
        }
        det += pow(-1, col) * A.data[col] * matrix_determinant(submatrix);
        memory_free(&submatrix);
    }
    return det;
}

void calculations() {
    
}

int main() {
    calculations();
    return 0;
}
