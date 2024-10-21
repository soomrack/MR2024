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
//Сложение матриц +
//Вычитание матриц + 
//Умножение матриц +
//Умножение матрицы на число +
//Вывод матрицы
//Транспонирование матрицы 
//Деление матриц
//Деление матрицы на число + 
//Экспонента матрицы
//Степень матрицы
//Нахождение определителя матрицы
//Копирование матрицы


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


void matrix_sum(Matrix *result, Matrix A, Matrix B)
{
    if (A.rows == B.rows & A.cols == B.cols) {
        for (size_t i = 0; i < A.rows * A.cols; i++) {
            result->data[i] = A.data[i] + B.data[i];   
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
        matrix_int(&result, A.rows, B.cols);
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


void matrix_sub(Matrix *result, Matrix A, Matrix B)
{
    if (A.rows == B.rows & A.cols == B.cols) {
        for (size_t i = 0; i < A.rows * A.cols; i++) {
            result->data[i] = A.data[i] - B.data[i];   
        }
    }
}


int main()
{

}