#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <math.h>


struct Matrix {
    size_t rows;
    size_t cols;
    double* data;
};

const struct Matrix MATRIX_NULL = {0, 0, NULL};

enum MatrixExceptionLevel {ERROR, WARNING, INFO, DEBUG};


// Сообщение об ошибке
void matrix_error (const enum MatrixExceptionLevel level, const char* location, const char* msg)
{
    if (level == ERROR) {
        printf("\nERROR\nLoc: %s\nText: %s\n", location, msg);
    }

    if (level == WARNING) {
        printf("\nWARNING\nLoc: %s\nText: %s\n", location, msg);
    }

    if (level == INFO) {
        printf("\nINFO\nLoc: %s\nText: %s\n", location, msg);
    }

    if (level == DEBUG) {
        printf("\nDEBUG\nLoc: %s\nText: %s\n", location, msg);
    }

}


// Инициализация матрицы
struct Matrix matrix_alloc (const size_t rows, const size_t cols)
{
    if (cols == 0 || rows == 0) {
        matrix_error(1, "matrix_alloc", "matrix ix0 or 0xj");
        return (struct Matrix){rows, cols, NULL};
    }

    if (rows * cols >= SIZE_MAX) {
        matrix_error(0, "matrix_alloc", "rows * cols >= SIZE_MAX");
        return MATRIX_NULL;
    }

    if (rows * cols * sizeof(double) >= SIZE_MAX) {
        matrix_error(0, "matrix_alloc", "rows * cols * sizeof(double) >= SIZE_MAX");
        return MATRIX_NULL;
    }
    
    struct Matrix A = {rows, cols, NULL};

    A.data = malloc(A.cols * A.rows * sizeof(double));

    if (A.data == NULL) {
        matrix_error(0, "matrix_alloc", "no memory allocated");
        return MATRIX_NULL;
    }
    return A;
}


// Освобождение памяти
void matrix_free (struct Matrix *A)
{
    free(A->data);
    *A = MATRIX_NULL;
}


// Нулевая матрица
void matrix_set_zero (const struct Matrix A)
{
    if (A.data == NULL) {
        return;
    }

    memset(A.data, 0, A.cols * A.rows * sizeof(double));
}


// Единичная матрица
struct Matrix matrix_set_identity (const struct Matrix A)
{  
    if (A.data == NULL) {
        return A;
    }

    struct Matrix B = matrix_alloc (A.rows, A.cols);
    
    matrix_set_zero(B);
    for (size_t idx = 0; idx < B.rows * B.cols; idx += B.cols + 1) {
        B.data[idx] = 1.0;
    }
    return B;
}


void matrix_print(const struct Matrix A)
{
    printf("_____________________________\n");
    for (size_t row = 0; row < A.rows; ++row) {
        for (size_t col = 0; col < A.cols; ++col) {
            printf("%.2f ", A.data[row * A.cols + col]);
        }
        printf("\n");
    }
    printf("\n");
}


// A = A + B
void matrix_add (const struct Matrix A, const struct Matrix B)
{
    if ((A.cols != B.cols) || (A.rows != B.rows )) {
        matrix_error(1,"A = A + B" ,"matrices of different dimensions");
        return;
    }
    
    for (size_t idx = 0; idx < A.cols * A.rows; ++idx) {
        A.data[idx] += B.data[idx];
    }
}


// C = A + B
struct Matrix matrix_sum(const struct Matrix A, const struct Matrix B)
{
    if ((A.cols != B.cols) || (A.rows != B.rows )) {
        matrix_error(1,"C = A + B" ,"matrices of different dimensions");
        return;
    }

    struct Matrix C = matrix_alloc (A.rows, A.cols);

    if (C.data == NULL) {
        return C;
    }

    memcpy(C.data, A.data, C.cols * C.rows * sizeof(double));
    matrix_add(C, B); 
    return C;
}


// C = A - B 
struct Matrix matrix_sub(const struct Matrix A, const struct Matrix B)
{
    if ((A.cols != B.cols) || (A.rows != B.rows )) {
        matrix_error(1,"C = A - B", "matrices of different dimensions");
        return;
    }

    struct Matrix C = matrix_alloc (A.rows, A.cols);

    if (C.data == NULL) {
        return C;
    }
    
    for (size_t idx = 0; idx < A.cols * A.rows; ++idx) {
        C.data[idx] = A.data[idx] - B.data[idx];
    }
    return C;
}


// A = A * k
void matrix_mult_k (const struct Matrix A, const double k)
{   
    if (A.data == NULL) {
        return;
    }

    for (size_t idx = 0; idx < A.cols * A.rows; ++idx) {
        A.data[idx] *= k;
    }
}


// C = A * B
struct Matrix matrix_mult (const struct Matrix A, const struct Matrix B)
{
    if (A.cols != B.rows) {
        matrix_error(1,"C = A * B" ,"incorrect matrix size");
        return;
    }

    struct Matrix C = matrix_alloc(A.rows, B.cols);

    if (C.data == NULL) {
        return C;
    }

    for (size_t rowA = 0; rowA < A.rows; ++rowA) {
        for (size_t colB = 0; colB < B.cols; ++colB) {
            C.data[rowA * (B.cols) + colB] = 0;
                
            for (size_t idx = 0; idx < A.cols; idx++) {
                  C.data[rowA * B.cols + colB] += A.data[rowA * A.cols + idx] * B.data[idx * B.cols + colB];
            }
        }
    }
    return C;
}


struct Matrix matrix_transp(const struct Matrix A)
{
    struct Matrix B = matrix_alloc(A.cols, A.rows);

    if (B.data == NULL) {
        return B;
    }

    for (size_t row = 0; row < A.rows; ++row) {
        for (size_t col = 0; col < A.cols; ++col) {
            B.data[row + A.rows * col] = A.data[col + A.cols * row];
        }
    }
    return B;
}


double matrix_det (const struct Matrix A)
{
    double det;
    if (A.rows != A.cols) {
        matrix_error(1, "matrix_det", "a non-square matrix");
        return NAN;
    }

    if (A.rows == 1) {
        det = A.data[0];
        return det;
    }

    if (A.rows == 2) {
        det = A.data[0] * A.data[3] - A.data[1] * A.data[2];
        return det;
    }

    if (A.rows == 3) {
        det = A.data[0] * A.data[4] * A.data[8] + A.data[1] * A.data[5] * A.data[6] + A.data[3] * A.data[7] * A.data[2] - \
        (A.data [2] * A.data[4] * A.data[6]) - A.data[3] * A.data[1] * A.data[8] - A.data[0] * A.data[5] * A.data[7];
        return det;
    }
    return NAN;
}


struct Matrix matrix_pow(const struct Matrix A, const unsigned int power)
{
    if (A.rows != A.cols) {
        matrix_error(1, "matrix_pow", "a non-square matrix");
        return MATRIX_NULL;
    }

    struct Matrix B = matrix_alloc (A.rows, A.cols);

    if (power == 0) {
        B = matrix_set_identity(A);
        return B; 
    }

    if (B.data == NULL) {
        return B;
    }

    memcpy(B.data, A.data, A.rows * A.cols * sizeof(double));

    for (unsigned int cur_pow = 1; cur_pow < power; ++cur_pow) {
        struct Matrix tmp = matrix_mult (B,A);
        memcpy(B.data, tmp.data, tmp.rows * tmp.cols * sizeof(double));
        matrix_free(&tmp);
    }
    return B;
}


double factorial (const unsigned int num)
{
    unsigned long long int res = 1;
    for (unsigned int idx = 1; idx <= num; idx++) {
        res *= idx;
    }

    return res;
}


// e ^ A
struct Matrix matrix_exp(const struct Matrix A, const unsigned int num)
{
    if (A.rows != A.cols) {
        matrix_error(WARNING, "matrix_pow", "a non-square matrix");
        return MATRIX_NULL;
    }

    struct Matrix E = matrix_alloc(A.rows, A.cols);   
  
    if (E.data == NULL) {
        return E;
    }

    E = matrix_set_identity(A);

    if (num == 1) {
        return E;
    }
    
    for (size_t cur_num = 1; cur_num < num; ++cur_num) {
        struct Matrix tmp = matrix_pow (A,cur_num);
        matrix_mult_k(tmp, 1/factorial(cur_num));
        
        struct Matrix buf = matrix_sum(E, tmp);
        memcpy(E.data, buf.data, buf.rows * buf.cols * sizeof(double));
        
        matrix_free(&tmp);
        matrix_free(&buf);
        }
    
    return E;
}



int main()
{
    printf("1");
    printf("adfsd\n");
    struct Matrix A,C,C2,B,D,E,F,T,I,P;
    A = matrix_alloc(3,2);
    A.data[0] = 0.0;
    A.data[1] = 2.0;
    A.data[2] = -5.0;
    A.data[3] = 1.0;
    A.data[4] = 4.0;
    A.data[5] = -3.0;

    B = matrix_alloc(2,2);

    for (size_t idx = 0; idx < B.rows * B.cols; ++idx) {
        B.data[idx] = idx;
    }

    printf("\nIdenity is\n");
    matrix_print(A);
    I = matrix_set_identity(A);
    matrix_print(I);
    matrix_print(A);


    printf("\nPow is\n");
    matrix_print(B);
    P = matrix_pow(B,5);
    matrix_print(P);

    printf("\ne^A\n");
    E = matrix_exp(B,3);
    matrix_print(E);

    matrix_pow(A,2);

    printf("\nEnd\n");

    


    /*matrix_print(A);
    matrix_print(B);
    printf("______________\n");
    matrix_add(A,B);
    matrix_print(A);
    printf("A = A + B");


    printf("______________\n");
    printf("______________\n");
    C = matrix_sum(A,B);
    matrix_print(C);
    printf("C = A + B");


    C2 = matrix_sub(A,B);
    matrix_print(C2);
    printf("C = A - B");


    matrix_mult_k(A,10);
    matrix_print(A);
    printf("A * K is done\n");


    matrix_print(B);
    D = matrix_pow(B,4);
    matrix_print(D); 
    printf("D ^ 4");


    printf("Matrix A det is %.2f", matrix_det(A));

    printf("\nD matrix is\n");
    F = matrix_mult(B,B);
    matrix_print(F);
    
    matrix_print(A);
    T = matrix_transp(A);
    matrix_print(T);
    printf("A ^ T is done\n");

    E = matrix_exp(B,2);
    matrix_print(E);

    I = matrix_alloc(3,3);
    matrix_zero(I);
    matrix_identity(I);
    matrix_print(I);
    printf("Goood");*/

    matrix_free(&A);
    matrix_free(&B);
    matrix_free(&C);
    matrix_free(&C2);
    matrix_free(&D);
    matrix_free(&F);
    matrix_free(&T);
    matrix_free(&E);
    matrix_free(&I);
    matrix_free(&P);
}
