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


struct Matrix create_matrix(size_t rows, size_t cols) // Функция для создания матрицы
{
    struct Matrix m;
    m.rows = rows;
    m.cols = cols;
    m.data = (double*)malloc(rows * cols * sizeof(double));
    return m;
}


void free_matrix(struct Matrix* m) // Функция для освобождения памяти матрицы
{
    free(m->data);
    m->data = NULL;
}


void print_matrix(const struct Matrix m) // Функция для печати матрицы
{
    for (size_t i = 0; i < m.rows; i++) {
        for (size_t j = 0; j < m.cols; j++) {
            printf("%.2f ", m.data[i * m.cols + j]);
        }
        printf("\n");
    }
}


struct Matrix add_matrices(const struct Matrix A, const struct Matrix B) // Сложение матриц
{
    if (A.rows != B.rows || A.cols != B.cols) {
        printf("Ошибка: Размеры матриц не подходят для сложения.\n");
        return create_matrix(0, 0); // Возвращаем пустую матрицу
    }

    struct Matrix C = create_matrix(A.rows, A.cols);
    for (size_t i = 0; i < A.rows * A.cols; i++) {
        C.data[i] = A.data[i] + B.data[i];
    }
    return C;
}


struct Matrix subtract_matrices(const struct Matrix A, const struct Matrix B) // Вычитание матриц
{
    if (A.rows != B.rows || A.cols != B.cols) {
        printf("Ошибка: Размеры матриц не подходят для вычитания.\n");
        return create_matrix(0, 0); // Возвращаем пустую матрицу
    }

    struct Matrix C = create_matrix(A.rows, A.cols);
    for (size_t i = 0; i < A.rows * A.cols; i++) {
        C.data[i] = A.data[i] - B.data[i];
    }
    return C;
}


struct Matrix multiply_matrices(const struct Matrix A, const struct Matrix B) // Умножение матриц
{
    if (A.cols != B.rows) {
        printf("Ошибка: Число столбцов первой матрицы не равно числу строк второй матрицы.\n");
        return create_matrix(0, 0); // Возвращаем пустую матрицу
    }

    struct Matrix C = create_matrix(A.rows, B.cols);
    for (size_t i = 0; i < A.rows; i++) {
        for (size_t j = 0; j < B.cols; j++) {
            C.data[i * B.cols + j] = 0;
            for (size_t k = 0; k < A.cols; k++) {
                C.data[i * B.cols + j] += A.data[i * A.cols + k] * B.data[k * B.cols + j];
            }
        }
    }
    return C;
}


struct Matrix transpose_matrix(const struct Matrix A) // Транспонирование матрицы
{
    struct Matrix T = create_matrix(A.cols, A.rows);
    for (size_t i = 0; i < A.rows; i++) {
        for (size_t j = 0; j < A.cols; j++) {
            T.data[j * T.cols + i] = A.data[i * A.cols + j];
        }
    }
    return T;
}


struct Matrix power_matrix(const struct Matrix A, int exponent) // Возведение матрицы в степень
{
    if (A.rows != A.cols) {
        printf("Ошибка: Матрица должна быть квадратной для возведения в степень.\n");
        return create_matrix(0, 0); // Возвращаем пустую матрицу
    }

    struct Matrix result = create_matrix(A.rows, A.cols);
    for (size_t i = 0; i < result.rows; i++) {
        for (size_t j = 0; j < result.cols; j++) {
            result.data[i * result.cols + j] = (i == j) ? 1.0 : 0.0; // Идентичная матрица
        }
    }

    for (int n = 0; n < exponent; n++) {
        struct Matrix temp = multiply_matrices(result, A);
        free_matrix(&result);
        result = temp;
    }

    return result;
}


struct Matrix multiply_by_scalar(const struct Matrix A, double scalar) // Умножение матрицы на число
{
    struct Matrix C = create_matrix(A.rows, A.cols);
    for (size_t i = 0; i < A.rows * A.cols; i++) {
        C.data[i] = A.data[i] * scalar;
    }
    return C;
}


double determinant(const struct Matrix A) // Определитель матрицы (для 2x2 и 3x3)
{
    if (A.rows == 1 && A.cols == 1) {
        return A.data[0];
    } else if (A.rows == 2 && A.cols == 2){
        return A.data[0] * A.data[3] - A.data[1] * A.data[2];
    }
    else if (A.rows == 3 && A.cols == 3) {
        return A.data[0] * (A.data[4] * A.data[8] - A.data[5] * A.data[7]) -
               A.data[1] * (A.data[3] * A.data[8] - A.data[5] * A.data[6]) +
               A.data[2] * (A.data[3] * A.data[7] - A.data[4] * A.data[6]);
    }
    return 0; // Для других размеров не реализовано
}


struct Matrix matrix_exponential(const struct Matrix A) // Матричная экспонента (приближенно)
{
    if (A.rows != A.cols) {
        printf("Ошибка: Матрица должна быть квадратной для вычисления экспоненты.n");
        return create_matrix(0, 0); // Возвращаем пустую матрицу
    }

    struct Matrix result = create_matrix(A.rows, A.cols);
    for (size_t i = 0; i < result.rows; i++) {
        for (size_t j = 0; j < result.cols; j++) {
            result.data[i * result.cols + j] = (i == j) ? 1.0 : 0.0; // Единичная матрица
        }
    }

    struct Matrix current_power = create_matrix(A.rows, A.cols);
    
    // Примерный алгоритм для вычисления экспоненты
    double factorial = 1.0;

    for (int n = 1; n <= 10; n++) { // Порядок разложения
        factorial *= n;

        if (n == 1) {
            current_power = multiply_by_scalar(A, 1.0);
        } else {
            struct Matrix temp_power = multiply_matrices(current_power, A);
            free_matrix(&current_power);
            current_power = temp_power;
        }

        struct Matrix temp_result = add_matrices(result, multiply_by_scalar(current_power, 1.0 / factorial));
        free_matrix(&result);
        result = temp_result;
        
        free_matrix(&current_power);
        current_power = create_matrix(A.rows, A.cols);
    }

    free_matrix(&current_power);

    return result;
}

int main() 
{
    struct Matrix A = create_matrix(3, 3);
    struct Matrix B = create_matrix(3, 3);

    double data_A[9] = {3, 1, 7, 0, 5, 7, 2, 5, 8};
    double data_B[9] = {5, 0, 8, 1, 9, 6, 3, 2, 1};

    memcpy(A.data, data_A, 9 * sizeof(double));
    memcpy(B.data, data_B, 9 * sizeof(double));

    // Печать исходных матриц
    printf("Матрица A:\n");
    print_matrix(A);

    printf("Матрица B:\n");
    print_matrix(B);

    // Сложение
    struct Matrix C = add_matrices(A, B);
    printf("Результат сложения:\n");
    print_matrix(C);

    // Вычитание
    struct Matrix D = subtract_matrices(A, B);
    printf("Результат вычитания:\n");
    print_matrix(D);

    // Умножение
    struct Matrix E = multiply_matrices(A, B);
    printf("Результат умножения:\n");
    print_matrix(E);

    // Транспонирование
    struct Matrix T = transpose_matrix(A);
    printf("Транспонированная матрица A:\n");
    print_matrix(T);

    // Возведение в степень
    int exponent = 2;
    struct Matrix F = power_matrix(A, exponent);
    printf("Матрица A в степени %d:\n", exponent);
    print_matrix(F);

    // Умножение на число
    double scalar = 5;
    struct Matrix G = multiply_by_scalar(A, scalar);
    printf("Матрица A умноженная на %2.f:\n", scalar);
    print_matrix(G);
    
    // Определитель
    printf("Определитель матрицы A: %2.f \n", determinant(A));
    
    // Матричная экспонента
    struct Matrix exp_result = matrix_exponential(A);
    printf("Матричная экспонента от A:\n");
    print_matrix(exp_result);

    // Освобождение памяти
    free_matrix(&A);
    free_matrix(&B);
    free_matrix(&C);
    free_matrix(&D);
    free_matrix(&E);
    free_matrix(&T);
    free_matrix(&F);
    free_matrix(&G);
    free_matrix(&exp_result);

    return 0;
}
