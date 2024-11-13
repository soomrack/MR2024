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


enum MatrixExceptionLevel { ERROR, WARNING, INFO, DEBUG };


const Matrix MATRIX_NULL = { 0, 0, NULL };


void matrix_exception(const enum MatrixExceptionLevel level, const char* msg)
{
    const char* levels[] = { "ERROR", "WARNING", "INFO", "DEBUG" };
    printf("%s: %s\n", levels[level], msg);
    if (level == ERROR) {
        exit(EXIT_FAILURE);
    }
}


Matrix matrix_alloc(const size_t rows, const size_t cols)
{
    if (rows == 0 || cols == 0) {
        matrix_exception(INFO, "Матрица имеет 0 строк или столбцов.");
        return MATRIX_NULL;
    }

    Matrix M = { rows, cols, (double*)malloc(rows * cols * sizeof(double)) };
    if (!M.data) {
        matrix_exception(ERROR, "Не удалось выделить память.");
        return MATRIX_NULL;
    }
    return M;
}


void matrix_free(Matrix* M)
{
    if (A == NULL) return;
    free(A->data);

}


void matrix_zero(Matrix M)
{
    if (M.data) {
        memset(M.data, 0, M.rows * M.cols * sizeof(double));
    }
}


int matrix_identity(const struct Matrix A)  // Создание единичной матрицы 
{
    if (A.data == NULL) return 1;
    matrix_zero(A);
    for (size_t idx = 0; idx < A.rows * A.cols; idx += A.rows + 1) A.data[idx] = 1.0;
    return 0;
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
        matrix_exception(WARNING, "Несовместимые размеры матриц для сложения.");
        return MATRIX_NULL;
    }

    Matrix C = matrix_alloc(C.rows, C.cols);

    for (size_t idx = 0; idx < C.rows * C.cols; ++idx) {
        C.data[idx] = A.data[idx] + B.data[idx];
    }
    return C;
}


Matrix matrix_subtract(const Matrix A, const Matrix B)
{
    if (A.rows != B.rows || A.cols != B.cols) {
        matrix_exception(WARNING, "Несовместимые размеры матриц для вычитания.");
        return MATRIX_NULL;
    }

    Matrix C = matrix_alloc(C.rows, C.cols);

    for (size_t idx = 0; idx < C.rows * C.cols; ++idx) {
        C.data[idx] = A.data[idx] - B.data[idx];
    }
    return C;
}


Matrix matrix_multiply(const Matrix A, const Matrix B)
{
    if (A.cols != B.rows) {
        matrix_exception(WARNING, "Несовместимые размеры матриц для умножения.");
        return MATRIX_NULL;
    }

    Matrix C = matrix_alloc(C.rows, C.cols);

    for (size_t row = 0; row < C.rows; ++row) {
        for (size_t col = 0; col < C.cols; ++col) {
            double sum = 0;
            for (size_t k = 0; k < C.cols; ++k) {
                sum += A.data[row * A.cols + k] * B.data[k * B.cols + col];
            }
            C.data[row * B.cols + col] = sum;
        }
    }
    return C;
}


Matrix matrix_transpose(const Matrix A)
{
    Matrix T = matrix_alloc(A.cols, A.rows);
    if (!T.data) return MATRIX_NULL;

    for (size_t row = 0; row < A.rows; ++row) {
        for (size_t col = 0; col < A.cols; ++col) {
            T.data[col * T.rows + row] = A.data[row * A.cols + col];
        }
    }
    return T;
}


Matrix matrix_by_scalar(const Matrix A, double scalar)
{
    Matrix C = matrix_alloc(A.rows, A.cols);
    if (!C.data) return MATRIX_NULL;

    for (size_t idx = 0; idx < A.rows * A.cols; ++idx) {
        C.data[idx] = A.data[idx] * scalar;
    }
    return C;
}


double matrix_det(const Matrix A)
{
    double det;
    if (A.rows != A.cols) {
        matrix_error(WARNING, "Матрица не подходит");
        return NAN;
    }

    if (A.rows == 0) {
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
        det = A.data[0] * A.data[4] * A.data[8]
            + A.data[1] * A.data[5] * A.data[6]
            + A.data[3] * A.data[7] * A.data[2]
            - A.data[2] * A.data[4] * A.data[6]
            - A.data[3] * A.data[1] * A.data[8]
            - A.data[0] * A.data[5] * A.data[7];
        return det;
    }
    return NAN;
}


unsigned long long factorial(unsigned int n)
{
    unsigned long long result = 1;
    for (unsigned int idx = 1; idx <= n; ++idx) {
        result *= idx;
    }
    return result;
}


Matrix matrix_power(const Matrix A, int power)
{
    if (A.rows != A.cols) {
        matrix_exception(WARNING, "Для возведения в степень матрица должна быть квадратной.");
        return MATRIX_NULL;
    }

    Matrix result = matrix_identity(A.rows);
    if (!result.data) return MATRIX_NULL;

    for (int idx = 0; idx < power; ++idx) {
        Matrix temp = matrix_multiply(result, A);
        matrix_free(&result);
        result = temp;
    }
    return result;
}


Matrix matrix_exponent(const Matrix A, const unsigned int num_terms)
{
    if (A.rows != A.cols) {
        matrix_exception(WARNING, "Для возведения в степень матрица должна быть квадратной.");
        return MATRIX_NULL;
    }

    Matrix E = matrix_identity(A.rows);
    Matrix term = matrix_identity(A.rows);

    for (size_t n = 1; n < num_terms; ++n) {
        Matrix temp = matrix_multiply(term, A);
        matrix_free(&term);
        term = matrix_by_scalar(temp, 1.0 / n);
        matrix_free(&temp);

        Matrix new_E = matrix_sum(E, term);
        matrix_free(&E);
        E = new_E;
    }
    matrix_free(&term);
    return E;
}


int main()
{
    Matrix A = matrix_alloc(3, 3);
    Matrix B = matrix_alloc(3, 3);


    double data_A[9] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    double data_B[9] = { 9, 8, 7, 6, 4, 4, 3, 2, 1 };


    memcpy(A.data, data_A, 9 * sizeof(double));
    memcpy(B.data, data_B, 9 * sizeof(double));

    // Печать исходных матриц
    printf("Matrix A:\n");
    matrix_print(A);

    printf("Matrix B:\n");
    matrix_print(B);

    // Сложение
    Matrix C = matrix_sum(A, B);
    printf("Result of addition:\n");
    matrix_print(C);

    // Вычитание
    Matrix D = matrix_subtract(A, B);
    printf("Subtraction result:\n");
    matrix_print(D);

    // Умножение
    Matrix E = matrix_multiply(A, B);
    printf("The result of multiplication:\n");
    matrix_print(E);

    // Транспонирование
    Matrix T = matrix_transpose(A);
    printf("Transposed matrix A:\n");
    matrix_print(T);

    // Возведение в степень
    int power = 3;
    Matrix F = matrix_power(A, power);
    printf("Matrix A to the power %d:\n", power);
    matrix_print(F);

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

    // Освобождение памяти
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
