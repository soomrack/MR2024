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


    A.data = malloc(rows * cols * sizeof(double));  // TODO: check for overflow
    if(A.data == NULL) {
        matrix_exception(ERROR, "Allocation memory fail.");
        return NULL_MATRIX;
    }

    if (A.data == NULL) {
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
    free(A->data);  // free( (*A).data  )
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
Matrix matrix_umnok(const Matrix A, const k)
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
Matrix matrix_delk(const Matrix A, const k)
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
    if (A.cols != A.rows) return 0;

    if (A.cols == 1 && A.rows == 1) {
        return A.data[0];
    }

    if (A.cols == 2 && A.rows == 2) {
        return A.data[0] * A.data[3] - A.data[1] * A.data[2];
    }

    if (A.cols == 3 && A.rows == 3) {
        return A.data[0] * A.data[4] * A.data[8]
            + A.data[1] * A.data[5] * A.data[6]
            + A.data[2] * A.data[7] * A.data[3]
            - A.data[2] * A.data[4] * A.data[6]
            - A.data[0] * A.data[5] * A.data[7]
            - A.data[1] * A.data[8] * A.data[3];
    }
    return 0;

}


Matrix matrix_exp(const Matrix A)
{
    if (A.cols != A.rows) return NULL_MATRIX;

    Matrix C = matrix_allocate(A.rows, A.cols);
    if (C.data == NULL) return NULL_MATRIX;



}


int main()
{
    Matrix A, B;
    A = matrix_allocate(2, 2);  // TODO: check for allocated
    B = matrix_allocate(2, 2);  // TODO: check for allocated
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


    matrix_free(&A);
    return 1;
}