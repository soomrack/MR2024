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

enum MatrixExceptionLevel {ERROR, WARNING, INFO};

const Matrix MATRIX_NULL = {0, 0, NULL};


// Ошибки
void matrix_error(const enum MatrixExceptionLevel level, char *msg)
{
    if(level == ERROR) {
        printf("ERROR: %s", msg);
    }

    if(level == WARNING) {
        printf("WARNING: %s", msg);
    }
}


Matrix matrix_alloc(const size_t rows, const size_t cols) 
{
    Matrix M;
    
    if (rows == 0 || cols == 0) {
        matrix_error(WARNING, "Матрица недокомлектована!");
        return (Matrix) {rows, cols, NULL};
    }
    
    
    M.data = (double*)malloc(rows * cols * sizeof(double));
    
    if (M.data == NULL) {
        matrix_error(WARNING, "Сбой выделения памяти!");
        return MATRIX_NULL;
    }
    
    M.rows = rows;
    M.cols = cols;
    return M;
}


void matrix_free(Matrix* M)  // Освобождение памяти матрицы
{
    free(M->data);
    *M = MATRIX_NULL;
}


// Создание единичной матрицы
Matrix identity_matrix(size_t size) 
{
    Matrix identity = {size, size, (double*)malloc(size * size * sizeof(double))};
    if (identity.data == NULL) {
        matrix_error(ERROR, "Ошбика выделения памяти на еденичную матрицу!.\n");
        exit(1);
    }

    for (size_t i = 0; i < size; i++) {
        for (size_t j = 0; j < size; j++) {
            identity.data[i * size + j] = (i == j) ? 1.0 : 0.0;
        }
    }

    return identity;
}


//Копирование матрицы
Matrix matrix_copy(const Matrix A) {
    Matrix copy = matrix_alloc(A.rows, A.cols);
    for (size_t i = 0; i < A.rows; i++) {
        for (size_t j = 0; j < A.cols; j++) {
            copy.data[i * A.cols + j] = A.data[i * A.cols + j];
        }
    }
    return copy;
}


void matrix_print(const Matrix M) // Вывод
{
    for (size_t row = 0; row < M.rows; row++) {
        for (size_t col = 0; col < M.cols; col++) {
            printf("%.2f ", M.data[row * M.cols + col]);
        }
        printf("\n");
    }
    printf("\n");
}
 
Matrix matrix_sum(const Matrix A, const Matrix B) // Сложение матриц
{
    if (A.rows != B.rows || A.cols != B.cols) {
        matrix_error(WARNING, "Размеры матриц не подходят для сложения!\n");
        return MATRIX_NULL;
    }

    Matrix C = matrix_alloc(A.rows, A.cols);
    
    for (size_t idx = 0; idx < C.rows * C.cols; idx++) {
        C.data[idx] = A.data[idx] + B.data[idx];
    }
    return C;
}


Matrix matrix_subtract(const Matrix A, const Matrix B) // Вычитание матриц
{
    if (A.rows != B.rows || A.cols != B.cols) {
        matrix_error(WARNING, "Размеры матриц не подходят для вычитания!\n");
        return MATRIX_NULL;
    }

    Matrix C = matrix_alloc(A.rows, A.cols);
    for (size_t idx = 0; idx < C.rows * C.cols; idx++) {
        C.data[idx] = A.data[idx] - B.data[idx];
    }
    return C;
}


Matrix matrix_multiply(const Matrix A, const Matrix B) // Умножение матриц
{
    if (A.cols != B.rows) {
        matrix_error(WARNING,"Число столбцов первой матрицы не равно числу строк второй матрицы!\n");
        return MATRIX_NULL;
    }

    Matrix C = matrix_alloc(A.rows, B.cols);
    
    for (size_t row = 0; row < C.rows; row++) {
        for (size_t col = 0; col < C.cols; col++) {
            C.data[row * B.cols + col] = 0;
            for (size_t idx = 0; idx < A.cols; idx++) {
                C.data[row * C.cols + col] += A.data[row * A.cols + idx] * B.data[idx * B.cols + col];
            }
        }
    }
    return C;
}


Matrix matrix_transpose(const Matrix A) // Транспонирование матрицы
{
    Matrix T = matrix_alloc(A.cols, A.rows);
    
    for (size_t row = 0; row < T.rows; row++) {
        for (size_t col = 0; col < T.cols; col++) {
            T.data[col * T.cols + row] = A.data[row * A.cols + col];
        }
    }
    return T;
}


// Возведение матрицы в степень
Matrix matrix_power(const Matrix A, int power) 
{
    if (A.rows != A.cols) {
        matrix_error(WARNING, "Матрица должна быть квадратной для возведения в степень!\n");
        return MATRIX_NULL;
    }
    
    Matrix result = identity_matrix(A.rows); // Создаем единичную матрицу

    for (int n = 0; n < power; n++) {
        Matrix temp = matrix_multiply(result, A);
        matrix_free(&result);
        result = temp;
    }

    return result;
}

// C = A * k
Matrix matrix_by_multiplier(const Matrix A, double multiplier) // Умножение матрицы на число
{
    Matrix C = matrix_alloc(A.rows, A.cols);
    
    for (size_t idx = 0; idx < C.rows * C.cols; idx++) {
        C.data[idx] = A.data[idx] * multiplier;
    }
    return C;
}


double matrix_determinant(const Matrix A) // Определитель матрицы (2на2 и 3на3)
{
    if (A.rows != A.cols || A.cols != A.rows) {
        matrix_error(WARNING, "Матрица должна быть квадратной для транспонирования!\n");
        return NAN;
    }
    
    if (A.rows == 1 && A.cols == 1) {
        return A.data[0];
    } 
    
    if (A.rows == 2 && A.cols == 2){
        return A.data[0] * A.data[3] - A.data[1] * A.data[2];
    }
    
    if (A.rows == 3 && A.cols == 3) {
        return A.data[0] * (A.data[4] * A.data[8] - A.data[5] * A.data[7]) -
               A.data[1] * (A.data[3] * A.data[8] - A.data[5] * A.data[6]) +
               A.data[2] * (A.data[3] * A.data[7] - A.data[4] * A.data[6]);
    }
    return 0; 
}


Matrix matrix_exponential(const Matrix A) // Экспонента
{
     if (A.rows != A.cols) {
        printf("Ошибка: Матрица должна быть квадратной для вычисления экспоненты!\n");
        return matrix_alloc(0, 0); 
    }

    Matrix result = matrix_alloc(A.rows, A.cols);
    for (size_t i = 0; i < result.rows; i++) {
        for (size_t j = 0; j < result.cols; j++) {
            result.data[i * result.cols + j] = (i == j) ? 1.0 : 0.0; // Единичная матрица
        }
    }

    Matrix current_power = matrix_alloc(A.rows, A.cols);
    current_power = matrix_copy(A); // Копируем A в current_power

    double factorial = 1.0;
    int n = 1;

    while (n <= 20) {  //чем больше n, тем больше точность
        factorial *= n;

        Matrix term = matrix_by_multiplier(current_power, 1.0 / factorial);
        Matrix temp_result = matrix_sum(result, term);
        
        matrix_free(&result);
        result = temp_result;

        Matrix temp_power = matrix_multiply(current_power, A);
        matrix_free(&current_power);
        current_power = temp_power;

        n++;
    }

    matrix_free(&current_power);
    return result;
}

int main() 
{
    Matrix A = matrix_alloc(3,3);
    Matrix B = matrix_alloc(3, 3);
    double data_A[9] = {1, 1, 5, 7, 0, 5, 9, 5, 8};
    double data_B[9] = {5, 7, 8, 1, 2, 6, 0, 4, 1};

    memcpy(A.data, data_A, 9 * sizeof(double));
    memcpy(B.data, data_B, 9 * sizeof(double));

    // Печать исходных матриц
    printf("Матрица A:\n");
    matrix_print(A);

    printf("Матрица B:\n");
    matrix_print(B);

    // Сложение
    Matrix Sum = matrix_sum(A, B);
    printf("Результат сложения:\n");
    matrix_print(Sum);

    // Вычитание
    Matrix Sub = matrix_subtract(A, B);
    printf("Результат вычитания:\n");
    matrix_print(Sub);

    // Умножение
    Matrix M = matrix_multiply(A, B);
    printf("Результат умножения:\n");
    matrix_print(M);

    // Транспонирование
    Matrix T = matrix_transpose(A);
    printf("Транспонированная матрица A:\n");
    matrix_print(T);

    // Возведение в степень
    int power = 3;
    Matrix P = matrix_power(A, power);
    printf("Матрица A в степени %d:\n", power);
    matrix_print(P);

    // Умножение на число
    double multiplier= 5;
    Matrix Mult = matrix_by_multiplier(A, multiplier);
    printf("Матрица A умноженная на %2.f:\n", multiplier);
    matrix_print(Mult);
    
    // Определитель
    printf("Определитель матрицы A: %2.f \n", matrix_determinant(A));
    
    // Матричная экспонента
    Matrix Exp = matrix_exponential(A);
    printf("Матричная экспонента от A:\n");
    matrix_print(Exp);

    // Освобождение памяти
    matrix_free(&A);
    matrix_free(&B);
    matrix_free(&Sum);
    matrix_free(&Sub);
    matrix_free(&M);
    matrix_free(&T);
    matrix_free(&P);
    matrix_free(&Mult);
    matrix_free(&Exp);

    return 0;
}


