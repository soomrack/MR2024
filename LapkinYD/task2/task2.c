#include "matrix.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define GET_VARIABLE_NAME(Variable) (#Variable)

int define_matrices(Matrix *A,  Matrix *B)
{
    size_t A_rows = 2;
    size_t A_cols = 2;

    ////double A_elements[8] =
    //{ 
    //    3.0, 4.9, 1.333, 4.5, 
    //   5.1, 1.2, 9.4, 1.33 
    //};

    //double A_elements[6] = { 1.5, 2.0, 5.6, 7.9, 33, 9.8};
    double A_elements[4] = { 1.5, 2.0, 5.6, 7.9};

    size_t B_rows = 2;
    size_t B_cols = 2;
    double B_elements[4] = 
    { 
        2.7, 1.8, 
        0.0, 0.99 
    };
    
    MATRIX_EXCEPTION exc = malloc_matrix(A, A_rows, A_cols);
    if (exc != OK){
        exception_handler(exc, __func__);
        return 1;
    }
    size_t size = sizeof(A_elements);
    exc = set_data(A, A_elements, size);
    if (exc != OK){
        exception_handler(exc, __func__);
        return 1;
    }
    exc = malloc_matrix(B, B_rows, B_cols);
    if (exc != OK){
        exception_handler(exc, __func__);
        return 1;
    }
    size = sizeof(B_elements);
    exc = set_data(B, B_elements, sizeof(B_elements));
    if (exc != OK){
        exception_handler(exc, __func__);
        return 1;
    }
}


void test_print(Matrix *A, const char *name_mtx)
{
    printf("Matrix %s:\n", name_mtx);
    MATRIX_EXCEPTION exc = print_matrix(A);
    if (exc != OK){
        //printf("Test 'test_print' canceled with error\n");
        exception_handler(exc, __func__);
    }
    printf("------------------\n");
}


void test_sum(Matrix *A, Matrix *B, Matrix *C)
{
    printf("Matrix SUM:\n");
    MATRIX_EXCEPTION exc = summarize(C, A, B);
    if (exc != OK){
        exception_handler(exc, __func__);
        return;
    }
    //printf("Test 'test_sum' canceled with error\n");
    print_matrix(C);
    printf("------------------\n");
}


void test_sub(Matrix *A, Matrix *B, Matrix *C)
{
    printf("Matrix SUB:\n");
    MATRIX_EXCEPTION exc = subtract(C, A, B);
    if (exc != OK){
        exception_handler(exc, __func__);
        return;
    }
    //printf("Test 'test_sub' canceled with error\n");
    print_matrix(C);
    printf("------------------\n");
}


void test_multiply(Matrix *A, Matrix *B, Matrix *C)
{
    printf("Matrix MULTIPLY:\n");
    MATRIX_EXCEPTION exc = multiply_matrices(C, A, B);
    if (exc != OK){
        exception_handler(exc, __func__);
        return;
    }
    print_matrix(C);
    printf("------------------\n");
    //printf("Test 'test_multiply' canceled with error\n");
}


void test_multiply_by_number(Matrix *A, double number)
{
    printf("Matrix MULTYPLY_BY_NUMBER:\n");
    multiply_by_number(A, number);
    print_matrix(A);
    printf("------------------\n");
    //printf("Test 'test_multiply_by_number' canceled with error\n");
}


void test_copy_mtx(Matrix *A, Matrix *C)
{
    printf("Matrix COPY:\n");
    MATRIX_EXCEPTION exc = copy_matrix(C, A);
    if (exc != OK){
        exception_handler(exc, __func__);
        return;
    }
    print_matrix(C);
    printf("------------------\n");
}


void test_identity(Matrix *A, Matrix *C)
{
    printf("Matrix IDENTITY:\n");
    MATRIX_EXCEPTION exc = get_identity_matrix(C, A->cols, A->rows);
    if (exc != OK){
        exception_handler(exc, __func__);
        return;
    }
    print_matrix(C);
    printf("------------------\n");
    //printf("Test 'test_identity_mtx' canceled with error\n");
}


void test_transpose(Matrix *A, Matrix *C)
{
    printf("Matrix TRANSPOSE:\n");
    MATRIX_EXCEPTION exc = transpose(C, A);
    if (exc != OK){
        exception_handler(exc, __func__);
        return;
    }
    print_matrix(C);
    printf("------------------\n");
    //printf("Test 'test_transpose' canceled with error\n");
}


void test_det(Matrix *A)
{
    double determinant;
    MATRIX_EXCEPTION exc = get_determinant(A, &determinant);
    if (exc != OK){
        exception_handler(exc, __func__);
        return;
    }
    printf("Matrix DETERMINANT: %lf\n", determinant);
    printf("------------------\n");
}


void test_inverse(Matrix *A, Matrix *C)
{
    MATRIX_EXCEPTION exc = invert_matrix(C, A);
    printf("Matrix INVERSE:\n");
    if (exc != OK){
        exception_handler(exc, __func__);
        return;
    }
    print_matrix(C);
    printf("------------------\n");
    //printf("Test 'test_inverse' canceled with error\n");
}


void test_pow(Matrix *A, Matrix *C, signed int power)
{
    MATRIX_EXCEPTION exc = rase_to_power(C, A, power);
    printf("Matrix POW:\n");
    if (exc != OK){
        exception_handler(exc, __func__);
        return;
    }
    print_matrix(C);
    printf("------------------\n");
    //printf("Test 'test_pow' canceled with error\n");
}


void test_exp(Matrix *A, Matrix *C)
{
    MATRIX_EXCEPTION exc = get_exp(C, A);
    printf("Matrix EXP:\n");
    if (exc != OK){
        exception_handler(exc, __func__);
        return;
    }
    print_matrix(C);
    printf("------------------\n");
    //printf("Test 'test_exp' canceled with error\n");
}

int main() 
{
    Matrix A, B, C, D, E, T, Y, U, I, O, P;

    if (define_matrices(&A, &B)){
        printf("Program is end :(\n");
        return 1;
    }
    
    test_print(&A, "A");
    test_print(&B, "B");

    test_sum(&A, &B, &C);
    test_sub(&A, &B, &D);

    int number = 5;

    test_copy_mtx(&A, &E);
    test_multiply_by_number(&E, number);

    test_identity(&A, &T);
    test_transpose(&A, &Y);
    test_det(&A);
    test_inverse(&A, &U);
    int power = 2;
    test_pow(&A, &I, power);
    test_exp(&A, &O);

    test_multiply(&A, &B, &P);

    free(A.data);
    free(B.data);
    free(C.data);
    free(D.data);
    free(E.data);
    free(T.data);
    free(Y.data);
    free(U.data);
    free(I.data);
    free(O.data);
    free(P.data);
    return 0;
}