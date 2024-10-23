#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>


typedef struct {
    size_t cols;
    size_t rows;
    double* data;
} Matrix;


typedef enum {
    MAT_OK = 0,
    MAT_ALLOC_ERR = 1,
    MAT_EMPTY_ERR = 2,
    MAT_SIZE_ERR = 3
} Matrix_status;


Matrix_status matrix_alloc(Matrix* m_ptr, const size_t rows, const size_t cols)
{
    if(m_ptr == NULL) return MAT_EMPTY_ERR;

    if(rows != 0 && cols != 0) {
        if(__SIZE_MAX__ / rows / cols / sizeof(double) == 0) return MAT_SIZE_ERR;
        m_ptr -> data = (double*)malloc(rows * cols * sizeof(double));
        if(m_ptr -> data == NULL) return MAT_ALLOC_ERR;
    }
    
    m_ptr -> rows = rows;
    m_ptr -> cols = cols;

    return MAT_OK;
}


Matrix_status matrix_free(Matrix* m_ptr)
{
    free(m_ptr -> data);
    return MAT_OK;
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


Matrix_status matrix_copy(const Matrix m, Matrix m_new)
{
    if(!matrix_equal_size(m, m_new)) return MAT_SIZE_ERR;

    for(size_t idx = 0; idx < m.cols * m.rows; ++idx) {
        m_new.data[idx] = m.data[idx];
    }

    return MAT_OK;
}


Matrix_status matrix_unitriangular(Matrix m)
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


Matrix_status matrix_mul_num(Matrix m, const double num)
{
    for(size_t idx = 0; idx < m.rows * m.cols; ++idx) {
        m.data[idx] *= num;
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


Matrix_status matrix_pow(Matrix m, double power)
{
    for(size_t idx = 0; idx < m.rows * m.cols; ++idx) {
        m.data[idx] = pow(m.data[idx], power);
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

/*
Matrix_status matrix_det(const Matrix m)  // разложение по первой строке. Стоит сделать отдельную функцию для получения минора 
{
    if(!matrix_is_square(m)) return MAT_SIZE_ERR;

    double det = 0;

    if(m.rows == 2) {
            det = m.data[0] * m.data[3] - m.data[1] * m.data[2];

            printf("%.3f\n", det);
            return MAT_OK;
    }

    for(size_t idx = 0; idx < m.cols; ++idx) {
        Matrix m_minor;
        Matrix* m_minor_ptr = &m_minor;
        short next_col = 0;
        matrix_alloc(m_minor_ptr, m.rows - 1, m.cols - 1);
        for(size_t rows_idx = 0; rows_idx < m_minor.rows; ++rows_idx) {
            for(size_t cols_idx = 0; cols_idx < m_minor.cols; ++cols_idx) {
                if(cols_idx == idx) {
                    next_col = 1;
                }
                m_minor.data[rows_idx * m_minor.cols + cols_idx] = m.data[(rows_idx + 1) * m.cols + cols_idx + next_col];
            }
        }

        if(m_minor.rows == 2) {
            det += m_minor.data[0] * m_minor.data[2] - m_minor.data[1] * m_minor.data[3];
            matrix_free(m_minor_ptr);
        }
        else matrix_det(m_minor);
        next_col = 0;

        
    }

    printf("%.3lf\n", det);
    return MAT_OK;
    
}
*/


Matrix_status matrix_det(Matrix m, Matrix m_upper_triang)  // Метод Гаусса
{
    if(!matrix_is_square(m)) return MAT_SIZE_ERR;

    double r_elem, det = 1;  // r_elem число, на которое умножается строка при сложении с другими строками
    matrix_copy(m, m_upper_triang);

    for(size_t idx = 0; idx < m.rows - 1; ++idx) {
        for(size_t rows_idx = idx + 1; rows_idx < m.rows; ++rows_idx) {
            r_elem = m_upper_triang.data[rows_idx * m_upper_triang.cols + idx] / m_upper_triang.data[idx * m_upper_triang.cols + idx];
            for(size_t cols_idx = 0; cols_idx < m.cols; ++cols_idx) {
                m_upper_triang.data[rows_idx * m_upper_triang.cols + cols_idx] -= m_upper_triang.data[idx * m_upper_triang.cols + cols_idx] * r_elem;
            }
        }
    }

    for(size_t idx = 0; idx < m_upper_triang.cols; ++idx) {
        det *= m_upper_triang.data[idx * m_upper_triang.cols + idx];
    }

    printf("%.3lf\n", det);
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


int main(void)
{
    Matrix m;
    Matrix* m_ptr = &m;

    Matrix m_triang;
    Matrix* m_triang_ptr = &m_triang;

    matrix_alloc(m_ptr, 5, 5);
    matrix_random(m, -6, 6);
    matrix_alloc(m_triang_ptr, 5, 5);

    matrix_print(m);
    //matrix_copy(m, m_triang);
    matrix_det(m, m_triang);
    matrix_print(m_triang);

    matrix_free(m_ptr);
    matrix_free(m_triang_ptr);

    return 0;
}