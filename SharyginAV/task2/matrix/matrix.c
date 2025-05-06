#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>


const Matrix EMPTY = {.cols = 0, .rows = 0, .data = NULL};


void print_log(LogLevel level, char* msg)
{
    switch(level)
    {
    case LOG_ERR:
        printf("ERROR: %s\n", msg);
        return;
    case LOG_WARN:
        printf("WARNING %s\n", msg);
        return;
    case LOG_INFO:
        printf("INFO: %s\n", msg);
        return;
    default:
        return;
    }
}


Matrix_status matrix_alloc(Matrix* M_ptr, const size_t rows, const size_t cols)
{
    print_log(LOG_INFO, "Выделение памяти для матрицы");

    if(M_ptr == NULL) {
        print_log(LOG_ERR, "Указатель NULL передан как аргумент");
        return MAT_EMPTY_ERR;
    }

    if(rows == 0 || cols == 0) {
        *M_ptr = EMPTY;
        print_log(LOG_ERR, "Одна из размерностей матрицы равна 0");
        return MAT_SIZE_ERR;
    }
    
    if(__SIZE_MAX__ / rows / cols / sizeof(double) == 0) {
        print_log(LOG_ERR, "Слишком большая матрица");
        return MAT_SIZE_ERR;
    }

    M_ptr->data = (double*)malloc(rows * cols * sizeof(double));
    if(M_ptr->data == NULL) {
        print_log(LOG_ERR, "Ошибка выделения памяти");
        return MAT_ALLOC_ERR;
    }
    
    M_ptr->rows = rows;
    M_ptr->cols = cols;

    return MAT_OK;
}


Matrix_status matrix_free(Matrix* M_ptr)
{
    print_log(LOG_INFO, "Освобождение памяти");

    if(M_ptr == NULL) {
        print_log(LOG_ERR, "Указатель NULL передан как аргумент");
        return MAT_NULL_ERR;
    }

    free(M_ptr->data);
    *M_ptr = EMPTY;
    return MAT_OK;
}


double matrix_get(const Matrix M, const size_t row_number, const size_t col_number)
{
    if(row_number > M.rows || col_number > M.cols) {
        print_log(LOG_ERR, "Недопустимый индекс");
        return MAT_SIZE_ERR;
    }

    if(M.cols == 0 || M.rows == 0) {
        print_log(LOG_ERR, "Одна из размерностей матрицы равна 0");
        return MAT_SIZE_ERR;
    }   

    return M.data[row_number * M.cols + col_number];
}


Matrix_status matrix_set(Matrix M, double element, const size_t row_number, const size_t col_number)
{
    if(row_number > M.rows || col_number > M.cols) {
        print_log(LOG_ERR, "Недопустимый индекс");
        return MAT_SIZE_ERR;
    }

    if(M.cols == 0 || M.rows == 0) {
        print_log(LOG_ERR, "Одна из размерностей матрицы равна 0");
        return MAT_SIZE_ERR;
    } 

    M.data[row_number * M.cols + col_number] = element;

    return MAT_OK;
}


double matrix_get_max_element(const Matrix M)
{
    if(M.cols == 0 || M.rows == 0) {
        print_log(LOG_ERR, "Одна из размерностей матрицы равна 0");
        return MAT_SIZE_ERR;
    }

    double max_element = M.data[0];

    for(size_t idx = 0; idx < M.cols * M.rows; ++idx) {
        if(M.data[idx] > max_element) max_element = M.data[idx];
    }

    return max_element;
}


double matrix_get_max_absolute_element(const Matrix M)
{
    if(M.cols == 0 || M.rows == 0) {
        print_log(LOG_ERR, "Одна из размерностей матрицы равна 0");
        return MAT_SIZE_ERR;
    } 

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


unsigned char matrix_equal_size(const Matrix A, const Matrix B)
{
    return (A.rows == B.rows && A.cols == B.cols);
}


unsigned char matrix_is_square(const Matrix M)
{
    return (M.rows == M.cols);
}


unsigned char matrix_compatible(const Matrix A, const Matrix B)
{
    return (A.cols == B.rows);
}


Matrix_status matrix_zeros(Matrix M)
{
    memset(M.data, 0, M.cols * M.rows * sizeof(double));

    return MAT_OK;
}


Matrix_status matrix_ones(Matrix M)
{
    for(size_t idx = 0; idx < M.cols * M.rows; ++idx) {
        M.data[idx] = 1.0;
    }

    return MAT_OK;
}


Matrix_status matrix_copy(Matrix M_new, const Matrix M)
{
    if(!matrix_equal_size(M, M_new)) {
        print_log(LOG_ERR, "Матрицы разного размера");
        return MAT_SIZE_ERR;
    }

    memcpy(M_new.data, M.data, M.cols * M.cols * sizeof(M.data[0]));

    return MAT_OK;
}


Matrix_status matrix_identity(Matrix M)
{
    if(M.cols == 0 || M.rows == 0) {
        print_log(LOG_ERR, "Одна из размерностей матрицы равна 0");
        return MAT_SIZE_ERR;
    } 

    if(!matrix_is_square(M)) {
        print_log(LOG_ERR, "Матрица не квадратная");
        return MAT_SIZE_ERR;
    }

    memset(M.data, 0, M.cols * M.rows * sizeof(double));

    for(size_t idx = 0; idx < M.rows; ++idx) {
        M.data[idx * M.rows + idx] = 1.0;
    }

    return MAT_OK;
}


Matrix_status matrix_upper_triangular(Matrix M)
{
    if(!matrix_is_square(M)) {
        print_log(LOG_ERR, "Матрица не квадратная");
        return MAT_SIZE_ERR;
    }

    for(size_t rows_idx = 0; rows_idx < M.rows; ++rows_idx) {
        for(size_t cols_idx = 0; cols_idx < rows_idx; ++cols_idx) {
            M.data[rows_idx * M.rows + cols_idx] = 0.0;
        }
    }
}


Matrix_status matrix_lower_triangular(Matrix M)
{
    if(!matrix_is_square(M)) {
        print_log(LOG_ERR, "Матрица не квадратная");
        return MAT_SIZE_ERR;
    }

    for(size_t rows_idx = 0; rows_idx < M.rows; ++rows_idx) {
        for(size_t cols_idx = rows_idx + 1; cols_idx < M.cols; ++cols_idx) {
            M.data[rows_idx * M.rows + cols_idx] = 0.0;
        }
    }
}


double factorial(const int unsigned n)
{
    double result = 1;
    for(int idx = 2; idx <= n; ++idx) {
        result *= (double)idx;
    }

    return result;
}


Matrix_status matrix_sum(Matrix* M_result, Matrix A, Matrix B)
{
    if(A.cols == 0 || A.rows == 0) {
        print_log(LOG_ERR, "Одна из размерностей матрицы равна 0");
        return MAT_SIZE_ERR;
    } 

    if(!(matrix_equal_size(A, B) && matrix_equal_size(*M_result, A))) {
        print_log(LOG_ERR, "Матрицы разного размера");
        return MAT_SIZE_ERR;
    }

    for(size_t idx = 0; idx < A.rows * A.cols; ++idx) {
        M_result->data[idx] = A.data[idx] + B.data[idx];
    }

    return MAT_OK;
}


Matrix_status matrix_add(Matrix A, const Matrix B)
{
    if(A.cols == 0 || A.rows == 0) {
        print_log(LOG_ERR, "Одна из размерностей матрицы равна 0");
        return MAT_SIZE_ERR;
    } 

    if(!matrix_equal_size(A, B)) {
        print_log(LOG_ERR, "Матрицы разного размера");
        return MAT_SIZE_ERR;
    }
        
    for(size_t idx = 0; idx < A.rows * A.cols; ++idx) {
        A.data[idx] += B.data[idx];
    }

    return MAT_OK;
}


Matrix_status matrix_difference(Matrix* M_result, Matrix A, Matrix B)
{
    if(A.cols == 0 || A.rows == 0) {
        print_log(LOG_ERR, "Одна из размерностей матрицы равна 0");
        return MAT_SIZE_ERR;
    } 

    if(!(matrix_equal_size(A, B) && matrix_equal_size(*M_result, A))) {
        print_log(LOG_ERR, "Матрицы разного размера");
        return MAT_SIZE_ERR;
    }

    for(size_t idx = 0; idx < A.rows * A.cols; ++idx) {
        M_result->data[idx] = A.data[idx] - B.data[idx];
    }

    return MAT_OK;
}


Matrix_status matrix_sub(Matrix A, const Matrix B)
{
    if(A.cols == 0 || A.rows == 0) {
        print_log(LOG_ERR, "Одна из размерностей матрицы равна 0");
        return MAT_SIZE_ERR;
    } 

    if(!matrix_equal_size(A, B)) {
        print_log(LOG_ERR, "Матрицы разного размера");
        return MAT_SIZE_ERR;
    }
        
    for(size_t idx = 0; idx < A.rows * A.cols; ++idx) {
        A.data[idx] -= B.data[idx];
    }

    return MAT_OK;
}


Matrix_status matrix_sub_rows(Matrix M, const size_t row_minuend, const size_t row_sub)
// row_minuend - уменьшаемая строка, row_sub - вычитаемая строка
{
    if(M.data == NULL) {
        print_log(LOG_ERR, "Указатель NULL передан как аргумент");
        return MAT_NULL_ERR;
    }

    if(row_minuend > M.cols || row_sub > M.cols) {
        print_log(LOG_ERR, "Недопустимый номер строки");
        return MAT_SIZE_ERR;
    }

    for(size_t idx = 0; idx < M.cols; ++idx) {
        M.data[row_minuend * M.cols + idx] -= M.data[row_sub * M.cols + idx];
    }

    return MAT_OK;
}


Matrix_status matrix_add_col(Matrix M, const size_t col_increased, const size_t col_added)
// col_increased - столбец к которому прибавляем, col_added - прибавлемый столбец
{
    if(M.data == NULL) {
        print_log(LOG_ERR, "Указатель NULL передан как аргумент");
        return MAT_NULL_ERR;
    }

    if(col_increased > M.cols || col_added > M.cols) {
        print_log(LOG_ERR, "Недопустимый номер стролбца");
        return MAT_SIZE_ERR;
    }

    for(size_t idx = 0; idx < M.cols; ++idx) {
        M.data[idx * M.cols + col_increased] += M.data[idx * M.cols + col_added];
    }

    return MAT_OK;
}


Matrix_status matrix_swap_rows(Matrix M, const size_t row_num_1, const size_t row_num_2)
{
    if(row_num_1 > M.cols || row_num_2 > M.cols) {
        print_log(LOG_ERR, "Недопустимый номер строки");
        return MAT_SIZE_ERR;
    }

    if(M.data == NULL) {
        print_log(LOG_ERR, "Указатель NULL передан как аргумент");
        return MAT_NULL_ERR;
    }

    double tmp_row[M.cols];
    size_t size = M.cols * sizeof(double);

    memcpy(tmp_row, M.data + row_num_1 * M.cols, size);
    memcpy(M.data + row_num_1 * M.cols, M.data + row_num_2 * M.cols, size);
    memcpy(M.data + row_num_2 * M.cols, tmp_row, size);

    return MAT_OK;
}


Matrix_status matrix_mul_num(Matrix M, const double num)
{
    if(M.data == NULL) {
        print_log(LOG_ERR, "Указатель NULL передан как аргумент");
        return MAT_NULL_ERR;
    }

    for(size_t idx = 0; idx < M.rows * M.cols; ++idx) {
        M.data[idx] *= num;
    }

    return MAT_OK;
}


Matrix_status matrix_row_mul_num(Matrix M, const size_t row_num, const double num)
{
    if(M.data == NULL) {
        print_log(LOG_ERR, "Указатель NULL передан как аргумент");
        return MAT_NULL_ERR;
    }

    if(row_num > M.cols) {
        print_log(LOG_ERR, "Недопустимый номер строки");
        return MAT_SIZE_ERR;
    }

    for(size_t idx = 0; idx < M.cols; ++idx) {
        M.data[row_num * M.cols + idx] *= num;
    }

    return MAT_OK;
}


Matrix_status matrix_mul(Matrix* M_result, const Matrix A, const Matrix B)
{
    if(M_result->data == NULL) {
        print_log(LOG_ERR, "Указатель NULL передан как аргумент");
        return MAT_NULL_ERR;
    }

    if(!(matrix_compatible(A, B) && (M_result->rows == A.rows) && (M_result->cols == B.cols))) {
        print_log(LOG_ERR, "Неправильный размер матриц");
        return MAT_SIZE_ERR;
    }

    matrix_zeros(*M_result);

    for(size_t idx_rows = 0; idx_rows < M_result->rows; ++idx_rows) {
        for(size_t idx_cols = 0; idx_cols < M_result->cols; ++idx_cols) {
            for(size_t idx = 0; idx < A.cols; ++idx) {
                M_result->data[idx_rows * M_result->rows + idx_cols] += A.data[idx_rows * A.rows + idx] * B.data[idx * B.cols + idx_cols];
            }
        }
    }

    return MAT_OK;
}


Matrix_status matrix_random(Matrix M, const long int lower_limit, const long int higher_limit)
{
    if(M.data == NULL) {
        print_log(LOG_ERR, "Указатель NULL передан как аргумент");
        return MAT_NULL_ERR;
    }

    srand(time(NULL));

    for(size_t idx = 0; idx < M.cols * M.rows; ++idx) {
        M.data[idx] = lower_limit + rand() % (higher_limit - lower_limit + 1);
    }

    return MAT_OK;
}


Matrix_status matrix_transp(Matrix M_transp, const Matrix M)
{   
    if(M.data == NULL || M_transp.data == NULL) {
        print_log(LOG_ERR, "Указатель NULL передан как аргумент");
        return MAT_NULL_ERR;
    }

    if(!(matrix_equal_size(M, M_transp))) {
        print_log(LOG_ERR, "Матрицы разного рамзера");
        return MAT_SIZE_ERR;
    }

    for(size_t rows_idx = 0; rows_idx < M.rows; ++rows_idx) {
        for(size_t cols_idx = 0; cols_idx < M.cols; ++cols_idx) {
            M_transp.data[cols_idx * M.rows + rows_idx] = M.data[rows_idx * M.cols + cols_idx];
        }
    }

    return MAT_OK;
}


void matrix_print(const Matrix M)
{
    if(M.data == NULL) {
        print_log(LOG_ERR, "Указатель NULL передан как аргумент");
        return;
    }

    if(M.cols == 0 || M.rows == 0) {
        print_log(LOG_ERR, "Одна из размерностей матрицы равна 0");
        return;
    }

    for(int row_idx = 0; row_idx < M.rows; ++row_idx) {
        for(int col_idx = 0; col_idx < M.cols; ++col_idx) {
            if(col_idx == 0) printf("[ ");
            (col_idx == M.cols - 1) ? printf("%-8.3f ]", M.data[row_idx * M.cols + col_idx]) : 
            printf("%-8.3f ", M.data[row_idx * M.cols + col_idx]);
        }

        printf("\n");
    }
    printf("\n");
    return;
}


double matrix_det(Matrix M)  // Метод Гаусса
{
    if(!matrix_is_square(M)) {
        print_log(LOG_ERR, "Матрица не квадратная");
        return NAN;
    }

    if(M.data == NULL) {
        print_log(LOG_ERR, "Указатель NULL передан как аргумент");
        return NAN;
    }

    double det = 1.0;
    Matrix M_tmp;
    matrix_alloc(&M_tmp, M.rows, M.cols);

    if(M_tmp.data == NULL) {
        print_log(LOG_ERR, "Указатель NULL передан как аргумент");
        return NAN;
    }

    if(M.data[0] > -0.001 && M.data[0] < 0.001) {
        matrix_add_col(M, 0, 1);
    }

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
    matrix_free(&M_tmp);
    
    return det;
}


Matrix_status matrix_pow(Matrix M_result, const Matrix M, unsigned int pow)
{
    if(M_result.data == NULL || M.data == NULL) {
        print_log(LOG_ERR, "Указатель NULL передан как аргумент");
        return MAT_NULL_ERR;
    }
    
    if(!matrix_is_square(M)) {
        print_log(LOG_ERR, "Матрица не квадратная");
        return MAT_SIZE_ERR;
    }
    if(!matrix_equal_size(M_result, M)) {
        print_log(LOG_ERR, "Матрицы разного размера");
        return MAT_SIZE_ERR;
    }

    Matrix M_temp;
    Matrix* M_temp_ptr = &M_temp;
    matrix_alloc(M_temp_ptr, M.cols, M.rows);

    if(pow == 0) {
        matrix_identity(M_result);
        matrix_free(&M_temp);
        return MAT_OK;
    }

    if(pow == 1) {
        matrix_copy(M_result, M);
        matrix_free(&M_temp);
        return MAT_OK;
    }

    matrix_copy(M_temp, M);
    
    for(int idx = 2; idx <= pow; ++idx) {
        matrix_mul(&M_result, M, M_temp);
        matrix_copy(M_temp, M_result);
        matrix_zeros(M_result);
    }

    matrix_free(&M_temp);
    return MAT_OK;
}



Matrix_status matrix_exp(Matrix M_exp, const Matrix M)
{
    if(M_exp.data == NULL || M.data == NULL) {
        print_log(LOG_ERR, "Указатель NULL передан как аргумент");
        return MAT_NULL_ERR;
    }

    if(!matrix_is_square(M)) {
        print_log(LOG_ERR, "Матрица не квадратная");
        return MAT_SIZE_ERR;
    }

    if(!matrix_equal_size(M_exp, M)) {
        print_log(LOG_ERR, "Матрицы разного размера");
        return MAT_SIZE_ERR;
    }

    Matrix M_tmp = {.cols = M.cols, .rows = M.rows};
    Matrix* M_tmp_ptr = &M_tmp;

    matrix_alloc(M_tmp_ptr, M.cols, M.rows);
    int k = 0;

    for(int k = 0; k <= 20; ++k) {
        matrix_pow(M_tmp, M, k);
        matrix_mul_num(M_tmp, 1.0/factorial(k));
        matrix_add(M_exp, M_tmp);
    }

    matrix_free(M_tmp_ptr);
    return MAT_OK;
}