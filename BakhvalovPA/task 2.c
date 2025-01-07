#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

struct Matrix {
    size_t rows;
    size_t cols;
    double *data;
};

typedef struct Matrix Matrix;
enum MatrixExceptionLevel { ERROR, WARNING, INFO, DEBUG };


void matrix_exception(const enum MatrixExceptionLevel level, const char *msg)
{
    if (level == ERROR) {
        printf("ERROR: %s \n", msg);
    }
    if (level == WARNING) {
        printf("WARNING: %s \n", msg);
    }
}


struct Matrix matrix_allocate(const size_t rows, const size_t cols)
{
    if (cols == 0 || rows == 0)
        return Matrix{ rows, cols, NULL };
   
    if (SIZE_MAX / cols / rows / sizeof(double) == 0)
        return Matrix{ rows, cols, NULL };

    double *data = (double*)malloc(rows * cols * sizeof(double));

    if (data == NULL) {
        matrix_exception(ERROR, "matrix allocation : ERROR data NULL");
        return Matrix{ rows, cols, NULL };
    }
    return Matrix{ cols, rows, data };
}


void matrix_print(const Matrix A)
{
    for (size_t row = 0; row < A.rows; ++row) {
        printf("| ");
        for (size_t col = 0; col < A.cols; ++col) {
            printf("%2.3f ", A.data[row * A.cols + col]);
        }
        printf("|\n");
    }
    printf("\n");
}


void matrix_free(struct Matrix *A)
{
    free(A->data); 
    *A = Matrix{ 0, 0, NULL };
}


void matrix_set(Matrix A)
{
    for (size_t idx = 0; idx < A.cols * A.rows; idx++) {
        A.data[idx] = rand() % 9;
    }
}


Matrix MATRIX_ZERO = { 0, 0, NULL };

Matrix matrix_zero(Matrix A)
{
    if (A.data != NULL) {
        memset(A.data, 0, sizeof(double) * A.rows * A.cols);
    }
    return MATRIX_ZERO;
}


Matrix matrix_identity(const size_t rows, const size_t cols)
{
    Matrix identity = matrix_allocate(rows, cols);
    if (identity.data == NULL) {
        matrix_exception(ERROR, "matrix identity: ERROR data");
        return Matrix{ rows, cols, NULL };
    }
    matrix_zero(identity);
    for (size_t idx = 0; idx < identity.rows * identity.cols; idx += identity.rows + 1)
    {
        identity.data[idx] = 1.0;
    }
    return identity;
}


Matrix matrix_sum(const struct Matrix A, const struct Matrix B)
{
     if( !((A.cols == B.cols) && (A.rows == B.rows)) ) {
        matrix_exception(ERROR, "matrix_add: incompatible sizes.");
        return MATRIX_ZERO;
    }
    Matrix add = matrix_allocate(A.rows, A.cols);
    for (size_t idx = 0; idx < add.rows * add.cols; ++idx) {
        add.data[idx] = A.data[idx] + B.data[idx];
    }
    return add;
}


Matrix matrix_sub(const struct Matrix A, const struct Matrix B)
{
    if (A.rows != B.rows || A.cols != B.cols) {
        matrix_exception(ERROR, "Matrix subtraction: Incorrect matrix size");
        return MATRIX_ZERO;
    }
    Matrix sub = matrix_allocate(A.rows, A.cols);
    for (size_t idx = 0; idx < sub.rows * sub.cols; ++idx) {
        sub.data[idx] = A.data[idx] - B.data[idx];
    }
    return sub;
}


Matrix matrix_multiplication(const struct Matrix A, const struct Matrix B)
{
    if (A.cols != B.rows) {
        matrix_exception(ERROR, "Matrix multiplication: The number of columns is not equal to the number of rows");
        return MATRIX_ZERO;
    }
    Matrix multi = matrix_allocate(A.rows, B.cols);
    for (size_t col = 0; col < multi.cols; col++) {
        for (size_t row = 0; row < multi.rows; row++) {
            double sum = 0;
            for (size_t idx = 0; idx < A.rows; idx++) {
                sum += A.data[idx * A.cols + col] * B.data[row * B.cols + idx];
            }
            multi.data[row * multi.cols + col] = sum;
        }
    }
    return multi;
}


Matrix matrix_multiplication_x(const Matrix A, const double x)
{
    Matrix M = matrix_allocate(A.rows, A.cols);
    for (size_t idx = 0; idx < M.cols * M.rows; ++idx) {
        M.data[idx] = A.data[idx] * x;
    }
    return M;
}


int matrix_transposition(const Matrix D, const Matrix A)
{
    if (A.rows != D.cols || A.cols != D.rows) return 1;
    for (size_t row = 0; row < D.rows; ++row) {
        for (size_t col = 0; col < D.cols; ++col) {
            D.data[row * D.cols + col] = A.data[col * D.rows + row];
        }
    }
    return 0;
}


double matrix_determinant(Matrix A)
{
    if (A.cols != A.rows) {
        matrix_exception(ERROR, "Matrix determinant : Incorrect matrix size");
        return NAN;
    }

    if (A.rows == 1) {
        return A.data[0];
    }
     
    if (A.rows == 2) {
        return A.data[0] * A.data[3] - A.data[1] * A.data[2];
    }
    
    if (A.rows == 3) {
        return A.data[0] * (A.data[4] * A.data[8] - A.data[5] * A.data[7]) -
            A.data[1] * (A.data[3] * A.data[8] - A.data[5] * A.data[6]) +
            A.data[2] * (A.data[3] * A.data[7] - A.data[4] * A.data[6]);
    }
}


Matrix matrix_power(Matrix A, const long long int n)
{
    if (A.rows != A.cols || n < 0) {
        matrix_exception(ERROR, "Matrix power : Incorrect matrix size");
        return MATRIX_ZERO;
    }

    Matrix matrix_powered_to_n = matrix_allocate(A.rows, A.cols);;
    matrix_powered_to_n = A;
    for (long long int power = 1; power < n; power++) {
        matrix_powered_to_n = matrix_multiplication(matrix_powered_to_n, A);
    }

    return matrix_powered_to_n;
}


Matrix matrix_exponent(const Matrix A, const long long int x) {
    if (A.rows != A.cols) {
        matrix_exception(ERROR, "Matrix exponent: Incorrect matrix siz");
        return MATRIX_ZERO;
    }

    Matrix result = matrix_identity(A.rows, A.cols);
    Matrix term = matrix_identity(A.rows, A.cols);

    for (size_t n = 1; n <= x; ++n) {
        Matrix temp = matrix_multiplication(term, A);
        matrix_free(&term);
        term = matrix_multiplication_x(temp, 1.0 / n);
        matrix_free(&temp);
        Matrix new_result = matrix_sum(result, term);
        matrix_free(&result);
        result = new_result;
    }

    matrix_free(&term);
    return result;
}


int main()
{
    struct Matrix A;
    A = matrix_allocate(3, 3);
    matrix_set(A);
    printf("Matrix A\n");
    matrix_print(A);

    struct Matrix B;
    B = matrix_allocate(3, 3);
    matrix_set(B);
    printf("Matrix B\n");
    matrix_print(B);

    Matrix add = matrix_sum(A, B);
    printf("Matrix +\n");
    matrix_print(add);

    Matrix sub = matrix_sub(A, B);
    printf("Matrix -\n");
    matrix_print(sub);

    Matrix multi = matrix_multiplication(A, B);
    printf("Matrix multiplication\n");
    matrix_print(multi);

    struct Matrix D;
    D = matrix_allocate(A.rows, A.cols);
    matrix_transposition(D, A);
    printf("Matrix transposition\n");
    matrix_print(D);

    Matrix multi_x = matrix_multiplication_x(A, 2);
    printf("Matrix * x \n");
    matrix_print(multi_x);

    Matrix iden = matrix_identity(5, 5);
    printf("Matrix identity\n");
    matrix_print(iden);

    Matrix exponent = matrix_exponent(A, 20);
    printf("Matrix exponent :\n");
    matrix_print(exponent);

    double matrix_det = matrix_determinant(A);
    printf("Matrix determinant:%lf\n", matrix_det);

    matrix_free(&A);
    matrix_free(&B);
    matrix_free(&D);
    matrix_free(&multi);
    matrix_free(&multi_x);
    matrix_free(&iden);
    matrix_free(&add);
    matrix_free(&sub);
    matrix_free(&exponent);
    return 0;
}
