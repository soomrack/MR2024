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
typedef struct Matrix Matrix;


enum MatrixExceptionLevel {ERROR, WARNING, INFO, DEBUG};


const struct Matrix MATRIX_NULL = {0, 0, NULL};


// Сообщение об ошибке
void matrix_error(const enum MatrixExceptionLevel level, const char* location, const char* msg)
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


Matrix matrix_alloc(const size_t rows, const size_t cols)
{
    if (cols == 0 || rows == 0) {
        matrix_error(INFO, "matrix_alloc", "matrix ix0 or 0xj");
        return (Matrix){rows, cols, NULL};
    }
    
    if (SIZE_MAX / cols / rows / sizeof(double)) {
        matrix_error(ERROR, "matrix_alloc", "Rows or cols is greater than size_t");
        return MATRIX_NULL;
    }  
    
    Matrix A = {rows, cols, NULL};

    A.data = malloc(A.cols * A.rows * sizeof(double));

    if (A.data == NULL) {
        matrix_error(ERROR, "matrix_alloc", "no memory allocated");
        return MATRIX_NULL;
    }
    return A;
}


void matrix_free(struct Matrix *A)
{
    if (A == NULL) {
        return;
    }
    
    free(A->data);
    *A = MATRIX_NULL;
}


// B := A
void matrix_copy(const Matrix B, const Matrix A) {
    
    if ((A.cols != B.cols) || (A.rows != B.rows )) {
        matrix_error(ERROR, "matrix_copy", "matrices of different dimensions");
        return;
    }

    if (B.data == NULL) {
        matrix_error(ERROR, "matrix_copy", "no memory allocated");
        return;
    }

    memcpy(B.data, A.data, A.cols * A.rows * sizeof(double));
}


void matrix_set_zero(const Matrix A)
{
    memset(A.data, 0, A.cols * A.rows * sizeof(double));
}


void matrix_set_identity(const Matrix A)
{   
    if (A.data == NULL) {
        matrix_error(ERROR, "matrix_set_idenety", "no memory allocated");
    }
    
    matrix_set_zero(A);
    for (size_t idx = 0; idx < A.rows * A.cols; idx += A.cols + 1) {
        A.data[idx] = 1.0;
    }
}


void matrix_print(const Matrix A)
{
    printf("\n_____________________________\n");
    for (size_t row = 0; row < A.rows; ++row) {
        for (size_t col = 0; col < A.cols; ++col) {
            printf("%.2f ", A.data[row * A.cols + col]);
        }
        printf("\n");
    }
    printf("\n");
}


// A += B
void matrix_add(const Matrix A, const Matrix B)
{
    if ((A.cols != B.cols) || (A.rows != B.rows )) {
        matrix_error(ERROR, "A = A + B", "matrices of different dimensions");
        return;
    }
    
    for (size_t idx = 0; idx < A.cols * A.rows; ++idx) {
        A.data[idx] += B.data[idx];
    }
}


// C = A + B
void matrix_sum(const Matrix C, const Matrix A, const Matrix B)
{
    if ((A.cols != B.cols) || (A.rows != B.rows )) {
        matrix_error(ERROR, "C = A + B", "matrices of different dimensions");
        return;
    }

    matrix_copy(C,A);
    matrix_add(C,B); 
}


// C = A - B 
void matrix_sub(const Matrix C, const Matrix A, const Matrix B)
{
    if ((A.cols != B.cols) || (A.rows != B.rows )) {
        matrix_error(ERROR,"C = A - B", "matrices of different dimensions");
        return;
    }

    if ((C.cols != A.cols) || (C.rows != A.rows)) {
        matrix_error(ERROR,"C = A - B", "matrix C has different dimensions");
        return;
    }
    
    for (size_t idx = 0; idx < A.cols * A.rows; ++idx) {
        C.data[idx] = A.data[idx] - B.data[idx];
    }
}


// A *= k
void matrix_mult_k(const Matrix A, const double k)
{   
    for (size_t idx = 0; idx < A.cols * A.rows; ++idx) {
        A.data[idx] *= k;
    }
}


// C = A * B
void matrix_mult(const Matrix C, const Matrix A, const Matrix B)
{
    if (A.cols != B.rows) {
        matrix_error(ERROR,"C = A * B" ,"incorrect matrix size");
        return;
    }

    if (C.rows != A.rows || C.cols != B.cols) {
        matrix_error(ERROR, "C = A * B", "incorrect matrix size");
        return;
    }

    for (size_t row = 0; row < C.rows; ++row) {
        for (size_t col = 0; col < C.cols; ++col) {
            C.data[row * (C.cols) + col] = 0;
                
            for (size_t idx = 0; idx < A.cols; idx++) {
                  C.data[row * C.cols + col] += A.data[row * A.cols + idx] * B.data[idx * B.cols + col];
            }
        }
    }
}


// B = A ^ T
void matrix_transp(const Matrix B, const Matrix A)
{
    if ((A.cols != B.cols) || (A.rows != B.rows )) {
        matrix_error(ERROR, "B = A ^ T", "matrices of different dimensions");
        return;
    }
    
    for (size_t row = 0; row < B.rows; ++row) {
        for (size_t col = 0; col < B.cols; ++col) {
            B.data[row + A.rows * col] = A.data[col + A.cols * row];
        }
    }
}


// determinant
double matrix_det(const Matrix A)
{
    double det;
    if (A.rows != A.cols) {
        matrix_error(WARNING, "matrix_det", "a non-square matrix");
        return NAN;
    }

    if (A.rows == 0) {
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
        det = A.data[0] * A.data[4] * A.data[8] 
        + A.data[1] * A.data[5] * A.data[6] 
        + A.data[3] * A.data[7] * A.data[2] 
        - A.data[2] * A.data[4] * A.data[6] 
        - A.data[3] * A.data[1] * A.data[8] 
        - A.data[0] * A.data[5] * A.data[7];
        return det;
    }
    return NAN;
}


// B = A ^ n
int matrix_pow(const Matrix B, const Matrix A, const unsigned int targ_power)
{
    if (A.rows != A.cols) {
        matrix_error(WARNING, "matrix_pow", "a non-square matrix");
        return 1;
    }
    
    if ((A.cols != B.cols) || (A.rows != B.rows )) {
        matrix_error(ERROR, "B = A ^ n", "matrices of different dimensions");
        return 1;
    }

    if (B.data == NULL) {
        return 0;
    }
    
    if (targ_power == 0) {
        matrix_copy(B,A);
        matrix_set_identity(B);
        return 1; 
    }

    Matrix tmp = matrix_alloc(A.rows, A.cols);
    if (tmp.data == NULL) {
        matrix_error(ERROR, "B = A ^ n", "no memory allocated");
        return 1;
    }

    matrix_copy(B,A);

    for (unsigned int pow = 1; pow < targ_power; ++pow) {
        matrix_mult(tmp, B, A);
        matrix_copy(B, tmp);
    }
    
    matrix_free(&tmp);
}


// e ^ A
int matrix_exp(const Matrix E, const Matrix A, const unsigned int targ_num)
{
    if (A.rows != A.cols) {
        matrix_error(ERROR, "matrix_pow", "a non-square matrix");
        return 1;
    }

    if (E.data == NULL) {
        return 0;
    }

    matrix_set_identity(E);

    if (targ_num == 1) {
        return 1;
    }

    Matrix tmp = matrix_alloc(A.rows, A.cols);
    Matrix buf_mult = matrix_alloc(A.rows, A.cols);

    if (tmp.data == NULL || buf_mult.data == NULL) {
        matrix_free(&tmp);
        matrix_free(&buf_mult);

        matrix_error(ERROR, "B = e ^ A", "no memory allocated");
        return 1;
    }

    matrix_copy(tmp,E);

    for (size_t num = 1; num < targ_num; ++num) { 

        matrix_mult(buf_mult, tmp, A);
        matrix_mult_k(buf_mult, 1.0/num);

        matrix_add(E, buf_mult);
        matrix_copy(tmp, buf_mult);
    }

    matrix_free(&tmp);
    matrix_free(&buf_mult);
    
}


int main()
{
    struct Matrix A,C,C2,B,D,E,F,T,I,P;
    A = matrix_alloc(3,2);
    A.data[0] = 0.0;
    A.data[1] = 2.0;
    A.data[2] = -5.0;
    A.data[3] = 1.0;
    A.data[4] = 4.0;
    A.data[5] = -3.0;

    B = matrix_alloc(2,2);
    C = matrix_alloc(2,2);
    D = matrix_alloc(2,2);
    F = matrix_alloc(2,2);
    E = matrix_alloc(2,2);


    for (size_t idx = 0; idx < B.rows * B.cols; ++idx) {
        B.data[idx] = idx;
    }

    matrix_print(B);
    matrix_copy(C,B);
    printf("\nwww\n");
    //matrix_add(C,B);
    matrix_print(C);
    
    matrix_mult(D,B,C);
    printf("\nMatrix D = B * C is\n");
    matrix_print(D);

    matrix_pow(F,B,5);
    printf("\nMatrix F = B^5 is\n");
    matrix_print(F); 

    matrix_exp(E,B,4);
    printf("\nMatrix E  ^ A is\n");
    matrix_print(E);



    /*printf("\nIdenity is\n");
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

    printf("\nEnd\n");*/

    


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
