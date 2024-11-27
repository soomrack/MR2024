#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>


struct Matrix {
    size_t rows;
    size_t cols;
    double *data;
};
typedef struct Matrix Matrix;


enum MatrixExceptionLevel {ERROR, WARNING, INFO, DEBUG};


void matrix_exception(const enum MatrixExceptionLevel level, char *msg)
{
    if(level == ERROR) {
        printf("ERROR: %s", msg);
    }

    if(level == WARNING) {
        printf("WARNING: %s", msg);
    }

    if(level == INFO) {
        printf("INFO: %s", msg);
    }
}


const Matrix NULL_MATRIX = {0, 0, NULL};


Matrix matrix_allocate(const size_t rows, const size_t cols)
{
    Matrix A;

    if(rows == 0 || cols == 0) {
        matrix_exception(ERROR, "");
        return (Matrix){rows, cols, NULL};
    }
    
    size_t size = rows * cols;
    if (size / rows != cols) {
        matrix_exception(ERROR, " ");
        return NULL_MATRIX;
    }

    size_t size_in_bytes = rows * cols * sizeof(double);
    if (rows >= SIZE_MAX / sizeof(double) / cols) {
        matrix_exception(ERROR, " ");
        return NULL_MATRIX;
    }

    A.data = malloc(rows * cols * sizeof(double));
    if(A.data == NULL) {
        matrix_exception(ERROR, " ");
        return NULL_MATRIX;
    }

    A.rows = rows;
    A.cols = cols;
    return A;
}


void matrix_free(Matrix *A)
{
    if (A == NULL){
        matrix_exception(ERROR, "");
        return;
    }
    free(A->data);
    *A = NULL_MATRIX;
}


void matrix_set(const Matrix A, const double *values)
{
    memcpy(A.data, values, A.rows * A.cols * sizeof(double));
}


void matrix_print(const Matrix A)
{
    for(size_t row = 0; row < A.rows; ++row) {
        for(size_t col = 0; col < A.cols; ++col) {
            printf("%2.3f ", A.data[row * A.cols + col]);
        }
        printf("\n");
    }
    printf("\n");
}


// C = A + B
Matrix matrix_add(const Matrix A, const Matrix B)
{
    if( !((A.cols == B.cols) && (A.rows == B.rows)) ) {
        matrix_exception(ERROR, "");
        return NULL_MATRIX;
    }

    Matrix C = matrix_allocate(A.rows, A.cols);
    
    for(size_t idx = 0; idx < C.rows * C.cols; ++idx) {
        C.data[idx] = A.data[idx] + B.data[idx];
    }
    return C;
}


// C = A - B 
Matrix matrix_minus(const Matrix A, const Matrix B)
{
    if (!((A.cols == B.cols) && (A.rows == B.rows))) 
    {
        matrix_exception(WARNING, "");
        return NULL_MATRIX;
    }

    Matrix C = matrix_allocate(A.rows, A.cols);
    for (size_t idx = 0; idx < C.rows * C.cols; idx++) {
        C.data[idx] = A.data[idx] - B.data[idx];
    }
    return C;
}


// C = A * B 
Matrix matrix_umno(const Matrix A, const Matrix B)
{
    if (A.cols != B.cols) return NULL_MATRIX;

    Matrix C = matrix_allocate(A.rows, B.cols);
    if (C.data == NULL) return C;
    
    
    for (size_t row = 0; row < A.rows; ++row) {
        for (size_t col = 0; col < B.cols; ++col){
            C.data[row * C.cols + col] = 0;
            for (size_t k = 0; k < A.cols; k++) {
                 C.data[row * C.cols + col] += A.data[row * A.cols + k] * B.data[k * B.cols + col];
            }
        }
    }
    return C;
}


// A *= k
Matrix matrix_umnok(const Matrix A, const double k)
{
    Matrix C = matrix_allocate(A.rows, A.cols);
    if (C.data == NULL) return NULL_MATRIX;

    memcpy(C.data, A.data, sizeof(C) * A.cols * A.rows);


    for (size_t idx = 0; idx < A.cols * A.rows; ++idx) {
        C.data[idx] *= k;
    }
    return C;
}


// A /= k
Matrix matrix_delk(const Matrix A, double k)
{
    Matrix C = matrix_allocate(A.rows, A.cols);
    if (C.data == NULL) return NULL_MATRIX;

    memcpy(C.data, A.data, sizeof(C) * A.cols * A.rows);


    for (size_t idx = 0; idx < A.cols * A.rows; ++idx) {
        C.data[idx] /= k;
    }
    return C;
}


// A^T
Matrix matrix_transpo(const Matrix A)
{
    Matrix C = matrix_allocate(A.cols, A.rows);
    
    if (C.data == NULL) return NULL_MATRIX;
    for (size_t row = 0; row < A.rows; ++row) {
        for (size_t col = 0; col < A.cols; ++col) {
            C.data[col * C.cols + row] = A.data[row * A.cols + col];
        }
    }
    return C;
}


// det (A)
double matrix_det(const Matrix A)
{
    if (A.cols != A.rows) return NAN;

    if (A.cols == 0) return NAN;

    if (A.cols == 1) return A.data[0];

    if (A.cols == 2) {
        return A.data[0] * A.data[3] - A.data[1] * A.data[2];
    }

    if (A.cols == 3) {
        return A.data[0] * A.data[4] * A.data[8]
            + A.data[1] * A.data[5] * A.data[6]
            + A.data[2] * A.data[7] * A.data[3]
            - A.data[2] * A.data[4] * A.data[6]
            - A.data[0] * A.data[5] * A.data[7]
            - A.data[1] * A.data[8] * A.data[3];
    }
    return NAN;
}



// единичная матрица
Matrix matrix_one(const Matrix A)
{
   Matrix C = matrix_allocate(A.cols, A.rows);
     for (size_t row = 0; row < A.rows; row++){
        for (size_t col =0; col < A.cols; col++){
            if (C.cols = C.rows){
                C.data[col * A.cols + col] = 1;
            }
            else {
                C.data[col * A.cols + row] = 0;
            }
        }
    }
    return C;
}


// копирование матрицы
Matrix matrix_copy(const Matrix A)
{
    if (A.data == NULL) {
        matrix_exception(ERROR, " \n");
        return NULL_MATRIX;
    }

    Matrix copy = matrix_allocate(A.cols, A.rows);

    memcpy(copy.data, A.data, A.cols * A.rows * sizeof(double));
    return copy;
}


// Матрица в степени
Matrix matrix_pow(const Matrix A, int pow)  
{
    if (A.rows != A.cols) {
        matrix_exception(WARNING, "\n");
        return NULL_MATRIX;
    }

    if (pow == 0)
    {
        return matrix_one(A);
    }
    
    Matrix C = matrix_copy(A);
    Matrix C1 = matrix_allocate(A.cols, A.rows); 

    for (int n = 1; n < pow; n++) {
        C1 = matrix_umno(C, A);
        matrix_free(&C);
        C = matrix_copy(C1);
    }

    return C;
}

// факториал
int factorial(int n)
{
        if (n == 1) {
        return 1;
    }

    return n * factorial(n - 1);
}


Matrix matrix_exp(const Matrix A, int n)
{
    if (A.rows != A.cols) {
        matrix_exception(ERROR, " \n");
        return NULL_MATRIX;
    }

    if (A.cols == 0) return NULL_MATRIX;

    Matrix exp = matrix_allocate(A.rows, A.cols);
    Matrix exp_pow = matrix_allocate(A.rows, A.cols);
    Matrix one = matrix_one(A);

    if (exp.data == NULL) return NULL_MATRIX;

    if (n == 0) {
        return matrix_one(A);
    }

    for (int i = 1; i < n; i++) {
        exp_pow = matrix_pow(A, i);
        double fact_exp = 1 / factorial(i);

        for (size_t idx = 0; idx < exp.rows * exp.cols; ++idx) {
            exp.data[idx] += (exp_pow.data[idx] * fact_exp + one.data[idx]) ;
        }
        matrix_free(&exp_pow);        
    }

    return exp;
   
}



int main()
{
    Matrix A, B;
    A = matrix_allocate(2, 2);  
    B = matrix_allocate(2, 2);  
    matrix_set(A, (double[]){
            1., 2.,
            3., 4.          
        });
    matrix_set(B, (double[]){
            1., 0.,
            0., 1.
        });
    printf("A:\n");
    matrix_print(A);
    printf("B:\n");
    matrix_print(B);

    Matrix C1 = matrix_add(A, B);
    printf("Plus:\n");
    matrix_print(C1);

    Matrix C2 = matrix_minus(A, B);
    printf("Minus:\n");
    matrix_print(C2);

    Matrix C3 = matrix_umno(A, B);
    printf("*:\n");
    matrix_print(C3);

    const k = 100;
    Matrix C4 = matrix_umnok(A, k);
    printf("*k:\n");
    matrix_print(C4);

    Matrix C5 = matrix_delk(A, k);
    printf("/k:\n");
    matrix_print(C5);

    Matrix C6 = matrix_transpo(A);
    printf("Trans:\n");
    matrix_print(C6);

    printf("det A: %.2lf\n", matrix_det(A));

    Matrix C7 = matrix_exp(A, 4);
    printf("Exp:\n");
    matrix_print(C7);
 

    matrix_free(&A);
    matrix_free(&B);
    matrix_free(&C1);
    matrix_free(&C2);
    matrix_free(&C3);
    matrix_free(&C4);
    matrix_free(&C5);
    matrix_free(&C6);
    matrix_free(&C7);
   

    return 1;
}