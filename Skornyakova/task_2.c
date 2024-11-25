#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

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

    if (SIZE_MAX / cols/  rows / sizeof(double) == 0){
       matrix_error(SIZE_ERR, "Matrix too large\n");
       return (Matrix){rows, cols, NULL};
    }

    A.data = (double *)malloc(rows * cols * sizeof(double));// посмотреть на деление, где там sizeof
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
  if (A->data == NULL) {
       matrix_error(NULL_ERR, "Incorrect matrix\n");
    };
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
Matrix  matrix_sum(const Matrix A, const Matrix B )
{
    if (!((A.cols == B.cols) && (A.rows == B.rows))) {
        matrix_error(SIZE_ERR, "Matrix size not equals\n");
    }
    Matrix C = matrix_init(A.cols, A.rows);
    for(size_t idx = 0; idx < A.cols * A.rows; idx ++) {
        C.data[idx] = A.data[idx] + B.data[idx];
    }
    matrix_print(C);
    return C;

}

// B = k*A 
Matrix multiplication_by_a_constant (const Matrix A, double k)
{
    Matrix B = matrix_init(A.rows, A.cols);
    for (size_t idx = 0; idx < A.cols * A.rows; idx ++) {
            B.data[idx] = A.data[idx] * k ;
    }
    matrix_print(B);
    return B; 
}

//  A += B
Matrix  matrix_add( Matrix A, const Matrix B )
{
   if ((A.cols == B.cols) && (A.rows == B.rows)) {
        for(size_t idx = 0; idx < A.cols * A.rows; idx ++)
        {
            A.data[idx] += B.data[idx];
        }
        matrix_print(A);
        return A; 
    }  
}

//C = A*B
Matrix matrix_multiply(const Matrix A, const Matrix B)
{ 
    if (!(A.cols == B.rows )){
        matrix_error(SIZE_ERR, "Incorrect matrix size \n");
    }
    Matrix C = matrix_init(A.rows, B.cols);
        for(size_t row = 0; row < A.rows; row ++){   
            for(size_t col = 0; col < B.cols; col ++){
                C.data[row*C.cols + col]= 0;
                for(size_t idx = 0; idx < A.cols; idx ++){
                    C.data[row * C.cols + col] += A.data[row * A.cols +idx] * B.data[B.cols * idx + col];
                }
            } 
        }
    return C;    
}

//B = (A)t
Matrix matrix_transp(const Matrix A)
{
    Matrix B = matrix_init(A.cols, A.rows);
    for (size_t i = 0; i < A.cols; i++ ){
        for( size_t j = 0; j < A.rows; j++){
            A.data[i * A.cols + j] = B.data[j*B.cols + i];
        }
    }
    return B;
    matrix_print(B);
}

//Определитель
double matrix_det(const Matrix A)
{   
    if (!(A.cols == A.rows)) {
       matrix_error(SIZE_ERR, "Incorrect matrix size \n");
    }
    if (A.data == NULL) {
       matrix_error(NULL_ERR, "Incorrect matrix\n");
    }
    double det = 1;
    double now;
    if (fabs(A.data[0]) < 0.000000001){
        for( size_t row = 1; row < A.rows; row++) {
            if(fabs(A.data[row * A.cols]) > 0.000000001) {
                for (size_t col = 0; col < A.cols; col++) {
                    now = A.data[col];
                    A.data[col] = A.data[row * A.cols + col];
                    A.data[row * A.cols + col] = now;   
                }
            }
        }
        det *= -1;
    }
    
    for (size_t col_0 = 0; col_0 < A.cols-1; col_0++){ 
        for(size_t row_0 = col_0 + 1; row_0 < A.rows; row_0++) {
            double k = A.data[row_0 * A.cols + col_0]/A.data[col_0 * A.cols + col_0];
            for(size_t col = col_0; col < A.cols; col++) {
                A.data[row_0 * A.cols + col] -= A.data[col_0 * A.cols + col] * k; 
            }
        }                   
    } 
    for (size_t i=0; i<A.cols; i++){
        det *= A.data[i * A.cols + i];
    }
    return det;
}

//копирование матрицы 
Matrix matrix_copy(const  Matrix A)
{
    Matrix copy = matrix_init(A.cols, A.rows);
    memcpy(copy.data,A.data, A.cols * A.rows * sizeof(double));
    return copy;
}

//факториал 
double factorial(const  int k)
{
    double factorial = 1.0;

    if(k == 0) {
        return factorial;
    }
    
    for (size_t i = 1; i <= k; ++i) {
       factorial *= i;
    }

    return factorial;
}

// B -Единичная матрица размерностью A
Matrix identity_matrix(Matrix A)
{
    Matrix B = matrix_init(A.cols, A.rows);
    for (size_t i=0; i<A.rows; i++){
        for (size_t j=0; j<A.cols; j++){
            if (i==j){
                B.data[i * A.cols + i] = 1;
            }
            else {
                B.data[i * A.cols + j] = 0;
            }
        }
    }
    return B;
}

// Возведение матрицы в степень
Matrix matrix_pow( Matrix A, int pow) 
{
    if (A.rows != A.cols) {
        matrix_error(SIZE_ERR, "Incorrect matrix size \n");
    
    }

    if (pow == 0) {
        Matrix result = identity_matrix(A);
        return result;
    }

    Matrix matrix_to_power = matrix_copy(A);
    Matrix result = matrix_init(A.rows, A.cols);

    for (int i = 1; i < pow; ++i) {
        result = matrix_multiply(matrix_to_power, A);
        matrix_free(&matrix_to_power);
        matrix_to_power =  matrix_copy(result); 
        
    }

    matrix_free(&result);
    return matrix_to_power;
}

//Экспонента матрицы
Matrix matrix_exp(const  Matrix A, int k)
{
    if (A.rows != A.cols) {
        matrix_error(SIZE_ERR, "Incorrect matrix size \n");
    
    }

    Matrix result = matrix_init(A.rows, A.cols);
    Matrix matrix_to_power = matrix_init(A.rows, A.cols);

    for (int n = 0; n < k; ++n) {
        matrix_to_power = matrix_pow(A, n);
        double fact = factorial(n);

        for (size_t i = 0; i < result.rows * result.cols; ++i) {
            result.data[i] += matrix_to_power.data[i] / fact;
        }

        matrix_free(&matrix_to_power);
    }

    return result;
}

int main()
{
   Matrix A, B,C;
    A = matrix_init(3, 3);  
    B = matrix_init(3, 3);  
    //C = matrix_init(3, 3);
    matrix_set(A, (double[]){
            3., 4., 5., 
            7., 8., 9., 
            6., 5., 3.
        });
    matrix_set(B, (double[]){
            1., 0., 1.,
            0., 1., 1.,
            1., 1., 1.
        });
    matrix_print(A);
    matrix_print(B);
    //C = matrix_multiply(B,B);
    //C = matrix_pow(B,2);
    C = identity_matrix(A);
    matrix_print(C);
    
    //double det = matrix_det(A);
    //printf("%f",det);
    
}