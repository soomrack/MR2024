#include <stdio.h>
#include <stdlib.h>


typedef struct 
{
    int rows;
    int cols;
    double *data;
} Matrix;


void matrix_init(Matrix *A, const int rows, const int cols)
{
    A->data = (double *)malloc(rows * cols * sizeof(double));
    A->rows = rows;
    A->cols = cols;
}


void matrix_sum(Matrix *sum, Matrix A, Matrix B)
{
    if (A.rows == B.rows & A.cols == B.cols) {
        for (size_t i = 0; i < A.rows * A.cols; i++)
        {
            sum->data[i] = A.data[i] + B.data[i];   
        }
        
    }
    
}


int main()
{

}