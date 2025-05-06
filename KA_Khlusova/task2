#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct Matrix 
{
    size_t rows;
    size_t cols;
    double* data;
};
typedef struct Matrix Matrix;

enum MatrixExceptionLevel {ERROR, WARNING, INFO, DEBUG};

const Matrix MATRIX_NULL = {0, 0, NULL};

void matrix_exception(const enum MatrixExceptionLevel level, char *msg)
{
    if(level == ERROR) {
        printf("ERROR: %s", msg);
    }

    if(level == WARNING) {
        printf("WARNING: %s", msg);
    }
}


Matrix matrix_allocate(const size_t rows, const size_t cols)
{
    Matrix A;
    
    if (rows == 0 || cols == 0) {
        matrix_exception(WARNING, "0 rows and colums in your matrix\n");
        return(Matrix){rows,cols,NULL};
    }
    
    size_t size = rows * cols;
    if (size/cols != rows){
        matrix_exception(ERROR, "matrix elements overflow\n");
        return MATRIX_NULL;
    }
    
    A.data = malloc(rows*cols*sizeof(double));
    if (A.data == NULL) {
        matrix_exception(ERROR, "Allocation memory full\n");
        return MATRIX_NULL;
    }
    
    A.rows = rows;
    A.cols = cols;
    return A;
}


void matrix_free(Matrix *A) //освобождаем память
{
    free (A->data);
    *A = MATRIX_NULL;
}

void matrix_set(const Matrix A, const double *values) //
{
    memcpy(A.data, values, A.rows * A.cols * sizeof(double));
}

void matrix_print(const Matrix A)
{
    for(size_t indexr = 0; indexr < A.rows; indexr++){
        for(size_t indexc = 0; indexc < A.cols; indexc++){
            printf("%.2f", A.data[indexr*A.cols + indexc]);
        }
        printf("\n");
    }
    printf("\n");
}


Matrix matrix_zero(const size_t rows, const size_t cols) //нулевая
{
    Matrix C = matrix_allocate(rows, cols);
       
    memset(C.data, 0, C.rows * C.cols * sizeof(double));

    return C;
}

Matrix matrix_one(const size_t rows, const size_t cols)  //единичная
{
    Matrix C = matrix_zero(rows, cols);
    size_t counter = 0;

    for (size_t index = 0; index < rows; index++) {
        C.data[index * rows + index] = 1.0;
    }

    return C;
}

//summ
Matrix matrix_summ(const Matrix A, const Matrix B)
{
    if(A.rows != B.rows || A.cols != B.cols){
       matrix_exception(WARNING, "The dimensions of the matrices are not suitable for addition");
       return MATRIX_NULL;
    }
    Matrix C = matrix_allocate(A.rows, A.cols);
    for (size_t index = 0; index < C.rows*C.cols; index++){
        C.data[index] = A.data[index] + B.data[index];
    }
    return C;
}

//subs
Matrix matrix_substract(const Matrix A, const Matrix B)
{
    if(A.rows != B.rows || A.cols != B.cols){
        matrix_exception(WARNING, "The dimensions of the matrices are not suitable for addition");
        return MATRIX_NULL;
    }
    Matrix C = matrix_allocate(A.rows, A.cols);
    for (size_t index = 0; index< C.rows * C.cols; index++){
        C.data[index]=A.data[index] - B.data[index];
    }
    return C;
}

//multiply
Matrix matrix_multiply(const Matrix A, const Matrix B)
{
    if (A.cols!=B.rows){
        matrix_exception(WARNING, "Number of rows and columns do not match");
        return MATRIX_NULL;
    }
    Matrix C = matrix_allocate(A.rows, B.cols);
    for (size_t indexr = 0; indexr<C.rows; indexr++){
        for (size_t indexc = 0; indexc<C.cols; indexc++){
            C.data[indexr*B.cols + indexc];
            for (size_t index = 0; index<A.cols; index++){
                C.data [indexr*C.cols + indexc] += A.data [indexr*A.cols + index]* B.data[index * B.cols + indexc];
            }
        }
    }
    return C;
}

//multiply K
Matrix matrix_k(const Matrix A, double k)
{
    Matrix C = matrix_allocate(A.rows, A.cols);
    for(size_t index = 0; index < C.rows * C.cols; index++) {
        C.data[index] = A.data[index] * k;
    }
    return C;
}

//T 
Matrix matrix_transposition(const Matrix A)
{
    Matrix C = matrix_allocate(A.rows, A.cols);
    for (size_t r = 0; r < C.rows; r++) {
        for (size_t c = 0; c < C.cols; c++) {
            C.data[c * C.cols + r] = A.data[r * A.cols + c];
        }
   }    
    
    return C;
}

//pow
Matrix matrix_pow(Matrix A, const unsigned int power)
{
    if (A.rows != A.cols) {
        matrix_exception(WARNING, "The dimensions of the matrices are not suitable\n");
        return MATRIX_NULL;
    }

    Matrix B = matrix_one(A.rows, A.cols);

    if (A.data==0) {
        matrix_exception(WARNING, "Memory not allocated to matrix\n");
        return MATRIX_NULL;
    }
    
    for (unsigned int n = 0; n < power; n++) {
        Matrix C = matrix_multiply(B, A);
        matrix_free(&B);
        B = C;
    }

    return B;
}
//det 
double matrix_determinant(const Matrix A)
{
    if (A.rows!=A.cols){
        matrix_exception(WARNING, "You need a square matrix to find determinant");
        return NAN;
    }
    if (A.rows == 1){
        return A.data[0];
    }
    if (A.rows == 2){
        return A.data[0]*A.data[3]-A.data[1]*A.data[2];
    }
    if (A.rows == 3){
        return A.data[0] * (A.data[4] * A.data[8] - A.data[5] * A.data[7]) -
               A.data[1] * (A.data[3] * A.data[8] - A.data[5] * A.data[6]) +
               A.data[2] * (A.data[3] * A.data[7] - A.data[4] * A.data[6]);
    }
}

//exp 

Matrix matrix_exp(Matrix A,const unsigned int accuracy)    //эксп
{
    if (A.rows!=A.cols){
        matrix_exception(WARNING, "You need a square matrix to find determinant");
        return MATRIX_NULL;
    }
    
    Matrix E = matrix_allocate(A.rows, A.cols);
    Matrix B = matrix_one(A.cols, A.rows);
    
    double fact = 1.0;

    for (int n = 1; n <= accuracy; n++) {
        fact *= n;
        Matrix P = matrix_pow(A, n);
        Matrix M = matrix_k(P, 1/fact);
        matrix_free(&P);
        Matrix C = matrix_summ(B, M);
        matrix_free(&B);
        B = C;
        matrix_free(&M);
    }

    return B;
}


//all
int main()
{
    double data_A[4] = {1, 2, 3, 4};
    double data_B[4] = {1, 2, 3, 4};

    Matrix A = matrix_allocate(2, 2);
    Matrix B = matrix_allocate(2, 2);

    matrix_set(A, data_A);
    matrix_set(B, data_B);

    if (A.data == NULL || B.data == NULL) {
        printf("Ошибка выделения памяти для одной из матриц.n");
        return 1; 
    }

    printf("A=\n");
    matrix_print(A);
    printf("B=\n");
    matrix_print(B);

    // Сложение
    printf("Addition:\n");
    matrix_print(matrix_summ(A, B));

    // Вычитание
    printf("Subtraction:\n");
    matrix_print(matrix_substract(A, B));

    // Умножение матриц
    printf("Multiplication:\n");
    matrix_print(matrix_multiply(A, B));

    // Траспонирование
    printf("Transposition:\n");
    matrix_print(matrix_transposition(A));

    // Умножение на число
    double k = 5;
    printf("A multiply %2.f:\n", k);
    matrix_print(matrix_k (A, k));
    
    // Определитель
    printf("Determinant A: %2.f \n", matrix_determinant(A));

    //Степень
    printf("Pow: \n");
    matrix_print (matrix_pow(A));

    // Экспонента
    printf("Exp A:\n");
    matrix_print(matrix_exp(A, 10));

    matrix_free(&A);
    matrix_free(&B);

}

