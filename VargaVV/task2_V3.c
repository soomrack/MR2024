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

typedef enum {
    ERROR,
    WARNING
} MatrixExceptionLevel;


const Matrix MATRIX_NULL = {0, 0, NULL};


// Ошибки
void matrix_error(const MatrixExceptionLevel level, const char* location, const char* msg) {
    if (level == ERROR) {
        printf("nERRORnLoc: %snText: %sn", location, msg);
    }

    if (level == WARNING) {
        printf("nWARNINGnLoc: %snText: %sn", location, msg);
    }
}

Matrix matrix_alloc(const size_t rows, const size_t cols) {
    Matrix M;

    if (rows == 0  || cols == 0) { 
        matrix_error(WARNING, "matrix_alloc", "Матрица содержит 0 элементов!\n");
        return (Matrix){rows, cols, NULL};
    }

    if (SIZE_MAX / cols < rows) { // Проверка переполнения
        matrix_error(ERROR, "matrix_alloc", "Число строк или столбцов равно 0\n");
        return (Matrix){0, 0, NULL}; // Возвращаем пустую матрицу
    }

    if (SIZE_MAX / (cols * sizeof(double)) < rows) { 
        matrix_error(ERROR, "matrix_alloc", "Не хватит места для выделения памяти под строки и столбцы\n");
        return (Matrix){0, 0, NULL}; // Возвращаем пустую матрицу
    }

    M.data = (double*)malloc(rows * cols * sizeof(double));

    if (M.data == NULL) {
        matrix_error("WARNING", "matrix_alloc", "Сбой выделения памяти!");
        return MATRIX_NULL;
    }

    M.rows = rows;
    M.cols = cols;
    return M;
}


void matrix_free(Matrix* M)  // Освобождение памяти матрицы
{
    if (M == NULL) {
        return;
    }

    free(M->data); 
    *M = MATRIX_NULL;
}


// Создание единичной матрицы
Matrix identity_matrix(size_t size) 
{
    Matrix identity = {size, size, (double*)malloc(size * size * sizeof(double))}; 
    if (identity.data == NULL) {
        matrix_error(ERROR, "identity_matrix", "Ошбика выделения памяти на еденичную матрицу!.\n");
    }

    Matrix M = matrix_alloc(M.rows, M.cols);
    memset(M.data, 0, M.cols * M.rows * sizeof(double));

    for (size_t row = 0; row < M.rows; row++) {                  
        for (size_t col = 0; col < M.cols; col++) {
            identity.data[row * size + col] = (row == col) ? 1.0 : 0.0;
        }
    }
    
    return identity;
}


//Копирование матрицы
Matrix matrix_copy(const Matrix A) {
    Matrix copy = matrix_alloc(A.rows, A.cols);
    for (size_t rows = 0; rows < A.rows; rows++) {
        for (size_t cols = 0; cols < A.cols; cols++) {
            copy.data[rows * A.cols + cols] = A.data[rows * A.cols + cols];
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
        matrix_error(WARNING, "matrix_sum", "Размеры матриц не подходят для сложения!\n");
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
        matrix_error(WARNING, "matrix_substract", "Размеры матриц не подходят для вычитания!\n");
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
        matrix_error(WARNING, "matrix_multiply", "Число столбцов первой матрицы не равно числу строк второй матрицы!\n");
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
Matrix matrix_power(const Matrix A, unsigned int power) 
{
    if (A.rows != A.cols) {
        matrix_error(WARNING, "matrix_power", "Матрица должна быть квадратной для возведения в степень!\n");
        return MATRIX_NULL;
    }
    
    Matrix result = identity_matrix(A.rows); // Создаем единичную матрицу   
    // ЕСЛИ не создалась, то выход

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


double matrix_determinant(const Matrix A) // Определитель матрицы 
{
    if (A.rows != A.cols || A.cols != A.rows) {
        matrix_error(WARNING, "matrix_determinant", "Матрица должна быть квадратной для транспонирования!\n");
        return NAN;
    }
    
    if (A.rows == 1) {
        return A.data[0];
    } 
    
    if (A.rows == 2){
        return A.data[0] * A.data[3] - A.data[1] * A.data[2];
    }
    
    if (A.rows == 3) {
        return A.data[0] * (A.data[4] * A.data[8] - A.data[5] * A.data[7]) -
               A.data[1] * (A.data[3] * A.data[8] - A.data[5] * A.data[6]) +
               A.data[2] * (A.data[3] * A.data[7] - A.data[4] * A.data[6]);
    }
    return NAN; 
}


Matrix matrix_exponential(const Matrix A) // Экспонента
{
    if (A.rows != A.cols) {
        printf("Ошибка: Матрица должна быть квадратной для вычисления экспоненты!n");
        return matrix_alloc(0, 0); 
    }

    Matrix result = matrix_alloc(A.rows, A.cols);
    result=identity_matrix(A.rows);  
    Matrix current_power = matrix_alloc(A.rows, A.cols);
    current_power = matrix_copy(A); // Копируем A в current_power

    double factorial = 1.0;
    int n = 1;

    for (n <= 40; n++;) {  
        factorial *= n;

        Matrix term = matrix_by_multiplier(current_power, 1.0 / factorial);
        Matrix temp_result = matrix_sum(result, term);                                                       

        matrix_free(&result); //
        result = temp_result;  

        Matrix temp_power = matrix_multiply(current_power, A);
        matrix_free(&current_power); // 
        current_power = temp_power;   

    }

    matrix_free(&current_power); // 
    return result; 
}

int main() 
{
    Matrix A = matrix_alloc(2, 2);
    Matrix B = matrix_alloc(2, 2);

    double data_A[4] = {1, 2, 3, 4};
    double data_B[4] = {1, 2, 3, 4};

 if (A.data == NULL || B.data == NULL) {
        printf("Ошибка выделения памяти для одной из матриц.n");
        return 1; 
    }

    memcpy(A.data, data_A, 4 * sizeof(double));
    memcpy(B.data, data_B, 4 * sizeof(double));

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