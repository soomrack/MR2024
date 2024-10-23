#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


struct Matrix {
    size_t rows;
    size_t cols;
    double* data;
};

struct Matrix memory_allocation(const size_t cols, const size_t rows) { // Выделение памяти
    struct Matrix A = { rows,cols,NULL };
    if (cols == 0 || rows == 0) return A;
    if (SIZE_MAX / A.cols / A.rows / sizeof(double) == 0) return A;
    A.data = (double*)malloc(A.rows * A.cols * sizeof(double));
    if (A.data == NULL) {
        printf("?Error_allocation?\n");
    }
    return A;
}

void output(const struct Matrix A) { // Блок вывода
    for (size_t col = 0; col < A.cols; ++col){ 
        printf("| ");
        for (size_t row = 0; row < A.rows; ++row)
        {

            printf("%lf ", A.data[row * A.cols + col]);
        }
        printf("|\n");
    }
}


void free_matrix(struct Matrix *A) { // Освобождение памяти
    if (A == NULL) return;
    free(A->data);
}


void matrix_random(Matrix A) { // Введение случайных значений в матрицу
    for (size_t idx = 0; idx < A.cols * A.rows; idx++) {
        A.data[idx] = rand() % 10;
    }
}


int matrix_zero(Matrix C) { //Создание нулевой матрицы
    for (size_t idx = 0; idx < C.rows * C.cols; idx++) {
        C.data[idx] = 0.0;
    }
    return 0;
}


int matrix_one(const struct Matrix C) {// Создание единичной матрицы 
    if (C.rows != C.cols) {
        printf("Error_one\n");
        return 1;
    }
    for (size_t idx = 0; idx < C.rows * C.cols; idx++) {
        C.data[idx] = 0.0;
        for (size_t idx = 0; idx < C.rows * C.cols; idx += C.rows + 1) C.data[idx] = 1.0;
    }
    return 0;
}


int matrix_add(const struct Matrix A, const struct Matrix B) { // Сложение матриц
    if (A.rows != B.rows || A.cols != B.cols) {
        printf("Error_add\n");
        return 1;
    }
    for (size_t idx = 0; idx < A.rows * A.cols; ++idx) {
        A.data[idx] += B.data[idx];
    }
    return 0;
}


int matrix_sub(const struct Matrix A, const struct Matrix B) { // Вычитание матриц
    if (A.rows != B.rows || A.cols != B.cols) {
        printf("Error_sub\n");
        return 1;
    }
        for (size_t idx = 0; idx < A.rows * A.cols; ++idx) {
            A.data[idx] -= B.data[idx];
        }
        return 0;
}


int matrix_multiplication(const struct Matrix A, const struct Matrix B, const struct Matrix multi ) {// умножение матриц
    if (A.cols != B.rows) {
        printf("Error_multiplication\n");
        return 1;
    }
    for (size_t col = 0; col < multi.cols; col++) {
        for (size_t row = 0; row < multi.rows; row++) {
            double sum = 0;
            for (size_t idx = 0; idx < A.rows ; idx++) {
                sum += A.data[idx * A.cols + col] * B.data[row * B.cols + idx];
        }
            multi.data[row * multi.cols + col] = sum;
        }
    }
    return 0;
}


int matrix_multiplication_x(const Matrix A, const double x) { // умножение матрицы на число
    for (size_t idx = 0; idx < A.cols * A.rows; ++idx) {
        A.data[idx] *= x;
    }
    return 0;
}


int matrix_div_x(const Matrix A, const double x) { // деление матрицы на число
    for (size_t idx = 0; idx < A.cols * A.rows; ++idx) {
        A.data[idx] /= x;
    }
    return 0;
}

int matrix_transposition(const Matrix A, const Matrix D) {  // транспонирование матрицы 
    for (size_t col = 0; col < D.cols; ++col) {
        for (size_t row = 0; row < D.rows; ++row) {
            D.data[row * D.cols + col] = A.data[col * D.rows + row];
        }
    }
    return 0;
}

/*
int matrix_det(const Matrix A) {
    if (A.cols != A.rows) {
        printf("Error_det\n");
        return 1;
    }
    for (siz_t col = 0; col < A.cols; ++col) {
        for (size_t row = 0; row < A.rows; ++row) {

        }
    }
}
*/

 


int main()
{
    struct Matrix A;
    A = memory_allocation(3, 2);
    matrix_random(A);
    printf("Matrix A\n");
    output(A);

    struct Matrix B;
    B = memory_allocation(2, 3);
    matrix_random(B);
    printf("Matrix B\n");
    output(B);

    struct Matrix C;
    int size_matrix_one = 4; //размер единичной матрицы
    C = memory_allocation(size_matrix_one, size_matrix_one);
    matrix_random(C);

    struct Matrix multi;
    multi = memory_allocation(B.rows, A.cols);
    matrix_multiplication(A, B, multi);
    printf("Matrix multiplication\n");
    output(multi);

    struct Matrix D;
    D = memory_allocation(A.rows, A.cols);
    matrix_transposition(A, D);
    printf("Matrix transposition\n");
    output(D);

    matrix_div_x(A, 2);
    printf("Matrix / x \n");
    output(A);

    matrix_multiplication_x(A, 2);
    printf("Matrix * x \n");
    output(A);

    matrix_add(A, B);
    printf("Matrix +\n");
    output(A);

    matrix_sub(A, B);
    printf("Matrix -\n");
    output(A);

    matrix_zero(A);
    printf("Matrix zero\n");
    output(A);

    matrix_one(C);
    printf("Matrix one\n");
    output(C);

   // matrix_det(A);
   // printf("Matrix det\n");



    free_matrix(&A);
    free_matrix(&B);
    free_matrix(&C);
    free_matrix(&D);
    free_matrix(&multi);
    return 0;
}