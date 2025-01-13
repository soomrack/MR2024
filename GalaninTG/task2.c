#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


struct Matrix 
{
    size_t rows;
    size_t cols;
    double *data;
};

typedef struct Matrix Matrix;

enum MatrixExceptionLevel {ERROR, WARNING, INFO, DEBUG};

const Matrix MATRIX_NULL = {0, 0, NULL};


void matrix_exception(const enum MatrixExceptionLevel level, const char *msg)
{
    if(level == ERROR) {
        printf("ERROR: %s", msg);
    }

    if(level == WARNING) {
        printf("WARNING: %s", msg);
    }
}

Matrix matrix_allocate(const size_t rows, const size_t cols) {
    Matrix A;

    if (rows == 0  || cols == 0) { 
        matrix_exception(WARNING, "Матрица содержит 0 элементов!\n");
        return (Matrix){rows, cols, NULL};
    }

    if (SIZE_MAX / cols < rows) { 
        matrix_exception(ERROR, "Число строк или столбцов равно 0\n");
        return (Matrix){0, 0, NULL}; 
    }

    if (SIZE_MAX / (cols * sizeof(double)) < rows) { 
        matrix_exception(ERROR, "Не хватит места для выделения памяти под строки и столбцы\n");
        return (Matrix){0, 0, NULL}; 
    }

    A.data = (double*)malloc(rows * cols * sizeof(double));

    if (A.data == NULL) {
        matrix_exception(WARNING, "Сбой выделения памяти!");
        return MATRIX_NULL;
    }

    A.rows = rows;
    A.cols = cols;
    return A;
}


void matrix_free(Matrix *A)
{
    if (A == NULL) {
        return;
    }

    free(A->data);
    *A = MATRIX_NULL;
}


void matrix_print(const Matrix M)
{
    for (size_t row = 0; row < M.rows; row++) {
        for (size_t col = 0; col < M.cols; col++) {
            printf("%.2f ", M.data[row * M.cols + col]);
        }
        printf("\n");
    }
    printf("\n");
}

Matrix matrix_add(const Matrix A, const Matrix B) 
{
    if (A.rows != B.rows || A.cols != B.cols) {
        matrix_exception(WARNING, "Размеры матриц не подходят для вычитания");
        return MATRIX_NULL;
    }

    Matrix C = matrix_allocate(A.rows, A.cols);

    for (size_t idx = 0; idx < C.rows * C.cols; idx++) {
        C.data[idx] = A.data[idx] + B.data[idx];
    }

    return C;
}

Matrix matrix_sub(const Matrix A, const Matrix B) 
{
    if (A.rows != B.rows || A.cols != B.cols) {
        matrix_exception(WARNING, "Размеры матриц не подходят для вычитания");
        return MATRIX_NULL;
    }

    Matrix C = matrix_allocate(A.rows, A.cols);

    for (size_t idx = 0; idx < C.rows * C.cols; idx++) {
        C.data[idx] = A.data[idx] - B.data[idx];
    }

    return C;
}


Matrix matrix_mult_number(const Matrix A, double k) 
{
    Matrix B = matrix_allocate(A.rows, A.cols);

    for (size_t idx = 0; idx < A.cols * A.rows; idx++) {
        B.data[idx] = A.data[idx] * k;
    }

    return B;
}


Matrix matrix_mult(const Matrix A, const Matrix B) 
{
    if (A.cols != B.rows) {
        matrix_exception(WARNING, "Размеры матриц не подходят для умножения");
        return MATRIX_NULL;
    }

    Matrix C = matrix_allocate(A.rows, B.cols);
    
    for (size_t row = 0; row < C.rows; row++) {
        for (size_t col = 0; col < C.cols; col++) {
            C.data[row * B.cols + col] = 0;
            for (size_t idx = 0; idx < A.cols; idx++) {
                C.data[row * C.cols + col] += A.data[row * A.cols + idx] * B.data[idx * B.cols + col];
            }
        }
    }

    return C;
}

Matrix matrix_trans(const Matrix A)
{
    Matrix B = matrix_allocate(A.cols, A.rows);

    for (size_t row = 0; row < B.rows; row++) {
        for (size_t col = 0; col < B.cols; col++) {
            B.data[col * B.cols + row] = A.data[row * A.cols + col];
        }
    }
    return B;
}


Matrix identity_matrix(size_t size) 
{
    Matrix E = {size, size, (double*)malloc(size * size * sizeof(double))};

    if (E.data == NULL) {
        matrix_exception(WARNING, "Ошибка выделения памяти на единичную матрицу!\n");

        return (Matrix){0, 0, NULL};
    }

    for (size_t row = 0; row < E.rows; row++) {
        for (size_t col = 0; col < E.cols; col++) {
            E.data[row * size + col] = (row == col) ? 1.0 : 0.0;
        }
    }

    return E;
}


Matrix matrix_pow(Matrix A, const unsigned int power)
{
    if (A.rows != A.cols) {
        matrix_exception(WARNING, "Размеры матрицы не подходят для возведения в степень");
        return MATRIX_NULL;
    }

    Matrix B = identity_matrix(A.rows);

    if (A.data==0) {
        matrix_exception(WARNING, "Память на матрицу не выделилась\n");
        return MATRIX_NULL;
    }
    
    for (unsigned int n = 0; n < power; n++) {
        Matrix C = matrix_mult(B, A);
        matrix_free(&B);
        B = C;
    }

    return B;
}


double determinant(const Matrix A)
{
    if (A.rows != A.cols) {
        matrix_exception(WARNING, "Размеры матрицы не подходят для вычисления определителя");
        return NAN;
    }

    if (A.rows == 1) {
        return A.data[0];
    }
    
    if (A.rows == 2) {
        return A.data[0] * A.data[3] - A.data[1] * A.data[2];
    }

    if (A.rows == 3) {
        return A.data[0] * A.data[4] * A.data[5] +
               A.data[1] * A.data[5] * A.data[6] +
               A.data[2] * A.data[3] * A.data[7] -
               A.data[2] * A.data[4] * A.data[6] -
               A.data[1] * A.data[3] * A.data[8] -
               A.data[0] * A.data[5] * A.data[7];
    }
    matrix_exception(WARNING, "I do not want to do it");
    return NAN;
}


Matrix matrix_exp(const Matrix A, const unsigned int accuracy) 
{
    if (A.rows != A.cols) {
        matrix_exception(WARNING, "Размеры матрицы не подходят для вычисления экспоненты");
        return MATRIX_NULL;
    }

    double fact = 1.0;

    Matrix B = identity_matrix(A.rows);

    for (int n = 1; n <= accuracy; n++) {
        fact *= n;
        Matrix P = matrix_pow(A, n);
        Matrix M = matrix_mult_number(P, 1/fact);
        matrix_free(&P);
        Matrix C = matrix_add(B, M);
        matrix_free(&B);
        B = C;
        matrix_free(&M);
    }

    return B;
}


void matrix_set(const Matrix A, const double *values)
{
    memcpy(A.data, values, A.rows * A.cols * sizeof(double));
}


int main()
{
    double data_A[4] = {1, 2, 3, 4};
    double data_B[4] = {1, 2, 3, 4};

    Matrix A = matrix_allocate(2, 2);
    Matrix B = matrix_allocate(2, 2);

    matrix_set(A, data_A);
    matrix_set(B, data_B);

    if (A.data == NULL || B.data == NULL) {
        printf("Ошибка выделения памяти для одной из матриц.n");
        return 1; 
    }

    printf("A=\n");
    matrix_print(A);
    printf("B=\n");
    matrix_print(B);

    // Сложение
    printf("Addition:\n");
    matrix_print(matrix_add(A, B));

    // Вычитание
    printf("Subtraction:\n");
    matrix_print(matrix_sub(A, B));

    // Умножение матриц
    printf("Multiplication:\n");
    matrix_print(matrix_mult(A, B));

    // Траспонирование
    printf("Transposition:\n");
    matrix_print(matrix_trans(A));

    // Возведение в степень
    int pow = 3;
    printf("Power %d:\n", pow);
    matrix_print(matrix_pow(A, pow));

    // Умножение на число
    double k = 5;
    printf("A mult %2.f:\n", k);
    matrix_print(matrix_mult_number(A, k));
    
    // Определитель
    printf("Determinant A: %2.f \n", determinant(A));
    
    // Экспонента
    printf("Exp A:\n");
    matrix_print(matrix_exp(A, 10));

    matrix_free(&A);
    matrix_free(&B);

}
