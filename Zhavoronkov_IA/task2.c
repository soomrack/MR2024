#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


struct Matrix
{
    size_t cols;
    size_t rows;
    double* data;
};
struct Matrix A;
struct Matrix B;


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


// A = A + B
int matrix_add (struct Matrix A, struct Matrix B)
{
    if (!((A.cols == B.cols) && (A.rows == B.rows ))) {
        error();
        return 1;
    }

    for (size_t idx = 0; idx < A.cols * A.rows; ++idx) {
		A.data[idx] += B.data[idx];
	}
    return 0;
}


// Выделение памяти
void malloc_give (struct Matrix* A)
{
    A->data = malloc(A->cols*A->rows*sizeof(double));
    //return A.data;
}


// Проверка на успешное выделение памяти
int malloc_check (const struct Matrix A)
{
    if (A.data == NULL) {
        error();
        return 1; 
    }
    return 0;
}


int main()
{
    A.cols = 3;
    A.rows = 3;
    B.cols = 3;
    B.rows = 3;

    malloc_give(&A);
    malloc_give(&B);

    malloc_check(A);
    malloc_check(B);
    

    *(A.data) = 1;
    A.data [1] = 2;
    *(A.data + 2) = 6.0;
    A.data[5] = 4.0;

    B.data[3] = 2.0;

    matrix_print(A);
    matrix_print(B);
    printf("______________\n");
    matrix_add(A,B);
    matrix_print(A);

    free(A.data);
    free(B.data);
}


