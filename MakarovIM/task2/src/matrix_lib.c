#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <stddef.h>
#include "matrix_lib.h"
#include <limits.h>


bool matrix_is_empty(const Matrix M) 
{
    return (M.data == NULL);
}


bool matrix_is_square(const Matrix M) 
{
    return (M.rows == M.cols);
}


bool matrix_equal_size(const Matrix A, const Matrix B) 
{
    return (A.rows == B.rows) && (A.cols == B.cols);
}


void  matrix_set(Matrix* M, size_t row, size_t col, double number) 
{
    if (row >= M->rows || col >= M->cols) {
        matrix_log(ERROR, __func__, "Индекс вышел за измерения матрицы");
        return;
    }
    M->data[row * M->cols + col] = number;
}


double matrix_get(const Matrix M, size_t row, size_t col) 
{
    if (row >= M.rows || col >= M.cols) {
        matrix_log(ERROR, __func__, "Индекс вышел за измерения матрицы");
        return NAN;
    }
    return M.data[row * M.cols + col];
}


void matrix_free(Matrix* M) 
{
    if (M->data) {
        free(M->data);
        M->data = NULL;
    }
}


void matrix_print(const Matrix M) 
{
    if (matrix_is_empty(M)) {
        printf("Матрица не инициализирована\n");
        return;
    }
    for (size_t row = 0; row < M.rows; ++row) {
        for (size_t col = 0; col < M.cols; ++col) {
            printf("%0.2f ", matrix_get(M, row, col));
        }
        printf("\n");
    }
    printf("\n");
}


void matrix_log(MatrixLogLevel level, const char* location, const char* msg) 
{
    const char* level_str;
    switch (level) {
        case ERROR: level_str = "ERROR"; break;
        case WARNING: level_str = "WARNING"; break;
        case INFO: level_str = "INFO"; break;
        default: level_str = "UNKNOWN"; break;
    }
    printf("%s: %s\nLocation: %s\n\n", level_str, msg, location);
}


Matrix matrix_allocate(const size_t rows, const size_t cols) 
{
    Matrix M;
    
    if (rows == 0 || cols == 0) {
        matrix_log(ERROR, __func__, "Матрица содержит 0 столбцов или строк");
        return (Matrix) {0, 0, NULL}; 
    }

    M.data = malloc(rows * cols * sizeof(double));
    if (M.data == NULL) {
        matrix_log(ERROR, __func__, "Сбой выделения памяти");
        return (Matrix) {0, 0, NULL}; 
    }

    M.rows = rows;
    M.cols = cols;
    return M;
}


Matrix matrix_identity(size_t size) 
{
    Matrix I = matrix_allocate(size, size);
    if (I.data == NULL) {
        matrix_log(ERROR, __func__, "Ошибка выделения памяти");
        return (Matrix){0, 0, NULL};
    }

    for (size_t row = 0; row < size; ++row) {
        for (size_t col = 0; col < size; ++col) {
            if (row == col) {
                I.data[row * size + col] = 1.0; 
            } else {
                I.data[row * size + col] = 0.0; 
            }
        }
    }
    return I;
}


MatrixStatus matrix_copy(const Matrix src, Matrix* dest) 
{
    if (dest->rows != src.rows || dest->cols != src.cols) {
        matrix_log(ERROR, __func__, "Размеры матриц не совпадают при копировании");
        return MAT_DIMENSION_ERR;
    }
    memcpy(dest->data, src.data, src.rows * src.cols * sizeof(double));
    return MAT_OK;
}


MatrixStatus matrix_sum(const Matrix A, const Matrix B, Matrix* result) 
{
    if (!matrix_equal_size(A, B)) {
        matrix_log(ERROR, __func__, "Матрицы должны быть одинаковых измерений");
        return MAT_DIMENSION_ERR;
    }

    *result = matrix_allocate(A.rows, A.cols);
    if (result->data == NULL) {
        matrix_log(ERROR, __func__, "Ошибка выделения памяти");
        return MAT_MEMORY_ERR;
    }

    for (size_t row = 0; row < A.rows; ++row) {
        for (size_t col = 0; col < A.cols; ++col) {
            result->data[row * result->cols + col] = matrix_get(A, row, col) + matrix_get(B, row, col);
        }
    }
    return MAT_OK;
}


MatrixStatus matrix_subtract(const Matrix A, const Matrix B, Matrix* result) 
{
    if (!matrix_equal_size(A, B)) {
        matrix_log(ERROR, __func__, "Матрицы должны быть одинаковых измерений");
        return MAT_DIMENSION_ERR;
    }

    *result = matrix_allocate(A.rows, A.cols);
    if (result->data == NULL) {
        matrix_log(ERROR, __func__, "Ошибка выделения памяти");
        return MAT_MEMORY_ERR;
    }

    for (size_t row = 0; row < A.rows; ++row) {
        for (size_t col = 0; col < A.cols; ++col) {
            result->data[row * result->cols + col] = matrix_get(A, row, col) - matrix_get(B, row, col);
        }
    }
    return MAT_OK;
}


MatrixStatus matrix_multiply(const Matrix A, const Matrix B, Matrix* result) 
{
    if (A.cols != B.rows) {
        matrix_log(ERROR, __func__, "Измерения матриц не пригодны для умножения");
        return MAT_DIMENSION_ERR;
    }

    *result = matrix_allocate(A.rows, B.cols);
    if (result->data == NULL) {
        matrix_log(ERROR, __func__, "Ошибка выделения памяти");
        return MAT_MEMORY_ERR;
    }

    for (size_t row = 0; row < A.rows; ++row) {
        for (size_t col = 0; col < B.cols; ++col) {
            for (size_t k = 0; k < A.cols; ++k) {
                result->data[row * result->cols + col] += matrix_get(A, row, k) * matrix_get(B, k, col);
            }
        }
    }
    return MAT_OK;
}


MatrixStatus matrix_scalar_multiply(Matrix M, double scalar, Matrix* result) 
{

    *result = matrix_allocate(M.rows, M.cols);
    if (result->data == NULL) {
        return MAT_MEMORY_ERR;
    }

    for (size_t row = 0; row < M.rows; ++row) {
        for (size_t col = 0; col < M.cols; ++col) {
            result->data[row * M.cols + col] = M.data[row * M.cols + col] * scalar;
        }
    }

    return MAT_OK;
}


MatrixStatus matrix_transpose(const Matrix M,  Matrix* result) 
{
    *result = matrix_allocate(M.rows, M.cols);
    for (size_t row = 0; row < M.rows; ++row) {
        for (size_t col = 0; col < M.cols; ++col) {
            result->data[col * result->cols + row] = result->data[row * result->cols + col];
        }
    }
    return MAT_OK;
}


double matrix_determinant(const Matrix M) 
{
    if (!matrix_is_square(M)) {
        matrix_log(ERROR, __func__, "Определитель может быть вычислен только для квадратных матриц");
        return NAN; 
    }

    double det;

    if (M.rows == 0) {
        return NAN;
    }

    if (M.rows == 1) {
        det = M.data[0];
        return det;
    }
    
    if (M.rows == 2) {
        return M.data[0] * M.data[3] - M.data[1] * M.data[2];
    }

    // Рекурсивное вычисление определителя для больших матриц
    if (M.rows == 3) {
        det = M.data[0] * M.data[4] * M.data[8] 
            + M.data[1] * M.data[5] * M.data[6] 
            + M.data[3] * M.data[7] * M.data[2] 
            - M.data[2] * M.data[4] * M.data[6] 
            - M.data[3] * M.data[1] * M.data[8] 
            - M.data[0] * M.data[5] * M.data[7];
        return det;
    }
    return NAN;
}


MatrixStatus matrix_power(const Matrix M, int power,  Matrix* result) 
{
    if (!matrix_is_square(M)) {
        matrix_log(ERROR, __func__, "В степень возводятся только квадратные матрицы");
        return MAT_DIMENSION_ERR; 
    }

    *result = matrix_identity(M.rows);
    if (result->data == NULL) {
        return MAT_MEMORY_ERR; 
    }

    for (int p = 0; p < power; ++p) {
        Matrix temp;
        if (matrix_multiply(*result, M, &temp) == MAT_OK) {
            matrix_free(result);
            *result = temp;
        } else {
            matrix_free(result);
            return MAT_INTERNAL_ERR;
        }
    }

    return MAT_OK;
}


unsigned long long factorial(unsigned int n) 
{
    if (n > 20) { 
        matrix_log(ERROR, __func__, "Факториал слишком велик для вычисления (n > 20)\n");
        return 0; 
    }

    if (n == 0 || n == 1) {
        return 1; // Факториал 0 и 1 равен 1
    }

    unsigned long long result = 1;

    for (unsigned int idx = 2; idx <= n; ++idx) {
        result *= idx;
    }

    return result;
}


MatrixStatus matrix_exponent(const Matrix A, unsigned int num, Matrix* result) 
{
    if (!matrix_is_square(A)) {
        matrix_log(WARNING, __func__, "Матрица должна быть квадратной для вычисления экспоненты");
        return MAT_DIMENSION_ERR;
    }

    *result = matrix_identity(A.rows);
    if (result->data == NULL) {
        return MAT_MEMORY_ERR; 
    }

    if (num == 1) {
        return MAT_OK; // Если степень равна 1, результат уже в result
    }

    for (size_t cur_num = 1; cur_num < num; ++cur_num) {
        Matrix tmp;

        MatrixStatus power_status = matrix_power(A, cur_num, &tmp);
        if (power_status != MAT_OK) {
            matrix_free(result);
            return MAT_INTERNAL_ERR;
        }

        Matrix scaled_tmp;

        MatrixStatus scale_status = matrix_scalar_multiply(tmp, 1.0 / factorial(cur_num), &scaled_tmp);
        matrix_free(&tmp); 
        if (scale_status != MAT_OK) {
            matrix_free(result);
            return MAT_INTERNAL_ERR; 
        }

        Matrix new_E;

        MatrixStatus sum_status = matrix_sum(*result, scaled_tmp, &new_E);
        matrix_free(&scaled_tmp); 

        if (sum_status != MAT_OK) {
            matrix_free(result);
            return sum_status; 
        }

        matrix_free(result); 
        *result = new_E;   
    }

    return MAT_OK; 
}


// numbers == &numbers[0]
// *numbers == numbers[0]