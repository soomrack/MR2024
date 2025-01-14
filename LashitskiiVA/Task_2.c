#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <locale.h>

struct Matrix 
{
    size_t rows;
    size_t cols;
    double* data;
};
typedef struct Matrix Matrix;

enum MatrixExceptionLevel {ERROR, WARNING, INFO, DEBUG};

const Matrix MATRIX_NULL = {0, 0, NULL};


// Сообщение об ошибке
void exM(const enum MatrixExceptionLevel level, char *msg)
{
    if(level == ERROR) {
        printf("ERROR: %s", msg);
    }

    if(level == WARNING) {
        printf("WARNING: %s", msg);
    }
    
    if(level == INFO) {
        printf("INFO: %s", msg);
    }
}


Matrix allM(const size_t rows, const size_t cols) 
{
    Matrix M;
    
    if (rows == 0 || cols == 0) {
        exM(INFO, "Матрица содержит 0 столбцов или строк");
        return (Matrix) {rows, cols, NULL};
    }
    
    size_t size = rows * cols;
    if (size / rows != cols) {
        exM(ERROR, "OVERFLOW: Переполнение количества элементов.");
        return MATRIX_NULL;
    }
    
    size_t size_in_bytes = size * sizeof(double);
    
    if (size_in_bytes / sizeof(double) != size) {
        exM(ERROR, "OVERFLOW: Переполнение выделенной памяти");
        return MATRIX_NULL;
    }
    
    M.data = malloc(rows * cols * sizeof(double));
    
    if (M.data == NULL) {
        exM(ERROR, "Сбой выделения памяти");
        return MATRIX_NULL;
    }
    
    M.rows = rows;
    M.cols = cols;
    return M;
}


void freeM(Matrix* M)  // Функция для освобождения памяти матрицы
{
    if (M == NULL){
        exM(ERROR, "Обращение к недопутимой области памяти");
        return;
    }
    
    free(M->data);
    *M = MATRIX_NULL;
}


// Нулевая матрица
void zM(const Matrix M)
{
    memset(M.data, 0, M.cols * M.rows * sizeof(double));
}


// Единичная матрица
Matrix iM(size_t size)
{
    Matrix M = allM(size, size);

    zM(M);

    for (size_t idx = 0; idx < size; idx++) {
        M.data[idx * size + idx] = 1.0;
    }

    return M; 
}



void prM(const Matrix M) // Функция для печати матрицы
{
    for (size_t row = 0; row < M.rows; row++) {
        for (size_t col = 0; col < M.cols; col++) {
            printf("%.2f ", M.data[row * M.cols + col]);
        }
        printf("\n");
    }
}


// C = A + B 
Matrix sumM(const Matrix A, const Matrix B) // Сложение матриц
{
    if (A.rows != B.rows || A.cols != B.cols) {
        exM(WARNING, "Размеры матриц не подходят для сложения.\n");
        return MATRIX_NULL;
    }

    Matrix C = allM(A.rows, A.cols);
    
    for (size_t idx = 0; idx < C.rows * C.cols; idx++) {
        C.data[idx] = A.data[idx] + B.data[idx];
    }
    return C;
}


// C = A - B 
Matrix subM(const Matrix A, const Matrix B) // Вычитание матриц
{
    if (A.rows != B.rows || A.cols != B.cols) {
        exM(WARNING, "Размеры матриц не подходят для вычитания.\n");
        return MATRIX_NULL;
    }

    Matrix C = allM(A.rows, A.cols);
    for (size_t idx = 0; idx < C.rows * C.cols; idx++) {
        C.data[idx] = A.data[idx] - B.data[idx];
    }
    return C;
}


// C = A * B
Matrix mulM(const Matrix A, const Matrix B) // Умножение матриц
{
    if (A.cols != B.rows) {
        exM(WARNING,"Число столбцов первой матрицы не равно числу строк второй матрицы.\n");
        return MATRIX_NULL;
    }

    Matrix C = allM(A.rows, B.cols);
    
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


Matrix TM(const Matrix A) // Транспонирование матрицы
{
    Matrix T = allM(A.cols, A.rows);
    
    for (size_t row = 0; row < T.rows; row++) {
        for (size_t col = 0; col < T.cols; col++) {
            T.data[col * T.cols + row] = A.data[row * A.cols + col];
        }
    }
    return T;
}


Matrix powM(const Matrix A, int power)  // Возведение матрицы в степень
{
    if (A.rows != A.cols) {
        exM(WARNING, "Матрица должна быть квадратной для возведения в степень.\n");
        return MATRIX_NULL;
    }
    
    Matrix result = iM(A.rows); // Создаем единичную матрицу

    for (int n = 0; n < power; n++) {
        Matrix temp = mulM(result, A);
        freeM(&result);
        result = temp;
    }

    return result;
}

// C = A * k
Matrix scalM(const Matrix A, double scalar) // Умножение матрицы на число
{
    Matrix C = allM(A.rows, A.cols);
    
    for (size_t idx = 0; idx < C.rows * C.cols; idx++) {
        C.data[idx] = A.data[idx] * scalar;
    }
    return C;
}


double detMt(const Matrix A) // Определитель матрицы (для 2x2 и 3x3)
{
    if (A.rows != A.cols) {
        exM(WARNING, "Матрица должна быть квадратной для транспонирования.\n");
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


double fakt (const unsigned int f) 
{
    unsigned long long int res = 1;
    for (unsigned int idx = 1; idx <= f; idx++) {
        res *= idx;
    }

    return res;
}


// e ^ A
Matrix expM(const Matrix A, const unsigned int num)
{
    if (A.rows != A.cols) {
        exM(WARNING, "Матрица должна быть квадратной для вычисления экспоненты");
        return MATRIX_NULL;
    }

    Matrix E = iM(A.rows);

    if (E.data == NULL) {
        return MATRIX_NULL; // Проверка на успешное выделение памяти
    }

    if (num == 1) {
        return E;
    }
    
    for (size_t cur_num = 1; cur_num < num; ++cur_num) {
        Matrix tmp = powM(A, cur_num);
        if (tmp.data == NULL) {
            freeM(&E); 
            return MATRIX_NULL;
        }

        Matrix scaled_tmp = scalM(tmp, 1.0 / fakt(cur_num)); 

        Matrix new_E = sumM(E, scaled_tmp);
        freeM(&E); 
        E = new_E; 

        freeM(&tmp); 
        freeM(&scaled_tmp);
    }
    
    return E; // Возвращаем результирующую матрицу
}


int main() 
{
    setlocale(LC_ALL, "Rus");
    Matrix A = allM(3,3);
    Matrix B = allM(3, 3);

    double data_A[9] = {3, 1, 7, 0, 5, 7, 2, 5, 8};
    double data_B[9] = {5, 0, 8, 1, 9, 6, 3, 2, 1};

    memcpy(A.data, data_A, 9 * sizeof(double));
    memcpy(B.data, data_B, 9 * sizeof(double));

    // Печать исходных матриц
    printf("Матрица A:\n");
    prM(A);

    printf("Матрица B:\n");
    prM(B);

    // Сложение
    Matrix C = sumM(A, B);
    printf("Результат сложения:\n");
    prM(C);

    // Вычитание
    Matrix D = subM(A, B);
    printf("Результат вычитания:\n");
    prM(D);

    // Умножение
    Matrix E = mulM(A, B);
    printf("Результат умножения:\n");
    prM(E);

    // Транспонирование
    Matrix T = TM(A);
    printf("Транспонированная матрица A:\n");
    prM(T);

    // Возведение в степень
    int power = 3;
    Matrix F = powM(A, power);
    printf("Матрица A в степени %d:\n", power);
    prM(F);

    // Умножение на число
    double scalar = 5;
    Matrix G = scalM(A, scalar);
    printf("Матрица A умноженная на %2.f:\n", scalar);
    prM(G);
    
    // Определитель
    printf("Определитель матрицы A: %2.f \n", detMt(A));
    
    //Матричная экспонента
    Matrix exponent_A = expM(A, 3);
    printf("Матричная экспонента от A:\n");
    prM(exponent_A);

    // Освобождение памяти
    freeM(&A);
    freeM(&B);
    freeM(&C);
    freeM(&D);
    freeM(&E);
    freeM(&T);
    freeM(&F);
    freeM(&G);
    freeM(&exponent_A);

    return 0;
}