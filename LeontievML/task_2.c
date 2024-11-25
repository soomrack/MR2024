
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <math.h>

const double scalar = 7;

struct Matrix
{
    size_t cols;
    size_t rows;
    double *data;
};

enum MatrixExceptionLevel
{
    ERROR,
    WARNING,
    INFO,
    DEBUG
};

const struct Matrix MATRIX_NULL = {0, 0, NULL};


void matrix_exception(const enum MatrixExceptionLevel level, char *msg)
{
    if (level == ERROR) {
        printf("ERROR: %s", msg);
    }

    if (level == WARNING) {
        printf("WARNING: %s", msg);
    }

    if (level == INFO) {
        printf("INFO: %s", msg);
    }
}


struct Matrix matrix_allocate(const size_t cols, const size_t rows)
{
    struct Matrix M;

    if (rows == 0 || cols == 0) {
        matrix_exception(INFO, "Матрица содержит 0 столбцов или строк");
        return (struct Matrix){M.cols, M.rows, NULL};
    }

    size_t size_in_bytes = rows * cols * sizeof(double);
    if (rows >= SIZE_MAX / sizeof(double) / cols) {
        matrix_exception(ERROR, "OVERFLOW: Переполнение выделенной памяти \n");
        return MATRIX_NULL;
    } // проверить перемполнение rows * cols

    if (size_in_bytes / sizeof(double) != rows * cols) {
        matrix_exception(ERROR, "OVERFLOW: Переполнение выделенной памяти");
        return MATRIX_NULL;
    }
    M.data = malloc(cols * rows * sizeof(double));

    if (M.data == NULL) {
        matrix_exception(ERROR, "Сбой выделения памяти");
        return MATRIX_NULL;
    }

    M.rows = rows;
    M.cols = cols;
    return M;
}


void matrix_free(struct Matrix *M)
{
    if (M == NULL) {
        matrix_exception(ERROR, "Обращение к недопутимой области памяти");
        return;
    }

    free(M->data);
    M->cols = 0;
    M->rows = 0; // bag
    return;
}


void matrix_zero(const struct Matrix M)
{
    memset(M.data, 0, M.cols * M.rows * sizeof(double));
}


void matrix_random(struct Matrix A)
{
    for (size_t elem = 0; elem < A.rows * A.cols; ++elem)
        A.data[elem] = (double)(rand() % 10);
}


int matrix_print(const struct Matrix M)
{
    for (size_t row = 0; row < M.rows; ++row) {
        for (size_t col = 0; col < M.cols; ++col) {
            printf("%lf ", M.data[row * M.cols + col]);
        }
        printf("\n");
    }

    return 0;
}


struct Matrix matrix_add(struct Matrix A, struct Matrix B)
{
    if (A.rows != B.rows || A.cols != B.cols) {
        matrix_exception(WARNING, "Размеры матриц не подходят для сложения.\n");
        return MATRIX_NULL;
    }

    struct Matrix result = matrix_allocate(A.rows, A.cols);

    for (size_t elem = 0; elem < result.rows * result.cols; ++elem) {
        result.data[elem] = A.data[elem] + B.data[elem];
    }

    return result;
}


struct Matrix matrix_substruct(struct Matrix A, struct Matrix B)
{
    if (A.rows != B.rows || A.cols != B.cols) {
        matrix_exception(WARNING, "Размеры матриц не подходят для сложения.\n");
        return MATRIX_NULL;
    }

    struct Matrix result = matrix_allocate(A.rows, A.cols);

    for (size_t elem = 0; elem < result.rows * result.cols; ++elem) {
        result.data[elem] = A.data[elem] - B.data[elem];
    }

    return result;
}


struct Matrix matrix_multiply(struct Matrix A, struct Matrix B)
{
    if (A.cols != B.rows) {
        matrix_exception(WARNING, "Число столбцов первой матрицы не равно числу строк второй матрицы.\n");
        return MATRIX_NULL;
    }

    struct Matrix result = matrix_allocate(A.rows, B.cols);

    for (size_t row = 0; row < result.rows; ++row) {
        for (size_t col = 0; col < result.cols; ++col) {
            for (size_t k = 0; k < result.cols; ++k) {
                result.data[row * result.cols + col] += A.data[row * result.cols + k] * B.data[k * result.cols + col];
            }
        }
    }

    return result;
}


struct Matrix matrix_scalar(struct Matrix A, double scalar)
{
    struct Matrix result = matrix_allocate(A.rows, A.cols);

    for (size_t row = 0; row < result.rows; ++row) {
        for (size_t col = 0; col < result.cols; ++col) {
            result.data[row * result.cols + col] = A.data[row * A.cols + col] * scalar;
        }
    }

    return result;
}


struct Matrix matrix_transposition(struct Matrix A)
{
    struct Matrix result = matrix_allocate(A.cols, A.rows);

    for (size_t row = 0; row < result.rows; ++row) {
        for (size_t col = 0; col < result.cols; ++col) {
            result.data[col * result.rows + row] = A.data[row * A.cols + col];
        }
    }

    return result;
}


double matrix_determinate(struct Matrix A)
{
    if (A.rows != A.cols) {
        matrix_exception(WARNING, "Матрица должна быть квадратной для нахождения определителя.\n");
        return NAN;
    }

    if (A.rows == 0 && A.cols == 0) {
        matrix_exception(ERROR, "Количесвто строк и столбцов не может быть меньше 1.\n");
        return NAN;
    }

    if (A.rows == 1 && A.cols == 1) {
        return A.data[0];
    }

    if (A.rows == 2 && A.cols == 2) {
        return A.data[0] * A.data[3] - A.data[1] * A.data[2];
    }

    if (A.rows == 3 && A.cols == 3) {
        return A.data[0] * (A.data[4] * A.data[8] - A.data[5] * A.data[7]) -
               A.data[1] * (A.data[3] * A.data[8] - A.data[5] * A.data[6]) +
               A.data[2] * (A.data[3] * A.data[7] - A.data[4] * A.data[6]);
    }

    matrix_exception(WARNING, "Определитель нельзя посчитать для матриц размером большим чем 3х3.\n");
    return NAN;
}


struct Matrix matrix_identity(struct Matrix M)
{
    struct Matrix I = matrix_allocate(M.cols, M.rows);
    matrix_zero(I);

    for (size_t row = 0; row < I.rows; ++row) {
        I.data[row * I.cols + row] = 1.0;
    }

    return I;
}


double factorial(const unsigned int n)
{
    double fact = 1.0;

    if (n == 0) {
        return fact;
    }

    for (unsigned int pow = 1; pow <= n; ++pow) {
        fact *= pow;
    }

    return fact;
}


struct Matrix matrix_copy(const struct Matrix src)
{
    if (src.data == NULL) {
        matrix_exception(ERROR, "Сбой выделения памяти");
        return MATRIX_NULL;
    }

    struct Matrix dest = matrix_allocate(src.cols, src.rows);
    memcpy(dest.data, src.data, src.cols * src.rows * sizeof(double));
    return dest;
}


struct Matrix matrix_power(struct Matrix A, unsigned power) // Возведение матрицы в степень
{
    if (A.rows != A.cols) {
        matrix_exception(WARNING, "Матрица должна быть квадратной для возведения в степень.\n");
        return MATRIX_NULL;
    }

    if (power == 0) {
        struct Matrix result = matrix_identity(A);
        return result;
    }

    struct Matrix temp = matrix_copy(A);

    if (power == 1) {
        return temp;
    }

    for (int i = 2; i < power; ++i) {
        struct Matrix result = matrix_multiply(temp, A);
        matrix_free(&temp);
        temp = result;
    }

    return temp;
}


struct Matrix matrix_exponent(const struct Matrix A, int terms)
{
    if (A.rows != A.cols) {
        matrix_exception(WARNING, "Матрица должна быть квадратной для вычисления экспоненты");
        return MATRIX_NULL;
    }

    struct Matrix result = matrix_allocate(A.rows, A.cols);
    struct Matrix temp = matrix_allocate(A.rows, A.cols);

    for (int n = 0; n < terms; ++n) {
        temp = matrix_power(A, n);
        double fact = 1 / factorial(n);

        result = matrix_scalar(temp, fact);
        matrix_free(&temp);
    }

    return result;
}


int main()
{
    srand(time(NULL));

    struct Matrix A = matrix_allocate(3, 3);
    struct Matrix B = matrix_allocate(3, 3);

    matrix_random(A);
    printf("Матрица A:\n");
    matrix_print(A);

    matrix_random(B);
    printf("Матрица B:\n");
    matrix_print(B);

    struct Matrix result_add = matrix_add(A, B);
    printf("Результат сложения:\n");
    matrix_print(result_add);
    matrix_free(&result_add);

    struct Matrix result_substruct = matrix_substruct(A, B);
    printf("Результат вычитания:\n");
    matrix_print(result_substruct);
    matrix_free(&result_substruct);

    struct Matrix result_multiply = matrix_multiply(A, B);
    printf("Результат умножения:\n");
    matrix_print(result_multiply);
    matrix_free(&result_multiply);

    struct Matrix result_transposition = matrix_transposition(A);
    printf("Транспонированная матрица A:\n");
    matrix_print(result_transposition);
    matrix_free(&result_transposition);

    struct Matrix result_scalar = matrix_scalar(A, scalar);
    printf("Матрица A умноженная на %lf:\n", scalar);
    matrix_print(result_scalar);
    matrix_free(&result_scalar);

    double determinate = matrix_determinate(A);
    printf("Определитель матрицы A:\n");
    printf("%f\n", determinate);

    int terms = 16;
    struct Matrix result_exponent = matrix_exponent(A, terms);
    printf("Экспонента матрицы A:\n");
    matrix_print(result_exponent);
    matrix_free(&result_exponent);

    matrix_free(&A);
    matrix_free(&B);

    return 0;
}
