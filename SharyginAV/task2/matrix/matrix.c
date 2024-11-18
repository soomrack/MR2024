#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>


const Matrix EMPTY = {.cols = 0, .rows = 0, .data = NULL};


Matrix_status matrix_alloc(Matrix* M_ptr, const size_t rows, const size_t cols)
{
    if(M_ptr == NULL) return MAT_EMPTY_ERR;

    if(rows != 0 && cols != 0) {
        if(__SIZE_MAX__ / rows / cols / sizeof(double) == 0) return MAT_SIZE_ERR;
        M_ptr -> data = (double*)malloc(rows * cols * sizeof(double));
        if(M_ptr -> data == NULL) return MAT_ALLOC_ERR;
    }
    
    M_ptr -> rows = rows;
    M_ptr -> cols = cols;

    return MAT_OK;
}


Matrix_status matrix_free(Matrix* M_ptr)
{
    free(M_ptr -> data);
    return MAT_OK;
}


double matrix_get(const Matrix M, const size_t row_number, const size_t col_number)
{
    if(row_number > M.rows || col_number > M.cols) {
        puts("Недопустимый индекс");
        return MAT_SIZE_ERR;
    }   

    return M.data[row_number * M.cols + col_number];
}


Matrix_status matrix_set(Matrix M, double element, const size_t row_number, const size_t col_number)
{
    if(row_number > M.rows || col_number > M.cols) {
        puts("Недопустимый индекс");
        return MAT_SIZE_ERR;
    }

    M.data[row_number * M.cols + col_number] = element;

    return MAT_OK;
}


double matrix_get_max_element(const Matrix M)
{
    double max_element = M.data[0];

    for(size_t idx = 0; idx < M.cols * M.rows; ++idx) {
        if(M.data[idx] > max_element) max_element = M.data[idx];
    }

    return max_element;
}


double matrix_get_max_absolute_element(const Matrix M)
{
    double max_element = M.data[0];

    for(size_t idx = 0; idx < M.cols * M.rows; ++idx) {
        if(fabs(M.data[idx]) > max_element) max_element = fabs(M.data[idx]);
    }

    return max_element;
}


unsigned char matrix_is_empty(const Matrix M)
{
    return (M.data == NULL);
}


unsigned char matrix_equal_size(const Matrix m1, const Matrix m2)
{
    return (m1.rows == m2.rows && m1.cols == m2.cols);
}


unsigned char matrix_is_square(const Matrix m)
{
    return (m.rows == m.cols);
}


unsigned char matrix_compatible(const Matrix m1, const Matrix m2)
{
    return (m1.cols == m2.rows);
}


Matrix_status matrix_zeros(Matrix m)
{
    memset(m.data, 0, m.cols * m.rows * sizeof(double));

    return MAT_OK;
}


Matrix_status matrix_ones(Matrix m)
{
    for(size_t idx = 0; idx < m.cols * m.rows; ++idx) {
        m.data[idx] = 1.0;
    }

    return MAT_OK;
}


Matrix_status matrix_copy(Matrix m_new, const Matrix m)
{
    if(!matrix_equal_size(m, m_new)) return MAT_SIZE_ERR;

    for(size_t idx = 0; idx < m.cols * m.rows; ++idx) {
        m_new.data[idx] = m.data[idx];
    }

    return MAT_OK;
}


Matrix_status matrix_identity(Matrix m)
{
    if(!matrix_is_square(m)) return MAT_SIZE_ERR;

    for(size_t idx = 0; idx < m.rows; ++idx) {
        m.data[idx * m.rows + idx] = 1.0;
    }

    return MAT_OK;
}


Matrix_status matrix_upper_triangular(Matrix m)
{
    if(!matrix_is_square(m)) return MAT_SIZE_ERR;

    for(size_t rows_idx = 0; rows_idx < m.rows; ++rows_idx) {
        for(size_t cols_idx = 0; cols_idx < rows_idx; ++cols_idx) {
            m.data[rows_idx * m.rows + cols_idx] = 0.0;
        }
    }
}


Matrix_status matrix_lower_triangular(Matrix m)
{
    if(!matrix_is_square(m)) return MAT_SIZE_ERR;

    for(size_t rows_idx = 0; rows_idx < m.rows; ++rows_idx) {
        for(size_t cols_idx = rows_idx + 1; cols_idx < m.cols; ++cols_idx) {
            m.data[rows_idx * m.rows + cols_idx] = 0.0;
        }
    }
}


double factorial(const int n)
{
    long double result = 1;
    for(int idx = 2; idx <= n; ++idx) {
        result *= (double)idx;
    }

    return result;
}


Matrix_status matrix_sum(Matrix* m_result, Matrix m1, Matrix m2)
{
    if(!(matrix_equal_size(m1, m2) && matrix_equal_size(*m_result, m1))) return MAT_SIZE_ERR;

    for(size_t idx = 0; idx < m1.rows * m1.cols; ++idx) {
        m_result -> data[idx] = m1.data[idx] + m2.data[idx];
    }

    return MAT_OK;
}


Matrix_status matrix_add(Matrix m1, const Matrix m2)
{
    if(!matrix_equal_size(m1, m2)) return MAT_SIZE_ERR;
        
    for(size_t idx = 0; idx < m1.rows * m1.cols; ++idx) {
        m1.data[idx] += m2.data[idx];
    }

    return MAT_OK;
}


Matrix_status matrix_difference(Matrix* m_result, Matrix m1, Matrix m2)
{
    if(!(matrix_equal_size(m1, m2) && matrix_equal_size(*m_result, m1))) return MAT_SIZE_ERR;

    for(size_t idx = 0; idx < m1.rows * m1.cols; ++idx) {
        m_result -> data[idx] = m1.data[idx] - m2.data[idx];
    }

    return MAT_OK;
}


Matrix_status matrix_sub(Matrix m1, const Matrix m2)
{
    if(!matrix_equal_size(m1, m2)) return MAT_SIZE_ERR;
        
    for(size_t idx = 0; idx < m1.rows * m1.cols; ++idx) {
        m1.data[idx] -= m2.data[idx];
    }

    return MAT_OK;
}


Matrix_status matrix_sub_rows(Matrix M, const size_t row_minuend, const size_t row_sub)
// row_minuend - уменьшаемая строка, row_sub - вычитаемая строка
{
    if(M.data == NULL) {
        puts("Ошибка NULL pointer");
        return MAT_NULL_ERR;
    }

    for(size_t idx = 0; idx < M.cols; ++idx) {
        M.data[row_minuend * M.cols + idx] -= M.data[row_sub * M.cols + idx];
    }

    return MAT_OK;
}


Matrix_status matrix_mul_num(Matrix m, const double num)
{
    for(size_t idx = 0; idx < m.rows * m.cols; ++idx) {
        m.data[idx] *= num;
    }

    return MAT_OK;
}


Matrix_status matrix_row_mul_num(Matrix M, const size_t row_num, const double num)
{
    for(size_t idx = 0; idx < M.cols; ++idx) {
        M.data[row_num * M.cols + idx] *= num;
    }

    return MAT_OK;
}


Matrix_status matrix_mul(Matrix* m_result, const Matrix m1, const Matrix m2)
{
    if(!(matrix_compatible(m1, m2) && (m_result->rows == m1.rows) && (m_result->cols == m2.cols))) return MAT_SIZE_ERR;

    for(size_t idx_rows = 0; idx_rows < m_result->rows; ++idx_rows) {
        for(size_t idx_cols = 0; idx_cols < m_result->cols; ++idx_cols) {
            for(size_t idx = 0; idx < m1.cols; ++idx) {
                m_result->data[idx_rows * m_result->rows + idx_cols] += m1.data[idx_rows * m1.rows + idx] * m2.data[idx * m2.cols + idx_cols];
            }
        }
    }

    return MAT_OK;
}


Matrix_status matrix_random(Matrix m, const long int lower_limit, const long int higher_limit)
{
    srand(time(NULL));

    for(size_t idx = 0; idx < m.cols * m.rows; ++idx) {
        m.data[idx] = lower_limit + rand() % (higher_limit - lower_limit + 1);
    }

    return MAT_OK;
}


Matrix_status matrix_transp(Matrix m, Matrix m_transp)
{   

    for(size_t rows_idx = 0; rows_idx < m.rows; ++rows_idx) {
        for(size_t cols_idx = 0; cols_idx < m.cols; ++cols_idx) {
            m_transp.data[cols_idx * m.rows + rows_idx] = m.data[rows_idx * m.cols + cols_idx];
        }
    }

    return MAT_OK;
}


void matrix_print(const Matrix matrix)
{
    for(int row_idx = 0; row_idx < matrix.rows; ++row_idx) {
        for(int col_idx = 0; col_idx < matrix.cols; ++col_idx) {
            if(col_idx == 0) printf("[ ");
            (col_idx == matrix.cols - 1) ? printf("%-8.3f ]", matrix.data[row_idx * matrix.cols + col_idx]) : 
            printf("%-8.3f ", matrix.data[row_idx * matrix.cols + col_idx]);
        }

        printf("\n");
    }
    printf("\n");
}


Matrix_status matrix_det(Matrix M)  // Метод Гаусса
{
    if(!matrix_is_square(M)) {
        puts("Матрица не квадратная");
        return MAT_SIZE_ERR;
    }

    if(M.data == NULL) {
        puts("Ошибка NULL pointer");
        return MAT_NULL_ERR;
    }

    double det = 1;
    Matrix M_tmp;
    Matrix* M_tmp_ptr = &M_tmp;
    matrix_alloc(M_tmp_ptr, M.rows, M.cols);

    matrix_copy(M_tmp, M);

    for(size_t idx_main = 0; idx_main < M.rows - 1; ++idx_main) {

        for(size_t idx = idx_main; idx < M.rows; ++idx) {
            det *= M_tmp.data[idx * M.cols + idx_main];
            matrix_row_mul_num(M_tmp, idx, 1.0/M_tmp.data[idx * M.cols + idx_main]);
        }

        for(size_t idx_2 = idx_main + 1; idx_2 < M.rows ; ++idx_2) {
            matrix_sub_rows(M_tmp, idx_2, idx_main);
        }  
    }

    det *= M_tmp.data[M.rows * M.cols - 1];
    matrix_free(M_tmp_ptr);
    printf("%.3lf\n", det);
    return MAT_OK;
}


Matrix_status matrix_pow(Matrix M_result, const Matrix M, int pow)
{
    if(!matrix_is_square(M)) return MAT_SIZE_ERR;
    if(!matrix_equal_size(M_result, M)) return MAT_SIZE_ERR;

    Matrix* M_result_ptr = &M_result;

    Matrix M_temp;
    Matrix* M_temp_ptr = &M_temp;
    matrix_alloc(M_temp_ptr, M.cols, M.rows);

    if(pow == 0) {
        matrix_identity(M_result);
        return MAT_OK;
    }

    if(pow == 1) {
        matrix_copy(M_result, M);
        return MAT_OK;
    }

    matrix_copy(M_temp, M);
    
    for(int idx = 2; idx <= pow; ++idx) {
        matrix_mul(M_result_ptr, M, M_temp);

        if(idx == pow) {
            matrix_free(M_temp_ptr);
            return MAT_OK;
        }

        matrix_copy(M_temp, M_result);
        matrix_zeros(M_result);
    }
}



Matrix_status matrix_exp(Matrix M_exp, const Matrix M)
{
    if(!matrix_is_square(M)) return MAT_SIZE_ERR;
    if(!matrix_equal_size(M_exp, M)) return MAT_SIZE_ERR;

    Matrix M_tmp = {.cols = M.cols, .rows = M.rows};
    Matrix* M_tmp_ptr = &M_tmp;

    matrix_alloc(M_tmp_ptr, M.cols, M.rows);
    int k = 0;

    while(1) {
        matrix_pow(M_tmp, M, k);
        matrix_mul_num(M_tmp, 1.0/factorial(k));
        matrix_add(M_exp, M_tmp);

        if(matrix_get_max_absolute_element(M_tmp) < 0.001) {
            matrix_free(M_tmp_ptr);
            return MAT_OK;
        }

        matrix_zeros(M_tmp);
        ++k;
    }

    matrix_free(M_tmp_ptr);

}