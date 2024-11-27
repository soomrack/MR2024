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
void matrix_error(const MatrixException error_level, const char *message) 
{
    if (error_level == ERROR) {
        fprintf(stderr, "ERROR: %s", message);
    } else if (error_level == WARNING) {
        fprintf(stderr, "WARNING: %s", message);
    }
}

// Функция для выделения памяти под матрицу
Matrix matrix_allocate(const size_t rows, const size_t cols) 
{
    Matrix matrix = MATRIX_ZERO;
    if (rows == 0 || cols == 0) {
        return matrix;
    }
    matrix.rows = rows;
    matrix.cols = cols;

    matrix.data = calloc(matrix.rows * matrix.cols, sizeof(double));
    if (matrix.data == NULL) {
        matrix_error(ERROR, "Ошибка обращения к памяти.\n");
        return MATRIX_ZERO;
    }
    return matrix;
}

// Функция для освобождения памяти матрицы
void memory_free(Matrix *matrix) 
{
    if (matrix != NULL) {
        matrix->rows = 0;
        matrix->cols = 0;
        free(matrix->data);
        matrix->data = NULL;
    }
}

// Функция для передачи массива в матрицу
void matrix_fill(Matrix *matrix, const double array_name[]) 
{
    if (matrix->data != NULL) {
        memcpy(matrix->data, array_name, matrix->rows * matrix->cols * sizeof(double));
    }
}

// Функция для вывода матрицы
void matrix_print(const Matrix *matrix, const char *message) 
{
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
void matrix_copy(Matrix destination, const Matrix source) 
{
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

    Matrix C = matrix_allocate(A.rows, A.cols);
    for (size_t idx = 0; idx < A.rows * A.cols; idx++) {
        C.data[idx] = A.data[idx] + B.data[idx];
    }
    return C;
}

// Функция вычитания матриц
Matrix matrix_subtract(const Matrix A, const Matrix B) 
{
    if (A.rows != B.rows || A.cols != B.cols) {
        matrix_error(ERROR, "Невозможно вычитать матрицы разных размеров.\n");
        return MATRIX_ZERO;
    }

    Matrix C = matrix_allocate(A.rows, A.cols);
    for (size_t idx = 0; idx < A.rows * A.cols; idx++) {
        C.data[idx] = A.data[idx] - B.data[idx];
    }
    return C;
}

// Функция умножения матриц
Matrix matrix_multiply(const Matrix A, const Matrix B) 
{
    if (A.cols != B.rows) {
        matrix_error(ERROR, "Невозможно перемножить матрицы несовместимых размеров.\n");
        return MATRIX_ZERO;
    }

     Matrix C = matrix_allocate(A.rows, B.cols);
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
Matrix matrix_multi_by_number(const Matrix A, double number) 
{
    Matrix C = matrix_allocate(A.rows, A.cols);
    for (size_t idx = 0; idx < A.rows * A.cols; idx++) {
        C.data[idx] = A.data[idx] * number;
    }
    return C;
}

// Функция транспонирования матрицы
Matrix matrix_transpose(const Matrix A) 
{
    Matrix T = matrix_allocate(A.cols, A.rows);
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
        Matrix submatrix = matrix_allocate(A.rows - 1, A.cols - 1);
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

// Функция нахождения матричной экспоненты
Matrix matrix_exponent(const Matrix A, size_t terms) 
{
    if (A.rows != A.cols) {
        matrix_error(ERROR, "Матричная экспонента определяется только для квадратных матриц.\n");
        return MATRIX_ZERO;
    }

    Matrix result = matrix_allocate(A.rows, A.cols);
    Matrix term = matrix_allocate(A.rows, A.cols);
    for (size_t i = 0; i < A.rows; i++) {              // Инициализация единичной матрицы
        result.data[i * A.cols + i] = 1.0;
        term.data[i * A.cols + i] = 1.0;
    }

    for (size_t n = 1; n < terms; n++) {
        term = matrix_multiply(term, A);
        double factor = 1.0 / tgamma(n + 1);
        for (size_t i = 0; i < term.rows * term.cols; i++) {
            result.data[i] += term.data[i] * factor;
        }
    }
    memory_free(&term);
    return result;
}

void calculations() {
    Matrix A = matrix_allocate(2, 2);
    Matrix B = matrix_allocate(2, 2);

    matrix_fill(&A, (double[]){1, 2, 3, 4});
    matrix_fill(&B, (double[]){5, 6, 7, 8});

    Matrix add_A_B = matrix_sum(A, B);
    printf("Сложение:\n");
    matrix_print(&add_A_B, "Результат суммирования: \n");

    Matrix multi_A_B = matrix_multiply(A, B);
    printf("Умножение:\n");
    matrix_print(&multi_A_B, "Результат умножения: \n");

    Matrix multi_by_numbder_A = matrix_multi_by_number(A, 2);
    matrix_print(&multi_by_numbder_A, "Результат умножения матрицы на число: \n");
    Matrix multi_by_numbder_B = matrix_multi_by_number(B, 2);
    matrix_print(&multi_by_numbder_B, "RРезультат умножения матрицы на число: \n");

    Matrix transpose_A = matrix_transpose(A);
    printf("Транспонирование A:\n");
    matrix_print(&transpose_A, "Результат транспонирования: \n");
    Matrix transpose_B = matrix_transpose(B);
    printf("Транспонирование A:\n");
    matrix_print(&transpose_B, "Результат транспонирования: \n");

    double det_A = matrix_determinant(A);
    printf("Определитель A: %.2f\n", det_A);
    double det_B = matrix_determinant(B);
    printf("Определитель A: %.2f\n", det_B);

    Matrix exp_A = matrix_exponent(A, 20);
    printf("Экспонента A:\n");
    matrix_print(&exp_A, "Результат возведения экспоненты в степень: \n");
    Matrix exp_B = matrix_exponent(A, 20);
    printf("Экспонента B:\n");
    matrix_print(&exp_B, "Результат возведения экспоненты в степень: \n");

    memory_free(&A);
    memory_free(&B);
    memory_free(&add_A_B);
    memory_free(&multi_A_B);
    memory_free(&multi_by_numbder_A);
    memory_free(&multi_by_numbder_B);
    memory_free(&transpose_A);
    memory_free(&transpose_B);
    memory_free(&exp_A);
    memory_free(&exp_B);
}

int main() {
    calculations();
    return 0;
}
