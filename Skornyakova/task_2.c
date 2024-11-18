#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

 typedef struct {
    size_t rows;
    size_t cols;
    double *data;
} Matrix;


typedef enum {
    NULL_ERR ,
    SIZE_ERR,
    OK,
    ALLOC_ERR,

} Status;

const Matrix EMPTY = {0, 0, NULL};


void matrix_error(const Status error, char * error_messsage)
{
      if(error == NULL_ERR ) {
        printf("NULL_ERR: %s", error_messsage);
    }

    if(error == SIZE_ERR) {
        printf("SIZE_ERR: %s", error_messsage);
    }  
    if(error == ALLOC_ERR) {
        printf("ALLOC_ERR: %s", error_messsage);
    }  
}


Matrix matrix_init(const size_t rows, const size_t cols )
{  
     Matrix A = {0, 0, NULL};

    if(rows == 0 || cols == 0) {
        return (Matrix){rows, cols, NULL};
    }

    if (SIZE_MAX / (cols * rows * sizeof(double)) == 0){
       matrix_error(SIZE_ERR, "Matrix too large\n");
       return (Matrix){rows, cols, NULL};
    }

    A.data = (double *)malloc(rows * cols * sizeof(double));
    if (A.data == NULL){
        matrix_error(ALLOC_ERR,"Failed to allocate memory for matrix\n");
        return A;
    } 
    A.rows = rows;
    A.cols = cols;
    return A;
}


void matrix_set(const Matrix A, const double *values)
{
    memcpy(A.data, values, A.rows * A.cols * sizeof(double));
}


void matrix_free(Matrix *A)
{
  free(A -> data);
  A->data = NULL;
  A->rows = 0;
  A->cols = 0;

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
    printf("\n");
}


// C = B + A
Status  matrix_sum( Matrix *C, const Matrix A, const Matrix B )
{
    if ((A.cols == B.cols) && (A.rows == B.rows)) {
        Matrix C = matrix_init(A.cols, A.rows );
        for(size_t idx = 0; idx < A.cols * A.rows; idx ++)
        {
            C.data[idx] = A.data[idx] + B.data[idx];
        }
        matrix_print(C);
        return OK; 
    }
    else{
        matrix_error(SIZE_ERR, "Matrix size not equals\n");
    }
}

// B = k*A 
Status multiplication_by_a_constant (const Matrix A, double k)
{
    Matrix B = matrix_init(A.rows, A.cols,);
    for (size_t idx = 0; idx < A.cols * A.rows; idx ++) {
            B.data[idx] = A.data[idx] * k ;
    }
    matrix_print(B);
    return OK; 
}

//  A += B
Status  matrix_add( Matrix *A, const Matrix B )
{
   if ((A->cols == B.cols) && (A->rows == B.rows)) {
        for(size_t idx = 0; idx < A->cols * A->rows; idx ++)
        {
            A->data[idx] += B.data[idx];
        }
        matrix_print(*A);
        return OK; 
    }
}

//C = A*B
Status matrix_multiply(const Matrix A, const Matrix B)
{ 
    if (A.cols == B.rows ){
        Matrix C = matrix_init(A.rows, B.cols);
        for(size_t row = 0; row < A.rows; row ++){   
            for(size_t col = 0; col < B.cols; col ++){
                for(size_t idx = 0; idx < A.cols; idx ++){
                    C.data[row * C.cols + col] += A.data[row * A.cols +idx] * B.data[B.cols * idx + col];
                }
            } 
        }
    return OK;    
    }
    else{
        matrix_error(SIZE_ERR, "Incorrect matrix size \n");
    }   

}

//B = (A)t
Status matrix_transp(const Matrix A)
{
    Matrix B = matrix_init(A.cols, A.rows);
    for (size_t i = 0; i < A.cols; i++ ){
        for( size_t j = 0; j < A.rows; j++){
            A.data[i * A.cols + j] = B.data[j*B.cols + i];
        }
    }
    return OK;
    matrix_print(B);
}
//matrix_copy
//matrix_det  определитель разделить на отдельные подфункции 
//matrix_exp
 
int main()
{
   Matrix A, B;
    A = matrix_init(2, 2);  
    B = matrix_init(2, 2);  
    matrix_set(A, (double[]){
            1., 2.,
            3., 4.
        });
    matrix_set(B, (double[]){
            1., 0.,
            0., 1.
        });
    matrix_print(A);
    matrix_print(B);
}