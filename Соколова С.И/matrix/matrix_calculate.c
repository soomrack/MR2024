#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>


struct Matrix {
    size_t rows;
    size_t cols;
    double *data;
};


typedef struct Matrix Matrix;


enum  MatrixExceptionLevel {ERROR, WARNING, INFO, DEBUG};


const Matrix Matrix_NULL = {0, 0, NULL};


void Matrix_exeption(const enum MatrixExceptionLevel level, char *msg)
{
    if (level == ERROR) {
        printf("ERROR %s", msg);
    }
    if (level == WARNING) {
        printf("WARNING %s", msg);
    }
}


Matrix matrix_allocate(const size_t rows, const size_t cols)
{
    Matrix A = {0, 0, NULL};

    if (rows == 0 || cols == 0) {
        return (Matrix){rows, cols, NULL};
    }

    if (SIZE_MAX /rows /cols/sizeof(double) == 0)
    {
        Matrix_exeption(ERROR, "OVERFLOW: Переполнение выделенной памяти");
        return (Matrix) {0, 0, NULL};
    }

    A.data = malloc(rows*cols*sizeof(double));
    if (A.data == NULL) {
        Matrix_exeption(ERROR, "Allocation memory fail");
        return A;
    }

    A.rows = rows;
    A.cols = cols;
    return A;
}  

Matrix matrix_random(const Matrix A)
{
    int sight;

    for (size_t i =0; i <A.rows; ++i) {
        for (size_t j = 0; j < A.cols; ++j) {
            sight = (rand()%2) * 2 - 1;
            A.data[i*A.cols + j] = (rand()%100) * sight; 
        }
    }
    return A;
}


void matrix_print(const Matrix A) 
{
    for (size_t row = 0; row<A.rows; ++row) {
        for (size_t col = 0; col< A.cols; ++col) {
            printf("%f\t", A.data[row*A.cols + col]);
        }
        printf("\n");
    }
    printf("\n");
}


void matrix_free(Matrix *A) 
{
    if (A == NULL) {
        Matrix_exeption(ERROR, "You transfer NULL matrix");
        return;
    }

    free(A->data);
    *A = (Matrix){0,0,NULL}; 
}


int matrix_sum(Matrix* C_result, const Matrix A, const Matrix B)
{
    if (C_result == NULL) {
        Matrix_exeption(WARNING, "You transfer NULL matrox");
        return 0;
    }

    if ((A.rows != B.rows) || (A.cols != B.cols)) {

        Matrix_exeption(ERROR, "You can't sum these matrixs. Rows don't equally Cols");
        return 0;
    }

    if ((A.rows != C_result->rows) || (A.cols != C_result->cols)) {
        Matrix_exeption(ERROR, "You passed the matrix the wrong size");
        return 0;
    }

    for (size_t idx = 0; idx < C_result->rows * C_result->cols; ++idx) {
            C_result->data[idx] =  A.data[idx] + B.data[idx];
    }
    return 1;
}


int matrix_substruct(Matrix* C_result, const Matrix A, const Matrix B)
{
    if (C_result == NULL) {
        Matrix_exeption(WARNING, "You transfer NULL matrox");
        return 0;
    }

     if ((A.rows != B.rows) || (A.cols != B.cols)) {

        Matrix_exeption(ERROR, "You can't sum these matrixs. Rows don't equally Cols");
        return 0;
    }

    if ((A.rows != C_result->rows) || (A.cols != C_result->cols)) {
        Matrix_exeption(ERROR, "You passed the matrix the wrong size");
        return 0;
    }

    for (size_t idx = 0; idx<C_result->rows * C_result->cols; ++idx) {
            C_result->data[idx] =  A.data[idx] - B.data[idx];
    }
    return 1;
}


int matrix_transponate(Matrix* At, const Matrix A)
{
    if (At == NULL) {
        Matrix_exeption(WARNING, "You transfer NULL matrix");
        return 0;
    }

    if ((A.rows != At->rows) || (A.cols != At->cols)) {
        Matrix_exeption(ERROR, "You passed the matrix the wrong size");
        return 0;
    }


    for (size_t row = 0; row < A.rows; ++row) {
        for (size_t col = 0; col < A.cols; ++col) {
            At->data[row*At->cols+col] = A.data[col*A.rows + row];
        }
    }
    return 1;
}


int matrix_multiplication(Matrix* C_result, const Matrix A, const Matrix B)
{
    if (C_result == NULL) {
        Matrix_exeption(WARNING, "You transfer NULL matrox");
        return 0;
    }

    if (A.cols != B.rows) {

        Matrix_exeption(ERROR, "You can't multiplication matrixs");
        return 0;
    }

    if ((A.rows != C_result->rows) || (B.cols != C_result->cols)) {
        Matrix_exeption(ERROR, "You passed the matrix the wrong size");
        return 0;
    }

    for (size_t row = 0; row < C_result->rows; ++row) {
        for (size_t col = 0; col < C_result->cols; ++col) {

            C_result->data[row*C_result->cols + col] = 0;

            for (size_t k = 0; k < B.rows; ++k) {
                C_result->data[row*C_result->cols + col] += A.data[row*A.cols + k] * B.data[k*B.cols+col];
            }
        }
    }
    return 1;
}


int matrix_multiplication_on_ratio(Matrix* C_result, const Matrix A, const double ratio)
{
    if (C_result == NULL) {
        Matrix_exeption(WARNING, "You transfer NULL matrox");
        return 0;
    }

    if ((A.rows != C_result->rows) || (A.cols != C_result->cols)) {
        Matrix_exeption(ERROR, "You passed the matrix the wrong size");
        return 0;
    }

    for (size_t idx = 0; idx < C_result->rows * C_result->cols; ++idx) {
            C_result->data[idx] = ratio * A.data[idx];
        }
    return 1;
}


double matrix_determinant(const Matrix A) 
{
    if (A.rows != A.cols) {
        Matrix_exeption(ERROR, "You can't get a determinant");
        return NAN;
    }

    double Det, det_sign;
    det_sign = 0;

    if (A.rows == 0) {
        Det = 0;
        return Det;
    }

    if (A.rows == 1) {
        Det = A.data[0];
        return Det;
    }

    else if (A.rows == 2) {

        Det = (A.data[0] * A.data[3]) - (A.data[1] * A.data[2]);
        return Det;
    }

    for (size_t col = 0; col < A.cols; ++col) {

        Matrix minor = matrix_allocate(A.rows-1, A.cols-1);

        if (minor.data == NULL) {
            return NAN;
        }

        size_t minor_row = 0;

        for (size_t row = 1; row < A.rows; ++row) {

            size_t minor_col = 0;

            for (size_t k_col=0; k_col < A.cols; ++k_col) {
                        
                if (col != k_col) {

                    minor.data[minor_row*(A.cols-1)+minor_col]=A.data[row*A.cols+k_col];
                    minor_col++;
                    }
                }
                minor_row++;
            }
        double minor_det = matrix_determinant(minor);

        det_sign += (col % 2 == 0 ? 1: -1) * A.data[col] *minor_det;

        free(minor.data);
    } 
    printf("%lf\n", det_sign);
    return det_sign;
}


int matrix_copy(Matrix* C_result, Matrix A)
{
    if (C_result == NULL) {
        Matrix_exeption(WARNING, "You transfer NULL matrox");
        return 0;
    }

    if ((A.rows != C_result->rows) || (A.cols != C_result->cols)) {
        Matrix_exeption(ERROR, "You can't copy");
        return 0;
    }

    memcpy(C_result->data, A.data, A.rows*A.cols*sizeof(double));
    return 1;
}


int matrix_one(Matrix* C_result, Matrix A)
{
    if (C_result == NULL) {
        Matrix_exeption(WARNING, "You transfer NULL matrox");
        return 0;
    }

    if (A.rows != A.cols) {
        Matrix_exeption(ERROR, "Your matrix doesn't squere");
        return 0;
    }

    if ((A.rows != C_result->rows) || (A.cols != C_result->cols)) {
        Matrix_exeption(ERROR, "You can't copy");
        return 0;
    }

    for (size_t row = 0; row < C_result->rows; ++row) {
        for (size_t col = 0; col < C_result->cols; ++col) {

            if (row == col) {
                C_result->data[row * C_result->cols + col] = 1;
            }

            else {
                C_result->data[row * C_result->cols + col] = 0;
            }
        }
    } 
    return 1;   
}


int matrix_involution(Matrix* C_result, Matrix A, const unsigned  int level)
{
    if (C_result == NULL) {
        Matrix_exeption(ERROR, "You transfer NULL matrix");
        return 0;
    }

    if (A.rows != A.cols) {

        Matrix_exeption(ERROR, "You can't multiply");
        return 0;
    }

    if ((A.rows != C_result->rows) || (A.cols != C_result->cols)) {
        Matrix_exeption(ERROR, "You can't copy");
        return 0;
    }


    if (level == 0) {
        matrix_one(C_result, A);
        return 0;
    }

    if (level == 1) {
        matrix_copy(C_result, A);
        return 0;
    }

    Matrix C = matrix_allocate(A.rows, A.cols);

    matrix_copy(&C, A);

    for (int idx = 1; idx < level; ++idx) {

        matrix_multiplication(C_result, C, A);

        for (size_t idx = 0; idx< C_result->rows*C_result->cols; ++idx) {
            C.data[idx] = C_result->data[idx];
        }
    }

    matrix_free(&C);
    return 1;
}


static double factorial(int n) 
{
    return (n < 2) ? 1 : n * factorial (n - 1);
}


void matrix_add(Matrix A, const Matrix B)
{
    if(A.cols == 0 || A.rows == 0) {
        Matrix_exeption(ERROR, "You can't add matrix");
        return;
    } 
        
    for(size_t idx = 0; idx < A.rows * A.cols; ++idx) {
        A.data[idx] += B.data[idx];
    }
}


void matrix_zeros(Matrix A)
{
    memset(A.data, 0, A.cols * A.rows * sizeof(double));
}



int matrix_exponent(Matrix* C_result, Matrix A)
{
    if (C_result == NULL) {
        Matrix_exeption(WARNING, "You transfer NULL matrix");
        return 0;
    }

    if (A.rows != A.cols) {
        Matrix_exeption(ERROR, "You can't get exponent");
        return 0;
    }

    if ((A.rows != C_result->rows) || (A.cols != C_result->cols)) {
        Matrix_exeption(ERROR, "You can't copy");
        return 0;
    }

    Matrix E = matrix_allocate(A.rows, A.cols);
    Matrix E_sum = matrix_allocate(A.rows, A.cols);
    Matrix C = matrix_allocate (A.rows, A.cols);

    matrix_zeros(E_sum);

    for (int k =0 ; k <= 20; ++k) {

        matrix_involution(&C, A, k);
        matrix_multiplication_on_ratio(&E, C, 1/factorial(k));
        matrix_add(E_sum, E);
    }

    
    matrix_copy(C_result, E_sum);
    matrix_free(&E_sum);
    matrix_free(&C);
    matrix_free(&E);
    return 1;
}


int main()
{
    Matrix A,B,C;

    srand(time(NULL));

    A = matrix_allocate(2,2);
    B = matrix_allocate(3,3);
    C = matrix_allocate(2,2);

    A = matrix_random(A);
    B = matrix_random(B);


    matrix_print(A);
    printf("---------------------------- \n");
    //matrix_print(B);
    //printf("---------------------------- \n");

    matrix_involution(&C, A, 2);
    matrix_print(C);


    matrix_free(&A);
    matrix_free(&B);
    matrix_free(&C);
}