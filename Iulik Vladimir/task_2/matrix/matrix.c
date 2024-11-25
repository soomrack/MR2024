#include "matrix.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <time.h>

// Факториал
static double factorial(int n)
{
    return (n < 2) ? 1 : n * factorial(n-1);
}


// Вывод ошибки
void matrix_error(const enum MatrixExceptionLevel level, const char *location, const char *msg)
{
    if (level == ERROR){
        printf("\nERROR in: %s Text: (%s)\n", location, msg);
    }

    if (level == DEBUG){
        printf("\nDEBUG\nLoc: %s\nText: %s\n", location, msg);
    }
}


// Выделение памяти для матрицы
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


// Освобождение памяти для матрицы
void matrix_free(struct Matrix *A)
{
    if (A->data != NULL){
        free(A->data);
        A->data = NULL;
    }
    A->cols = 0;
    A->rows = 0;
}


// Заполнение матрицы случайными числами
void matrix_fill_random(struct Matrix A)
{
    int sight;
    for (size_t idx = 0; idx < A.rows * A.cols; ++idx){
        sight = (rand() % 2) * 2 - 1;
        A.data[idx] = (double)((rand() % 10) * sight);
    }
}


// Вывод матрицы
void matrix_print(const struct Matrix A)
{
    for (size_t row = 0; row < A.rows; ++row){
        for (size_t col = 0; col < A.cols; ++col){
            printf("%lf \t", A.data[row * A.cols + col]);
        }
        printf("\n");
    }
}


// Сложение матриц
struct Matrix matrix_add(struct Matrix A, struct Matrix B)
{
    if (A.cols != B.cols || A.rows != B.rows){
        matrix_error(ERROR, "C = A + B", "размеры матриц не совпадают для сложения");
        return (struct Matrix){0, 0, NULL};
    }
    struct Matrix C = matrix_allocate(A.rows, A.cols);

    for (size_t idx = 0; idx < A.rows * A.cols; ++idx){
        C.data[idx] = A.data[idx] + B.data[idx];
    }

    return C;
}


// Вычитание матриц
struct Matrix matrix_subtract(struct Matrix A, struct Matrix B)
{
    if (A.cols != B.cols || A.rows != B.rows){
        matrix_error(ERROR, "C = A - B", "размеры матриц не совпадают для вычитания");
        return (struct Matrix){0, 0, NULL};
    }

	struct Matrix C = matrix_allocate(A.rows, A.cols);
	
    for (size_t idx = 0; idx < A.rows * A.cols; ++idx){
        C.data[idx] = A.data[idx] - B.data[idx];
    }

    return C;
}


// Копирование матрицы
struct Matrix matrix_copy(struct Matrix A)
{
    struct Matrix C = matrix_allocate(A.rows, A.cols);

    for (size_t idx = 0; idx < A.rows * A.cols; ++idx){
        C.data[idx] = A.data[idx];
    }

    return C;
}


// Умножение матрицы на константу
struct Matrix matrix_multipliy_const(struct Matrix A, double constant)
{
    struct Matrix result = matrix_allocate(A.rows, A.cols);
    for (size_t idx = 0; idx < A.rows * A.cols; ++idx){
        result.data[idx] = A.data[idx] * constant;
    }

    return result;
}


// Умножение матриц
struct Matrix matrix_multiply(struct Matrix A, struct Matrix B)
{
    if (A.cols != B.rows){
        matrix_error(ERROR, "C = A * B", "Умножение двух матриц не удалось (неверные размеры матриц)");
        return (struct Matrix){0, 0, NULL};
    }

    struct Matrix C = matrix_allocate(B.cols, A.rows);

    if (C.data == NULL){
        return C;
    }

    for (size_t row = 0; row < A.rows; ++row){
        for (size_t col = 0; col < B.cols; ++col){
            C.data[row * C.cols + col] = 0;
            for (size_t k_col = 0; k_col < A.cols; ++k_col){
                C.data[row * C.cols + col] += A.data[row * A.cols + k_col] * B.data[k_col * B.cols + col];
            }
        }
    }

    return C;
}


// Возведение матрицы в степень
struct Matrix matrix_pow(struct Matrix A, const size_t degree)
{
	if (A.rows != A.cols) {
        matrix_error(ERROR, "C = A^n" ,"Возведение в степень матрицы не удалось (неверные размеры матриц)");
        return (struct Matrix){0, 0, NULL};
    }
    
    struct Matrix C = matrix_allocate(A.rows, A.cols);
    C = matrix_copy(A);
    
    if (degree == 0){
        for (size_t row = 0; row < C.rows; ++row){
            for (size_t col = 0; col < C.cols; ++col){
                if (row == col){
                    C.data[row * C.cols + col] = 1;
                }
                else{
                    C.data[row * C.cols + col] = 0;
                }
            }
        }
    }
    for (size_t idx = 1; idx < degree; ++idx)
    {
        struct Matrix temp = matrix_multiply(A, C);
        matrix_free(&C);
        C = matrix_copy(temp);
        matrix_free(&temp);
    }
    return C;
}


// Транспонирование матрицы
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


// Экспоненциация матрицы
struct Matrix matrix_exp(struct Matrix A)
{
    if (A.cols != A.rows){
        matrix_error(ERROR, "exp(A)", "Экспонента матрицы не удалась (неверные размеры матриц)");
        return (struct Matrix){0, 0, NULL};
    }

    struct Matrix C = matrix_allocate(A.rows, A.cols);
    struct Matrix E = matrix_allocate(A.rows, A.cols);
    struct Matrix E_SUM = matrix_allocate(A.rows, A.cols);
    E_SUM = matrix_pow(A,0);

    for (int idx = 1; idx <= 20; ++idx){
        C = matrix_pow(A,idx);
        E = matrix_multipliy_const(C,1/factorial(idx));
        E_SUM = matrix_add(E_SUM, E);
        matrix_free(&C);
        matrix_free(&E);
    }
    return E_SUM;
}


// Вычисление определителя
double determinant(struct Matrix A)
{
    if (A.rows != A.cols){
        matrix_error(ERROR, "|A|", "Определитель невозможно вычислить (неверные размеры матриц)");
        return NAN;
    }

    if (A.rows == 1){
        return A.data[0];
    }

    if (A.rows == 2){
        return A.data[0] * A.data[3] - A.data[1] * A.data[2];
    }

    double det = 0;

    for (size_t col = 0; col < A.cols; col++){
        struct Matrix submatrix = matrix_allocate(A.rows - 1, A.cols - 1);

        for (size_t row = 1; row < A.rows; ++row){
            size_t col_idx = 0;
            for (size_t subcol = 0; subcol < A.cols; ++subcol){
                if (subcol == col){
                    continue;
                }
                submatrix.data[(row - 1) * (A.cols - 1) + col_idx++] = A.data[row * A.cols + subcol];
            }
        }
        det += pow(-1, col) * A.data[col] * determinant(submatrix);
        matrix_free(&submatrix);
    }

    return det;
}

