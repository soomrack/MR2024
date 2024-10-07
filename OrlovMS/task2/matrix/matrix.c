#include "matrix.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


MStatus_t matrix_init(Matrix* m, const size_t rows, const size_t cols)
{
    if(m == NULL) return MAT_EMPTY_ERR;
    if(!matrix_is_empty(*m)) matrix_free(m);
    if(__SIZE_MAX__ / rows / cols / sizeof(double) == 0) return MAT_SIZE_ERR;

    m->data = (double*)malloc(rows * cols * sizeof(double));
    if(m->data == NULL) return MAT_ALLOC_ERR;

    m->rows = rows;
    m->cols = cols;

    return MAT_OK;
}


void matrix_free(Matrix* m)
{
    if(matrix_is_empty(*m)) return;

    free(m->data);
    m->data = NULL;
    m->rows = 0;
    m->cols = 0;
}


void matrix_set(Matrix m, const size_t row, const size_t col, const double number)
{
    if(row >= m.rows || col >= m.cols || m.data == NULL) return;

    m.data[row * m.cols + col] = number;
}


double matrix_get(const Matrix m, const size_t row, const size_t col)
{
    if(row >= m.rows || col >= m.cols || m.data == NULL) return NAN;

    return m.data[row * m.cols + col];
}


unsigned char matrix_is_empty(const Matrix m)
{
    return (m.rows == 0 || m.cols == 0 || m.data == NULL);
}


unsigned char matrix_is_square(const Matrix m)
{
    return (m.rows == m.cols);
}


unsigned char matrix_equal_size(const Matrix m1, const Matrix m2)
{
    return (m1.rows == m2.rows) && (m1.cols == m2.cols);
}


void matrix_print(const Matrix m)
{
    if(matrix_is_empty(m)) {
        printf("Matrix is empty\n");
    }
    else {
        printf("Matrix size: %ux%u\n", m.rows, m.cols);
        for(size_t i = 0; i < (m.rows * m.cols); i += m.cols) {
            printf("[ ");
            for(size_t j = 0; j < m.cols - 1; j++) {
                printf("%8.3f, ", m.data[i + j]);
            }
            printf("%8.3f ]\n", m.data[i +  m.cols - 1]);
        }
    }
}


MStatus_t matrix_zeros(Matrix m)
{
    if(matrix_is_empty(m)) return MAT_EMPTY_ERR;

    memset(m.data, 0, m.rows * m.cols * sizeof(double));

    return MAT_OK;
}


MStatus_t matrix_ones(Matrix m)
{
    if(!matrix_is_square(m)) return MAT_SIZE_ERR;

    if(matrix_zeros(m) == MAT_EMPTY_ERR) return MAT_EMPTY_ERR;
    for(size_t i = 0; i < m.rows * m.cols; i += m.cols + 1) {
        m.data[i] = 1.0;
    }

    return MAT_OK;
}


MStatus_t matrix_copy(const Matrix src, Matrix* dest)
{
    if(matrix_is_empty(src) || dest == NULL) return MAT_EMPTY_ERR;

    if(!matrix_equal_size(src, *dest)) {
        if(matrix_init(dest, src.rows, src.cols) != MAT_OK) return MAT_ALLOC_ERR;
    }

    memcpy(dest->data, src.data, src.rows * src.cols * sizeof(double));

    return MAT_OK;
}


MStatus_t matrix_sum(const Matrix m1, const Matrix m2, Matrix* result)
{
    if(matrix_is_empty(m1) || matrix_is_empty(m2) || result == NULL) return MAT_EMPTY_ERR;
    if(!matrix_equal_size(m1, m2)) return MAT_SIZE_ERR;

    if(!matrix_equal_size(m1, *result)) {
        if(matrix_init(result, m1.rows, m1.cols) != MAT_OK) return MAT_ALLOC_ERR;
    }

    for(size_t i = 0; i < m1.rows * m1.cols; i++) {
        result->data[i] = m1.data[i] + m2.data[i];
    }

    return MAT_OK;
}


MStatus_t matrix_sub(const Matrix m1, const Matrix m2, Matrix* result)
{
    if(matrix_is_empty(m1) || matrix_is_empty(m2) || result == NULL) return MAT_EMPTY_ERR;
    if(!matrix_equal_size(m1, m2)) return MAT_SIZE_ERR;

    if(!matrix_equal_size(m1, *result)) {
        if(matrix_init(result, m1.rows, m1.cols) != MAT_OK) return MAT_ALLOC_ERR;
    }

    for(size_t i = 0; i < m1.rows * m1.cols; i++) {
        result->data[i] = m1.data[i] - m2.data[i];
    }

    return MAT_OK;
}


MStatus_t matrix_mul_num(const Matrix m, const double number, Matrix* result)
{
    if(matrix_is_empty(m) || result == NULL) return MAT_EMPTY_ERR;

    if(!matrix_equal_size(m, *result)) {
        if(matrix_init(result, m.rows, m.cols) != MAT_OK) return MAT_ALLOC_ERR;
    }

    for(size_t i = 0; i < m.rows * m.cols; i++) {
        result->data[i] = m.data[i] * number;
    }

    return MAT_OK;
}


MStatus_t matrix_mul(const Matrix m1, const Matrix m2, Matrix* result)
{
    if(matrix_is_empty(m1) || matrix_is_empty(m2) || result == NULL) return MAT_EMPTY_ERR;
    if(m1.cols != m2.rows) return MAT_SIZE_ERR;

    unsigned char same_matrix = 0;
    Matrix* result_ptr;
    Matrix result_tmp = {0};
    if((m1.data == result->data) || (m2.data == result->data)) {
        same_matrix = 1;
        result_ptr = &result_tmp;
    }
    else {
        result_ptr = result;
    }

    if((result_ptr->rows != m1.rows) || (result_ptr->cols != m2.cols)) {
        if(matrix_init(result_ptr, m1.rows, m2.cols) != MAT_OK) return MAT_ALLOC_ERR;
    }

    for(size_t i = 0; i < m1.rows; i++) {
        for(size_t j = 0; j < m2.cols; j++) {
            double sum = 0.0;
            for(size_t k = 0; k < m1.cols; k++) {
                sum += m1.data[i * m1.cols + k] * m2.data[k * m2.cols + j];
            }
            result_ptr->data[i * result->cols + j] = sum;
        }
    }

    if(same_matrix) {
        if(matrix_copy(result_tmp, result) != MAT_OK) {
            matrix_free(&result_tmp);
            return MAT_ALLOC_ERR;
        }
        matrix_free(&result_tmp);
    }

    return MAT_OK;
}


MStatus_t matrix_pow(const Matrix m, const unsigned exp, Matrix* result)
{
    if(matrix_is_empty(m) || result == NULL) return MAT_EMPTY_ERR;
    if(!matrix_is_square(m)) return MAT_SIZE_ERR;

    unsigned char same_matrix = 0;
    Matrix* result_ptr;
    Matrix result_tmp = {0};
    if(m.data == result->data) {
        same_matrix = 1;
        result_ptr = &result_tmp;
    }
    else {
        result_ptr = result;
    }

    if(!matrix_equal_size(m, *result_ptr)) {
        if(matrix_init(result_ptr, m.rows, m.cols) != MAT_OK) return MAT_ALLOC_ERR;
    }
    matrix_ones(*result_ptr);

    Matrix tmp = {0};
    if(matrix_init(&tmp, m.rows, m.cols) != MAT_OK) {
        if(same_matrix) matrix_free(&result_tmp);
        return MAT_ALLOC_ERR;
    }

    for(unsigned i = 0; i < exp; i++) {
        matrix_mul(*result_ptr, m, &tmp);
        matrix_copy(tmp, result_ptr);
    }

    matrix_free(&tmp);

    if(same_matrix) {
        if(matrix_copy(result_tmp, result) != MAT_OK) {
            matrix_free(&result_tmp);
            return MAT_ALLOC_ERR;
        }
        matrix_free(&result_tmp);
    }

    return MAT_OK;
}


MStatus_t matrix_exp(const Matrix m, Matrix* result)
{
    if(matrix_is_empty(m) || result == NULL) return MAT_EMPTY_ERR;
    if(!matrix_is_square(m)) return MAT_SIZE_ERR;

    unsigned char same_matrix = 0;
    Matrix* result_ptr;
    Matrix result_tmp = {0};
    if(m.data == result->data) {
        same_matrix = 1;
        result_ptr = &result_tmp;
    }
    else {
        result_ptr = result;
    }

    if(!matrix_equal_size(m, *result_ptr)) {
        if(matrix_init(result_ptr, m.rows, m.cols) != MAT_OK) return MAT_ALLOC_ERR;
    }
    matrix_ones(*result_ptr);

    Matrix term = {0};
    if(matrix_init(&term, m.rows, m.cols) != MAT_OK) {
        if(same_matrix) matrix_free(&result_tmp);
        return MAT_ALLOC_ERR;
    }
    matrix_ones(term);

    Matrix tmp = {0};
    if(matrix_init(&tmp, m.rows, m.cols) != MAT_OK) {
        matrix_free(&term);
        if(same_matrix) matrix_free(&result_tmp);
        return MAT_ALLOC_ERR;
    }

    for(unsigned i = 1; i < 10; i++) {
        matrix_mul(term, m, &tmp);
        matrix_copy(tmp, &term);
        matrix_mul_num(term, 1 / (double)i, &term);

        matrix_sum(*result_ptr, term, result_ptr);
    }
    matrix_free(&tmp);
    matrix_free(&term);

    if(same_matrix) {
        if(matrix_copy(result_tmp, result) != MAT_OK) {
            matrix_free(&result_tmp);
            return MAT_ALLOC_ERR;
        }
        matrix_free(&result_tmp);
    }

    return MAT_OK;
}


MStatus_t matrix_transp(Matrix m)
{
    if(matrix_is_empty(m)) return MAT_EMPTY_ERR;
    if(!matrix_is_square(m)) return MAT_SIZE_ERR;

    for(size_t i = 1; i < m.rows; i++) {
        for(size_t j = 0; j < i; j++) {
            double tmp = m.data[i * m.cols + j];
            m.data[i * m.cols + j] = m.data[j * m.cols + i];
            m.data[j * m.cols + i] = tmp;
        }
    }

    return MAT_OK;
}


MStatus_t matrix_det(const Matrix m, double* det)
{
    if(matrix_is_empty(m) || det == NULL) return MAT_EMPTY_ERR;
    if(!matrix_is_square(m)) return MAT_SIZE_ERR;

    Matrix m_tmp = {0};
    if(matrix_copy(m, &m_tmp) != MAT_OK) return MAT_ALLOC_ERR;

    *det = 1.0;
    for(size_t i = 0; i < m_tmp.rows - 1; i++) {
        if(fabs(m_tmp.data[i * (m_tmp.cols + 1)]) < 0.00001) {
            size_t idx_non_zero = i;
            for(size_t j = i + 1; j < m_tmp.rows; j++) {
                if(fabs(m_tmp.data[j * m_tmp.cols + i]) >= 0.00001) {
                    idx_non_zero = j;
                    break;
                }
            }
            if(idx_non_zero != i) {
                for(size_t j = i; j < m_tmp.cols; j++) {
                    double tmp = m_tmp.data[i * m_tmp.cols + j];
                    m_tmp.data[i * m_tmp.cols + j] = m_tmp.data[idx_non_zero * m_tmp.cols + j];
                    m_tmp.data[idx_non_zero * m_tmp.cols + j] = tmp;
                }
                *det *= -1.0;
            }
            else {
                *det = 0.0;
                break;
            }
        }
        for(size_t j = i + 1; j < m_tmp.rows; j++) {
            double mult = m_tmp.data[j * m_tmp.cols + i] / m_tmp.data[i * (m_tmp.cols + 1)];
            m_tmp.data[j * m_tmp.cols + i] = 0.0;
            for(size_t k = i + 1; k < m_tmp.cols; k++) {
                m_tmp.data[j * m_tmp.cols + k] = m_tmp.data[j * m_tmp.cols + k] - mult * m_tmp.data[i * m_tmp.cols + k];
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
