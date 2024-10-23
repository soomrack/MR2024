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

// Выделение памяти для матрицы
struct Matrix matrix_allocate(const size_t cols, const size_t rows)
{
    struct Matrix A = {cols, rows, NULL};

    if (cols == 0 || rows == 0)
        return A;

    // Проверка переполнения перед выделением памяти
    if (SIZE_MAX / cols / rows < sizeof(double))
        return (struct Matrix){0, 0, NULL};

    A.data = (double *)malloc(cols * rows * sizeof(double));

    if (A.data == NULL)
        return (struct Matrix){0, 0, NULL};

    return A;
}


// Освобождение памяти для матрицы
void matrix_free(struct Matrix *A)
{
    if (A->data != NULL)
    {
        free(A->data);
        A->data = NULL;
    }
    A->cols = 0;
    A->rows = 0;
}


// Заполнение матрицы случайными числами
void matrix_fill_random(struct Matrix A)
{
    for (size_t i = 0; i < A.rows * A.cols; ++i)
    {
        A.data[i] = (double)(rand() % 10); // случайное число от 0.0 до 9.9
    }
}


// Печать матрицы
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


// Сложение матриц A и B, результат в A
int matrix_add(struct Matrix A, struct Matrix B, struct Matrix result)
{
    if (A.cols != B.cols || A.rows != B.rows)
    {
        return -1; // Ошибка: размеры матриц не совпадают
    }

    for (size_t i = 0; i < A.rows * A.cols; ++i)
    {
        result.data[i] = A.data[i] + B.data[i];
    }

    return 0; // Успех
}


// Вычитание матриц A и B, результат в A
int matrix_subtract(struct Matrix A, struct Matrix B, struct Matrix result)
{
    if (A.cols != B.cols || A.rows != B.rows)
    {
        return -1; // Ошибка: размеры матриц не совпадают
    }

    for (size_t i = 0; i < A.rows * A.cols; ++i)
    {
        result.data[i] = A.data[i] - B.data[i];
    }

    return 0; // Успех
}


// Умножение матриц A и B, результат в новой матрице C
struct Matrix matrix_multiply(struct Matrix A, struct Matrix B)
{
    if (A.cols != B.rows)
    {
        return (struct Matrix){0, 0, NULL}; // Ошибка: количество столбцов A не равно количеству строк B
    }

    struct Matrix C = matrix_allocate(B.cols, A.rows);

    if (C.data == NULL)
    {
        return C; // Ошибка выделения памяти
    }

    for (size_t i = 0; i < A.rows; ++i)
    {
        for (size_t j = 0; j < B.cols; ++j)
        {
            C.data[i * C.cols + j] = 0;
            for (size_t k = 0; k < A.cols; ++k)
            {
                C.data[i * C.cols + j] += A.data[i * A.cols + k] * B.data[k * B.cols + j];
            }
        }
    }

    return C;
}


int main()
{
    srand(time(NULL)); // Инициализация генератора случайных чисел

    struct Matrix A = matrix_allocate(2, 2);
    struct Matrix B = matrix_allocate(2, 2);
    struct Matrix result_add = matrix_allocate(A.cols, A.rows);
    struct Matrix result_subtract = matrix_allocate(A.cols, A.rows);

    if (A.data == NULL || B.data == NULL)
    {
        printf("Ошибка выделения памяти\n");
        return 1;
    }

    // Заполняем матрицы случайными числами
    matrix_fill_random(A);
    matrix_fill_random(B);

    printf("Матрица A:\n");
    matrix_print(A);

    printf("Матрица B:\n");
    matrix_print(B);

    // Складываем матрицы с проверкой на ошибку
    if (matrix_add(A, B, result_add) == -1)
    {
        printf("Ошибка: размеры матриц не совпадают для сложения\n");
    }
    else
    {
        printf("Результат A + B:\n");
        matrix_print(result_add);
    }

    // Вычитаем матрицы с проверкой на ошибку
    if (matrix_subtract(A, B, result_subtract) == -1)
    {
        printf("Ошибка: размеры матриц не совпадают для вычитания\n");
    }
    else
    {
        printf("Результат A - B:\n");
        matrix_print(result_subtract);
    }

    // Умножаем матрицы
    struct Matrix C = matrix_multiply(A, B);
    if (C.data != NULL)
    {
        printf("Результат A * B:\n");
        matrix_print(C);
        matrix_free(&C);
    }
    else
    {
        printf("Умножение не удалось (неверные размеры матриц)\n");
    }

    // Освобождаем память
    matrix_free(&A);
    matrix_free(&B);

    return 0;
}
