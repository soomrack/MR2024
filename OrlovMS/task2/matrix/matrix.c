#include "matrix.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


MatrixStatus_t matrix_alloc(Matrix* M, const size_t rows, const size_t cols)
{
    if(M == NULL) return MAT_EMPTY_ERR;
    if(__SIZE_MAX__ / rows / cols / sizeof(double) == 0) return MAT_SIZE_ERR;

    M->data = (double*)malloc(rows * cols * sizeof(double));
    if(M->data == NULL) return MAT_ALLOC_ERR;

    M->rows = rows;
    M->cols = cols;

    return MAT_OK;
}


void matrix_free(Matrix* M)
{
    free(M->data);
    M->data = NULL;
    M->rows = 0;
    M->cols = 0;
}


void matrix_set(Matrix M, const size_t row, const size_t col, const double number)
{
    if(row >= M.rows || col >= M.cols || M.data == NULL) return;

    M.data[row * M.cols + col] = number;
}


double matrix_get(const Matrix M, const size_t row, const size_t col)
{
    if(row >= M.rows || col >= M.cols || M.data == NULL) return NAN;

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
    if(matrix_is_empty(M)) return MAT_EMPTY_ERR;

    memset(M.data, 0, M.rows * M.cols * sizeof(double));

    return MAT_OK;
}


MatrixStatus_t matrix_ones(Matrix M)
{
    if(!matrix_is_square(M)) return MAT_SIZE_ERR;

    if(matrix_zeros(M) == MAT_EMPTY_ERR) return MAT_EMPTY_ERR;
    for(size_t idx = 0; idx < M.rows * M.cols; idx += M.cols + 1) {
        M.data[idx] = 1.0;
    }

    return MAT_OK;
}


MatrixStatus_t matrix_copy(Matrix* dest, const Matrix src)
{
    if(dest == NULL) return MAT_EMPTY_ERR;
    if(!matrix_equal_size(src, *dest)) return MAT_SIZE_ERR;

    memcpy(dest->data, src.data, src.rows * src.cols * sizeof(double));

    return MAT_OK;
}


MatrixStatus_t matrix_clone(Matrix* dest, const Matrix src)
{
    if(dest == NULL) return MAT_EMPTY_ERR;
    if(!matrix_equal_size(src, *dest)) {
        matrix_free(dest);
        matrix_alloc(dest, src.rows, src.cols);
    }

    memcpy(dest->data, src.data, src.rows * src.cols * sizeof(double));

    return MAT_OK;
}


MatrixStatus_t matrix_sum(Matrix* result, const Matrix A, const Matrix B)
{
    if(result == NULL) return MAT_EMPTY_ERR;
    if(!(matrix_equal_size(A, B) && matrix_equal_size(A, *result))) return MAT_SIZE_ERR;

    for(size_t idx = 0; idx < A.rows * A.cols; idx++) {
        result->data[idx] = A.data[idx] + B.data[idx];
    }

    return MAT_OK;
}


MatrixStatus_t matrix_sub(Matrix* result, const Matrix A, const Matrix B)
{
    if(result == NULL) return MAT_EMPTY_ERR;
    if(!(matrix_equal_size(A, B) && matrix_equal_size(A, *result))) return MAT_SIZE_ERR;

    for(size_t idx = 0; idx < A.rows * A.cols; idx++) {
        result->data[idx] = A.data[idx] - B.data[idx];
    }

    return MAT_OK;
}


MatrixStatus_t matrix_mul_num(Matrix* result, const Matrix M, const double number)
{
    if(result == NULL) return MAT_EMPTY_ERR;
    if(!matrix_equal_size(M, *result)) return MAT_SIZE_ERR;

    for(size_t idx = 0; idx < M.rows * M.cols; idx++) {
        result->data[idx] = M.data[idx] * number;
    }

    return MAT_OK;
}


MatrixStatus_t matrix_mul(Matrix* result, const Matrix A, const Matrix B)
{
    if(result == NULL) return MAT_EMPTY_ERR;
    if(A.cols != B.rows) return MAT_SIZE_ERR;
    if((result->rows != A.rows) || (result->cols != B.cols)) return MAT_SIZE_ERR;

    unsigned char is_same_matrix = 0;
    double* data_ptr;
    if((A.data == result->data) || (B.data == result->data)) {
        is_same_matrix = 1;
        data_ptr = (double*)malloc(result->rows * result->cols * sizeof(double));
        if(data_ptr == NULL) return MAT_ALLOC_ERR;
    }
    else {
        data_ptr = result->data;
    }

    for(size_t row = 0; row < A.rows; row++) {
        for(size_t col = 0; col < B.cols; col++) {
            data_ptr[row * result->cols + col] = 0.0;
            for(size_t idx = 0; idx < A.cols; idx++) {
                data_ptr[row * result->cols + col] += A.data[row * A.cols + idx] * B.data[idx * B.cols + col];
            }
        }
    }

    if(is_same_matrix) {
        free(result->data);
        result->data = data_ptr;
    }

    return MAT_OK;
}


MatrixStatus_t matrix_pow(Matrix* result, const Matrix M, const unsigned int exp)
{
    if(result == NULL) return MAT_EMPTY_ERR;
    if(!matrix_is_square(M)) return MAT_SIZE_ERR;
    if(!matrix_equal_size(M, *result)) return MAT_SIZE_ERR;

    unsigned char is_same_matrix = 0;
    Matrix* result_ptr;
    Matrix result_tmp = {0};
    if(M.data == result->data) {
        is_same_matrix = 1;
        if(matrix_alloc(&result_tmp, result->rows, result->cols) != MAT_OK) return MAT_ALLOC_ERR;
        result_ptr = &result_tmp;
    }
    else {
        result_ptr = result;
    }

    matrix_ones(*result_ptr);

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
        free(result->data);
        result->data = result_tmp.data;
    }

    return MAT_OK;
}


MatrixStatus_t matrix_exp(Matrix* result, const Matrix M)
{
    if(result == NULL) return MAT_EMPTY_ERR;
    if(!matrix_is_square(M)) return MAT_SIZE_ERR;
    if(!matrix_equal_size(M, *result)) return MAT_SIZE_ERR;

    unsigned char is_same_matrix = 0;
    Matrix* result_ptr;
    Matrix result_tmp = {0};
    if(M.data == result->data) {
        is_same_matrix = 1;
        if(matrix_alloc(&result_tmp, result->rows, result->cols) != MAT_OK) return MAT_ALLOC_ERR;
        result_ptr = &result_tmp;
    }
    else {
        result_ptr = result;
    }

    matrix_ones(*result_ptr);

    Matrix term = {0};
    if(matrix_alloc(&term, M.rows, M.cols) != MAT_OK) {
        if(is_same_matrix) matrix_free(&result_tmp);
        return MAT_ALLOC_ERR;
    }
    matrix_ones(term);

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
        free(result->data);
        result->data = result_tmp.data;
    }

    return MAT_OK;
}


MatrixStatus_t matrix_transp(Matrix M)
{
    if(!matrix_is_square(M)) return MAT_SIZE_ERR;

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
    if(det == NULL) return MAT_EMPTY_ERR;
    if(matrix_is_empty(M)) {
        *det = 0.0;
        return MAT_OK;
    }
    if(!matrix_is_square(M)) return MAT_SIZE_ERR;

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
