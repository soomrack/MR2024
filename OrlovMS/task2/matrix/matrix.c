#include "matrix.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>


LogLevel_t current_level = LOG_NONE;


void matrix_set_log_level(LogLevel_t level)
{
    current_level = level;
}


#ifdef MATRIX_LOG_ENABLE
static void print_log(LogLevel_t level, const char *format, ...)
{
    if(level <= current_level) {
        switch (level)
        {
        case LOG_ERR:
            printf("\033[0;31mERROR: \033[0m");
            break;
        case LOG_WARN:
            printf("\033[1;33mWARNING: \033[0m");
            break;
        case LOG_INFO:
            printf("INFO: ");
        default:
            break;
        }
        va_list list;
        va_start(list, format);
        vprintf(format, list);
        va_end(list);
    }
}


#define PRINT_LOG(level, ...) print_log(level, __VA_ARGS__)
#else
#define PRINT_LOG(level, ...) ((void)0)
#endif


MatrixStatus_t matrix_alloc(Matrix* M, const size_t rows, const size_t cols)
{
    PRINT_LOG(LOG_INFO, "allocate matrix with size: %lux%lu\n", rows, cols);

    if(M == NULL) {
        PRINT_LOG(LOG_ERR, "NULL pointer passed as argument\n");
        return MAT_EMPTY_ERR;
    }

    if(rows == 0 || cols == 0) {
        M->rows = 0;
        M->cols = 0;
        M->data = NULL;
        PRINT_LOG(LOG_WARN, "empty matrix allocation\n");
        return MAT_OK;
    }

    if(__SIZE_MAX__ / rows / cols / sizeof(double) == 0) {
        PRINT_LOG(LOG_ERR, "matrix size too big\n");
        return MAT_SIZE_ERR;
    }
    M->data = (double*)malloc(rows * cols * sizeof(double));
    if(M->data == NULL) {
        M->rows = 0;
        M->cols = 0;
        PRINT_LOG(LOG_ERR, "matrix allocation error\n");
        return MAT_ALLOC_ERR;
    }

    M->rows = rows;
    M->cols = cols;

    return MAT_OK;
}


void matrix_free(Matrix* M)
{
    PRINT_LOG(LOG_INFO, "free matrix\n");

    free(M->data);
    M->data = NULL;
    M->rows = 0;
    M->cols = 0;
}


void matrix_set(Matrix M, const size_t row, const size_t col, const double number)
{
    if(row >= M.rows || col >= M.cols) {
        PRINT_LOG(LOG_ERR, "index out of bound\n");
        return;
    }

    M.data[row * M.cols + col] = number;
}


double matrix_get(const Matrix M, const size_t row, const size_t col)
{
    if(row >= M.rows || col >= M.cols) {
        PRINT_LOG(LOG_ERR, "index out of bound\n");
        return NAN;
    }

    return M.data[row * M.cols + col];
}


unsigned char matrix_is_empty(const Matrix M)
{
    return (M.data == NULL);
}


unsigned char matrix_is_square(const Matrix M)
{
    return (M.rows == M.cols);
}


unsigned char matrix_equal_size(const Matrix A, const Matrix B)
{
    return (A.rows == B.rows) && (A.cols == B.cols);
}


void matrix_print(const Matrix M)
{
    if(matrix_is_empty(M)) {
        printf("Matrix is empty\n");
    }
    else {
        printf("Matrix size: %ux%u\n", M.rows, M.cols);
        for(size_t row = 0; row < (M.rows * M.cols); row += M.cols) {
            printf("[ ");
            for(size_t idx = 0; idx < M.cols - 1; idx++) {
                printf("%8.3f, ", M.data[row + idx]);
            }
            printf("%8.3f ]\n", M.data[row +  M.cols - 1]);
        }
    }
}


MatrixStatus_t matrix_zeros(Matrix M)
{
    PRINT_LOG(LOG_INFO, "make zero matrix\n");

    if(matrix_is_empty(M)) {
        PRINT_LOG(LOG_WARN, "matrix is empty\n");
        return MAT_OK;
    }

    memset(M.data, 0, M.rows * M.cols * sizeof(double));

    return MAT_OK;
}


MatrixStatus_t matrix_identity(Matrix M)
{
    PRINT_LOG(LOG_INFO, "make identity matrix\n");

    if(!matrix_is_square(M)) {
        PRINT_LOG(LOG_ERR, "matrix not square\n");
        return MAT_SIZE_ERR;
    }

    matrix_zeros(M);
    for(size_t idx = 0; idx < M.rows * M.cols; idx += M.cols + 1) {
        M.data[idx] = 1.0;
    }

    return MAT_OK;
}


MatrixStatus_t matrix_copy(Matrix* dest, const Matrix src)
{
    PRINT_LOG(LOG_INFO, "matrix copy\n");

    if(dest == NULL) {
        PRINT_LOG(LOG_ERR, "NULL pointer passed as argument\n");
        return MAT_EMPTY_ERR;
    }
    if(!matrix_equal_size(src, *dest)) {
        PRINT_LOG(LOG_ERR, "matrix size not equals\n");
        return MAT_SIZE_ERR;
    }
    if(matrix_is_empty(src)) {
        PRINT_LOG(LOG_WARN, "matrix is empty\n");
        return MAT_OK;
    }

    memcpy(dest->data, src.data, src.rows * src.cols * sizeof(double));

    return MAT_OK;
}


MatrixStatus_t matrix_clone(Matrix* dest, const Matrix src)
{
    PRINT_LOG(LOG_INFO, "matrix clone\n");

    if(dest == NULL) {
        PRINT_LOG(LOG_ERR, "NULL pointer passed as argument\n");
        return MAT_EMPTY_ERR;
    }
    if(!matrix_equal_size(src, *dest)) {
        PRINT_LOG(LOG_INFO, "matrix size not equals, reallocation needed\n");
        matrix_free(dest);
        if(matrix_alloc(dest, src.rows, src.cols) != MAT_OK) return MAT_ALLOC_ERR;
    }
    if(matrix_is_empty(src)) {
        PRINT_LOG(LOG_WARN, "matrix is empty\n");
        return MAT_OK;
    }

    memcpy(dest->data, src.data, src.rows * src.cols * sizeof(double));

    return MAT_OK;
}


MatrixStatus_t matrix_sum(Matrix* result, const Matrix A, const Matrix B)
{
    PRINT_LOG(LOG_INFO, "matrix sum\n");

    if(result == NULL) {
        PRINT_LOG(LOG_ERR, "NULL pointer passed as argument\n");
        return MAT_EMPTY_ERR;
    }
    if(!matrix_equal_size(A, B)) {
        PRINT_LOG(LOG_ERR, "A, B matrix size not equals\n");
        return MAT_SIZE_ERR;
    }
    if(!matrix_equal_size(A, *result)) {
        PRINT_LOG(LOG_ERR, "result matrix size not equals\n");
        return MAT_SIZE_ERR;
    }

    for(size_t idx = 0; idx < A.rows * A.cols; idx++) {
        result->data[idx] = A.data[idx] + B.data[idx];
    }

    return MAT_OK;
}


MatrixStatus_t matrix_sub(Matrix* result, const Matrix A, const Matrix B)
{
    PRINT_LOG(LOG_INFO, "matrix subtraction\n");

    if(result == NULL) {
        PRINT_LOG(LOG_ERR, "NULL pointer passed as argument\n");
        return MAT_EMPTY_ERR;
    }
    if(!matrix_equal_size(A, B)) {
        PRINT_LOG(LOG_ERR, "A, B matrix size not equals\n");
        return MAT_SIZE_ERR;
    }
    if(!matrix_equal_size(A, *result)) {
        PRINT_LOG(LOG_ERR, "result matrix size not equals\n");
        return MAT_SIZE_ERR;
    }

    for(size_t idx = 0; idx < A.rows * A.cols; idx++) {
        result->data[idx] = A.data[idx] - B.data[idx];
    }

    return MAT_OK;
}


MatrixStatus_t matrix_mul_num(Matrix* result, const Matrix M, const double number)
{
    PRINT_LOG(LOG_INFO, "matrix multiplication by number\n");

    if(result == NULL) {
        PRINT_LOG(LOG_ERR, "NULL pointer passed as argument\n");
        return MAT_EMPTY_ERR;
    }
    if(!matrix_equal_size(M, *result)) {
        PRINT_LOG(LOG_ERR, "matrix size not equals\n");
        return MAT_SIZE_ERR;
    }

    for(size_t idx = 0; idx < M.rows * M.cols; idx++) {
        result->data[idx] = M.data[idx] * number;
    }

    return MAT_OK;
}


MatrixStatus_t matrix_mul(Matrix* result, const Matrix A, const Matrix B)
{
    PRINT_LOG(LOG_INFO, "matrix multiplication\n");

    if(result == NULL) {
        PRINT_LOG(LOG_ERR, "NULL pointer passed as argument\n");
        return MAT_EMPTY_ERR;
    }
    if(A.cols != B.rows) {
        PRINT_LOG(LOG_ERR, "A.cols, B.rows not equals\n");
        return MAT_SIZE_ERR;
    }
    if((result->rows != A.rows) || (result->cols != B.cols)) {
        PRINT_LOG(LOG_ERR, "result matrix size not equals\n");
        return MAT_SIZE_ERR;
    }

    unsigned char is_same_matrix = 0;
    Matrix* result_ptr;
    Matrix result_tmp = {0};
    if((A.data == result->data) || (B.data == result->data)) {
        is_same_matrix = 1;
        PRINT_LOG(LOG_INFO, "input and output is same matrix, allocation temporary result matrix\n");
        if(matrix_alloc(&result_tmp, result->rows, result->cols) != MAT_OK) return MAT_ALLOC_ERR;
        result_ptr = &result_tmp;
    }
    else {
        result_ptr = result;
    }

    for(size_t row = 0; row < A.rows; row++) {
        for(size_t col = 0; col < B.cols; col++) {
            result_ptr->data[row * result->cols + col] = 0.0;
            for(size_t idx = 0; idx < A.cols; idx++) {
                result_ptr->data[row * result->cols + col] += A.data[row * A.cols + idx] * B.data[idx * B.cols + col];
            }
        }
    }

    if(is_same_matrix) {
        PRINT_LOG(LOG_INFO, "input and output is same matrix, free old data\n");
        free(result->data);
        result->data = result_tmp.data;
    }

    return MAT_OK;
}


MatrixStatus_t matrix_pow(Matrix* result, const Matrix M, const unsigned int exp)
{
    PRINT_LOG(LOG_INFO, "matrix power\n");

    if(result == NULL) {
        PRINT_LOG(LOG_ERR, "NULL pointer passed as argument\n");
        return MAT_EMPTY_ERR;
    }
    if(!matrix_is_square(M)) {
        PRINT_LOG(LOG_ERR, "matrix not square\n");
        return MAT_SIZE_ERR;
    }
    if(!matrix_equal_size(M, *result)) {
        PRINT_LOG(LOG_ERR, "matrix size not equals\n");
        return MAT_SIZE_ERR;
    }

    unsigned char is_same_matrix = 0;
    Matrix* result_ptr;
    Matrix result_tmp = {0};
    if(M.data == result->data) {
        is_same_matrix = 1;
        PRINT_LOG(LOG_INFO, "input and output is same matrix, allocation temporary result matrix\n");
        if(matrix_alloc(&result_tmp, result->rows, result->cols) != MAT_OK) return MAT_ALLOC_ERR;
        result_ptr = &result_tmp;
    }
    else {
        result_ptr = result;
    }

    matrix_identity(*result_ptr);

    PRINT_LOG(LOG_INFO, "allocation temporary matrix for internal usage\n");
    Matrix tmp = {0};
    if(matrix_alloc(&tmp, M.rows, M.cols) != MAT_OK) {
        if(is_same_matrix) matrix_free(&result_tmp);
        return MAT_ALLOC_ERR;
    }

    for(unsigned int i = 0; i < exp; i++) {
        matrix_mul(&tmp, *result_ptr, M);
        matrix_copy(result_ptr, tmp);
    }

    matrix_free(&tmp);

    if(is_same_matrix) {
        PRINT_LOG(LOG_INFO, "input and output is same matrix, free old data\n");
        free(result->data);
        result->data = result_tmp.data;
    }

    return MAT_OK;
}


MatrixStatus_t matrix_exp(Matrix* result, const Matrix M)
{
    PRINT_LOG(LOG_INFO, "matrix exponent\n");

    if(result == NULL) {
        PRINT_LOG(LOG_ERR, "NULL pointer passed as argument\n");
        return MAT_EMPTY_ERR;
    }
    if(!matrix_is_square(M)) {
        PRINT_LOG(LOG_ERR, "matrix not square\n");
        return MAT_SIZE_ERR;
    }
    if(!matrix_equal_size(M, *result)) {
        PRINT_LOG(LOG_ERR, "matrix size not equals\n");
        return MAT_SIZE_ERR;
    }

    unsigned char is_same_matrix = 0;
    Matrix* result_ptr;
    Matrix result_tmp = {0};
    if(M.data == result->data) {
        is_same_matrix = 1;
        PRINT_LOG(LOG_INFO, "input and output is same matrix, allocation temporary result matrix\n");
        if(matrix_alloc(&result_tmp, result->rows, result->cols) != MAT_OK) return MAT_ALLOC_ERR;
        result_ptr = &result_tmp;
    }
    else {
        result_ptr = result;
    }

    matrix_identity(*result_ptr);

    PRINT_LOG(LOG_INFO, "allocation temporary matrix for internal usage\n");
    Matrix term = {0};
    if(matrix_alloc(&term, M.rows, M.cols) != MAT_OK) {
        if(is_same_matrix) matrix_free(&result_tmp);
        return MAT_ALLOC_ERR;
    }
    matrix_identity(term);

    PRINT_LOG(LOG_INFO, "allocation temporary matrix for internal usage\n");
    Matrix tmp = {0};
    if(matrix_alloc(&tmp, M.rows, M.cols) != MAT_OK) {
        matrix_free(&term);
        if(is_same_matrix) matrix_free(&result_tmp);
        return MAT_ALLOC_ERR;
    }

    for(unsigned i = 1; i < 10; i++) {
        matrix_mul(&tmp, term, M);
        matrix_copy(&term, tmp);
        matrix_mul_num(&term, term, 1 / (double)i);

        matrix_sum(result_ptr, *result_ptr, term);
    }

    matrix_free(&tmp);
    matrix_free(&term);

    if(is_same_matrix) {
        PRINT_LOG(LOG_INFO, "input and output is same matrix, free old data\n");
        free(result->data);
        result->data = result_tmp.data;
    }

    return MAT_OK;
}


MatrixStatus_t matrix_transp(Matrix M)
{
    PRINT_LOG(LOG_INFO, "matrix transposition\n");

    if(!matrix_is_square(M)) {
        PRINT_LOG(LOG_ERR, "matrix not square\n");
        return MAT_SIZE_ERR;
    }

    for(size_t row = 1; row < M.rows; row++) {
        for(size_t idx = 0; idx < row; idx++) {
            double tmp = M.data[row * M.cols + idx];
            M.data[row * M.cols + idx] = M.data[idx * M.cols + row];
            M.data[idx * M.cols + row] = tmp;
        }
    }

    return MAT_OK;
}


MatrixStatus_t matrix_det(double* det, const Matrix M)
{
    PRINT_LOG(LOG_INFO, "matrix determinant\n");

    if(det == NULL) {
        PRINT_LOG(LOG_ERR, "NULL pointer passed as argument\n");
        return MAT_EMPTY_ERR;
    }
    if(matrix_is_empty(M)) {
        *det = 0.0;
        PRINT_LOG(LOG_WARN, "empty matrix\n");
        return MAT_OK;
    }
    if(!matrix_is_square(M)) {
        PRINT_LOG(LOG_ERR, "matrix not square\n");
        return MAT_SIZE_ERR;
    }

    PRINT_LOG(LOG_INFO, "clone temporary matrix for internal usage\n");
    Matrix m_tmp = {0};
    if(matrix_clone(&m_tmp, M) != MAT_OK) return MAT_ALLOC_ERR;

    // Gauss method
    *det = 1.0;
    for(size_t row_base = 0; row_base < m_tmp.rows - 1; row_base++) {
        if(fabs(m_tmp.data[row_base * (m_tmp.cols + 1)]) < 0.00001) {
            size_t idx_non_zero = row_base;
            for(size_t row = row_base + 1; row < m_tmp.rows; row++) {
                if(fabs(m_tmp.data[row * m_tmp.cols + row_base]) >= 0.00001) {
                    idx_non_zero = row;
                    break;
                }
            }
            if(idx_non_zero != row_base) {
                for(size_t idx = row_base; idx < m_tmp.cols; idx++) {
                    double tmp = m_tmp.data[row_base * m_tmp.cols + idx];
                    m_tmp.data[row_base * m_tmp.cols + idx] = m_tmp.data[idx_non_zero * m_tmp.cols + idx];
                    m_tmp.data[idx_non_zero * m_tmp.cols + idx] = tmp;
                }
                *det *= -1.0;
            }
            else {
                *det = 0.0;
                break;
            }
        }
        for(size_t row = row_base + 1; row < m_tmp.rows; row++) {
            double mult = m_tmp.data[row * m_tmp.cols + row_base] / m_tmp.data[row_base * (m_tmp.cols + 1)];
            m_tmp.data[row * m_tmp.cols + row_base] = 0.0;
            for(size_t idx = row_base + 1; idx < m_tmp.cols; idx++) {
                m_tmp.data[row * m_tmp.cols + idx] = m_tmp.data[row * m_tmp.cols + idx] - mult * m_tmp.data[row_base * m_tmp.cols + idx];
            }
        }
    }

    if(*det != 0.0) {
        for(size_t i = 0; i < m_tmp.rows; i++) {
            *det *= m_tmp.data[i * m_tmp.cols + i];
        }
    }

    matrix_free(&m_tmp);
    return MAT_OK;
}
