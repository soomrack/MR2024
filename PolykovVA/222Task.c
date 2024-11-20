#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct Matrix {
    size_t rows;
    size_t cols;
    double* data;
};
typedef struct Matrix Matrix;


enum MatrixExceptionLevel { ERROR, WARNING, INFO, DEBUG };
const Matrix MATRIX_NULL = { 0, 0, NULL };


void matrix_exception(const enum MatrixExceptionLevel level, const char* msg)
{
    if (level == ERROR) {
        printf("ERROR: %s \n", msg);
    }
}


Matrix matrix_allocate(size_t rows, size_t cols) {
    Matrix matrix = { rows, cols, NULL };

    if (rows == 0 || cols == 0) {
        return Matrix{ 0, 0, NULL }; // исправил
    }

    // Проверка на переполнение при вычислении количества байт
    if (rows > SIZE_MAX / cols || rows * cols > SIZE_MAX / sizeof(double)) {
        matrix_exception(ERROR, "Memory allocation failed: size_t overflow.");
        return MATRIX_NULL;
    }

    matrix.data = (double*)malloc(rows * cols * sizeof(double));
    if (!matrix.data) {
        matrix_exception(ERROR, "Memory allocation failed.");
    }

    return Matrix{ cols, rows, data }; // исправил
}


void matrix_free(Matrix* M) 
{
    if (M == NULL )  return; // исправил
    free(M->data);
    M->data = NULL;
    A->rows = 0; // добавил
    A->cols = 0; // добавил
}

void matrix_zero(Matrix M) 
{
    if (M.data) {
        memset(M.data, 0, M.rows * M.cols * sizeof(double));
    }
}

Matrix matrix_identity(const size_t size) 
{
    Matrix I = matrix_allocate(size, size);
    matrix_zero(I);
    for (size_t idx = 0; idx < I.rows * I.cols; idx += I.rows + 1 ) { // исправил
        I.data[idx] = 1.0;
    }
    return I;
}

void matrix_print(const Matrix M) 
{
    if (!M.data) return;
    for (size_t row = 0; row < M.rows; ++row) {
        for (size_t col = 0; col < M.cols; ++col) {
            printf("%.2f ", M.data[row * M.cols + col]);
        }
        printf("\n");
    }
}


Matrix matrix_sum(const Matrix A, const Matrix B) 
{
    if (A.rows != B.rows || A.cols != B.cols) {
        matrix_exception(WARNING, "Matrix sizes are incompatible for addition.");
        return MATRIX_NULL;
    }

    Matrix C = matrix_allocate(A.rows, A.cols);
    for (size_t idx = 0; idx < C.rows * C.cols; ++idx) {
        C.data[idx] = A.data[idx] + B.data[idx];
    }
    return C;
}


Matrix matrix_subtract(const Matrix A, const Matrix B) 
{
    if (A.rows != B.rows || A.cols != B.cols) {
        matrix_exception(WARNING, "Matrix sizes are incompatible for subtraction.");
        return MATRIX_NULL;
    }

    Matrix C = matrix_allocate(A.rows, A.cols);
    for (size_t idx = 0; idx < C.rows * C.cols; ++idx) {
        C.data[idx] = A.data[idx] - B.data[idx];
    }
    return C;
}


Matrix matrix_multiply(const Matrix A, const Matrix B) 
{
    if (A.cols != B.rows) {
        matrix_exception(WARNING, "Matrix sizes are incompatible for multiplication.");
        return MATRIX_NULL;
    }

    Matrix C = matrix_allocate(A.rows, B.cols);
    for (size_t row = 0; row < C.rows; ++row) {
        for (size_t col = 0; col < C.cols; ++col) {
            double sum = 0;
            for (size_t idx = 0; idx < A.cols; ++idx) {
                sum += A.data[row * A.cols + idx] * B.data[idx * B.cols + col];
            }
            C.data[row * C.cols + col] = sum;
        }
    }
    return C;
}


Matrix matrix_transpose(const Matrix A) 
{
    Matrix T = matrix_allocate(A.cols, A.rows);
    for (size_t row = 0; row < A.rows; ++row) {
        for (size_t col = 0; col < A.cols; ++col) {
            T.data[col * T.rows + row] = A.data[row * A.cols + col];
        }
    }
    return T;
}


Matrix matrix_by_scalar(const Matrix A, double scalar) 
{
    Matrix C = matrix_allocate(A.rows, A.cols);
    for (size_t idx = 0; idx < С.rows * С.cols; ++idx) {
        C.data[idx] = A.data[idx] * scalar;
    }
    return C;
}


double matrix_determinant(const Matrix A) 
{
    if (A.rows != A.cols) {
        matrix_exception(ERROR, "Matrix is not square, determinant cannot be calculated.");
        return NAN;
    }

    if (A.rows == 1) {//Добавил
        return A.data[0];
    }

    if (A.rows == 2) {
        return A.data[0] * A.data[3] - A.data[1] * A.data[2];
    }

    if (A.rows == 3) {
        return A.data[0] * A.data[4] * A.data[8]
            + A.data[1] * A.data[5] * A.data[6]
            + A.data[2] * A.data[3] * A.data[7]
            - A.data[2] * A.data[4] * A.data[6]
            - A.data[0] * A.data[5] * A.data[7]
            - A.data[1] * A.data[3] * A.data[8];
    }
    if (A.rows > 3) {
        matrix_exception(ERROR, "Matrix size exceeds 3x3, determinant cannot be calculated."); // добавил
        return NAN; 
    }
    
    return NAN;//исправил
}


Matrix matrix_exponent(const Matrix A, const unsigned int x) 
{
    if (A.rows != A.cols) {
        matrix_exception(ERROR, "Matrix exponent: ERROR");
        return (Matrix){ 0, 0, NULL };
    }

    Matrix result = matrix_identity(A.rows);
    Matrix term = matrix_identity(A.rows);

    for (unsigned int n = 1; n <= x; ++n) {
        Matrix temp = matrix_multiply(term, A);
        matrix_free(&term);
        term = matrix_by_scalar(temp, 1.0 / n);  // исравил
        matrix_free(&temp);

        Matrix new_result = matrix_subtract(result, term);  
        matrix_free(&result);
        result = new_result;
    }
    matrix_free(&term);
    return result;
}


int main() {
    Matrix A = matrix_allocate(3, 3);
    Matrix B = matrix_allocate(3, 3);

    double data_A[9] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    double data_B[9] = { 9, 8, 7, 6, 5, 4, 3, 2, 1 };

    memcpy(A.data, data_A, 9 * sizeof(double));
    memcpy(B.data, data_B, 9 * sizeof(double));

    printf("Matrix A:\n");
    matrix_print(A);

    printf("Matrix B:\n");
    matrix_print(B);

    Matrix C = matrix_sum(A, B);
    printf("Result of addition:\n");
    matrix_print(C);

    Matrix D = matrix_subtract(A, B);
    printf("Result of addition:\n");
    matrix_print(D);

    // Умножение
    Matrix E = matrix_multiply(A, B);
    printf("The result of multiplication:\n");
    matrix_print(E);

    // Транспонирование
    Matrix T = matrix_transpose(A);
    printf("Transposed matrix A:\n");
    matrix_print(T);


    // Умножение на число
    double scalar = 5;
    Matrix G = matrix_by_scalar(A, scalar);
    printf("Matrix A multiplied by %2.f:\n", scalar);
    matrix_print(G);

    // Определитель
    printf("Determinant of a matrix A: %2.f \n", matrix_determinant(A));

    //Матричная экспонента
    Matrix exponent_A = matrix_exponent(A, 3);
    printf("Matrix exponential от A:\n");
    matrix_print(exponent_A);

    matrix_free(&A);
    matrix_free(&B);
    matrix_free(&C);
    matrix_free(&D);
    matrix_free(&E);
    matrix_free(&T);
    matrix_free(&F);
    matrix_free(&G);
    matrix_free(&exponent_A);

    return 0;
}