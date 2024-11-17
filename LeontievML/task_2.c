#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <math.h>

const int RATIO = 7;

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
    if (level == ERROR)
    {
        printf("ERROR: %s", msg);
    }

    if (level == WARNING)
    {
        printf("WARNING: %s", msg);
    }

    if (level == INFO)
    {
        printf("INFO: %s", msg);
    }
}

struct Matrix matrix_allocate(const size_t cols, const size_t rows)
{
    struct Matrix M;

    if (rows == 0 || cols == 0)
    {
        matrix_exception(INFO, "Матрица содержит 0 столбцов или строк");
        return MATRIX_NULL;
    }

    size_t size_in_bytes = rows * cols * sizeof(double);
    if (size_in_bytes / sizeof(double) != rows * cols)
    {
        matrix_exception(ERROR, "OVERFLOW: Переполнение выделенной памяти");
        return MATRIX_NULL;
    }
    M.data = malloc(cols * rows * sizeof(double));

    if (M.data == NULL)
    {
        matrix_exception(ERROR, "Сбой выделения памяти");
        return MATRIX_NULL;
    }

    M.rows = rows;
    M.cols = cols;
    return M;
}

int matrix_free(struct Matrix *M)
{
    if (M->data != NULL)
    {
        free(M->data);
        M->data = NULL;
    }
    M->cols = 0;
    M->rows = 0;
}

void matrix_zero(const struct Matrix M)
{
    memset(M.data, 0, M.cols * M.rows * sizeof(double));
}

void matrix_random(struct Matrix A)
{
    for (size_t i = 0; i < A.rows * A.cols; ++i)
        A.data[i] = (double)(rand() % 10);
}

int matrix_print(const struct Matrix M)
{
    for (size_t i = 0; i < M.rows; ++i)
    {
        for (size_t j = 0; j < M.cols; ++j)
        {
            printf("%lf ", M.data[i * M.cols + j]);
        }
        printf("\n");
    }

    return 0;
}

struct Matrix matrix_add(struct Matrix A, struct Matrix B)
{
    if (A.rows != B.rows || A.cols != B.cols)
    {
        matrix_exception(WARNING, "Размеры матриц не подходят для сложения.\n");
        return MATRIX_NULL;
    }

    struct Matrix result = matrix_allocate(A.rows, A.cols);

    for (size_t i = 0; i < result.rows; ++i)
    {
        for (size_t j = 0; j < result.cols; ++j)
            result.data[i * result.cols + j] = 0; // Обнуляем текущий элемент
    }

    for (size_t i = 0; i < result.rows * result.cols; ++i)
    {
        result.data[i] = A.data[i] + B.data[i];
    }

    return result;
}

struct Matrix matrix_substruct(struct Matrix A, struct Matrix B)
{
    if (A.rows != B.rows || A.cols != B.cols)
    {
        matrix_exception(WARNING, "Размеры матриц не подходят для сложения.\n");
        return MATRIX_NULL;
    }

    struct Matrix result = matrix_allocate(A.rows, A.cols);

    for (size_t i = 0; i < result.rows; ++i)
    {
        for (size_t j = 0; j < result.cols; ++j)
            result.data[i * result.cols + j] = 0; // Обнуляем текущий элемент
    }

    for (size_t i = 0; i < result.rows * result.cols; ++i)
    {
        result.data[i] = A.data[i] - B.data[i];
    }

    return result;
}

struct Matrix matrix_multiply(struct Matrix A, struct Matrix B)
{
    if (A.cols != B.rows)
    {
        matrix_exception(WARNING, "Число столбцов первой матрицы не равно числу строк второй матрицы.\n");
        return MATRIX_NULL;
    }

    struct Matrix result = matrix_allocate(A.rows, B.cols);

    for (size_t i = 0; i < result.rows; ++i)
    {
        for (size_t j = 0; j < result.cols; ++j)
        {
            result.data[i * result.cols + j] = 0; // Обнуляем текущий элемент
        }
    }

    // Умножение матриц
    for (size_t i = 0; i < A.rows; ++i)
    { // Проход по строкам матрицы A
        for (size_t j = 0; j < B.cols; ++j)
        { // Проход по столбцам матрицы B
            for (size_t k = 0; k < A.cols; ++k)
            { // Проход по столбцам матрицы A и строкам матрицы B
                result.data[i * result.cols + j] += A.data[i * A.cols + k] * B.data[k * B.cols + j];
            }
        }
    }

    return result;
}

struct Matrix matrix_ratio(struct Matrix A, int RATIO)
{
    struct Matrix result = matrix_allocate(A.rows, A.cols);

    for (size_t i = 0; i < result.rows; ++i)
    {
        for (size_t j = 0; j < result.cols; ++j)
        {
            result.data[i * result.cols + j] = 0; // Обнуляем текущий элемент
        }
    }

    // Умножение матрицы на коэффициент
    for (size_t i = 0; i < A.rows; ++i)
    { // Проход по строкам матрицы A
        for (size_t j = 0; j < A.cols; ++j)
        { // Проход по столбцам матрицы A
            {
                result.data[i * result.cols + j] += A.data[i * A.cols + j] * RATIO;
            }
        }
    }

    return result;
}

struct Matrix matrix_transposition(struct Matrix A)
{
    struct Matrix result = matrix_allocate(A.cols, A.rows);

    for (size_t i = 0; i < result.rows; ++i)
    {
        for (size_t j = 0; j < result.cols; ++j)
        {
            result.data[i * result.cols + j] = 0; // Обнуляем текущий элемент
        }
    }

    // Транспонирование матрицы
    for (size_t i = 0; i < A.rows; ++i)
    { // Проход по строкам матрицы A
        for (size_t j = 0; j < A.cols; ++j)
        { // Проход по столбцам матрицы A
            {
                result.data[j * result.cols + i] += A.data[i * A.cols + j];
            }
        }
    }

    return result;
}

struct Matrix matrix_gauss(struct Matrix A)
{
    struct Matrix C = matrix_allocate(A.rows, A.cols);

    matrix_zero(C);

    for (size_t i = 0; i < A.rows * A.cols; ++i)
    {
        C.data[i] = A.data[i];
    }

    for (size_t k = 0; k < C.rows; ++k) // проход по столбцам от 1 до k
    {

        for (size_t i = k + 1; i < C.rows; ++i) // проход по строкам от i до 1
        {
            double factor = C.data[i * C.cols + k] / C.data[k * C.cols + k];
            for (size_t j = k; j < C.cols; ++j)
            {
                C.data[i * C.cols + j] = C.data[i * C.cols + j] - factor * C.data[k * C.cols + j];
            }
        }
    }

    return C;
}

double matrix_determinate(struct Matrix C)
{
    if (C.rows != C.cols)
    {
        matrix_exception(WARNING, "Матрица должна быть квадратной для нахождения определителя.\n");
        return 1;
    }

    double determinate = 1;
    for (size_t k = 0; k < C.rows; ++k)
    {
        determinate *= C.data[k * C.rows + k];
    }

    return determinate;
}

struct Matrix matrix_identity(struct Matrix M)
{
    for (size_t i = 0; i < M.rows; ++i)
    {
        for (size_t j = 0; j < M.cols; ++j)
        {
            M.data[i * M.cols + j] = (i == j) ? 1 : 0;
        }
    }
    return M;
}

double factorial(const int n)
{
    size_t fact = 1;
    for (size_t i = 1; i < n; i++)
    {
        fact *= i;
    }

    return fact;
}

void matrix_copy(struct Matrix A, const struct Matrix B)
{
    for (size_t i = 0; i < A.rows * A.cols; ++i)
    {
        A.data[i] = B.data[i];
    }
}

struct Matrix matrix_power(struct Matrix A, int power) // Возведение матрицы в степень
{
    if (A.rows != A.cols)
    {
        matrix_exception(WARNING, "Матрица должна быть квадратной для возведения в степень.\n");
        return MATRIX_NULL;
    }

    struct Matrix result = matrix_allocate(A.cols, A.rows);

    matrix_zero(result);
    matrix_copy(result, A);

    for (int p = 1; p < power; p++)
    {
        result = matrix_multiply(A, result);
    }

    return result;
}

struct Matrix matrix_exponent(const struct Matrix A, int terms)
{

    if (A.rows != A.cols)
    {
        matrix_exception(WARNING, "Матрица должна быть квадратной для вычисления экспоненты");
        return MATRIX_NULL;
    }

    struct Matrix result = matrix_allocate(A.rows, A.cols);
    result = matrix_identity(result);

    struct Matrix temp = matrix_allocate(A.rows, A.cols);

    for (int n = 1; n < terms; ++n)
    {
        // Вычисляем temp = A^n
        temp = matrix_power(A, n);
        int fact = factorial(n);

        // Делим temp на факториал и сохраняем результат в term
        for (size_t i = 0; i < result.rows * result.cols; ++i)
        {
            result.data[i] += temp.data[i] / fact;
        }
    }

    // Освобождаем память, выделенную для вспомогательных матриц
    matrix_free(&temp);

    return result;
}

int main()
{
    srand(time(NULL));

    struct Matrix A = matrix_allocate(3, 3);
    struct Matrix B = matrix_allocate(3, 3);
    struct Matrix result_add = matrix_allocate(A.cols, A.rows);
    struct Matrix result_substruct = matrix_allocate(A.cols, A.rows);
    struct Matrix result_multiply = matrix_allocate(A.rows, B.cols);
    struct Matrix result_ratio = matrix_allocate(A.cols, A.rows);
    struct Matrix result_transposition = matrix_allocate(A.rows, A.cols);
    struct Matrix C = matrix_allocate(A.cols, A.rows);
    struct Matrix E = matrix_allocate(A.cols, A.rows);
    struct Matrix result = matrix_allocate(A.cols, A.rows);
    struct Matrix result_power = matrix_allocate(A.cols, A.rows);
    struct Matrix result_exponent = matrix_allocate(A.cols, A.rows);

    matrix_random(A);
    printf("Матрица A:\n");
    matrix_print(A);

    matrix_random(B);
    printf("Матрица B:\n");
    matrix_print(B);

    result_add = matrix_add(A, B);
    printf("Результат сложения:\n");
    matrix_print(result_add);

    // Вычитание
    result_substruct = matrix_substruct(A, B);
    printf("Результат вычитания:\n");
    matrix_print(result_substruct);

    // Умножение
    result_multiply = matrix_multiply(A, B);
    printf("Результат умножения:\n");
    matrix_print(result_multiply);

    // Транспонирование
    result_transposition = matrix_transposition(A);
    printf("Транспонированная матрица A:\n");
    matrix_print(result_transposition);

    // Умножение на число
    result_ratio = matrix_ratio(A, RATIO);
    printf("Матрица A умноженная на %2.f:\n", RATIO);
    matrix_print(result_ratio);

    C = matrix_gauss(A);
    double determinate = matrix_determinate(C);
    printf("Определитель матрицы A:\n");
    printf("%f\n", determinate);

    int terms = 10;
    result_exponent = matrix_exponent(A, terms);
    printf("Экспонента матрицы A:\n");
    matrix_print(result_exponent);

    matrix_free(&A);
    matrix_free(&B);
    matrix_free(&result_add);
    matrix_free(&result_substruct);
    matrix_free(&result_multiply);
    matrix_free(&result_ratio);
    matrix_free(&result_transposition);
    matrix_free(&result_exponent);
    matrix_free(&C);
    matrix_free(&E);

    return 0;
}
