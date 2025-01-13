
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <math.h>


struct Matrix
{
    size_t cols;
    size_t rows;
    double *data;//первая ячейка памяти
};
const double scalar = 7;
enum MatrixExceptionLevel
{
    ERROR,
    WARNING,
    INFO,
    DEBUG
};//создание массива

const struct Matrix MATRIX_NULL = {0, 0, NULL};

//вывод информации
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

// Выделение памяти для матрицы
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

// Освобождение памяти для матрицы
void matrix_free(const struct Matrix *M)
{
    if (M == NULL) {
        matrix_exception(ERROR, "Обращение к недопутимой области памяти");
        return;
    }

    free(M->data);
    M = NULL;
    return;
}


void matrix_zero(const struct Matrix M)
{
    memset(M.data, 0, M.cols * M.rows * sizeof(double));
}

// Заполнение матрицы случайными числами
void matrix_random(const struct Matrix A)
{
    for (size_t elem = 0; elem < A.rows * A.cols; ++elem)
        A.data[elem] = (double)(rand() % 10);
}

//принтим матрицу
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


// Сложение матриц A и B
struct Matrix matrix_add(const struct Matrix A, const struct Matrix B)
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

// Вычитание матриц A и B
struct Matrix matrix_substruct(const struct Matrix A, const struct Matrix B)
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

// Умножение матриц A и B
struct Matrix matrix_multiply(const struct Matrix A,const struct Matrix B)
{
    if (A.cols != B.rows) {
        matrix_exception(WARNING, "Число столбцов первой матрицы не равно числу строк второй матрицы.\n");
        return MATRIX_NULL;
    }

    struct Matrix result = matrix_allocate(A.rows, B.cols);
    matrix_zero(result);

    for (size_t row = 0; row < result.rows; ++row) {
        for (size_t col = 0; col < result.cols; ++col) {
            for (size_t k = 0; k < result.cols; ++k) {
                result.data[row * result.cols + col] += A.data[row * result.cols + k] * B.data[k * result.cols + col];
            }
        }
    }

    return result;
}

//умножение матрицы на число
struct Matrix matrix_scalar(const struct Matrix A, double scalar)
{
    struct Matrix result = matrix_allocate(A.rows, A.cols);

    for (size_t elem = 0; elem < result.rows * result.cols; ++elem) {
        result.data[elem] = A.data[elem] * scalar;
    }

    return result;
}

//транспонирование
struct Matrix matrix_transposition(const struct Matrix A)
{
    struct Matrix result = matrix_allocate(A.cols, A.rows);

    for (size_t row = 0; row < result.rows; ++row) {
        for (size_t col = 0; col < result.cols; ++col) {
            result.data[col * result.rows + row] = A.data[row * A.cols + col];
        }
    }

    return result;
}

//вычисляем определитель
double matrix_determinate(const struct Matrix A)
{
    if (A.rows != A.cols) {
        matrix_exception(WARNING, "Матрица должна быть квадратной для нахождения определителя.\n");
        return NAN;
    }

    if (A.rows == 0 && A.cols == 0) {
        matrix_exception(ERROR, "Количесвто строк и столбцов не может быть меньше 1.\n");
        return NAN;
    }
    // Базовый случай для матрицы 1x1
    if (A.rows == 1 && A.cols == 1) {
        return A.data[0];
    }
    // Базовый случай для матрицы 2x2
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


struct Matrix matrix_identity(const struct Matrix M)
{
    struct Matrix I = matrix_allocate(M.cols, M.rows);
    matrix_zero(I);

    for (size_t row = 0; row < I.rows; ++row) {
        I.data[row * I.cols + row] = 1.0;
    }

    return I;
}

//факториал
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

//копия матрицы
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


void matrix_copy_void(const struct Matrix src, const struct Matrix dest)
{
    memcpy(src.data, dest.data, dest.cols * dest.rows * sizeof(double));
    return;
}

//возведение в степень
struct Matrix matrix_power(const struct Matrix A, unsigned int power) // Возведение матрицы в степень
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

    for (unsigned int k = 2; k < power; ++k) {
        struct Matrix result = matrix_multiply(temp, A);
        matrix_free(&temp);
        temp = result;
    }

    return temp;
}

//экспонента
struct Matrix matrix_exponent(const struct Matrix A, int terms)
{
    if (A.rows != A.cols) {
        matrix_exception(WARNING, "Матрица должна быть квадратной для вычисления экспоненты");
        return MATRIX_NULL;
    }

    struct Matrix result = matrix_allocate(A.rows, A.cols);

    for (int n = 0; n < terms; ++n) {
        struct Matrix temp = matrix_power(A, n);
        double fact = 1 / factorial(n);

        struct Matrix result_new = matrix_scalar(temp, fact);
        matrix_copy_void(result, result_new);

        matrix_free(&temp);
        matrix_free(&result_new);
    }

    return result;
}


int main()
{
    srand(time(NULL));// Инициализация рандома

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
