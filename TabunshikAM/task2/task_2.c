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

// Уровни исключений для обработки ошибок
enum matrix_exception_level {ERROR, WARNING};

// Функция для обработки исключений
void Matrix_exception(const enum matrix_exception_level level, char *msg) 
{
    if (level == ERROR) {
        printf("ERROR: %s\n", msg);
    } 
    else if (level == WARNING) {
        printf("WARNING: %s\n", msg);
    }
}

// Функция для выделения памяти под матрицу
Matrix allocate_matrix(const size_t cols, const size_t rows) 
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

// Функция для освобождения памяти, выделенной под матрицу
void free_matrix(Matrix* mat) 
{
    if (mat == NULL || mat->data == NULL) return;
    free(mat->data);
    *mat = (Matrix){0, 0, NULL};
}

// Заполнение матрицы нулями
void fill_matrix_with_zeros(Matrix mat) 
{
    memset(mat.data, 0, mat.cols * mat.rows * sizeof(double));
}

// Заполнение матрицы случайными числами
void fill_matrix_with_random(Matrix mat) 
{
    int sign = 0;
    for (size_t i = 0; i < mat.cols * mat.rows; i++) {
        sign = (rand() % 2) * 2 - 1;
        mat.data[i] = (double)(rand() % 8) * sign;
    }
}

// Создание единичной матрицы
void create_identity_matrix(Matrix mat) 
{
    fill_matrix_with_zeros(mat);
    for (size_t idx = 0; idx < mat.cols * mat.rows; idx += mat.cols + 1)
        mat.data[idx] = 1;
}

// Вывод матрицы на экран
void print_matrix(const Matrix mat) 
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

// Проверка на нулевую матрицу
static int matrix_is_zero(const Matrix mat) {
    for(size_t idx = 0; idx < mat.rows * mat.cols; idx++) {
        if(mat.data[idx] != 0) return 0;
    }

    return 1;
}

// Копирование одной матрицы в другую
Matrix copy_matrix(Matrix dest, const Matrix src) 
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

// Сложение двух матриц
Matrix add_matrices(Matrix result, const Matrix A, const Matrix B) 
{
    if (A.data == NULL || B.data == NULL ) {
        Matrix_exception(ERROR, "matrix is empty");
        return (Matrix){0, 0, NULL};
    }
    if (A.cols != B.cols || A.rows != B.rows) {
        Matrix_exception(ERROR, "matrix dimensions do not match for addition");
        return (Matrix){0, 0, NULL};
    }
    for (size_t i = 0; i < A.cols * A.rows; i++) {
        result.data[i] = A.data[i] + B.data[i];
    }
    return result;
}

// Вычитание матриц
Matrix subtract_matrices(Matrix result, const Matrix A, const Matrix B) 
{
    if (A.data == NULL || B.data == NULL ) {
        Matrix_exception(ERROR, "matrix is empty");
        return (Matrix){0, 0, NULL};
    }
    if (A.cols != B.cols || A.rows != B.rows) {
        Matrix_exception(ERROR, "matrix dimensions do not match for subtraction");
        return (Matrix){0, 0, NULL};
    }
    for (size_t i = 0; i < A.cols * A.rows; i++) {
        result.data[i] = A.data[i] - B.data[i];
    }
    return result;
}

// Умножение двух матриц
Matrix multiply_matrices(Matrix result, const Matrix A, const Matrix B) 
{
    if (A.data == NULL || B.data == NULL ) {
        Matrix_exception(ERROR, "matrix is empty");
        return (Matrix){0, 0, NULL};
    }
    if (A.cols != B.rows) {
        Matrix_exception(ERROR, "matrix dimensions do not match for multiplication");
        return (Matrix){0, 0, NULL};
    }
    fill_matrix_with_zeros(result);
    for (size_t col = 0; col < B.cols; col++) {
        for (size_t row = 0; row < A.rows; row++) {
            for (size_t k = 0; k < B.rows; k++) {
                result.data[row * result.cols + col] += A.data[row * A.cols + k] * B.data[k * B.cols + col];
            }
        }
    }
    return result;
}

// Вычисление определителя методом Гаусса
double determinant_matrix(Matrix mat) 
{
    if (mat.data == NULL ) {
        Matrix_exception(ERROR, "matrix is empty");
        return NAN;
    }

    if (mat.cols != mat.rows) {
        Matrix_exception(ERROR, "matrix must be square to compute determinant");
        return NAN;
    }
    
    double det = 1;
    
    if (matrix_is_zero(mat) == 1){
            det = 0;
            return det;
            }

    for (size_t col = 0; col < mat.cols; col++) {
        double max = fabs(mat.data[col * mat.cols + col]);
        size_t pivot_row = col;

        // Поиск строки с максимальным элементом в текущем столбце
        for (size_t row = col + 1; row < mat.rows; row++) {
            double value = fabs(mat.data[row * mat.cols + col]);
            if (value > max) {
                max = value;
                pivot_row = row;
            }
        }
        // Меняем строки местами
        if (pivot_row != col) {
            for (size_t k = 0; k < mat.cols; k++) {
                double temp = mat.data[col * mat.cols + k];
                mat.data[col * mat.cols + k] = mat.data[pivot_row * mat.cols + k];
                mat.data[pivot_row * mat.cols + k] = temp;
            }
            det *= -1; 
        }

        // Прямой ход метода Гаусса
        for (size_t row = col + 1; row < mat.rows; row++) {
            double factor = mat.data[row * mat.cols + col] / mat.data[col * mat.cols + col];
            for (size_t k = col; k < mat.cols; k++) {
                mat.data[row * mat.cols + k] -= factor * mat.data[col * mat.cols + k];
            }
        }

        // Умножаем на диагональный элемент
        det *= mat.data[col * mat.cols + col];
    }

    return det;
}

// Возведение матрицы в степень
Matrix involute_matrix(Matrix involution_res,const Matrix A, const unsigned  level)
{
    if (A.data == NULL) {
        Matrix_exception(ERROR, "matrix is empty");
        return (Matrix){0, 0, NULL};
    }
	if(!((A.cols == A.rows))) {
    	Matrix_exception(ERROR, "matrix must be square to compute involution operation");
    	return (Matrix) {0,0,NULL};
	}
    if (matrix_is_zero(A) == 1 && level != 0){
        fill_matrix_with_zeros(involution_res);
        return involution_res;
    }  
	if (level == 0) {
        create_identity_matrix(involution_res);
        return involution_res;
    }
    copy_matrix(involution_res, A);
    Matrix C = allocate_matrix(A.cols,A.rows);
    for (size_t idx = 1; idx < level; idx++) {
    	fill_matrix_with_zeros(C);
        C = multiply_matrices(C, involution_res, A);
        copy_matrix(involution_res, C);
    }
    free_matrix(&C);
    return involution_res;
}


// Деление матрицы на число
Matrix divide_matrix(struct Matrix divide_matrix, const struct Matrix A, const float divider)
{
    if (A.data == NULL) {
        Matrix_exception(ERROR, "matrix is empty");
        return (Matrix){0, 0, NULL};
    }
	if(divider == 0){
    	Matrix_exception(ERROR, "The divider cant be = 0");
    	return (Matrix) {0,0,NULL};
	}
	for(size_t idx = 0; idx < A.cols * A.rows; idx++) {
        divide_matrix.data[idx] = A.data[idx] / divider;    
    }
    return divide_matrix;
}

// Вычисление факториала 
static double factorial(int num) 
{
    double f = 1;
    if(num == 0) return f;
	for(double i = 1; i <= num; i++)f *= i;
    return f;
}

// Вычисление экспоненты матрицы 
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
	
	Matrix D = allocate_matrix(A.rows, A.cols);
	
	for (int idx = 0; idx <= level; idx++){
		copy_matrix(D, A);             
		involute_matrix(D, A, idx);
		divide_matrix(D, D, factorial(idx));
		exponent_res = add_matrices(exponent_res, exponent_res, D);
	}                                 
	free_matrix(&D);                 
    return exponent_res;
}

// Основная функция
int main() {

    srand(time(NULL));

    Matrix A = allocate_matrix(2, 2);
    fill_matrix_with_random(A);

    Matrix B = allocate_matrix(2, 2);
    fill_matrix_with_random(B);

    printf("Matrix A:\n");
    print_matrix(A);

    printf("Matrix B:\n");
    print_matrix(B);

    // Сложение матриц
    Matrix sum = allocate_matrix(A.cols, A.rows);
    sum = add_matrices(sum, A, B);
    printf("Matrix Sum:\n");
    print_matrix(sum);

    // Вычитание матриц
    Matrix diff = allocate_matrix(A.cols, A.rows);
    diff = subtract_matrices(diff, A, B);
    printf("Matrix Difference:\n");
    print_matrix(diff);

    // Умножение матриц
    Matrix Mult = allocate_matrix(B.cols, A.rows);
    Mult = multiply_matrices(Mult, A, B);
    printf("Matrix Multiply:\n");
    print_matrix(Mult);

    // Вычисление определителя A
    Matrix temp_for_det_A = allocate_matrix(A.cols, A.rows);
    copy_matrix(temp_for_det_A, A);
    double det_A = determinant_matrix(temp_for_det_A);
    printf("Determinant of A: %lf\n", det_A);
    
    // Вычисление определителя B
    Matrix temp_for_det_B = allocate_matrix(B.cols, B.rows);
    copy_matrix(temp_for_det_B, B);
    double det_B = determinant_matrix(temp_for_det_B);
    printf("Determinant of B: %lf\n", det_B);
    
    // Вычисление экспоненты матрицы A
    Matrix Exp_A = allocate_matrix(A.cols, A.rows);
    Exp_A = matrix_exponent(Exp_A, A, 20);
    printf("Matrix A Exponent:\n");
    print_matrix(Exp_A);

    // Вычисление экспоненты матрицы A
    Matrix Exp_B = allocate_matrix(B.cols, B.rows);
    Exp_B = matrix_exponent(Exp_B, A, 20);
    printf("Matrix B Exponent:\n");
    print_matrix(Exp_B);


    free_matrix(&A);
    free_matrix(&B);
    free_matrix(&sum);
    free_matrix(&diff);
    free_matrix(&Mult);
    free_matrix(&temp_for_det_A);
    free_matrix(&temp_for_det_B);
    free_matrix(&Exp_A);
    free_matrix(&Exp_B);
    return 0;
}
