#include <stdio.h>
#include <stdlib.h>



typedef struct 
{
    int rows;
    int cols;
    double *data;
} Matrix;


Matrix matrix_init(const int rows, const int cols)
{
    Matrix A;

    A.data = (double *)malloc(rows * cols * sizeof(double));
    A.rows = rows;
    A.cols = cols;
    
    return A;
}


void matrix_fill(Matrix *A)
{
    for (size_t i = 0; i < A->rows * A->cols; i++) {
        A->data[i] = (i + 1)*(i + 1);
    }
}



Matrix unit_matrix(int rows, int cols)
{
    Matrix I = matrix_init(rows, cols);

    for (int i = 0; i < I.cols * I.rows; i ++) {
        if (i / cols == i % cols) {
            I.data[i] = (double)1;
        }
        else {
            I.data[i] = (double)0;
        }   
    }

    return I;
}


Matrix matrix_clone(Matrix A)
{
    Matrix result = matrix_init(A.rows, A.cols);

    for (size_t i = 0; i < A.rows * A.cols; i++) {
        result.data[i] = A.data[i];
    }

    return result;     
}


Matrix matrix_sum(Matrix A, Matrix B)
{
    Matrix result = matrix_init(A.rows, A.cols);
    
    if (A.rows == B.rows & A.cols == B.cols) {
        for (size_t i = 0; i < A.rows * A.cols; i++) {
            result.data[i] = A.data[i] + B.data[i];   
        }
    }

    return result;
}


Matrix matrix_sub(Matrix A, Matrix B)
{
    Matrix result;

    if (A.rows == B.rows & A.cols == B.cols) {
        for (size_t i = 0; i < A.rows * A.cols; i++) {
            result.data[i] = A.data[i] - B.data[i];   
        }
    }

    return result;
}


Matrix matrix_mul_num(Matrix A, double num)
{
    Matrix result;

    for (size_t i = 0; i < A.rows * A.cols; i++) {
        result.data[i] = A.data[i] * num;
    }

    return result;
}


Matrix matrix_div_num(Matrix A, double num)
{
    Matrix result;

    for (size_t i = 0; i < A.rows * A.cols; i++) {
        result.data[i] = A.data[i] / num;
    }

    return result;
}


Matrix matrix_mul(Matrix A, Matrix B)
{
    Matrix result;

    if (A.cols == B.rows) {
        result = matrix_init(A.rows, B.cols);
        for (size_t row = 0; row < A.rows; row++) {
            for (size_t col = 0; col < B.cols; col++) {
                result.data[row * result.cols + col] = 0;
                for (size_t i = 0; i < A.cols; i++) {
                    result.data[row * result.cols + col] += (double)A.data[row * A.cols + i] * B.data[i * B.cols + col];
                }
            }
        }
    }

    return result;
}


Matrix matrix_pow(Matrix A, int power){
    Matrix result;
    
    if (A.cols == A.rows && power > 0) {
        for (size_t i = 1; i < power; i++) {
            result = matrix_mul(result, A);
        }
    }

    return result;
}


void matrix_print(Matrix A)
{   
    for (size_t i = 0; i < A.cols * A.rows; i++) {
        if ((i) % A.cols == 0) {
            printf("|");
        }
        printf("%.2f|", A.data[i]);
        if ((i) % A.cols == A.cols - 1) {
            printf("\n");
        }
    }
    printf("\n");
}


void matrix_transp(Matrix *A)
{
    Matrix B = matrix_clone(*A);
    *A = matrix_init(A->cols, A->rows);
    for (int i = 0; i < B.cols * B.rows; i++) {
        A->data[B.rows * (i % B.cols) + i / B.cols] = B.data[i];
    }
}


double matrix_det(Matrix A)
{
    Matrix B = matrix_clone(A);
    Matrix C = unit_matrix(A.rows, A.cols);
    
    double result = 1;
    if(B.rows == B.cols) {
        for (size_t col_nul = 0; col_nul < B.cols; col_nul++) {
            for (size_t row = col_nul + 1; row < B.rows; row++) {
                double proportion = (double)B.data[row * B.cols + col_nul] / B.data[col_nul * B.cols + col_nul];
                for (size_t col = col_nul; col < B.cols; col++) {
                    B.data[row * B.cols + col] -= (double)B.data[col_nul * B.cols + col] * proportion;
                    C.data[row * C.cols + col] -= (double)C.data[col_nul * C.cols + col] * proportion;
                }
            }
            result *= (double)B.data[col_nul * B.cols + col_nul];
        }
    }
    return result;
}


Matrix matrix_reverse(Matrix A)
{
    Matrix result;

    printf("Matrix reverse\n\n");
    Matrix B = matrix_clone(A);
    result = unit_matrix(A.rows, A.cols);
   
    if(B.rows == B.cols) {  
        for (size_t col_nul = 0; col_nul <= B.cols; col_nul++) {
            for (size_t row = col_nul + 1; row < B.rows; row++) {
                double proportion = (double)B.data[row * B.cols + col_nul] / B.data[col_nul * B.cols + col_nul];
                for (size_t col = 0; col < B.cols; col++) {
                    B.data[row * B.cols + col] -= (double)B.data[col_nul * B.cols + col] * proportion;
                    result.data[row * B.cols + col] -= (double) result.data[col_nul * B.cols + col] * proportion;
                }
            }
        }
        for (int row = 0; row < B.rows; row++){
            double koef = B.data[row * B.cols + row];
            for (int col = 0; col < B.cols; col++){
                B.data[row * B.cols + col] /= koef;
                result.data[row * B.cols + col] /= koef;
            }
        }
        for (int col_nul = B.cols - 1; col_nul >= 0; col_nul--) {
            for (int row = col_nul - 1; row >= 0; row--) {
                double proportion = (double)B.data[row * B.cols + col_nul] / B.data[col_nul * B.cols + col_nul];
                for (int col = B.cols - 1; col >= 0; col--) {
                    B.data[row * B.cols + col] -= (double)B.data[col_nul * B.cols + col] * proportion;
                    result.data[row * B.cols + col] -= (double) result.data[col_nul * B.cols + col] * proportion;
                }   
            }
            matrix_print(B);
            matrix_print(result);
        }  
    }  

    return result; 
}


Matrix matrix_div(Matrix A, Matrix B)
{
    Matrix result;

    printf("Matrix multiply\n\n");
    Matrix D = matrix_clone(B);
    printf("Matrix multiply clone\n\n");
    matrix_print(D);
    D = matrix_reverse(B);
    printf("Matrix multiply reverse and normal matrix\n\n");
    matrix_print(D);
    matrix_print(A);
    result = matrix_mul(A, D);

    return result;
}


int factorial(int num)
{
    int fact = 1;
    for (int i = 1; i <= num; i++)
    {
        fact *= i;
    }
    return fact;
}


Matrix matrix_exponent(Matrix A, int num)
{
    Matrix result;

    Matrix E;
    E = unit_matrix(A.rows, A.cols);

    for (int cur_num = 1; cur_num < num; cur_num++)
    {
        Matrix temporary = matrix_clone(A);
        Matrix temporary2 = matrix_clone(temporary);
        
        temporary = matrix_pow(A, cur_num);
        temporary2 = matrix_mul_num(temporary, (double) 1.0 / factorial(cur_num));
        Matrix E_copy = matrix_clone(E);
        E = matrix_sum(E_copy, temporary2);
    }
    result = matrix_clone(E);
    
    return result;
}


int main()  
{
    Matrix A;
    Matrix B;
    Matrix C;
    Matrix D;

    A = matrix_init(3, 3);
    matrix_fill(&A);
    B = matrix_init(3, 3);
    matrix_fill(&B);
    printf("");
    matrix_print(A);
    matrix_print(B);
    A = matrix_sum(A, B);
    matrix_print(A);

    C = matrix_init(3, 3);
    matrix_fill(&C);
    D = matrix_init(3, 3);
    matrix_fill(&D);





    return 0;
}