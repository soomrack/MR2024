#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>

 typedef struct Matrix{
    size_t rows;
    size_t cols;
    double *data;
} Matrix;


typedef enum {
    NULL_ERR ,
    SIZE_ERR,
    OK,


}Status;

const Matrix EMPTY = {0, 0, NULL};


void matrix_error(char * error_messsage)
{
    printf("Error: %s\n", error_messsage);
}

Status matrix_init( Matrix *A, const size_t rows, const size_t cols )
{   
    if (A == NULL) {
        return NULL_ERR;
        matrix_error("Failed to allocate memory for matrix");
    }

    if (SIZE_MAX < (cols * rows * sizeof(double)))
    {
       matrix_error( "Matrix too large");
       return SIZE_ERR;
    }

    A -> data = (double *)malloc(rows * cols * sizeof(double));
    A -> rows = rows;
    A -> cols = cols;
    return OK;
}

void matrix_free(Matrix *A)
{
  free(A -> data);
  A -> rows = 0;
  A -> cols = 0;

}

Status size_check( const Matrix A, const  Matrix B )
{
    if ((A.cols == B.cols) && (A.rows == B.rows)) return OK;

}

// C = B + A
Status  matrix_sum( Matrix *result, Matrix A, Matrix B )
{
   if (size_check( A ,B ) == OK)
   {
    for(size_t idx = 0; idx < A.cols * A.rows; idx ++)
    {
        result -> data[idx] = A.data[idx] + B.data[idx];
    }
   return OK; 
   }
}
// A = k*B 
Status multiplication_by_a_constant (Matrix *result , const Matrix A, double k)
{
   for (size_t idx = 0; idx < A.cols * A.rows; idx ++) 
   {
        result -> data[idx] = A.data[idx] * k ;
    }
   return OK; 
}
// matrix_add A += B



//matrix_print
//matrix_multiply произведение матриц 
//matrix_det  определитель разделить на отдельные подфункции 
//matrix_exp
//matrix_copy
 
