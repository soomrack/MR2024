#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
	srand(time(NULL));

    // A
    struct Matrix A = matrix_allocate(3, 3);
    matrix_fill_random(A);
    printf("Матрица A:\n");
    matrix_print(A);

    // B
    struct Matrix B = matrix_allocate(3, 3);
    matrix_fill_random(B);
    printf("Матрица B:\n");
    matrix_print(B);
    
    // A+B
    struct Matrix C = matrix_add(A, B);
    printf("Результат A + B:\n");
    matrix_print(C);
    matrix_free(&C);
    
    // A-B
    C = matrix_subtract(A, B);
    printf("Результат A - B:\n");
    matrix_print(C);
    matrix_free(&C);

	// A*k
    C = matrix_multiply_const(A, 2);
    printf("Результат A * k:\n");
    matrix_print(C);
    matrix_free(&C);

    // A*B
    C = matrix_multiply(A, B);
    printf("Результат A * B:\n");
    matrix_print(C);
    matrix_free(&C);
    
    // A^n
    C = matrix_pow(A, 2);
    printf("Результат A^n:\n");
    matrix_print(C);
    matrix_free(&C);
    
    // Транспонирование
    C = matrix_transpon(A);
    printf("Результат транспонирования:\n");
    matrix_print(C);
    matrix_free(&C);
    
    // Определитель
    double det = determinant(A);
    printf("Результат определителя:");
    printf("%lf \n", det);
    
    // Экспонента
    C = matrix_exp(A);
    printf("Результат exp(A):\n");
    matrix_print(C);
    matrix_free(&C);
    
    
    matrix_free(&A);
    matrix_free(&B);

    return 0;
}
