#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


typedef struct 
{
    int rows;
    int cols;
    double *data;
} Matrix;

enum MatrixExceptionType {ERROR, WARNING, INFO};

const Matrix NULL_MATRIX = {0, 0, NULL}; // Нулевая матрица



void matrix_exception(enum MatrixExceptionType type, char *message) //Сообщение об ошибке
{
    if (type == ERROR) {
        printf("ERROR: %s\n", message);
    }
    else if (type == WARNING) {
        printf("WARNING: %s\n", message);
    } 
    else if (type == INFO) {
        printf("INFO: %s\n", message);
    }
    
}


void matrix_free(Matrix* M) //Функция для освобождения памяти матриц
{
    if (M == NULL){
        matrix_exception(ERROR, "Обращение к недопутимой области памяти - Освобождение памяти");
        return;
    }
    
    free(M->data);
    *M = NULL_MATRIX;
}


Matrix matrix_init(const int rows, const int cols) //Инициализация матрицы
{
    Matrix M1;

    if (rows == 0 || cols == 0) {
        matrix_exception(INFO, "Матрица без строк и столбцов - Инициализация матрицы");
        return (Matrix) {rows, cols, NULL};
    }
    
    int size = rows * cols;
    if (size / rows  != cols) {
        matrix_exception(ERROR, "OVERFLOW - Переполнение количества элементов - Инициализация матрицы");
        return NULL_MATRIX;
    }

    int size_in_bytes = size * sizeof(double);
    if (size_in_bytes / sizeof(double) != size) {
        matrix_exception(ERROR, "OVERFLOW: Переполнение выделенной памяти - Инициализация матрицы");
        return NULL_MATRIX;
    }

    M1.data = (double *)malloc(rows * cols * sizeof(double));

    if (M1.data == NULL) {
        matrix_exception(ERROR, "Сбой выделения памяти - Инициализация матрицы");
        return NULL_MATRIX;
    }
    
    M1.rows = rows;
    M1.cols = cols;
    
    return M1;
}


void matrix_create_zero(const Matrix M) //Создание нулевой матрицы
{
    memset(M.data, 0, M.cols * M.rows * sizeof(double));    
}


Matrix matrix_create_unit(unsigned int rows) // Создание единичной матрицы
{
    if (rows == 0) {
        return NULL_MATRIX;
    }

    Matrix I = matrix_init(rows, rows);

    for (int i = 0; i < I.cols * I.rows; i ++) {
        if (i / rows == i % rows) {
            I.data[i] = (double)1;
        }
        else {
            I.data[i] = (double)0;
        }   
    }

    return I;
}


void matrix_fill(Matrix *M1) //Заполнение матрицы арифмитической последовательностью
{
    for (size_t i = 0; i < M1->rows * M1->cols; i++) {
        M1->data[i] = (i + 1);
    }
}


void matrix_fill_power(Matrix *M1, int power) //Заполенение матрицы арифмитической прогрессией power-порядка
{
    for (size_t i = 0; i < M1->rows * M1->cols; i++) {
        M1->data[i] = pow((i + 1), power);
    }
}


Matrix matrix_clone(Matrix M1) // Клонирование матрицы
{
    Matrix result = matrix_init(M1.rows, M1.cols);

    for (size_t i = 0; i < M1.rows * M1.cols; i++) {
        result.data[i] = M1.data[i];
    }

    return result;     
}


Matrix matrix_sum(Matrix M1, Matrix M2) // Суммирование матриц
{   
    if (M1.rows != M2.rows && M1.cols != M2.cols){
        matrix_exception(ERROR, "Разные размеры матриц - Сложение матриц");
        return NULL_MATRIX;
    }

    Matrix result = matrix_init(M1.rows, M1.cols);
    
    for(size_t i = 0; i < M1.rows * M1.cols; i++) {
        result.data[i] = M1.data[i] + M2.data[i];   
    }

    return result;
}


Matrix matrix_sub(Matrix M1, Matrix M2) // Вычитание матриц
{   
    if (M1.rows != M2.rows && M1.cols != M2.cols){
        matrix_exception(ERROR, "Разные размеры матриц - Вычитание матриц");
        return NULL_MATRIX;
    }

    Matrix result = matrix_init(M1.rows, M1.cols);;

    if (M1.rows == M2.rows & M1.cols == M2.cols) {
        for (size_t i = 0; i < M1.rows * M1.cols; i++) {
            result.data[i] = M1.data[i] - M2.data[i];   
        }
    }

    return result;
}


Matrix matrix_mul_num(Matrix M1, double num) // Умножение матрицы на число
{
    Matrix result = matrix_init(M1.rows, M1.cols);

    for (size_t i = 0; i < M1.rows * M1.cols; i++) {
        result.data[i] = M1.data[i] * num;
    }

    return result;
}


Matrix matrix_div_num(Matrix M1, double num) // Деление матрицы на число
{
    Matrix result = matrix_init(M1.rows, M1.cols);;

    for (size_t i = 0; i < M1.rows * M1.cols; i++) {
        result.data[i] = (double)M1.data[i] / (double)num;
    }

    return result;
}


Matrix matrix_mul(Matrix M1, Matrix M2) // Умножение матрицы
{
    if (M1.cols != M2.rows) {
        matrix_exception(ERROR, "Количество столбцов первой матрицы не равно числу строк второй матрицы - Умножение матриц");
        return NULL_MATRIX;
    }
    
    Matrix result = matrix_init(M1.rows, M1.cols);;

    if (M1.cols == M2.rows) {
        result = matrix_init(M1.rows, M2.cols);
        for (size_t row = 0; row < M1.rows; row++) {
            for (size_t col = 0; col < M2.cols; col++) {
                result.data[row * result.cols + col] = 0;
                for (size_t i = 0; i < M1.cols; i++) {
                    result.data[row * result.cols + col] += (double)M1.data[row * M1.cols + i] * M2.data[i * M2.cols + col];
                }
            }
        }
    }

    return result;
}


Matrix matrix_pow(Matrix M1, unsigned int power) // Возведение матрицы в степень
{
    
    if (M1.cols != M1.rows && power != 1) {
        matrix_exception(ERROR, "Матрица должна быть квадратной - Возведение в степень");
        return NULL_MATRIX;
    }

    if (power == 0) {
        return matrix_create_unit(M1.rows);
    }

    Matrix result = matrix_clone(M1);

    for (size_t i = 1; i < power; i++) {
            result = matrix_mul(result, M1);
    }

    return result;
}


void matrix_print(Matrix M1) // Вывод матриц в консоль
{   
    for (size_t i = 0; i < M1.cols * M1.rows; i++) {
        if ((i) % M1.cols == 0) {
            printf("|");
        }
        printf("%.2f|", M1.data[i]);
        if ((i) % M1.cols == M1.cols - 1) {
            printf("\n");
        }
    }
    printf("\n");
}


void matrix_transp(Matrix *A) // Транспонирование матрицы
{
    Matrix M = matrix_clone(*A);
    *A = matrix_init(A->cols, A->rows);
    
    for (int i = 0; i < M.cols * M.rows; i++) {
        A->data[M.rows * (i % M.cols) + i / M.cols] = M.data[i];
    }
    matrix_free(&M);
}


double matrix_det(Matrix M1) // Нахождение определителя матрицы
{
    if(M1.data == NULL) {
        matrix_exception(ERROR, "Матрица не должна быть пустой - Нахождение определителя");
        return NAN;
    }
    if(M1.cols != M1.rows) {
        matrix_exception(ERROR, "Матрица должна быть квадратной - Нахождение определителя");
        return NAN;
    }

    Matrix M2 = matrix_clone(M1);
    
    double result = 1;
    if(M2.rows == M2.cols) {
        for (size_t col_nul = 0; col_nul < M2.cols; col_nul++) {
            for (size_t row = col_nul + 1; row < M2.rows; row++) {
                double proportion = (double)M2.data[row * M2.cols + col_nul] / M2.data[col_nul * M2.cols + col_nul];
                for (size_t col = col_nul; col < M2.cols; col++) {
                    M2.data[row * M2.cols + col] -= (double)M2.data[col_nul * M2.cols + col] * proportion;
                }
            }
            result *= (double)M2.data[col_nul * M2.cols + col_nul];
        }
    }

    matrix_free(&M2);
    return result;
}


Matrix matrix_reverse(Matrix M1) // Нахождение обратной матрицы
{
    if(M1.data == NULL) {
        matrix_exception(ERROR, "Матрица не должна быть пустой - Нахождение обратной матрицы");
        return NULL_MATRIX;
    }
    if(M1.cols != M1.rows) {
        matrix_exception(ERROR, "Матрица должна быть квадратной - Нахождение обратной матрицы");
        return NULL_MATRIX;
    }
    if(matrix_det(M1) == 0) {
        matrix_exception(WARNING, "Определитель матрицы равен 0 - Нахождение обратной матрицы");
        return NULL_MATRIX;
    }

    Matrix result = matrix_create_unit(M1.rows);
    Matrix M2 = matrix_clone(M1);
   
    if(M2.rows == M2.cols) {  
        for (size_t col_nul = 0; col_nul <= M2.cols; col_nul++) {
            for (size_t row = col_nul + 1; row < M2.rows; row++) {
                double proportion = (double)M2.data[row * M2.cols + col_nul] / M2.data[col_nul * M2.cols + col_nul];
                for (size_t col = 0; col < M2.cols; col++) {
                    M2.data[row * M2.cols + col] -= (double)M2.data[col_nul * M2.cols + col] * proportion;
                    result.data[row * M2.cols + col] -= (double) result.data[col_nul * M2.cols + col] * proportion;
                }
            }
        }
        for (int row = 0; row < M2.rows; row++){
            double koef = M2.data[row * M2.cols + row];
            for (int col = 0; col < M2.cols; col++){
                M2.data[row * M2.cols + col] /= koef;
                result.data[row * M2.cols + col] /= koef;
            }
        }
        for (int col_nul = M2.cols - 1; col_nul >= 0; col_nul--) {
            for (int row = col_nul - 1; row >= 0; row--) {
                double proportion = (double)M2.data[row * M2.cols + col_nul] / M2.data[col_nul * M2.cols + col_nul];
                for (int col = M2.cols - 1; col >= 0; col--) {
                    M2.data[row * M2.cols + col] -= (double)M2.data[col_nul * M2.cols + col] * proportion;
                    result.data[row * M2.cols + col] -= (double) result.data[col_nul * M2.cols + col] * proportion;
                }   
            }
        }  
    }  

    matrix_free(&M2);

    return result; 
}


Matrix matrix_div(Matrix M1, Matrix M2) // Деление матриц
{
    if (M2.data == NULL || M1.data == NULL) {
        matrix_exception(ERROR, "Матрицы не должны быть пустыми - Деление матриц");
        return NULL_MATRIX;
    }
    if (M1.cols != M2.cols && M1.rows != M2.rows) {
        matrix_exception(ERROR, "Количество столбцов первой матрицы не равно числу строк второй матрицы - Деление матриц");
        return NULL_MATRIX;
    }

    Matrix result = matrix_init(M1.rows, M1.cols);
    Matrix M3 = matrix_reverse(M2);
    result = matrix_mul(M1, M3);
    matrix_free(&M3);

    return result;
}


int factorial(const unsigned int num) // Нахождение факториала
{
    int fact = 1;
    for (int i = 1; i <= num; i++)
    {
        fact *= i;
    }
    return fact;
}


Matrix matrix_exponent(Matrix M1, int num) // Нахождение експоненты матрицы
{
    if (M1.rows != M1.cols) {
        matrix_exception(WARNING, "Матрица должна быть квадратной - Экспонента матрицы");
        return NULL_MATRIX;
    }

    Matrix E1 = matrix_create_unit(M1.rows);

    if (E1.data == NULL) {
        matrix_exception(ERROR, "Сбой выделения памяти - Экспонента матрицы");
        return NULL_MATRIX;
    }

    for (int cur_num = 1; cur_num < num; cur_num++)
    {
        Matrix temporary = matrix_clone(M1);
        temporary = matrix_pow(M1, cur_num);
        matrix_print(temporary);
        temporary = matrix_mul_num(temporary, (double) 1.0 / factorial(cur_num));
        Matrix E_copy = matrix_clone(E1);
        E1 = matrix_sum(E_copy, temporary);
        matrix_free(&E_copy);
        matrix_free(&temporary);
    }
    
    return E1;
}


int main()  
{
    Matrix A = matrix_init(2, 2);;
    Matrix B = matrix_init(2, 2);;
    Matrix C = matrix_init(3, 3);;
    Matrix D = matrix_init(3, 3);;
    Matrix E = matrix_init(3, 3);;
    Matrix F = matrix_init(3, 3);;
    Matrix G = matrix_init(3, 3);;

    matrix_fill(&A);
    matrix_fill_power(&B, 2);
    //Вывод матриц A и B
    printf("Матрица A:\n");
    matrix_print(A);
    printf("Матрица B:\n");
    matrix_print(B);

    //Умножение матрицы на число
    A = matrix_mul_num(A, 3);
    printf("Умноженная на 3 матрица A:\n");
    matrix_print(A);

    //Деление матрицы на число
    B = matrix_div_num(B, 2);
    printf("Поделённая на 2 матрица B:\n");
    matrix_print(B);

    //Суммирование матриц
    A = matrix_sum(A, B);
    printf("Сложение матрицы A и B:\n");
    matrix_print(A);

    matrix_fill_power(&C, 3);
    matrix_fill_power(&D, 2);

    //Вывод матриц C и D
    printf("Матрица C:\n");
    matrix_print(C);
    printf("Матрица D:\n");
    matrix_print(D);

    //Вычитание матриц
    C = matrix_sub(C, D);
    printf("Вычитание матриц C и D:\n");
    matrix_print(C);


    
    matrix_fill_power(&E, 2);
    matrix_fill_power(&F, 2);
    matrix_fill_power(&G, 2);

    return 0;
}