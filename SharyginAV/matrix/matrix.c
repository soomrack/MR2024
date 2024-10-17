#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


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


unsigned char matrix_equal_size(Matrix m1, Matrix m2)
{
    return (m1.rows == m2.rows && m1.cols == m2.cols);
}


Matrix_status matrix_zeros(Matrix m)
{
    memset(m.data, 0, m.cols * m.rows * sizeof(double));

    return MAT_OK;
}


Matrix_status matrix_ones(Matrix m)
{
    for(int idx = 0; idx < m.cols * m.rows; ++idx) {
        m.data[idx] = 1.0;
    }

    return MAT_OK;
}


Matrix_status matrix_add(Matrix A, Matrix B)
{
    if(!matrix_equal_size(A, B)) return MAT_SIZE_ERR;
        
    for(size_t idx = 0; idx < A.rows * A.cols; ++idx) {
        A.data[idx] += B.data[idx];
    }

    return MAT_OK;
}


Matrix_status matrix_sub(Matrix A, Matrix B)
{
    if(!matrix_equal_size(A, B)) return MAT_SIZE_ERR;
        
    for(size_t idx = 0; idx < A.rows * A.cols; ++idx) {
        A.data[idx] -= B.data[idx];
    }

    return MAT_OK;
}


Matrix_status matrix_random(Matrix m)
{
    srand(time(NULL));

    for(int idx = 0; idx < m.cols * m.rows; ++idx) {
        //(*m_ptr).data[idx] = rand();
        m.data[idx] = rand();
    }

    return MAT_OK;
}


void matrix_print(Matrix matrix)
{
    for(int row_idx = 0; row_idx < matrix.rows; ++row_idx) {
        for(int col_idx = 0; col_idx < matrix.cols; ++col_idx) {
            if(col_idx == 0) printf("[ ");
            (col_idx == matrix.cols - 1) ? printf("%-15.3f ]", matrix.data[row_idx * matrix.cols + col_idx]) : 
            printf("%-15.3f ", matrix.data[row_idx * matrix.cols + col_idx]);
        }

        printf("\n");
    }
    printf("\n");
}


int main(void)
{
    Matrix m1;
    Matrix* m1_ptr = &m1;

    Matrix m2;
    Matrix* m2_ptr = &m2;

    

    matrix_alloc(m1_ptr, 5, 5);
    matrix_random(m1);

    matrix_alloc(m2_ptr, 5, 4);
    matrix_random(m2);

    matrix_print(m1);
    matrix_print(m2);

    matrix_add(m1, m2);
    matrix_print(m1);

    matrix_free(m1_ptr);
    matrix_free(m2_ptr);




    return 0;
}