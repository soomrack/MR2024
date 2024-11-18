#include "matrix_lib.h"
#include <stdio.h>
#include <string.h>


int main() {
    Matrix A = matrix_allocate(3, 3);
    Matrix B = matrix_allocate(3, 3);
    
    if (A.data == NULL || B.data == NULL) {
        matrix_log(ERROR, __func__, "Сбой выделения памяти");
        return MAT_MEMORY_ERR;
    }

    double data_A[9] = {3, 1, 7, 0, 5, 7, 2, 5, 8};
    double data_B[9] = {5, 0, 8, 1, 9, 6, 3, 2, 1};

    memcpy(A.data, data_A, 9 * sizeof(double));
    memcpy(B.data, data_B, 9 * sizeof(double));

    printf("Матрица А:\n");
    matrix_print(A);
    printf("Матрица B:\n");
    matrix_print(B);

    Matrix result;
    if (matrix_sum(&result, A, B) == MAT_OK) {
        printf("Сумма матриц:\n");
        matrix_print(result);
        matrix_free(&result);
    }

    if (matrix_subtract(&result, A, B) == MAT_OK) {
        printf("Разность матриц:\n");
        matrix_print(result);
        matrix_free(&result);
    }

    if (matrix_multiply(&result, A, B) == MAT_OK) {
        printf("Произведение матриц:\n");
        matrix_print(result);
        matrix_free(&result);
    }

    double scalar = 2.5;
    if (matrix_scalar_multiply(&result, A, scalar) == MAT_OK) {
        printf("Умножение матрицы на %0.2f:\n", scalar);
        matrix_print(result);
        matrix_free(&result);
    }

    int power = 4;
    if (matrix_power(&result, A, 4) == MAT_OK) {
        printf("Возведение в %d степень матрицы A:\n", power);
        matrix_print(result);
        matrix_free(&result);
    }


    if (matrix_copy(&A, B) == MAT_OK) {
        printf("Копирование матриц:\n");
        printf("src:\n");
        matrix_print(A);
        printf("dest:\n");
        matrix_print(B);
    }
    

    int num = 10;
    if (matrix_exponent(&result, A, num) == MAT_OK) {
        printf("Экспонента матрицы А:\n");
        matrix_print(result);
        matrix_free(&result); 
    }
    matrix_free(&A);
    matrix_free(&B);
    return 0;
}