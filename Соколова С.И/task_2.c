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
    free(A->data);
    *A = (Matrix){0,0,NULL}; 
}


Matrix matrix_sum(const Matrix A, const Matrix B)
{
    if ((A.rows != B.rows) || (A.cols != B.cols)) {

        Matrix_exeption(ERROR, "You can't sum these matrixs. Rows don't equally Cols");
        return (Matrix) {0, 0, NULL};
    }

    Matrix C = matrix_allocate(A.rows, A.cols);

    for (size_t idx = 0; idx < C.rows * C.cols; ++idx) {
            C.data[idx] =  A.data[idx] + B.data[idx];
    }

    return C;
}


Matrix matrix_substruct(const Matrix A, const Matrix B)
{
     if ((A.rows != B.rows) || (A.cols != B.cols)) {

        Matrix_exeption(ERROR, "You can't sum these matrixs. Rows don't equally Cols");
        return (Matrix) {0, 0, NULL};
    }

    Matrix C = matrix_allocate(A.rows, A.cols);

    for (size_t idx = 0; idx<C.rows * C.cols; ++idx) {
            C.data[idx] =  A.data[idx] - B.data[idx];
    }

    return C;
}


Matrix matrix_transponate(const Matrix A)
{
    Matrix At = matrix_allocate(A.rows, A.cols);

    for (size_t row = 0; row < A.rows; ++row) {
        for (size_t col = 0; col < A.cols; ++col) {
            At.data[row*At.cols+col] = A.data[col*A.rows + row];
        }
    }

    return At;
}


Matrix matrix_multiplication(const Matrix A, const Matrix B)
{
    if (A.cols != B.rows) {

        Matrix_exeption(ERROR, "You can't multiplication matrixs");
        return (Matrix) {0,0,NULL};
    }

    Matrix C = matrix_allocate(A.rows, B.cols);

    for (size_t row = 0; row < C.rows; ++row) {
        for (size_t col = 0; col < C.cols; ++col) {

            C.data[row*C.cols + col] = 0;

            for (size_t k = 0; k < B.rows; ++k) {
                C.data[row*C.cols + col] += A.data[row*A.cols + k] * B.data[k*B.cols+col];
            }
        }
    }

    return C;
}


Matrix matrix_multiplication_on_ratio(const Matrix A, const int ratio)
{
    Matrix C = matrix_allocate(A.rows, A.cols);

    for (size_t idx = 0; idx < C.rows * C.cols; ++idx) {
            C.data[idx] = ratio * A.data[idx];
        }

    return C;
}


double matrix_determinant(const Matrix A) 
{
    if (A.rows == A.cols) {

        double Det, det_sign;
        det_sign = 0;

        if (A.rows == 1) {

            Det = A.data[0];
            return Det;
        }

        else if (A.rows == 2) {

            Det = (A.data[0] * A.data[3]) - (A.data[1] * A.data[2]);
            return Det;
        }

        if (A.rows >= 3) {

            for (size_t col = 0; col < A.cols; ++col) {

                Matrix minor = matrix_allocate(A.rows-1, A.cols-1);

                if (minor.data == NULL) {
                    Matrix_exeption(ERROR, "Allocation memomry fail");
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
        }
    printf("%lf\n", det_sign);
    return det_sign;
    }

    else {
        Matrix_exeption(ERROR, "You can't take a determinant");
        return -1;
    }
}


Matrix matrix_copy(Matrix A)
{
    Matrix C = matrix_allocate(A.rows, A.cols);

    for (size_t idx = 0; idx < A.rows * A.cols; ++idx) {
        C.data[idx] = A.data[idx];
    }

    return C;
}


Matrix matrix_involution(Matrix A, const unsigned  level)
{
    if (A.rows != A.cols) {

        Matrix_exeption(ERROR, "You can't multiply");
        return (Matrix) {0,0,NULL};
    }

    Matrix C = matrix_allocate(A.rows, A.cols);

    if (level == 0) {

        for (size_t row = 0; row < C.rows; ++row) {
            for (size_t col = 0; col < C.cols; ++col) {

                if (row == col) {
                    C.data[row * C.cols + col] = 1;
                }

                else {
                    C.data[row * C.cols + col] = 0;
                }
            }
        }
        return C;
    }

    C = matrix_copy(A);

    for (int idx = 1; idx < level; ++idx) {

        C = matrix_multiplication(C,A);
    }

    return C;
}

Matrix *matrix_exponent(Matrix *A)
{
    if (A->rows == A->cols) {

        Matrix *pAe; 

        for (int k =0 ; k = 20; ++k) {

            
        }
    }
}


int main()
{
    Matrix A,B;

    srand(time(NULL));

    A = matrix_allocate(3,3);
    B = matrix_allocate(3,3);

    A = matrix_random(A);
    B = matrix_random(B);


    matrix_print(A);
    printf("---------------------------- \n");
    matrix_print(B);
    printf("---------------------------- \n");

    Matrix C = matrix_involution(A, 0);
    matrix_print(C);


    matrix_free(&A);
    matrix_free(&B);
}