#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <stddef.h>
#include "matrix_lib.h"
#include <limits.h>
#include <stdint.h>

bool pointer_on_matrix_is_null(const Matrix* M)
{
    if (M == NULL) {
        return MAT_UNINITIALIZED_ERR;
    }
}  


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
    if(M == NULL) {
        matrix_log(ERROR, __func__, "Передан NULL указатель\n");
        return;
    }

    free(M->data);
    *M = (Matrix) {0, 0, NULL};
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
        matrix_log(ERROR, __func__, "Матрица содержит 0 столбцов или строк\n");
        return (Matrix) {rows, cols, NULL}; 
    }

    if (rows >= SIZE_MAX / sizeof(double) / cols) {
		return (Matrix) {0, 0, NULL};
    }

    M.data = malloc(rows * cols * sizeof(double));

    if (M.data == NULL) {
        matrix_log(ERROR, __func__, "Сбой выделения памяти\n");
        return (Matrix) {rows, cols, NULL}; 
    }

    M.rows = rows;
    M.cols = cols;
    return M;
}


MatrixStatus matrix_zero(Matrix* M)
{
    if (matrix_is_empty(*M)) {
        matrix_log(WARNING, __func__, "Матрица пустая\n");
        return MAT_OK;
    }

    memset(M->data, 0, M->rows * M->cols * sizeof(double));

    return MAT_OK;
}


Matrix matrix_identity(size_t size) 
{
    Matrix I = matrix_allocate(size, size);

    matrix_zero(&I);
    for (size_t idx = 0; idx < I.rows * I.cols; idx += I.cols + 1) {
        I.data[idx] = 1.0;
    }
    return I;
}


MatrixStatus matrix_copy(Matrix* dest, const Matrix src) 
{
    if (!(pointer_on_matrix_is_null(dest))) {
        matrix_log(ERROR, __func__, "Указатель на матрицу dest NULL\n");
        return MAT_UNINITIALIZED_ERR;
    }
    if (!matrix_equal_size(src, *dest)) {
        matrix_log(ERROR, __func__, "Размеры матриц не совпадают при копировании\n");
        return MAT_DIMENSION_ERR;
    }
    if (matrix_is_empty(src)) {
        matrix_log(ERROR, __func__, "Матрица src пустая\n");
        return MAT_OK;
    }


    memcpy(dest->data, src.data, src.rows * src.cols * sizeof(double));
    return MAT_OK;
}


MatrixStatus matrix_sum(Matrix* result, const Matrix A, const Matrix B) 
{
    if (!(pointer_on_matrix_is_null(result))) {
        matrix_log(ERROR, __func__, "Указатель на матрицу result NULL\n");
        return MAT_UNINITIALIZED_ERR;
    }

    if (!matrix_equal_size(A, B)) {
        matrix_log(ERROR, __func__, "Матрицы должны быть одинаковых измерений\n");
        return MAT_DIMENSION_ERR;
    }

    *result = matrix_allocate(A.rows, A.cols);
    if (result->data == NULL) {
        return MAT_MEMORY_ERR;
    }

    for (size_t idx = 0; idx < A.rows * A.cols; ++idx) {
        result->data[idx] = A.data[idx] + B.data[idx];
    }

    return MAT_OK;
}


MatrixStatus matrix_subtract(Matrix* result, const Matrix A, const Matrix B) 
{
    if (!(pointer_on_matrix_is_null(result))) {
        matrix_log(ERROR, __func__, "Указатель на матрицу result NULL\n");
        return MAT_UNINITIALIZED_ERR;
    }

    if (!matrix_equal_size(A, B)) {
        matrix_log(ERROR, __func__, "Матрицы должны быть одинаковых измерений");
        return MAT_DIMENSION_ERR;
    }

    *result = matrix_allocate(A.rows, A.cols);
    if (result->data == NULL) {
        return MAT_MEMORY_ERR;
    }

    for (size_t idx = 0; idx < A.rows * A.cols; ++idx) {
        result->data[idx] = A.data[idx] - B.data[idx];
    }

    return MAT_OK;
}


MatrixStatus matrix_multiply(Matrix* result, const Matrix A, const Matrix B) 
{
    if (!(pointer_on_matrix_is_null(result))) {
        matrix_log(ERROR, __func__, "Указатель на матрицу result NULL\n");
        return MAT_UNINITIALIZED_ERR;
    }

    if (!matrix_equal_size(A, B)) {
        matrix_log(ERROR, __func__, "Измерения матриц не пригодны для умножения");
        return MAT_DIMENSION_ERR;
    }

    *result = matrix_allocate(A.rows, B.cols);
    if (result->data == NULL) {
        return MAT_MEMORY_ERR;
    }
    
    matrix_zero(result);
    for (size_t row = 0; row < A.rows; ++row) {
        for (size_t col = 0; col < B.cols; ++col) {
            for (size_t k = 0; k < A.cols; ++k) {
                result->data[row * result->cols + col] += matrix_get(A, row, k) * matrix_get(B, k, col);
            }
        }
    }
    return MAT_OK;
}


MatrixStatus matrix_scalar_multiply(Matrix* result, Matrix M, double scalar) 
{
    if (!(pointer_on_matrix_is_null(result))) {
        matrix_log(ERROR, __func__, "Указатель на матрицу result NULL\n");
        return MAT_UNINITIALIZED_ERR;
    }
    
    *result = matrix_allocate(M.rows, M.cols);
    if (result->data == NULL) {
        return MAT_MEMORY_ERR;
    }

    for (size_t idx = 0; idx < M.rows * M.cols; ++idx) {
        result->data[idx] = M.data[idx] * scalar;
    }

    return MAT_OK;
}


MatrixStatus matrix_transpose(Matrix* result, const Matrix M) 
{
    if (!(pointer_on_matrix_is_null(result))) {
        matrix_log(ERROR, __func__, "Указатель на матрицу result NULL\n");
        return MAT_UNINITIALIZED_ERR;
    }
    
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
        matrix_log(ERROR, __func__, "Определитель может быть вычислен только для квадратных матриц\n");
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

    if (M.rows == 3) {
        det = M.data[0] * M.data[4] * M.data[8] 
            + M.data[1] * M.data[5] * M.data[6] 
            + M.data[3] * M.data[7] * M.data[2] 
            - M.data[2] * M.data[4] * M.data[6] 
            - M.data[3] * M.data[1] * M.data[8] 
            - M.data[0] * M.data[5] * M.data[7];
        return det;
    }

    matrix_log(WARNING, __func__, "Определитель может быть вычислен только для третьего порядка и ниже!!!\n");

    return NAN;
}


MatrixStatus matrix_power(Matrix* result, const Matrix M, int power) 
{
    if (!(pointer_on_matrix_is_null(result))) {
        matrix_log(ERROR, __func__, "Указатель на матрицу result NULL\n");
        return MAT_UNINITIALIZED_ERR;
    }

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
        if (matrix_multiply(&temp, *result, M) == MAT_OK) {
            matrix_free(result);
            *result = temp;
        } else {
            matrix_free(result);
            return MAT_INTERNAL_ERR;
        }
    }

    return MAT_OK;
}


double factorial(unsigned int n) 
{
    if (n > 20) { 
        matrix_log(ERROR, __func__, "Факториал слишком велик для вычисления (n > 20)\n");
        return NAN; 
    }

    if (n == 0 || n == 1) {
        return 1; // Факториал 0 и 1 равен 1
    }

    double result = 1;

    for (unsigned int idx = 2; idx <= n; ++idx) {
        result *= idx;
    }

    return result;
}


MatrixStatus matrix_exponent(Matrix* result, const Matrix A, unsigned int num) 
{
    if (!(pointer_on_matrix_is_null(result))) {
        matrix_log(ERROR, __func__, "Указатель на матрицу result NULL\n");
        return MAT_UNINITIALIZED_ERR;
    }

    if (!matrix_is_square(A)) {
        matrix_log(WARNING, __func__, "Матрица должна быть квадратной для вычисления экспоненты");
        return MAT_DIMENSION_ERR;
    }

    *result = matrix_identity(A.rows);
    if (result->data == NULL) {
        return MAT_MEMORY_ERR; 
    }

    if (num == 0 || num == 1) {
        return MAT_OK; // Если степень равна 0 или 1, результат уже в result
    }

    for (size_t cur_num = 1; cur_num < num; ++cur_num) {
        Matrix tmp;

        MatrixStatus power_status = matrix_power(&tmp, A, cur_num);
        if (power_status != MAT_OK) {
            matrix_free(result);
            return MAT_INTERNAL_ERR;
        }

        Matrix scaled_tmp;

        MatrixStatus scale_status = matrix_scalar_multiply(&scaled_tmp, tmp, 1.0 / factorial(cur_num));
        matrix_free(&tmp); 
        if (scale_status != MAT_OK) {
            matrix_free(result);
            return MAT_INTERNAL_ERR; 
        }

        Matrix new_E;

        MatrixStatus sum_status = matrix_sum(&new_E, *result, scaled_tmp);
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
