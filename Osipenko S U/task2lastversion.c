#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <string.h>

enum MatrixExceptionLevel {ERROR, DEBUG};//создание массива

struct Matrix
{
    size_t cols;
    size_t rows;
    double *data;//первая ячейка памяти
}


void matrix_error(const enum MatrixExceptionLevel level, const char* location, const char* msg)//определние ошибки
{
    if (level == ERROR) {
        printf("\nERROR in: %s Text: (%s)\n", location, msg);
    }

    if (level == DEBUG) {
        printf("\nDEBUG\nLoc: %s\nText: %s\n", location, msg);
    }

}

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
        free(A->data);//если 0 то и так ноль
        A->data = NULL;
    }
    A->cols = 0;
    A->rows = 0;
}

// Заполнение матрицы случайными числами
void matrix_fill_random(struct Matrix A)
{
    int sight;
    for (size_t idx = 0; idx < A.rows * A.cols; ++idx)
    {
        sight = (rand() % 2) * 2 - 1;
        A.data[idx] = (double)((rand() % 100) * sight);
    }
}

//принтим матрицу
void matrix_print(const struct Matrix A)
{
    for (size_t row = 0; row < A.rows; ++row)
    {
        for (size_t col= 0; col < A.cols; ++col)
        {
            printf("%lf \t", A.data[row * A.cols + col]);
        }
        printf("\n");
    }
}

// Сложение матриц A и B
int matrix_add(struct Matrix A, struct Matrix B, struct Matrix result)
{
    if (A.cols != B.cols || A.rows != B.rows)
    {
        matrix_error(ERROR, "C = A + B", "размеры матриц не совпадают для сложения");
        return NAN;
    }

    for (size_t idx = 0; idx < A.rows * A.cols; ++idx)
    {
        result.data[idx] = A.data[idx] + B.data[idx];
    }

    return 0;
}

// Вычитание матриц A и B
int matrix_subtract(struct Matrix A, struct Matrix B, struct Matrix result)
{
    if (A.cols != B.cols || A.rows != B.rows)
    {
        matrix_error(ERROR, "C = A - B", "размеры матриц не совпадают для вычитания");
        return NAN;
    }

    for (size_t idx = 0; idx < A.rows * A.cols; ++idx)
    {
        result.data[idx] = A.data[idx] - B.data[idx];
    }

    return 0;
}

//умножение матрицы на число
struct Matrix matrix_multipliy_const(struct Matrix A, double constant)
{
    struct Matrix result = matrix_allocate(A.rows, A.cols);
    for (size_t idx = 0; idx < A.rows * A.cols; ++idx)
    {
        result.data[idx] = A.data[idx] * constant;
    }

    return result;
}

// Умножение матриц A и B, результат в новой матрице C
struct Matrix matrix_multiply(struct Matrix A, struct Matrix B)
{
    if (A.cols != B.rows)
    {
        matrix_error(ERROR, "C = A * B", "Умножение двух матриц не удалось (неверные размеры матриц)");
        return (struct Matrix){0, 0, NULL};
    }

    struct Matrix C = matrix_allocate(B.cols, A.rows);

    if (C.data == NULL)
    {
        return C;
    }

    for (size_t row = 0; row < A.rows; ++row)
    {
        for (size_t col = 0; col < B.cols; ++col)
        {
            C.data[row * C.cols + col] = 0;
            for (size_t k_col = 0; k_col < A.cols; ++k_col)
            {
                C.data[row * C.cols + col] += A.data[row * A.cols + k_col] * B.data[k_col * B.cols + col];
            }
        }
    }

    return C;
}

struct Matrix matrix_transpon(struct Matrix A)
{
    struct Matrix result = matrix_allocate(A.rows, A.cols);

    for (size_t row = 0; row < A.rows; ++row)
    {
        for (size_t col = 0; col < A.cols; ++col)
        {
            result.data[col * A.rows + row] = A.data[row * A.cols + col];
        }
    }
    return result;
}

//вычисляем определитель
double determinant(struct Matrix A)
{
    if (A.rows != A.cols)
    {
        matrix_error(ERROR, "|A|", "Определитель невозможно вычислить (неверные размеры матриц)");
        return 0;
    }

    // Базовый случай для матрицы 1x1
    if (A.rows == 1)
    {
        return A.data[0];
    }

    // Базовый случай для матрицы 2x2
    if (A.rows == 2)
    {
        return A.data[0] * A.data[3] - A.data[1] * A.data[2];
    }

    double det = 0;

    // Разложение по первой строке
    for (size_t col = 0; col < A.cols; col++)
    {
        struct Matrix minor = matrix_allocate(A.cols - 1, A.rows - 1);//создаём миноры

        int minor_row = 0;
        for (size_t row = 1; row < A.rows; row++)
        {
            int minor_col = 0;
            for (size_t k_col = 0; k_col < A.cols; k_col++)
            {
                if (k_col != col)
                {
                    minor.data[minor_row * (A.cols - 1) + minor_col] = A.data[row * A.cols + k_col];
                    minor_col++;
                }
            }
            minor_row++;
        }

        double minor_det = determinant(minor);

        det += (col % 2 == 0 ? 1 : -1) * A.data[col] * minor_det;

        free(minor.data);
    }
    
    return det;
}
//копия матрицы
struct Matrix matrix_copy(struct Matrix A, const struct Matrix B){
    for (size_t i = 0; i < A.rows * A.cols; i++)
    {
        A.data[i]=B.data[i]
    }
}
//возведение в степень
struct Matrix matrix_power(struct Matrix, int power){
    if (A.rows != A.cols)
    {
        matrix_error(ERROR, "|A|", "Определитель невозможно вычислить (неверные размеры матриц)");
        return 0;
    }
    struct Matrix result = matrix_allocate(A.cols, A.rows);

    matrix_copy(result, A);

    for (int p = 1; p < power; p++)
    {
        result = matrix_multiply(A, result);
    }

    return result;
}
//единичная матрица
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

//факториал
double factorial(const int n)
{
    double fact = 1.0;
    for (size_t i = 1; i <= n; ++i)
    {
        fact *= i;
    }

    return fact;
}

//экспонента
struct Matrix matrix_exponent(const struct Matrix A, int terms)
{

    if (A.rows != A.cols)
    {
        matrix_error(ERROR, "|A|", "Определитель невозможно вычислить (неверные размеры матриц)");
        return 0;
    }

    struct Matrix result = matrix_allocate(A.rows, A.cols);
    matrix_identity(result);
    struct Matrix temp = matrix_allocate(A.rows, A.cols);

    for (int n = 1; n < terms; ++n)
    {
        // Вычисляем temp = A^n
        temp = matrix_power(A, n);
        double fact = factorial(n);

        // Делим temp на факториал и сохраняем результат в term
        for (size_t i = 0; i < result.rows * result.cols; ++i)
        {
            result.data[i] += temp.data[i] / fact;
        }8
    }

    matrix_free(&temp);
    // Освобождаем память, выделенную для вспомогательных матриц

    return result;
}

int main()
{
    srand(time(NULL)); // Инициализация рандома

    struct Matrix A = matrix_allocate(3, 3);
    struct Matrix B = matrix_allocate(3, 3);

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
    if (matrix_add(A, B, result_add) != NAN)
    {
        printf("Результат A + B:\n");
        matrix_print(result_add);
    }

    // Вычитаем матрицы с проверкой на ошибку
    if (matrix_subtract(A, B, result_subtract) != NAN)
    {
        printf("Результат A - B:\n");
        matrix_print(result_subtract);
    }
    
    //Умножаем матрицу на число
    struct Matrix matrix_const = matrix_multipliy_const(A, 2);
    printf("\nРезультат умножения на константу\n");
    matrix_print(matrix_const);
    


    // Умножаем матрицы
    struct Matrix C = matrix_multiply(A, B);
    if (C.data != NULL)
    {
        printf("Результат A * B:\n");
        matrix_print(C);;
    }

    //Транспонируем матрицу
    struct Matrix transpon = matrix_transpon(A);
    printf("Результат транспонирования\n");
    matrix_print(transpon);
    
    //Расчитываем определитель
    double det = determinant(A);
    if (det != NAN)
    {
        printf("Результат определителя:\n");
        printf("%lf", det);
    }

    //Возведение в степень
    struct Matrix P = matrix_power(A, 3);
    printf("Результат возведения в степень\n");
    matrix_print(P);

    //Находим экспоненту
    struct Matrix exp(A, 2)
    printf("Результат нахождения экспоненты\n");
    matrix_print(exp);

    // Освобождаем память
    matrix_free(&A);
    matrix_free(&B);
    matrix_free(&result_add);
    matrix_free(&result_subtract);
    matrix_free(&matrix_const);
    matrix_free(&C);
    matrix_free(&transpon);
    matrix_free(&P);
    matrix_free(&exp);

    return 0;
}
