#include "matrix.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>


LogLevel CURRENT_LEVEL = LOG_NONE;


void matrix_set_log_level(LogLevel level)
{
    CURRENT_LEVEL = level;
}


static void print_log(LogLevel level, const char *format, ...)
{
#ifdef MATRIX_LOG_ENABLE
    if(level <= CURRENT_LEVEL) {
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
#endif
}


const Matrix EMPTY = {.rows = 0, .cols = 0, .data = NULL};


MatrixStatus matrix_alloc(Matrix* M, const size_t rows, const size_t cols)
{
    print_log(LOG_INFO, "allocate matrix with size: %lux%lu\n", rows, cols);

    if(M == NULL) {
        print_log(LOG_ERR, "NULL pointer passed as argument\n");
        return MAT_EMPTY_ERR;
    }

    if(rows == 0 || cols == 0) {
        *M = EMPTY;
        print_log(LOG_WARN, "empty matrix allocation\n");
        return MAT_OK;
    }

    if(__SIZE_MAX__ / rows / cols / sizeof(double) == 0) {
        print_log(LOG_ERR, "matrix size too big\n");
        return MAT_SIZE_ERR;
    }

    M->data = (double*)malloc(rows * cols * sizeof(double));
    if(M->data == NULL) {
        *M = EMPTY;
        print_log(LOG_ERR, "matrix allocation error\n");
        return MAT_ALLOC_ERR;
    }

    M->rows = rows;
    M->cols = cols;

    return MAT_OK;
}


void matrix_free(Matrix* M)
{
    print_log(LOG_INFO, "free matrix\n");

    if(M == NULL) {
        print_log(LOG_ERR, "NULL pointer passed as argument\n");
        return;
    }

    free(M->data);
    *M = EMPTY;
}


void matrix_set(const Matrix M, const size_t row, const size_t col, const double number)
{
    if(row >= M.rows || col >= M.cols) {
        print_log(LOG_ERR, "index out of bound\n");
        return;
    }

    M.data[row * M.cols + col] = number;
}


double matrix_get(const Matrix M, const size_t row, const size_t col)
{
    if(row >= M.rows || col >= M.cols) {
        print_log(LOG_ERR, "index out of bound\n");
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
        return;
    }

    printf("Matrix size: %ux%u\n", M.rows, M.cols);
    for(size_t row = 0; row < (M.rows * M.cols); row += M.cols) {
        printf("[ ");
        for(size_t idx = 0; idx < M.cols - 1; idx++) {
            printf("%8.3f, ", M.data[row + idx]);
        }
        printf("%8.3f ]\n", M.data[row +  M.cols - 1]);
    }
}


MatrixStatus matrix_set_zeros(const Matrix M)
{
    print_log(LOG_INFO, "make zero matrix\n");

    if(matrix_is_empty(M)) {
        print_log(LOG_WARN, "matrix is empty\n");
        return MAT_OK;
    }

    memset(M.data, 0, M.rows * M.cols * sizeof(double));

    return MAT_OK;
}


MatrixStatus matrix_set_identity(const Matrix M)
{
    print_log(LOG_INFO, "make identity matrix\n");

    if(!matrix_is_square(M)) {
        print_log(LOG_ERR, "matrix not square\n");
        return MAT_SIZE_ERR;
    }

    matrix_set_zeros(M);
    for(size_t idx = 0; idx < M.rows * M.cols; idx += M.cols + 1) {
        M.data[idx] = 1.0;
    }

    return MAT_OK;
}


MatrixStatus matrix_copy(Matrix* dest, const Matrix src)
{
    print_log(LOG_INFO, "matrix copy\n");

    if(dest == NULL) {
        print_log(LOG_ERR, "NULL pointer passed as argument\n");
        return MAT_EMPTY_ERR;
    }
    if(!matrix_equal_size(src, *dest)) {
        print_log(LOG_ERR, "matrix size not equals\n");
        return MAT_SIZE_ERR;
    }
    if(matrix_is_empty(src)) {
        print_log(LOG_WARN, "matrix is empty\n");
        return MAT_OK;
    }

    memcpy(dest->data, src.data, src.rows * src.cols * sizeof(double));

    return MAT_OK;
}


MatrixStatus matrix_clone(Matrix* dest, const Matrix src)
{
    print_log(LOG_INFO, "matrix clone\n");

    if(dest == NULL) {
        print_log(LOG_ERR, "NULL pointer passed as argument\n");
        return MAT_EMPTY_ERR;
    }
    if(matrix_is_empty(src)) {
        matrix_free(dest);
        print_log(LOG_WARN, "matrix is empty\n");
        return MAT_OK;
    }
    if(!matrix_equal_size(src, *dest)) {
        print_log(LOG_INFO, "matrix size not equals, reallocation needed\n");
        matrix_free(dest);
        if(matrix_alloc(dest, src.rows, src.cols) != MAT_OK) return MAT_ALLOC_ERR;
    }

    memcpy(dest->data, src.data, src.rows * src.cols * sizeof(double));

    return MAT_OK;
}


MatrixStatus matrix_sum(Matrix* result, const Matrix A, const Matrix B)
{
    print_log(LOG_INFO, "matrix sum\n");

    if(result == NULL) {
        print_log(LOG_ERR, "NULL pointer passed as argument\n");
        return MAT_EMPTY_ERR;
    }
    if(!matrix_equal_size(A, B)) {
        print_log(LOG_ERR, "A, B matrix size not equals\n");
        return MAT_SIZE_ERR;
    }
    if(!matrix_equal_size(A, *result)) {
        print_log(LOG_ERR, "result matrix size not equals\n");
        return MAT_SIZE_ERR;
    }

    for(size_t idx = 0; idx < A.rows * A.cols; idx++) {
        result->data[idx] = A.data[idx] + B.data[idx];
    }

    return MAT_OK;
}


MatrixStatus matrix_sub(Matrix* result, const Matrix A, const Matrix B)
{
    print_log(LOG_INFO, "matrix subtraction\n");

    if(result == NULL) {
        print_log(LOG_ERR, "NULL pointer passed as argument\n");
        return MAT_EMPTY_ERR;
    }
    if(!matrix_equal_size(A, B)) {
        print_log(LOG_ERR, "A, B matrix size not equals\n");
        return MAT_SIZE_ERR;
    }
    if(!matrix_equal_size(A, *result)) {
        print_log(LOG_ERR, "result matrix size not equals\n");
        return MAT_SIZE_ERR;
    }

    for(size_t idx = 0; idx < A.rows * A.cols; idx++) {
        result->data[idx] = A.data[idx] - B.data[idx];
    }

    return MAT_OK;
}


MatrixStatus matrix_mul_num(Matrix* result, const Matrix M, const double number)
{
    print_log(LOG_INFO, "matrix multiplication by number\n");

    if(result == NULL) {
        print_log(LOG_ERR, "NULL pointer passed as argument\n");
        return MAT_EMPTY_ERR;
    }
    if(!matrix_equal_size(M, *result)) {
        print_log(LOG_ERR, "matrix size not equals\n");
        return MAT_SIZE_ERR;
    }

    for(size_t idx = 0; idx < M.rows * M.cols; idx++) {
        result->data[idx] = M.data[idx] * number;
    }

    return MAT_OK;
}


MatrixStatus matrix_mul(Matrix* result, const Matrix A, const Matrix B)
{
    print_log(LOG_INFO, "matrix multiplication\n");

    if(result == NULL) {
        print_log(LOG_ERR, "NULL pointer passed as argument\n");
        return MAT_EMPTY_ERR;
    }
    if(A.cols != B.rows) {
        print_log(LOG_ERR, "A.cols, B.rows not equals\n");
        return MAT_SIZE_ERR;
    }
    if((result->rows != A.rows) || (result->cols != B.cols)) {
        print_log(LOG_ERR, "result matrix size not equals\n");
        return MAT_SIZE_ERR;
    }

    char is_same_matrix = 0;
    Matrix* result_ptr = result;
    Matrix result_tmp = EMPTY;
    if((A.data == result->data) || (B.data == result->data)) {
        is_same_matrix = 1;
        print_log(LOG_INFO, "input and output is same matrix, allocation temporary result matrix\n");
        if(matrix_alloc(&result_tmp, result->rows, result->cols) != MAT_OK) return MAT_ALLOC_ERR;
        result_ptr = &result_tmp;
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
        print_log(LOG_INFO, "input and output is same matrix, copy data from temporary\n");
        memcpy(result->data, result_tmp.data, result_tmp.rows * result_tmp.cols * sizeof(double));
        matrix_free(&result_tmp);
    }

    return MAT_OK;
}


MatrixStatus matrix_pow(Matrix* result, const Matrix M, const unsigned int pow)
{
    print_log(LOG_INFO, "matrix power\n");

    if(result == NULL) {
        print_log(LOG_ERR, "NULL pointer passed as argument\n");
        return MAT_EMPTY_ERR;
    }
    if(!matrix_is_square(M)) {
        print_log(LOG_ERR, "matrix not square\n");
        return MAT_SIZE_ERR;
    }
    if(!matrix_equal_size(M, *result)) {
        print_log(LOG_ERR, "matrix size not equals\n");
        return MAT_SIZE_ERR;
    }

    char is_same_matrix = 0;
    Matrix* result_ptr = result;
    Matrix result_tmp = EMPTY;
    if(M.data == result->data) {
        is_same_matrix = 1;
        print_log(LOG_INFO, "input and output is same matrix, allocation temporary result matrix\n");
        if(matrix_alloc(&result_tmp, result->rows, result->cols) != MAT_OK) return MAT_ALLOC_ERR;
        result_ptr = &result_tmp;
    }

    matrix_set_identity(*result_ptr);

    print_log(LOG_INFO, "allocation temporary matrix for internal usage\n");
    Matrix tmp = EMPTY;
    if(matrix_alloc(&tmp, M.rows, M.cols) != MAT_OK) {
        if(is_same_matrix) matrix_free(&result_tmp);
        return MAT_ALLOC_ERR;
    }

    for(unsigned int cnt = 0; cnt < pow; cnt++) {
        matrix_mul(&tmp, *result_ptr, M);
        matrix_copy(result_ptr, tmp);
    }

    matrix_free(&tmp);

    if(is_same_matrix) {
        print_log(LOG_INFO, "input and output is same matrix, copy data from temporary\n");
        memcpy(result->data, result_tmp.data, result_tmp.rows * result_tmp.cols * sizeof(double));
        matrix_free(&result_tmp);
    }

    return MAT_OK;
}


MatrixStatus matrix_exp(Matrix* result, const Matrix M)
{
    print_log(LOG_INFO, "matrix exponent\n");

    if(result == NULL) {
        print_log(LOG_ERR, "NULL pointer passed as argument\n");
        return MAT_EMPTY_ERR;
    }
    if(!matrix_is_square(M)) {
        print_log(LOG_ERR, "matrix not square\n");
        return MAT_SIZE_ERR;
    }
    if(!matrix_equal_size(M, *result)) {
        print_log(LOG_ERR, "matrix size not equals\n");
        return MAT_SIZE_ERR;
    }

    char is_same_matrix = 0;
    Matrix* result_ptr = result;
    Matrix result_tmp = EMPTY;
    if(M.data == result->data) {
        is_same_matrix = 1;
        print_log(LOG_INFO, "input and output is same matrix, allocation temporary result matrix\n");
        if(matrix_alloc(&result_tmp, result->rows, result->cols) != MAT_OK) return MAT_ALLOC_ERR;
        result_ptr = &result_tmp;
    }

    matrix_set_identity(*result_ptr);

    print_log(LOG_INFO, "allocation temporary matrix for internal usage\n");
    Matrix term = EMPTY;
    if(matrix_alloc(&term, M.rows, M.cols) != MAT_OK) {
        if(is_same_matrix) matrix_free(&result_tmp);
        return MAT_ALLOC_ERR;
    }
    matrix_set_identity(term);

    print_log(LOG_INFO, "allocation temporary matrix for internal usage\n");
    Matrix tmp = EMPTY;
    if(matrix_alloc(&tmp, M.rows, M.cols) != MAT_OK) {
        matrix_free(&term);
        if(is_same_matrix) matrix_free(&result_tmp);
        return MAT_ALLOC_ERR;
    }

    for(unsigned num = 1; num < 10; num++) {
        matrix_mul(&tmp, term, M);
        matrix_copy(&term, tmp);
        matrix_mul_num(&term, term, 1 / (double)num);

        matrix_sum(result_ptr, *result_ptr, term);
    }

    matrix_free(&tmp);
    matrix_free(&term);

    if(is_same_matrix) {
        print_log(LOG_INFO, "input and output is same matrix, copy data from temporary\n");
        memcpy(result->data, result_tmp.data, result_tmp.rows * result_tmp.cols * sizeof(double));
        matrix_free(&result_tmp);
    }

    return MAT_OK;
}


static void swap_double(double* first, double* second)
{
    double tmp = *first;
    *first = *second;
    *second = tmp;
}


MatrixStatus matrix_transp(const Matrix M)
{
    print_log(LOG_INFO, "matrix transposition\n");

    if(!matrix_is_square(M)) {
        print_log(LOG_ERR, "matrix not square\n");
        return MAT_SIZE_ERR;
    }

    for(size_t row = 1; row < M.rows; row++) {
        for(size_t idx = 0; idx < row; idx++) {
            swap_double(M.data + row * M.cols + idx, M.data + idx * M.cols + row);
        }
    }

    return MAT_OK;
}


static size_t matrix_find_non_zero_in_col(const Matrix M, const size_t row_start)
{
    for(size_t row = row_start + 1; row < M.rows; row++) {
        if(fabs(M.data[row * M.cols + row_start]) >= 0.00001) {
            return row;
        }
    }
    return row_start;
}


static void matrix_swap_rows(const Matrix M, const size_t row_A, const size_t row_B)
{
    for(size_t idx = 0; idx < M.cols; idx++) {
        swap_double(M.data + row_A * M.cols + idx, M.data + row_B * M.cols + idx);
    }
}


static void matrix_sub_row(const Matrix M, const size_t row, const size_t row_base, const double ratio)
{
    for(size_t idx = 0; idx < M.cols; idx++) {
        M.data[row * M.cols + idx] -= ratio * M.data[row_base * M.cols + idx];
    }
}


MatrixStatus matrix_det(double* det, const Matrix M)
{
    print_log(LOG_INFO, "matrix determinant\n");

    if(det == NULL) {
        print_log(LOG_ERR, "NULL pointer passed as argument\n");
        return MAT_EMPTY_ERR;
    }
    if(matrix_is_empty(M)) {
        *det = 0.0;
        print_log(LOG_WARN, "empty matrix\n");
        return MAT_OK;
    }
    if(!matrix_is_square(M)) {
        print_log(LOG_ERR, "matrix not square\n");
        return MAT_SIZE_ERR;
    }

    print_log(LOG_INFO, "clone temporary matrix for internal usage\n");
    Matrix T = EMPTY;
    if(matrix_clone(&T, M) != MAT_OK) return MAT_ALLOC_ERR;

    // Gauss method
    *det = 1.0;
    for(size_t row_base = 0; row_base < T.rows - 1; row_base++) {
        if(fabs(T.data[row_base * (T.cols + 1)]) < 0.00001) {
            size_t idx_non_zero = matrix_find_non_zero_in_col(T, row_base);
            if(idx_non_zero == row_base) {
                *det = 0.0;
                matrix_free(&T);
                return MAT_OK;
            }

            matrix_swap_rows(T, row_base, idx_non_zero);
            *det *= -1.0;
        }
        for(size_t row = row_base + 1; row < T.rows; row++) {
            double mult = T.data[row * T.cols + row_base] / T.data[row_base * (T.cols + 1)];
            matrix_sub_row(T, row, row_base, mult);
        }
    }

    for(size_t idx = 0; idx < T.rows; idx++) {
        *det *= T.data[idx * T.cols + idx];
    }

    matrix_free(&T);
    return MAT_OK;
}
