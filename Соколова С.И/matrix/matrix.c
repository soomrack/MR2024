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

    size_t size_in_bytes = rows * cols * sizeof(double);
    if (size_in_bytes / sizeof(double) != rows * cols)
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
    free(A->data);
    *A = (Matrix){0,0,NULL}; 
}


Matrix matrix_sum(const Matrix A, const Matrix B, Matrix* C_result)
{
    if ((A.rows != B.rows) || (A.cols != B.cols)) {

        Matrix_exeption(ERROR, "You can't sum these matrixs. Rows don't equally Cols");
        return Matrix_NULL;
    }

    for (size_t idx = 0; idx < C_result->rows * C_result->cols; ++idx) {
            C_result->data[idx] =  A.data[idx] + B.data[idx];
    }

    return Matrix_NULL;
}


Matrix matrix_substruct(const Matrix A, const Matrix B, Matrix* C_result)
{
     if ((A.rows != B.rows) || (A.cols != B.cols)) {

        Matrix_exeption(ERROR, "You can't sum these matrixs. Rows don't equally Cols");
        return Matrix_NULL;
    }

    for (size_t idx = 0; idx<C_result->rows * C_result->cols; ++idx) {
            C_result->data[idx] =  A.data[idx] - B.data[idx];
    }

    return Matrix_NULL;
}


Matrix matrix_transponate(const Matrix A, Matrix* At)
{

    for (size_t row = 0; row < A.rows; ++row) {
        for (size_t col = 0; col < A.cols; ++col) {
            At->data[row*At->cols+col] = A.data[col*A.rows + row];
        }
    }

    return Matrix_NULL;
}


Matrix matrix_multiplication(const Matrix A, const Matrix B, Matrix* C_result)
{
    if (A.cols != B.rows) {

        Matrix_exeption(ERROR, "You can't multiplication matrixs");
        return Matrix_NULL;
    }

    for (size_t row = 0; row < C_result->rows; ++row) {
        for (size_t col = 0; col < C_result->cols; ++col) {

            C_result->data[row*C_result->cols + col] = 0;

            for (size_t k = 0; k < B.rows; ++k) {
                C_result->data[row*C_result->cols + col] += A.data[row*A.cols + k] * B.data[k*B.cols+col];
            }
        }
    }

    return Matrix_NULL;
}


Matrix matrix_multiplication_on_ratio(const Matrix A, const double ratio, Matrix* C_result)
{
    for (size_t idx = 0; idx < C_result->rows * C_result->cols; ++idx) {
            C_result->data[idx] = ratio * A.data[idx];
        }

    return Matrix_NULL;
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


Matrix matrix_copy(Matrix A, Matrix* C_result)
{
    if ((A.rows != C_result->rows) || (A.cols != C_result->cols)) {
        Matrix_exeption(ERROR, "");
        return Matrix_NULL;
    }

    for (size_t idx = 0; idx < A.rows * A.cols; ++idx) {
        C_result->data[idx] = A.data[idx];
    }

    return Matrix_NULL;
}


Matrix matrix_involution(Matrix A, const unsigned  level, Matrix* C_result)
{
    if (A.rows != A.cols) {

        Matrix_exeption(ERROR, "You can't multiply");
        return Matrix_NULL;
    }


    if (level == 0) {

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
        return Matrix_NULL;
    }
    Matrix C = matrix_allocate(A.rows, A.cols);
    Matrix C_sum = matrix_allocate(A.rows, A.cols);

    matrix_copy(A, &C);

    for (int idx = 1; idx < level; ++idx) {

        matrix_multiplication(C, A, &C_sum);

        for (size_t idx = 0; idx< C_result->rows*C_result->cols; ++idx) {
            C_result->data[idx] += C_sum.data[idx];
        }

        matrix_free(&C_sum);
    }

    matrix_free(&C);

    return Matrix_NULL;
}


static double factorial(int n) 
{
    return (n < 2) ? 1 : n * factorial (n - 1);
}


/*Matrix matrix_exponent(Matrix A)
{
    if (A.rows != A.cols) {
        Matrix_exeption(ERROR, "You can't get exponent");
        return (Matrix) {0, 0, NULL};
    }

    Matrix E = matrix_allocate(A.rows, A.cols);
    Matrix E_sum = matrix_allocate(A.rows, A.cols);
    Matrix C = matrix_allocate (A.rows, A.cols);

    E_sum = matrix_involution(A,0);

    for (int k =1 ; k <= 20; ++k) {

        C = matrix_involution(A,k);
        E = matrix_multiplication_on_ratio(C, 1/factorial(k));
        E_sum = matrix_sum(E_sum, E);
        matrix_free(&C);
        matrix_free(&E);
    }

    return E_sum;
}*/


int main()
{
    Matrix A,B,C;

    srand(time(NULL));

    A = matrix_allocate(3,3);
    B = matrix_allocate(3,3);
    C = matrix_allocate(3,3);

    A = matrix_random(A);
    B = matrix_random(B);


    matrix_print(A);
    printf("---------------------------- \n");
    //matrix_print(B);
    //printf("---------------------------- \n");

    matrix_involution(A,2,&C);
    matrix_print(C);


    matrix_free(&A);
    matrix_free(&B);
    matrix_free(&C);
}