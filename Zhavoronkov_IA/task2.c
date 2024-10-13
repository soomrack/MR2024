#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>


struct Matrix
{
    size_t cols;
    size_t rows;
    double* data;
};
struct Matrix A;
struct Matrix B;
struct Matrix C;
struct Matrix C2;


void error()
{
	printf("Error!\n");
}


int matrix_print(const struct Matrix A)
{
    printf("_____________________________\n");
    for (size_t row = 0; row < A.rows; ++row) {
        for (size_t col = 0; col < A.cols; ++col) {
            printf(" %4.2f ", A.data[A.cols*row + col]);
        }
        printf("\n");
    }
    printf("\n");
    return 0;
}


// Выделение памяти
void malloc_give (struct Matrix* A)
{
    A->data = malloc(A->cols * A->rows * sizeof(double));
}


// Проверка на успешное выделение памяти
int malloc_check (const struct Matrix* A)
{
    if (A->data == NULL) {
        error();
        printf("haha");
        return 1; 
    }
    return 0;
}


// A = A + B
int matrix_add (struct Matrix A, struct Matrix B)
{
    if ((A.cols != B.cols) || (A.rows != B.rows )) {
        error();
        return 1;
    }

    for (size_t idx = 0; idx < A.cols * A.rows; ++idx) {
		A.data[idx] += B.data[idx];
	}
    return 0;
}


// C = A + B
struct Matrix matrix_sum(const struct Matrix A, const struct Matrix B)
{
    if ((A.cols != B.cols) || (A.rows != B.rows )) {
        error();
        exit;
    }

    struct Matrix C;
    C.cols = A.cols;
    C.rows = A.rows;
    malloc_give(&C);

    if (malloc_check(&C)) {
        exit;
    } 

    memcpy(C.data, A.data, C.cols * C.rows * sizeof(double));
    matrix_add(C, B); 
    return C;
}


// C = A - B 
struct Matrix matrix_min(const struct Matrix A, const struct Matrix B)
{
    if ((A.cols != B.cols) || (A.rows != B.rows )) {
        error();
        exit;
    }

    struct Matrix C;
    C.cols = A.cols;
    C.rows = A.rows;
    malloc_give(&C);

    if (malloc_check(&C)) {
        exit;
    }
    
    for (size_t idx = 0; idx < A.cols * A.rows; ++idx) {    
        C.data[idx] = A.data[idx] - B.data[idx];
    }
    return C;
}


// A = A * k
void matrix_k (struct Matrix A, double k)
{   
    malloc_check(&A);
    for (size_t idx = 0; idx < A.cols * A.rows; ++idx) {
		A.data[idx] *= k;
	}
}


int main()
{
    A.cols = 3;
    A.rows = 3;
    B.cols = 3;
    B.rows = 3;
    
    malloc_give(&A);
    malloc_give(&B);
    
    if (malloc_check(&A) || malloc_check(&B)) {
        return 1; // Завершение программы в случае ошибки
    } 

    *(A.data) = 1;
    A.data [1] = -2;
    *(A.data + 2) = 6.0;
    A.data[5] = 4.0;

    B.data[3] = 2.0;

    matrix_print(A);
    matrix_print(B);
    printf("______________\n");
    matrix_add(A,B);
    matrix_print(A);

    printf("______________\n");
    printf("______________\n");
    C = matrix_sum(A,B);
    matrix_print(C);

    C2 = matrix_min(A,B);
    matrix_print(C2);

    matrix_k(A,10);
    matrix_print(A);

    free(A.data);
    free(B.data);
    free(C.data);
    free(C2.data);
}


