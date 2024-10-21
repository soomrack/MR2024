#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <math.h>


struct Matrix
{
    size_t cols;
    size_t rows;
    double* data;
};

const struct Matrix MATRIX_NULL = {0, 0, NULL};


// Сообщение об ошибке
void matrix_error (const char* Er_type)
{
	printf("\nError!\nProblem: %s\n", Er_type);
}


// Инициализация матрицы
struct Matrix matrix_alloc (const size_t rows, const size_t cols)
{
    if (cols == 0 || rows == 0) {
		matrix_error("zero value");
        return MATRIX_NULL;
	}

    if (rows * cols >= SIZE_MAX / sizeof(double)) {
        matrix_error("exceeded the size");
        return MATRIX_NULL;
    }

    struct Matrix A;
	A.cols = cols;
	A.rows = rows;

    A.data = malloc(A.cols * A.rows * sizeof(double));

    if (A.data == NULL) {
        matrix_error("no memory allocated");
        return MATRIX_NULL;
    }
    return A;
}


// Освобождение памяти
void matrix_free (struct Matrix *A)
{
    if (A->data == NULL) {
        return;
    }

    free(A->data);
    *A = MATRIX_NULL;
}


// Нулевая матрица
void matrix_zero (const struct Matrix A)
{
    if (A.data == NULL) {
        matrix_error("no memory allocated");       
        return;
    }

    memset(A.data, 0, A.cols * A.rows * sizeof(double));
}


// Единичная матрица
struct Matrix matrix_identity (const struct Matrix A)
{
    if (A.rows != A.cols) {
        matrix_error("a non-square matrix");
        return MATRIX_NULL;
    }
    
    if (A.data == NULL) {
        matrix_error("no memory allocated");
        return MATRIX_NULL;
    }

    matrix_zero(A);
    for (size_t idx = 0; idx < A.rows * A.cols; idx += A.cols + 1) {
        A.data[idx] = 1.0;
    }
    return A;
}


void matrix_print(const struct Matrix A)
{
    printf("_____________________________\n");
    for (size_t row = 0; row < A.rows; ++row) {
        for (size_t col = 0; col < A.cols; ++col) {
            printf(" %.2f ", A.data[A.cols*row + col]);
        }
        printf("\n");
    }
    printf("\n");
}


// A = A + B
void matrix_add (struct Matrix A, struct Matrix B)
{
    if ((A.cols != B.cols) || (A.rows != B.rows )) {
        matrix_error("matrices of different dimensions");
        return;
    }
    
    if (A.data == NULL || B.data == NULL) {
        matrix_error("no memory allocated for one of the transferred matrices");
        return;
    }

    for (size_t idx = 0; idx < A.cols * A.rows; ++idx) {
		A.data[idx] += B.data[idx];
	}
}


// C = A + B
struct Matrix matrix_sum(const struct Matrix A, const struct Matrix B)
{
    if ((A.cols != B.cols) || (A.rows != B.rows )) {
        matrix_error("matrices of different dimensions");
        return MATRIX_NULL;
    }

    struct Matrix C = matrix_alloc (A.rows, A.cols);

    if (C.data == NULL) {
        matrix_error("no memory allocated");
        return MATRIX_NULL;
    }

    memcpy(C.data, A.data, C.cols * C.rows * sizeof(double));
    matrix_add(C, B); 
    return C;
}


// C = A - B 
struct Matrix matrix_sub(const struct Matrix A, const struct Matrix B)
{
    if ((A.cols != B.cols) || (A.rows != B.rows )) {
        matrix_error("matrices of different dimensions");
        return MATRIX_NULL;
    }

    struct Matrix C = matrix_alloc (A.rows, A.cols);

    if (C.data == NULL) {
        matrix_error("no memory allocated");
        return MATRIX_NULL;
    }
    
    for (size_t idx = 0; idx < A.cols * A.rows; ++idx) {
		C.data[idx] = A.data[idx] - B.data[idx];
    }
    return C;
}


// A = A * k
void matrix_mult_k (struct Matrix A, double k)
{   
    if (A.data == NULL) {
        matrix_error("matrix data is NULL");
        return;
    }

    for (size_t idx = 0; idx < A.cols * A.rows; ++idx) {
		A.data[idx] *= k;
	}
}


// C = A * B
struct Matrix matrix_mult (const struct Matrix A, const struct Matrix B)
{
    if (A.cols != B.rows) {
        matrix_error("incorrect matrix size");
        return MATRIX_NULL;
    }

    struct Matrix C = matrix_alloc(A.rows, B.cols);

    if (C.data == NULL) {
        matrix_error("no memory allocated");
        return MATRIX_NULL;
    }

    for (size_t rowA = 0; rowA < A.rows; ++rowA) {
        for (size_t colB = 0; colB < B.cols; ++colB) {
            C.data[rowA * (B.cols) + colB] = 0;
                
            for (size_t idx = 0; idx < A.cols; idx++) {
                  C.data[rowA * B.cols + colB] += A.data[rowA * A.cols + idx] * B.data[idx * B.cols + colB];
            }
        }
    }
    return C;
}


struct Matrix matrix_transp(const struct Matrix A)
{
    struct Matrix B = matrix_alloc(A.cols, A.rows);

    if (B.data == NULL) {
        matrix_error("no memory allocated");
        return MATRIX_NULL;
    }

    for (size_t row = 0; row < A.rows; ++row) {
        for (size_t col = 0; col < A.cols; ++col) {
            B.data[row + A.rows * col] = A.data[col + A.cols * row];
        }
    }
    return B;
}


double matrix_det (const struct Matrix A)
{
    double det;
    if (A.rows != A.cols) {
        matrix_error("a non-square matrix");
        return NAN;
    }

    if (A.rows == 1) {
        det = A.data[0];
        return det;
    }

    if (A.rows == 2) {
        det = A.data[0] * A.data[3] - A.data[1] * A.data[2];
        return det;
    }

    if (A.rows == 3) {
        det = A.data[0] * A.data[4] * A.data[8] + A.data[1] * A.data[5] * A.data[6] + A.data[3] * A.data[7] * A.data[2] - \
        (A.data [2] * A.data[4] * A.data[6]) - A.data[3] * A.data[1] * A.data[8] - A.data[0] * A.data[5] * A.data[7];
        return det;
    }
    return NAN;
}


struct Matrix matrix_pow(const struct Matrix A, unsigned int power)
{
    if (A.rows != A.cols) {
        matrix_error("a non-square matrix");
        return MATRIX_NULL;
    }

    if (power == 0) {
        return matrix_identity(A); 
    }

    struct Matrix B = matrix_alloc (A.rows, A.cols);

    if (B.data == NULL) {
        matrix_error("no memory allocated");
        return B;
    }

    memcpy(B.data, A.data, A.rows * A.cols * sizeof(double));

    for (unsigned int cur_pow = 1; cur_pow < power; ++cur_pow) {
        struct Matrix tmp = matrix_mult (B,A);
        
        matrix_free(&B);
        B = tmp;
        matrix_free(&tmp);
        //B = matrix_mult(B,A);
    }
    return B;
}


unsigned long long int factorial (const unsigned int num)
{
    unsigned long long int res = 1;
    for (unsigned int i = 1; i <= num; i++) {
        res *= i;
    }

    return res;
}


// e ^ A
struct Matrix matrix_exp(const struct Matrix A, const double accuracy)
{
    if (A.rows != A.cols) {
        matrix_error("a non-square matrix");
        return MATRIX_NULL;
    }

    if (accuracy == 1) {
        return matrix_identity(A);
    }

    struct Matrix E = matrix_alloc(A.rows, A.cols);   
    
    if (E.data == NULL) {
        matrix_error("no memory allocated");
        return E;
    }


	for (size_t cur_acc = 1; cur_acc <= accuracy; ++cur_acc) {
        struct Matrix tmp = matrix_pow (A,cur_acc);
        matrix_mult_k(tmp, 1/factorial(cur_acc));
        
		struct Matrix buf = matrix_sum(E, tmp);
		matrix_free(&E);
        E = buf;
        
		matrix_free(&tmp);
	}
	return E;
}


int main()
{
    struct Matrix A,C,C2,B,D,E,F,T;
    A = matrix_alloc(3,2);
    A.data[0] = 0.0;
    A.data[1] = 2.0;
    A.data[2] = -5.0;
    A.data[3] = 1.0;
    A.data[4] = 4.0;
    A.data[5] = -3.0;

    B = matrix_alloc(2,2);

    for (size_t idx = 0; idx < B.rows * B.cols; ++idx) {
        B.data[idx] = idx;
    }

    matrix_print(A);
    matrix_print(B);
    printf("______________\n");
    matrix_add(A,B);
    matrix_print(A);

    printf("______________\n");
    printf("______________\n");
    C = matrix_sum(A,B);
    matrix_print(C);

    C2 = matrix_sub(A,B);
    matrix_print(C2);

    matrix_mult_k(A,10);
    matrix_print(A);

    matrix_print(B);
    D = matrix_pow(B,4);
    matrix_print(D); 

    printf("Matrix A det is %.2f", matrix_det(A));

    printf("\nD matrix is\n");
    F = matrix_mult(B,B);
    matrix_print(F);
    
    matrix_print(A);
    T = matrix_transp(A);
    matrix_print(T);

    E = matrix_exp(B,2);
    matrix_print(E);

    matrix_free(&A);
    matrix_free(&B);
    matrix_free(&C);
    matrix_free(&C2);
    matrix_free(&D);
    matrix_free(&F);
    matrix_free(&T);
    matrix_free(&E);
}
