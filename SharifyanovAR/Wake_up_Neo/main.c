#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include <math.h>


struct Matrix
{
    size_t rows;
    size_t cols;
    double* data;
};
typedef struct Matrix Matrix;

enum MatrixExceptionLevel 
{ 
    ERROR, 
    WARNING, 
    INFO, 
    DEBUG 
};

const Matrix MATRIX_NULL = { 0, 0, NULL };


void matrix_exception(const enum MatrixExceptionLevel level, const char* location, const char* msg)
{
    if (level == ERROR) {
        printf("\nERROR\nLoc: %s\nText: %s\n", location, msg);
    }

    if (level == WARNING) {
        printf("\nWARNING\nLoc: %s\nText: %s\n", location, msg);
    }

    if (level == INFO) {
        printf("\nINFO\nLoc: %s\nText: %s\n", location, msg);
    }

    if (level == DEBUG) {
        printf("\nDEBUG\nLoc: %s\nText: %s\n", location, msg);
    }

}

// Function returns MATRIX_NULL if fail (ноль строк/столбцов/переполнение по компонентам/переполнние по памяти)
struct Matrix matrix_allocate(const size_t rows, const size_t cols)
{
    Matrix M;

    if (rows == 0 || cols == 0) {
       // matrix_exception(INFO, "There are zero cols or rows in Matrix");
        return (Matrix) { rows, cols, NULL };
    }

    size_t size = rows * cols;
    if (size / rows != cols) {
       // matrix_exception(ERROR, "Overflow of components");
        return MATRIX_NULL;
    }

    size_t size_in_bytes = size * sizeof(double);

    if (size_in_bytes / sizeof(double) != size) {
        //matrix_exception(ERROR, "Overflow of allocate memory");
        return MATRIX_NULL;
    }

    M.data = malloc(rows * cols * sizeof(double));

    if (M.data == NULL) {
       // matrix_exception(ERROR, "Error in memory allocation");
        return MATRIX_NULL;
    }

    M.rows = rows;
    M.cols = cols;
    return M;
}


struct Matrix matrix_create(const size_t rows, const size_t cols, const double* values) {
    struct Matrix A = matrix_allocate(rows, cols);
    if (A.data == NULL) return A;

    if (values != NULL) {
        memcpy(A.data, values, rows * cols * sizeof(double));
    }
    else {
        memset(A.data, 0, rows * cols * sizeof(double)); 
    }
    return A;
}


void matrix_free(struct Matrix* A)
{
    free(A->data);
    *A = MATRIX_NULL;
}


// A += B
int matrix_add(struct Matrix* A, const struct Matrix* B) {
    if (A == NULL || B == NULL) {
      //  matrix_exception(ERROR, "Pointer on matrix arguments is NULL");
        return 1;
    }
    if (A->rows != B->rows || A->cols != B->cols) return 1;

    for (size_t idx = 0; idx < A->cols * A->rows; idx++) {
        A->data[idx] += B->data[idx];
    }
    return 0;
}


// C = A + B
struct Matrix matrix_sum(const struct Matrix A, const struct Matrix B)
{
    if (A.rows != B.rows || A.cols != B.cols) return MATRIX_NULL;

    struct Matrix C = matrix_allocate(A.rows, A.cols);
    memcpy(C.data, 0, A.rows * A.cols * sizeof(double));
    if (C.data == NULL) return MATRIX_NULL;

    memcpy(C.data, A.data, C.cols * C.rows * sizeof(Matrix));
    matrix_add(&C, &B);
    return C;
}


// A -= B
int matrix_subtraction(struct Matrix* A, const struct Matrix* B) {
    if (A == NULL || B == NULL) {
       // matrix_exception(ERROR, "Pointer on matrix arguments is NULL");
        return 1;
    }
    if (A->rows != B->rows || A->cols != B->cols) return 1;

    for (size_t idx = 0; idx < A->cols * A->rows; idx++) {
        A->data[idx] -= B->data[idx];
    }
    return 0;
}


// C = A - B 
struct Matrix matrix_diff(const struct Matrix A, const struct Matrix B)
{
    if (A.rows != B.rows || A.cols != B.cols) return MATRIX_NULL;

    struct Matrix C = matrix_allocate(A.rows, A.cols);
    memcpy(C.data, 0, A.rows * A.cols * sizeof(double));
    if (C.data == NULL) return C;


    memcpy(C.data, A.data, C.cols * C.rows * sizeof(Matrix));
    matrix_subtraction(&C, &B);

    return C;
}


// coeff * A
struct Matrix matrix_mult_scalar(const struct Matrix A, const double scalar)
{
    struct Matrix C = matrix_allocate(A.rows, A.cols);
    //memcpy(C.data, 0, A.rows * A.cols * sizeof(double));
    if (C.data == NULL) return MATRIX_NULL;

    for (size_t idx = 0; idx < A.cols * A.rows; idx++) {
        C.data[idx] = scalar * A.data[idx];
    }
    return C;
}

double matrix_trace(const struct Matrix A) {
    double sum = 0;
    /*
    if (A.data = NULL) {
      matrix_exception(ERROR, "Matrix pointer os NULL");
       return 1;
     }
     */

    if (A.rows != A.cols) return 1;

    for (size_t idx = 0; idx < A.rows; idx++) {
       sum += A.data[idx*A.rows+idx];
    }
    return sum;
}

// C = A * B
struct Matrix matrix_mult(const struct Matrix A, const struct Matrix B) {
    if (A.cols != B.rows) return MATRIX_NULL;

    struct Matrix C = matrix_allocate(A.rows, B.cols);
    memcpy(C.data, 0, A.rows * A.cols * sizeof(double));
    if (C.data == NULL) return MATRIX_NULL;

    for (size_t i = 0; i < A.rows; i++) {
        for (size_t j = 0; j < B.cols; j++) {
            C.data[i * B.cols + j] = 0.0;
            for (size_t k = 0; k < A.cols; k++) {
                C.data[i * B.cols + j] += A.data[i * A.cols + k] * B.data[k * B.cols + j];
            }
        }
    }
    return C;
}


struct Matrix matrix_transp(const struct Matrix A)
{
    struct Matrix C = matrix_allocate(A.cols, A.rows);
    memcpy(C.data, 0, A.rows * A.cols * sizeof(double));
    if (C.data == NULL)
        return C;

    for (size_t row = 0; row < A.rows; ++row)
        for (size_t col = 0; col < A.cols; ++col)
            C.data[row * A.cols + col] = A.data[row + col * A.cols];

    return C;
}


double det(const struct Matrix A) {
    if (A.rows != A.cols) return NAN;

    if (A.cols == 1) return A.data[0];
    if (A.cols == 2) return A.data[0] * A.data[3] - A.data[1] * A.data[2];

    double determinant = 0.0;
    for (size_t col = 0; col < A.cols; col++) {
        struct Matrix sub = matrix_allocate(A.rows - 1, A.cols - 1);
        if (sub.data == NULL) return NAN;

        
        for (size_t i = 1; i < A.rows; i++) {
            for (size_t j = 0, sub_col = 0; j < A.cols; j++) {
                if (j == col) continue;
                sub.data[(i - 1) * sub.cols + sub_col++] = A.data[i * A.cols + j];
            }
        }
        determinant += (col % 2 == 0 ? 1 : -1) * A.data[col] * det(sub);
        matrix_free(&sub);
    }
    return determinant;
}


struct Matrix sum_for_matrix_exp(struct Matrix A, unsigned int level)
{
    struct Matrix S = MATRIX_NULL, C = MATRIX_NULL;
    double n = 1.0;

    struct Matrix SUM = matrix_allocate(A.rows, A.cols);
    memcpy(SUM.data, 0, A.rows * A.cols * sizeof(double));
    if (SUM.data == NULL) {
        matrix_free(&SUM);
        return MATRIX_NULL;
    }

    memcpy(SUM.data, A.data, SUM.cols * SUM.rows * sizeof(Matrix));

    for (unsigned int counter = 2; counter <= level; counter++) {
        C = matrix_mult(SUM, A);
        if (C.data == NULL) {
            matrix_free(&C);
            matrix_free(&SUM);
            matrix_free(&S);
            return MATRIX_NULL;
        }
        memcpy(SUM.data, C.data, SUM.cols * SUM.rows * sizeof(Matrix));
        matrix_free(&C);
    }


    for (unsigned int counter = 1; counter <= level; counter++) n *= counter;

    S = matrix_mult_scalar(SUM, 1 / n);
    matrix_free(&SUM);
    return S;
}


struct Matrix matrix_E(const struct Matrix A)
{
    struct Matrix E = matrix_allocate(A.cols, A.rows);
    memcpy(E.data, 0, A.rows * A.cols * sizeof(double));
    if (E.data == NULL) return MATRIX_NULL;

    for (int idx = 0; idx < A.rows * A.cols; idx++) E.data[idx] = 0;
    for (int idx = 0; idx < A.rows; idx++) E.data[idx + idx * A.cols] = 1.;

    return E;
}


// C = e ^ (A)
struct Matrix matrix_exp(struct Matrix A, unsigned long int level)
{
    struct Matrix C = MATRIX_NULL, SUMEXP = MATRIX_NULL;

    if (A.rows != A.cols) return MATRIX_NULL;

    SUMEXP = matrix_E(A);
    matrix_add(&SUMEXP, &A);

    for (unsigned int count = 2; count <= level; count++) {
        C = sum_for_matrix_exp(A, count);
        if (C.data == NULL) {
            matrix_free(&C);
            matrix_free(&SUMEXP);
            return MATRIX_NULL;
        }
        matrix_add(&SUMEXP, &C);
        matrix_free(&C);
    }


    return SUMEXP;
}


void matrix_print(const struct Matrix A)
{
    for (size_t row = 0; row < A.rows; ++row) {
        printf("[ ");
        for (size_t col = 0; col < A.cols; ++col) {
            printf("%4.2f ", A.data[A.cols * row + col]);
        }
        printf("]\n");
    }
    printf("\n");
}


int main()
{

    struct Matrix B, C, C2, C3;

    printf("\nFirst matrix\n");
    struct Matrix A = matrix_create(3, 3, (double[]) { 1, 2, 3, 4, 5, 6, 7, 8, 9});
    matrix_create(A.cols, A.rows, &A);
    matrix_print(A);

    printf("Multiplying the first matrix by a scalar\n");
    C = matrix_mult_scalar(A, 2);
    matrix_print(C);

    
   // double sum_test = matrix_trace(A);
    printf("%4.2f ", matrix_trace(A));

    
    printf("Second matrix\n");
    struct Matrix B = matrix_create(3, 3, (double[]) { 3, 6, 2, 6, 0, -2, 9, 3, 0 });
    matrix_create(B.cols, B.rows, &B);
    matrix_print(B);

    printf("Sum1 of matrices\n");
    matrix_add(&A, &B);
    matrix_print(A);

    printf("Sum2 of matrices\n");
    C = matrix_sum(A, B);
    matrix_print(C);


    printf("Sub1 of matrices\n");
    matrix_subtraction(&A, &B);
    matrix_print(A);

    printf("Sub2 of matrices\n");
    C = matrix_diff(A, B);
    matrix_print(C);



    printf("Multiplying the first matrix by a second matrix\n");
    C = matrix_mult(A, B);
    matrix_print(C);

    printf("Transposed first matrix\n");
    C = matrix_transp(A);
    matrix_print(C);

    printf("Determinant of the first matrix\n");
    det(A);
    printf("%4.2f \n", det(A));

    printf("\nExponent of the first matrix\n");
    sum_for_matrix_exp(A, 2);
    C3 = matrix_exp(A, 2);
    matrix_print(C3);

    matrix_free(&A);
    matrix_free(&B);
    matrix_free(&C);
    matrix_free(&C3);
    
    return 0;
}