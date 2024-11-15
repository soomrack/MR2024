#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


struct Matrix {
    size_t rows;
    size_t cols;
    double *data;
};


typedef struct Matrix Matrix;


Matrix *pa, *pb;


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

const Matrix *matrix_random(const Matrix *A)
{
    int sight;

    for (size_t i =0; i <A->rows; ++i) {
        for (size_t j = 0; j < A->cols; ++j) {
            sight = (rand()%2) * 2 - 1;
            A->data[i*A->cols + j] = (rand()%100) * sight; 
        }
    }
    return A;
}


void matrix_print(const Matrix *A) 
{
    for (size_t row = 0; row<A->rows; ++row) {
        for (size_t col = 0; col< A->cols; ++col) {
            printf("%f\t", A->data[row*A->cols + col]);
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


Matrix *matrix_sum(Matrix *A, Matrix *B)
{
    if ((A->rows == B->rows) && (A->cols == B->cols)) {

        Matrix C = matrix_allocate(A->rows, A->cols);

        for (size_t row = 0; row<A->rows; ++row) {
            for (size_t col = 0; col< A->cols; ++col) {
                C.data[row*C.cols + col] =  A->data[row*A->cols + col] + B->data[row*B->cols + col];
            }
        }

        matrix_print(&C);
        return NULL;
    }
    else {
        Matrix_exeption(ERROR, "You can't sum these matrixs. Rows don't equally Cols");
        return NULL;
    }
}


Matrix *matrix_substruct(Matrix *A, Matrix *B)
{
    if ((A->rows == B->rows) && (A->cols == B->cols)) {

        Matrix C = matrix_allocate(A->rows, A->cols);

        for (size_t row = 0; row<A->rows; ++row) {
            for (size_t col = 0; col< A->cols; ++col) {
                C.data[row*C.cols + col] =  A->data[row*A->cols + col] - B->data[row*B->cols + col];
            }
        }

        matrix_print(&C);
        return NULL;
    }

    else {
        Matrix_exeption(ERROR, "You can't subsruct matrixs. Rows don't equally Cols");
        return NULL;
    }
}


Matrix *matrix_multiplication(Matrix *A, Matrix *B)
{
    if (A->cols == B-> rows) {

        Matrix C = matrix_allocate(A->rows, B->cols);

        
    }
}


int main()
{
    Matrix A,B;

    srand(time(NULL));

    A = matrix_allocate(3,3);
    B = matrix_allocate(3,3);


    pa = matrix_random(&A);
    pb = matrix_random(&B);

    matrix_print(pa);
    printf("---------------------------- \n");
    matrix_print(pb);
    printf("---------------------------- \n");

    
    matrix_substruct(pa,pb);


    matrix_free(pa);
    matrix_free(pb);
}