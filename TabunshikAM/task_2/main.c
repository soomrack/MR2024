#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

struct Matrix 
{
    size_t cols;
    size_t rows;
    double *data;
};

typedef struct Matrix Matrix;


enum matrix_exception_level {ERROR, WARNING};


void Matrix_exception(const enum matrix_exception_level level, char *msg) 
{
    if (level == ERROR) {
        printf("ERROR: %s\n", msg);
    } 
    else if (level == WARNING) {
        printf("WARNING: %s\n", msg);
    }
}


Matrix matrix_allocate(const size_t cols, const size_t rows) 
{
    Matrix mat = {0, 0, NULL};
    
    if (cols == 0 || rows == 0) return (Matrix){cols, rows, NULL};
    
    if ((SIZE_MAX * sizeof(double)) / (rows * cols) < 1) {
        Matrix_exception(ERROR, "memory allocation overflow");
        return mat;
    }
    
    mat.cols = cols;
    mat.rows = rows;
    mat.data = (double *)malloc(cols * rows * sizeof(double));
    if (mat.data == NULL) {
        Matrix_exception(ERROR, "failed to allocate memory for matrix");
        return (Matrix){0, 0, NULL};
    }
    return mat;
}


void matrix_free(Matrix* mat) 
{
    if (mat == NULL || mat->data == NULL) return;
    free(mat->data);
    *mat = (Matrix){0, 0, NULL};
}

// Заполнение матрицы нулями добавить проверк на 0 размер заменить числа на 1.0 indx на indx retern 0
void matrix_set_zeros(Matrix mat) 
{
    memset(mat.data, 0.0, mat.cols * mat.rows * sizeof(double));
}


void fill_matrix_with_random(Matrix mat) 
{
    int sign = 0;
    for (size_t indx = 0; indx < mat.cols * mat.rows; indx++) {
        sign = (rand() % 2) * 2 - 1;
        mat.data[indx] = (double)(rand() % 8) * sign;
    }
}


void matrix_set_identity(Matrix mat) 
{
    matrix_set_zeros(mat);
    for (size_t indx = 0; indx < mat.cols * mat.rows; indx += mat.cols + 1)
        mat.data[indx] = 1;
}


void matrix_print(const Matrix mat) 
{
    if (mat.data == NULL) return;

    printf("\n");
    for (size_t row = 0; row < mat.rows; row++) {
        for (size_t col = 0; col < mat.cols; col++) {
            printf("%lf ", mat.data[row * mat.cols + col]);
        }
        printf("\n");
    }
    printf("\n");
}


static int matrix_is_zero(const Matrix mat) {
    for(size_t indx = 0; indx < mat.rows * mat.cols; indx++) {
        if(mat.data[indx] != 0) return 0;
    }

    return 1;
}


Matrix matrix_copy(Matrix dest, const Matrix src) 
{
    if (src.data == NULL) {
        Matrix_exception(ERROR, "matrix is empty");
        return (Matrix){0, 0, NULL};
    }
    if (dest.cols != src.cols || dest.rows != src.rows) {
        Matrix_exception(ERROR, "matrix dimensions do not match for copying");
        return (Matrix){0, 0, NULL};
    }
    memcpy(dest.data, src.data, src.cols * src.rows * sizeof(double));
    return dest;
}


Matrix matrix_sum(Matrix result, const Matrix A, const Matrix B) 
{
    if (A.data == NULL) {
        Matrix_exception(ERROR, "matrix is empty");
        return (Matrix){0, 0, NULL};
    }
    if (A.cols != B.cols || A.rows != B.rows) {
        Matrix_exception(ERROR, "matrix dimensions do not match for addition");
        return (Matrix){0, 0, NULL};
    }
    for (size_t indx = 0; indx < A.cols * A.rows; indx++) {
        result.data[indx] = A.data[indx] + B.data[indx];
    }
    return result;
}


Matrix matrix_subtract(Matrix result, const Matrix A, const Matrix B) 
{
    if (A.data == NULL) {
        Matrix_exception(ERROR, "matrix is empty");
        return (Matrix){0, 0, NULL};
    }
    if (A.cols != B.cols || A.rows != B.rows) {
        Matrix_exception(ERROR, "matrix dimensions do not match for subtraction");
        return (Matrix){0, 0, NULL};
    }
    for (size_t indx = 0; indx < A.cols * A.rows; indx++) {
        result.data[indx] = A.data[indx] - B.data[indx];
    }
    return result;
}


Matrix matrix_multiply(Matrix result, const Matrix A, const Matrix B) 
{
    if (A.data == NULL) {
        Matrix_exception(ERROR, "matrix is empty");
        return (Matrix){0, 0, NULL};
    }
    if (A.cols != B.rows) {
        Matrix_exception(ERROR, "matrix dimensions do not match for multiplication");
        return (Matrix){0, 0, NULL};
    }
    matrix_set_zeros(result);
    for (size_t col = 0; col < B.cols; col++) {
        for (size_t row = 0; row < A.rows; row++) {
            for (size_t k = 0; k < B.rows; k++) {
                result.data[row * result.cols + col] += A.data[row * A.cols + k] * B.data[k * B.cols + col];
            }
        }
    }
    return result;
}


double matrix_determinant(Matrix mat) 
{
    if (mat.data == NULL ) {
        Matrix_exception(ERROR, "matrix is empty");
        return NAN;
    }

    if (mat.cols != mat.rows) {
        Matrix_exception(ERROR, "matrix must be square to compute determinant");
        return NAN;
    }
    
    double det = 1.0;
    
    if (matrix_is_zero(mat)) return 0;

    for (size_t col = 0; col < mat.cols; col++) {
        double max = fabs(mat.data[col * mat.cols + col]);
        size_t pivot_row = col;


        for (size_t row = col + 1; row < mat.rows; row++) {
            double value = fabs(mat.data[row * mat.cols + col]);
            if (value > max) {
                max = value;
                pivot_row = row;
            }
        }

        if (pivot_row != col) {
            for (size_t k = 0; k < mat.cols; k++) {
                double temp = mat.data[col * mat.cols + k];
                mat.data[col * mat.cols + k] = mat.data[pivot_row * mat.cols + k];
                mat.data[pivot_row * mat.cols + k] = temp;
            }
            det *= -1.0; 
        }


        for (size_t row = col + 1; row < mat.rows; row++) {
            double factor = mat.data[row * mat.cols + col] / mat.data[col * mat.cols + col];
            for (size_t k = col; k < mat.cols; k++) {
                mat.data[row * mat.cols + k] -= factor * mat.data[col * mat.cols + k];
            }
        }


        det *= mat.data[col * mat.cols + col];
    }

    return det;
}


Matrix matrix_involute(const Matrix involution_res,const Matrix A, const unsigned level)
{
    if (A.data == NULL) {
        Matrix_exception(ERROR, "matrix is empty");
        return (Matrix){0, 0, NULL};
    }
	if(!((A.cols == A.rows))) {
    	Matrix_exception(ERROR, "matrix must be square to compute involution operation");
    	return (Matrix) {0,0,NULL};
	}
	if (level == 0) {
        matrix_set_identity(involution_res);
        return involution_res;
    }    
    if (matrix_is_zero(A) == 1){
        matrix_set_zeros(involution_res);
        return involution_res;
    }  
    matrix_copy(involution_res, A);
    Matrix C = matrix_allocate(A.cols,A.rows);
    for (size_t idx = 1; idx < level; idx++) {
        matrix_free(&C);
        C = matrix_multiply(C, involution_res, A);
        matrix_copy(involution_res, C);
    }
    matrix_free(&C);
    return involution_res;
}



Matrix matrix_divide(struct Matrix matrix_divide, const struct Matrix A, const double divider)
{
    if (A.data == NULL) {
        Matrix_exception(ERROR, "matrix is empty");
        return (Matrix){0, 0, NULL};
    }
	if(divider == 0){
    	Matrix_exception(ERROR, "The divider cant be = 0");
    	return (Matrix) {0,0,NULL};
	}
	for(size_t indx = 0; indx < A.cols * A.rows; indx++) {
        matrix_divide.data[indx] = A.data[indx] / divider;    
    }
    return matrix_divide;
}


static double factorial(int num) 
{
    double f = 1;
    if(num == 0) return f;
	for(double indx = 1; indx <= num; indx++)f *= indx;
    return f;
}


Matrix matrix_exponent(Matrix exponent_res,const Matrix A, const int  level)
{
    if (A.data == NULL) {
        Matrix_exception(ERROR, "matrix is empty");
        return (Matrix){0, 0, NULL};
    }
	if(!((A.cols == A.rows))){
    	Matrix_exception(ERROR, "matrix must be square to compute exponent operation");
    	return (Matrix) {0,0,NULL};
	}
	
	Matrix D = matrix_allocate(A.rows, A.cols);
	
	for (int idx = 0; idx <= level; idx++){
		matrix_copy(D, A);             
		matrix_involute(D, A, idx);
		matrix_divide(D, D, factorial(idx));
		exponent_res = matrix_sum(exponent_res, exponent_res, D);
	}                                 
	matrix_free(&D);                 
    return exponent_res;
}

// Основная функция
int main() {

    srand(time(NULL));

    Matrix A = matrix_allocate(3, 3);
    fill_matrix_with_random(A);

    Matrix B = matrix_allocate(3, 3);
    fill_matrix_with_random(B);

    printf("Matrix A:\n");
    matrix_print(A);

    printf("Matrix B:\n");
    matrix_print(B);

    // Сложение матриц
    Matrix sum = matrix_allocate(A.cols, A.rows);
    sum = matrix_sum(sum, A, B);
    printf("Matrix Sum:\n");
    matrix_print(sum);

    // Вычитание матриц
    Matrix diff = matrix_allocate(A.cols, A.rows);
    diff = matrix_subtract(diff, A, B);
    printf("Matrix Difference:\n");
    matrix_print(diff);

    // Умножение матриц
    Matrix Mult = matrix_allocate(B.cols, A.rows);
    Mult = matrix_multiply(Mult, A, B);
    printf("Matrix Multiply:\n");
    matrix_print(Mult);

    // Вычисление определителя A
    Matrix temp_for_det_A = matrix_allocate(A.cols, A.rows);
    matrix_copy(temp_for_det_A, A);
    double det_A = matrix_determinant(temp_for_det_A);
    printf("Determinant of A: %lf\n", det_A);
    
    // Вычисление определителя B
    Matrix temp_for_det_B = matrix_allocate(B.cols, B.rows);
    matrix_copy(temp_for_det_B, B);
    double det_B = matrix_determinant(temp_for_det_B);
    printf("Determinant of B: %lf\n", det_B);
    
    // Вычисление экспоненты матрицы A
    Matrix Exp_A = matrix_allocate(A.cols, A.rows);
    Exp_A = matrix_exponent(Exp_A, A, 20);
    printf("Matrix A Exponent:\n");
    matrix_print(Exp_A);

    // Вычисление экспоненты матрицы A
    Matrix Exp_B = matrix_allocate(B.cols, B.rows);
    Exp_B = matrix_exponent(Exp_B, A, 20);
    printf("Matrix B Exponent:\n");
    matrix_print(Exp_B);


    matrix_free(&A);
    matrix_free(&B);
    matrix_free(&sum);
    matrix_free(&diff);
    matrix_free(&Mult);
    matrix_free(&temp_for_det_A);
    matrix_free(&temp_for_det_B);
    matrix_free(&Exp_A);
    matrix_free(&Exp_B);
    return 0;
}
