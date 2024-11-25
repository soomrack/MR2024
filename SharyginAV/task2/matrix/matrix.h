#pragma once
#include <stddef.h>

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
    MAT_SIZE_ERR = 3,
    MAT_NULL_ERR = 4
} Matrix_status;


Matrix_status matrix_alloc(Matrix* M_ptr, const size_t rows, const size_t cols);


Matrix_status matrix_free(Matrix* M_ptr);


double matrix_get(const Matrix M, const size_t row_number, const size_t col_number);


Matrix_status matrix_set(Matrix M, double element, const size_t row_number, const size_t col_number);

double matrix_get_max_element(const Matrix M);


double matrix_get_max_absolute_element(const Matrix M);


unsigned char matrix_is_empty(const Matrix M);


unsigned char matrix_equal_size(const Matrix m1, const Matrix m2);


unsigned char matrix_is_square(const Matrix m);


unsigned char matrix_compatible(const Matrix m1, const Matrix m2);


Matrix_status matrix_zeros(Matrix m);


Matrix_status matrix_ones(Matrix m);


Matrix_status matrix_copy(Matrix m_new, const Matrix m);


Matrix_status matrix_identity(Matrix m);


Matrix_status matrix_upper_triangular(Matrix m);


Matrix_status matrix_lower_triangular(Matrix m);


double factorial(const unsigned int n);


Matrix_status matrix_sum(Matrix* m_result, Matrix m1, Matrix m2);


Matrix_status matrix_add(Matrix m1, const Matrix m2);

Matrix_status matrix_difference(Matrix* m_result, Matrix m1, Matrix m2);


Matrix_status matrix_sub(Matrix m1, const Matrix m2);


Matrix_status matrix_sub_rows(Matrix M, const size_t row_minuend, const size_t row_sub);


Matrix_status matrix_mul_num(Matrix m, const double num);


Matrix_status matrix_row_mul_num(Matrix M, const size_t row_num, const double num);


Matrix_status matrix_mul(Matrix* m_result, const Matrix m1, const Matrix m2);


Matrix_status matrix_random(Matrix m, const long int lower_limit, const long int higher_limit);


Matrix_status matrix_transp(Matrix m, Matrix m_transp);


void matrix_print(const Matrix matrix);


double matrix_det(Matrix M);  // Метод Гаусса


Matrix_status matrix_pow(Matrix M_result, const Matrix M, int pow);



Matrix_status matrix_exp(Matrix M_exp, const Matrix M);

