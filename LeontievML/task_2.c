#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

struct Matrix
{
    size_t cols;
    size_t rows;
    double *data;
};

struct Matrix matrix_allocate(const size_t cols, const size_t rows)
{
    struct Matrix A = {cols, rows, NULL};

    if (cols == 0 || rows == 0)
        return A;

    if (SIZE_MAX / cols / rows < sizeof(double))
        return (struct Matrix){0, 0, NULL};

    A.data = (double *)malloc(cols * rows * sizeof(double));

    if (A.data == NULL)
        return (struct Matrix){0, 0, NULL};

    return A;
}

void matrix_free(struct Matrix *A)
{
    if (A->data != NULL)
    {
        free(A->data);
        A->data = NULL;
    }

    A->rows = 0;
    A->cols = 0;
}

void matrix_random(struct Matrix A)
{
    for (size_t i = 0; i < A.rows * A.cols; ++i)
        A.data[i] = (double)(rand() % 10);
}

void matrix_print(const struct Matrix A)
{
    for (size_t i = 0; i < A.rows; ++i)
    {
        for (size_t j = 0; j < A.cols; ++j)
        {
            printf("%lf ", A.data[i * A.cols + j]);
        }
        printf("\n");
    }
}

int matrix_add(struct Matrix A, struct Matrix B, struct Matrix result)
{
    if (A.cols != B.cols || A.rows != B.rows)
    {
        return -1;
    }

    for (size_t i = 0; i < result.rows; ++i)
    {
        for (size_t j = 0; j < result.cols; ++j)
            result.data[i * result.cols + j] = 0; // Обнуляем текущий элемент
    }

    for (size_t i = 0; i < A.rows * A.cols; ++i)
    {
        result.data[i] = A.data[i] + B.data[i];
    }

    return 0;
}

int matrix_substruct(struct Matrix A, struct Matrix B, struct Matrix result)
{
    if (A.cols != B.cols || A.rows != B.rows)
        return -1;

    for (size_t i = 0; i < result.rows; ++i)
    {
        for (size_t j = 0; j < result.cols; ++j)
            result.data[i * result.cols + j] = 0; // Обнуляем текущий элемент
    }

    for (size_t i = 0; i < A.rows * A.cols; ++i)
        result.data[i] = A.data[i] - B.data[i];
}

int matrix_multiply(struct Matrix A, struct Matrix B, struct Matrix result)
{
    if (A.cols != B.rows)
        return -1;

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

    return 0;
}

int main()
{
    srand(time(NULL));

    struct Matrix A = matrix_allocate(2, 2);
    struct Matrix B = matrix_allocate(2, 2);
    struct Matrix result_add = matrix_allocate(A.cols, A.rows);
    struct Matrix result_substruct = matrix_allocate(A.cols, A.rows);
    struct Matrix result_multiply = matrix_allocate(A.rows, B.cols);

    if (A.data == NULL || B.data == NULL)
    {
        printf("Ошибка выделения памяти\n");
        return 1;
    }

    matrix_random(A);
    printf("Матрица A:\n");
    matrix_print(A);

    matrix_random(B);
    printf("Матрица B:\n");
    matrix_print(B);

    if (matrix_add(A, B, result_add) == -1)
    {
        printf("Ошибка, размеры матриц не подходят для сложения\n");
    }
    else
    {
        printf("Результат сложения матриц:\n");
        matrix_print(result_add);
    }

    if (matrix_substruct(A, B, result_substruct) == -1)
    {
        printf("Ошибка, размеры матриц не подходят для сложения\n");
    }
    else
    {
        printf("Результат разности матриц:\n");
        matrix_print(result_substruct);
    }

    if (matrix_multiply(A, B, result_multiply) == -1)
    {
        printf("Ошибка, размеры матриц не подходят для умножения\n");
    }
    else
    {
        printf("Результат умножения матриц:\n");
        matrix_print(result_multiply);
    }

    matrix_free(&A);
    matrix_free(&B);
    matrix_free(&result_add);
    matrix_free(&result_subtract);
    matrix_free(&result_multiply);

    return 0;
}