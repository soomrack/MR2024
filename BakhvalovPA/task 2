#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

struct Matrix {
    size_t rows;
    size_t cols;
    double* data;
};

typedef struct Matrix Matrix;
enum MatrixExceptionLevel { ERROR, INFO, DEBUG };


void matrix_exception(const enum MatrixExceptionLevel level, const char* msg)
{
    if (level == ERROR) {
        printf("ERROR: %s \n", msg);
    }
}


struct Matrix matrix_allocation(const size_t rows, const size_t cols)
{
    if (cols == 0 || rows == 0)
        return Matrix{ 0, 0, NULL };
    if (SIZE_MAX / cols / rows / sizeof(double) == 0)
        return Matrix{ 0, 0, NULL };

    double* data = (double*)malloc(rows * cols * sizeof(double));

    if (data == NULL) {
        matrix_exception(ERROR, "matrix allocation : ERROR data NULL");
        return Matrix{ 0, 0, NULL };
    }
    return Matrix{ cols, rows, data };
}


void matrix_output(const struct Matrix A)
{
    for (size_t col = 0; col < A.cols; ++col) {
        printf("| ");
        for (size_t row = 0; row < A.rows; ++row) {
            printf("%lf ", A.data[row * A.cols + col]);
        }
        printf("|\n");
    }
}


void matrix_free(struct Matrix* A)
{
    if (A == NULL) return;
    free(A->data);
    A->data = NULL;
    A->rows = 0;
    A->cols = 0;
}


void matrix_random(Matrix A)
{
    for (size_t idx = 0; idx < A.cols * A.rows; idx++) {
        A.data[idx] = rand() % 10;
    }
}


Matrix matrix_zero(Matrix A)
{
    if (A.data != NULL) {
        memset(A.data, 0, sizeof(double) * A.rows * A.cols);
    }
    return A;
}


Matrix matrix_identity(size_t size)
{
    Matrix identity = matrix_allocation(size, size);
    if (identity.data == NULL) {
        matrix_exception(ERROR, "matrix identity: error data");
        return Matrix{ 0, 0, NULL };
    }
    matrix_zero(identity);
    for (size_t idx = 0; idx < identity.rows * identity.cols; idx += identity.rows + 1)
    {
        identity.data[idx] = 1.0;
    }
    return identity;
}


// C = A + B
Matrix matrix_sum(const struct Matrix A, const struct Matrix B)
{
    if (A.rows != B.rows || A.cols != B.cols) {
        matrix_exception(ERROR, "Matrix sum : Incorrect matrix size");
        return Matrix{ 0, 0, NULL };
    }
    Matrix add = matrix_allocation(A.rows, A.cols);
    for (size_t idx = 0; idx < add.rows * add.cols; ++idx) {
        add.data[idx] = A.data[idx] + B.data[idx];
    }
    return add;
}


// C = A - B
Matrix matrix_sub(const struct Matrix A, const struct Matrix B)
{
    if (A.rows != B.rows || A.cols != B.cols) {
        matrix_exception(ERROR, "Matrix subtraction: Incorrect matrix size");
        return Matrix{ 0, 0, NULL };
    }
    Matrix sub = matrix_allocation(A.rows, A.cols);
    for (size_t idx = 0; idx < sub.rows * sub.cols; ++idx) {
        sub.data[idx] = A.data[idx] - B.data[idx];
    }
    return sub;
}


// C = A * B
Matrix matrix_multiplication(const struct Matrix A, const struct Matrix B)
{
    if (A.cols != B.rows) {
        matrix_exception(ERROR, "Matrix multiplication: The number of columns is not equal to the number of rows");
        return Matrix{ 0, 0, NULL };
    }
    Matrix multi = matrix_allocation(A.rows, B.cols);
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


// A = A * const
Matrix matrix_multiplication_x(const Matrix A, const double x)
{
    Matrix M = matrix_allocation(A.rows, A.cols);
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
        return NULL;
    }

    if (A.rows == 1) {
        return A.data[0];
    }
    else if (A.rows == 2) {
        return A.data[0] * A.data[3] - A.data[1] * A.data[2];
    }
    else if (A.rows == 3) {
        return A.data[0] * (A.data[4] * A.data[8] - A.data[5] * A.data[7]) -
            A.data[1] * (A.data[3] * A.data[8] - A.data[5] * A.data[6]) +
            A.data[2] * (A.data[3] * A.data[7] - A.data[4] * A.data[6]);
    }
}


Matrix matrix_power(Matrix A, const long long int n)
{
    if (A.rows != A.cols || n < 0) {
        matrix_exception(ERROR, "Matrix power : Incorrect matrix size");
        return Matrix{ 0, 0, NULL };
    }

    Matrix matrix_powered_to_n = matrix_allocation(A.rows, A.cols);;
    matrix_powered_to_n = A;
    for (long long int power = 1; power < n; power++) {
        matrix_powered_to_n = matrix_multiplication(matrix_powered_to_n, A);
    }

    return matrix_powered_to_n;
}


Matrix matrix_exponent(const Matrix& A, const long long int x) {
    if (A.rows != A.cols) {
        matrix_exception(ERROR, "Matrix exponent: Incorrect matrix siz");
        return Matrix{ 0, 0, NULL };
    }

    Matrix result = matrix_identity(A.rows);
    Matrix term = matrix_identity(A.rows);

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
    A = matrix_allocation(3, 3);
    /* matrix_set(A, (double[]) {
           1., 2.,
           3., 4.,
           5., 6.,
       });*/
    matrix_random(A);
    printf("Matrix A\n");
    matrix_output(A);

    struct Matrix B;
    B = matrix_allocation(3, 3);
    matrix_random(B);
    printf("Matrix B\n");
    matrix_output(B);


    Matrix add = matrix_sum(A, B);
    printf("Matrix +\n");
    matrix_output(add);


    Matrix sub = matrix_sub(A, B);
    printf("Matrix -\n");
    matrix_output(sub);


    Matrix multi = matrix_multiplication(A, B);
    printf("Matrix multiplication\n");
    matrix_output(multi);


    struct Matrix D;
    D = matrix_allocation(A.rows, A.cols);
    matrix_transposition(D, A);
    printf("Matrix transposition\n");
    matrix_output(D);


    Matrix multi_x = matrix_multiplication_x(A, 2);
    printf("Matrix * x \n");
    matrix_output(multi_x);

    Matrix iden = matrix_identity(5);
    printf("Matrix identity\n");
    matrix_output(iden);

    Matrix exponent = matrix_exponent(A, 20);
    printf("Matrix exponent :\n");
    matrix_output(exponent);

    double matrix_det = matrix_determinant(A);
    printf("Matrix determinant:%lf\n", matrix_det);

    matrix_free(&A);
    matrix_free(&B);
    matrix_free(&D);
    matrix_free(&multi);
    matrix_free(&multi_x);
    matrix_free(&add);
    matrix_free(&sub);
    matrix_free(&exponent);
    return 0;
}
