#include <stdio.h>
#include <stdlib.h>


typedef struct 
{
    int rows;
    int cols;
    double *data;
} Matrix;

//Необходимые функции матриц
//Инициализация матрицы +
//Заполнение матрицы +
//Копирование матрицы + 
//Вывод матрицы +
//Сложение матриц +
//Вычитание матриц + 
//Умножение матрицы на число +
//Деление матрицы на число + 
//Умножение матриц +
//Степень матрицы (натуральное число) +
//Нахождение определителя матрицы
//Транспонирование матрицы +
//Деление матриц
//Экспонента матрицы




void matrix_init(Matrix *A, const int rows, const int cols)
{
    A->data = (double *)malloc(rows * cols * sizeof(double));
    A->rows = rows;
    A->cols = cols;
}


void matrix_fill(Matrix *A)
{
    for (size_t i = 0; i < A->rows * A->cols; i++) {
        A->data[i] = i + 1;
    }
}


void matrix_clone(Matrix *result, Matrix A)
{
    matrix_init(&*result, A.rows, A.cols);
    for (size_t i = 0; i < A.rows * A.cols; i++) {
        result->data[i] = A.data[i];
    }
    
}


void matrix_sum(Matrix *result, Matrix A, Matrix B)
{
    if (A.rows == B.rows & A.cols == B.cols) {
        for (size_t i = 0; i < A.rows * A.cols; i++) {
            result->data[i] = A.data[i] + B.data[i];   
        }
    }
}


void matrix_sub(Matrix *result, Matrix A, Matrix B)
{
    if (A.rows == B.rows & A.cols == B.cols) {
        for (size_t i = 0; i < A.rows * A.cols; i++) {
            result->data[i] = A.data[i] - B.data[i];   
        }
    }
}


void matrix_mul_num(Matrix *result, Matrix A, double num)
{
    for (size_t i = 0; i < A.rows * A.cols; i++) {
        result->data[i] = A.data[i] * num;
    }
}


void matrix_div_num(Matrix *result, Matrix A, double num)
{
    for (size_t i = 0; i < A.rows * A.cols; i++) {
        result->data[i] = A.data[i] / num;
    }
}


void matrix_mul(Matrix *result, Matrix A, Matrix B)
{
    if (A.cols == B.rows) {
        matrix_init(&*result, A.rows, B.cols);
        for (size_t row = 0; row < A.rows; row++) {
            for (size_t col = 0; col < B.cols; col++) {
                result->data[row * result->cols + col] = 0;
                for (size_t i = 0; i < A.cols; i++) {
                    result->data[row * result->cols + col] += A.data[row * A.cols + i] * B.data[i * B.cols + col];
                }
            }
        }
    }
}


void matrix_pow(Matrix *result, Matrix A, int power){
    if (A.cols == A.rows && power > 0) {
        for (size_t i = 1; i < power; i++) {
            matrix_mul(&*result, *result, A);
        }
    }
}


void matrix_print(Matrix A)
{   
    for (size_t i = 0; i < A.cols * A.rows; i++) {
        if ((i) % A.cols == 0) {
            printf("|");
        }
        printf("%.2f|", A.data[i]);
        if ((i) % A.cols == A.cols - 1) {
            printf("\n");
        }
    }
    printf("\n");
}


void matrix_transp(Matrix *A)
{
    Matrix B;
    matrix_clone(&B, *A);
    matrix_init(&*A, A->cols, A->rows);
    for (int i = 0; i < B.cols * B.rows; i++) {
        A->data[B.rows * (i % B.cols) + i / B.cols] = B.data[i];
    }
}


double matrix_det(Matrix A)
{
    Matrix B;
    matrix_clone(&B, A);
    double result = 1;
    if(A.rows == A.cols) {
        for (size_t col_nul = 0; col_nul < A.cols; col_nul++) {
            for (size_t row = col_nul + 1; row < A.rows; row++) {
                double proportion = B.data[col_nul * B.cols + col_nul] / B.data[row * B.cols + col_nul];
                for (size_t col = col_nul; col < A.cols; col++) {
                    B.data[row * B.cols + col] -= B.data[col_nul * B.cols + col] * proportion;
                }
                result *= B.data[col_nul * B.cols + col_nul];
            }
        }
    }
    return result;
}


int main()  
{
    Matrix first_matrix;
    Matrix second_matrix;
    Matrix third_matrix;
    matrix_init(&first_matrix, 3, 3);
    matrix_fill(&first_matrix);
    matrix_print(first_matrix);
    matrix_pow(&first_matrix, first_matrix, 3);
    matrix_print(first_matrix);
    matrix_init(&second_matrix, 2, 3);
    matrix_init(&third_matrix, 3, 3);
    matrix_fill(&second_matrix);
    matrix_fill(&third_matrix);
    matrix_print(second_matrix);
    matrix_transp(&second_matrix);
    matrix_print(second_matrix);
    matrix_print(third_matrix);
    matrix_clone(&third_matrix, second_matrix);
    matrix_print(third_matrix);

}