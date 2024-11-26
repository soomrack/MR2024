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

const Matrix MATRIX_NULL = {0, 0, NULL};



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
    *M = MATRIX_NULL;
}


Matrix matrix_init(const int rows, const int cols) //Инициализация матрицы
{
    Matrix A;

    if (rows == 0 || cols == 0) {
        matrix_exception(INFO, "Матрица без строк и столбцов - Инициализация матрицы");
        return (Matrix) {rows, cols, NULL};
    }
    
    int size = rows * cols;
    if (size / rows  != cols) {
        matrix_exception(ERROR, "OVERFLOW - Переполнение количества элементов - Инициализация матрицы");
        return MATRIX_NULL;
    }

    int size_in_bytes = size * sizeof(double);
    if (size_in_bytes / sizeof(double) != size) {
        matrix_exception(ERROR, "OVERFLOW: Переполнение выделенной памяти - Инициализация матрицы");
        return MATRIX_NULL;
    }

    A.data = (double *)malloc(rows * cols * sizeof(double));

    if (A.data == NULL) {
        matrix_exception(ERROR, "Сбой выделения памяти - Инициализация матрицы");
        return MATRIX_NULL;
    }
    
    A.rows = rows;
    A.cols = cols;
    
    return A;
}


void matrix_create_zero(const Matrix M) //Создание нулевой матрицы
{
    memset(M.data, 0, M.cols * M.rows * sizeof(double));    
}


Matrix matrix_create_unit(unsigned int rows) // Создание единичной матрицы
{
    if (rows == 0) {
        return MATRIX_NULL;
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


void matrix_fill(Matrix *A) //Заполнение матрицы арифмитической последовательностью
{
    for (size_t i = 0; i < A->rows * A->cols; i++) {
        A->data[i] = (i + 1);
    }
}


void matrix_fill_power(Matrix *A, int power) //Заполенение матрицы арифмитической прогрессией power-порядка
{
    for (size_t i = 0; i < A->rows * A->cols; i++) {
        A->data[i] = pow((i + 1), power);
    }
}


Matrix matrix_clone(Matrix A) // Клонирование матрицы
{
    Matrix result = matrix_init(A.rows, A.cols);

    for (size_t i = 0; i < A.rows * A.cols; i++) {
        result.data[i] = A.data[i];
    }

    return result;     
}


Matrix matrix_sum(Matrix A, Matrix B) // Суммирование матриц
{   
    if (A.rows != B.rows && A.cols != B.cols){
        matrix_exception(ERROR, "Разные размеры матриц - Сложение матриц");
        return MATRIX_NULL;
    }

    Matrix result = matrix_init(A.rows, A.cols);
    
    for(size_t i = 0; i < A.rows * A.cols; i++) {
        result.data[i] = A.data[i] + B.data[i];   
    }

    return result;
}


Matrix matrix_sub(Matrix A, Matrix B) // Вычитание матриц
{   
    if (A.rows != B.rows && A.cols != B.cols){
        matrix_exception(ERROR, "Разные размеры матриц - Вычитание матриц");
        return MATRIX_NULL;
    }

    Matrix result = matrix_init(A.rows, A.cols);;

    if (A.rows == B.rows & A.cols == B.cols) {
        for (size_t i = 0; i < A.rows * A.cols; i++) {
            result.data[i] = A.data[i] - B.data[i];   
        }
    }

    return result;
}


Matrix matrix_mul_num(Matrix A, double num) // Умножение матрицы на число
{
    Matrix result;

    for (size_t i = 0; i < A.rows * A.cols; i++) {
        result.data[i] = A.data[i] * num;
    }

    return result;
}


Matrix matrix_div_num(Matrix A, double num) // Деление матрицы на число
{
    Matrix result;

    for (size_t i = 0; i < A.rows * A.cols; i++) {
        result.data[i] = A.data[i] / num;
    }

    return result;
}


Matrix matrix_mul(Matrix A, Matrix B) // Умножение матрицы
{
    if (A.cols != B.rows) {
        matrix_exception(ERROR, "Количество столбцов первой матрицы не равно числу строк второй матрицы - Умножение матриц");
        return MATRIX_NULL;
    }
    
    Matrix result;

    if (A.cols == B.rows) {
        result = matrix_init(A.rows, B.cols);
        for (size_t row = 0; row < A.rows; row++) {
            for (size_t col = 0; col < B.cols; col++) {
                result.data[row * result.cols + col] = 0;
                for (size_t i = 0; i < A.cols; i++) {
                    result.data[row * result.cols + col] += (double)A.data[row * A.cols + i] * B.data[i * B.cols + col];
                }
            }
        }
    }

    return result;
}


Matrix matrix_pow(Matrix A, unsigned int power) // Возведение матрицы в степень
{
    
    if (A.cols != A.rows && power != 1) {
        matrix_exception(ERROR, "Матрица должна быть квадратной - Возведение в степень");
        return MATRIX_NULL;
    }

    if (power == 0) {
        return matrix_create_unit(A.rows);
    }

    Matrix result = matrix_clone(A);

    for (size_t i = 1; i < power; i++) {
            result = matrix_mul(result, A);
    }

    return result;
}


void matrix_print(Matrix A) // Вывод матриц в консоль
{   
    for (size_t i = 0; i < A.cols * A.rows; i++) {
        if ((i) % A.cols == 0) {
            printf("|");
        }
        printf("%.2f|", A.data[i]);
        if ((i) % A.cols == A.cols - 1) {
            printf("\n");
        }
    }
    printf("\n");
}


void matrix_transp(Matrix *A) // Транспонирование матрицы
{
    Matrix B = matrix_clone(*A);
    *A = matrix_init(A->cols, A->rows);
    
    for (int i = 0; i < B.cols * B.rows; i++) {
        A->data[B.rows * (i % B.cols) + i / B.cols] = B.data[i];
    }
    matrix_free(&B);
}


double matrix_det(Matrix A) // Нахождение определителя матрицы
{
    if(A.data == NULL) {
        matrix_exception(ERROR, "Матрица не должна быть пустой - Нахождение определителя");
        return NAN;
    }
    if(A.cols != A.rows) {
        matrix_exception(ERROR, "Матрица должна быть квадратной - Нахождение определителя");
        return NAN;
    }

    Matrix B = matrix_clone(A);
    
    double result = 1;
    if(B.rows == B.cols) {
        for (size_t col_nul = 0; col_nul < B.cols; col_nul++) {
            for (size_t row = col_nul + 1; row < B.rows; row++) {
                double proportion = (double)B.data[row * B.cols + col_nul] / B.data[col_nul * B.cols + col_nul];
                for (size_t col = col_nul; col < B.cols; col++) {
                    B.data[row * B.cols + col] -= (double)B.data[col_nul * B.cols + col] * proportion;
                }
            }
            result *= (double)B.data[col_nul * B.cols + col_nul];
        }
    }

    matrix_free(&B);
    return result;
}


Matrix matrix_reverse(Matrix A) // Нахождение обратной матрицы
{
    if(A.data == NULL) {
        matrix_exception(ERROR, "Матрица не должна быть пустой - Нахождение обратной матрицы");
        return MATRIX_NULL;
    }
    if(A.cols != A.rows) {
        matrix_exception(ERROR, "Матрица должна быть квадратной - Нахождение обратной матрицы");
        return MATRIX_NULL;
    }
    if(matrix_det(A) == 0) {
        matrix_exception(WARNING, "Определитель матрицы равен 0 - Нахождение обратной матрицы");
        return MATRIX_NULL;
    }

    Matrix result = matrix_create_unit(A.rows);
    Matrix B = matrix_clone(A);
   
    if(B.rows == B.cols) {  
        for (size_t col_nul = 0; col_nul <= B.cols; col_nul++) {
            for (size_t row = col_nul + 1; row < B.rows; row++) {
                double proportion = (double)B.data[row * B.cols + col_nul] / B.data[col_nul * B.cols + col_nul];
                for (size_t col = 0; col < B.cols; col++) {
                    B.data[row * B.cols + col] -= (double)B.data[col_nul * B.cols + col] * proportion;
                    result.data[row * B.cols + col] -= (double) result.data[col_nul * B.cols + col] * proportion;
                }
            }
        }
        for (int row = 0; row < B.rows; row++){
            double koef = B.data[row * B.cols + row];
            for (int col = 0; col < B.cols; col++){
                B.data[row * B.cols + col] /= koef;
                result.data[row * B.cols + col] /= koef;
            }
        }
        for (int col_nul = B.cols - 1; col_nul >= 0; col_nul--) {
            for (int row = col_nul - 1; row >= 0; row--) {
                double proportion = (double)B.data[row * B.cols + col_nul] / B.data[col_nul * B.cols + col_nul];
                for (int col = B.cols - 1; col >= 0; col--) {
                    B.data[row * B.cols + col] -= (double)B.data[col_nul * B.cols + col] * proportion;
                    result.data[row * B.cols + col] -= (double) result.data[col_nul * B.cols + col] * proportion;
                }   
            }
        }  
    }  

    matrix_free(&B);

    return result; 
}


Matrix matrix_div(Matrix A, Matrix B) // Деление матриц
{
    if (B.data == NULL || A.data == NULL) {
        matrix_exception(ERROR, "Матрицы не должны быть пустыми - Деление матриц");
        return MATRIX_NULL;
    }
    if (A.cols != B.cols && A.rows != B.rows) {
        matrix_exception(ERROR, "Количество столбцов первой матрицы не равно числу строк второй матрицы - Деление матриц");
        return MATRIX_NULL;
    }

    Matrix result;
    Matrix D = matrix_reverse(B);
    result = matrix_mul(A, D);
    matrix_free(&D);

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


Matrix matrix_exponent(Matrix A, int num) // Нахождение експоненты матрицы
{
    if (A.rows != A.cols) {
        matrix_exception(WARNING, "Матрица должна быть квадратной - Экспонента матрицы");
        return MATRIX_NULL;
    }

    Matrix E = matrix_create_unit(A.rows);

    if (E.data == NULL) {
        matrix_exception(ERROR, "Сбой выделения памяти - Экспонента матрицы");
        return MATRIX_NULL;
    }

    for (int cur_num = 1; cur_num < num; cur_num++)
    {
        Matrix temporary = matrix_clone(A);
        temporary = matrix_pow(A, cur_num);
        matrix_print(temporary);
        temporary = matrix_mul_num(temporary, (double) 1.0 / factorial(cur_num));
        Matrix E_copy = matrix_clone(E);
        E = matrix_sum(E_copy, temporary);
        matrix_free(&E_copy);
        matrix_free(&temporary);
    }
    
    return E;
}


int main()  
{
    Matrix A;
    Matrix B;
    Matrix C;
    Matrix D;

    A = matrix_init(3, 3);
    matrix_fill_power(&A, 2);
    B = matrix_init(3, 3);
    matrix_fill_power(&B, 2);
    printf("");
    matrix_print(A);
    matrix_print(B);
    A = matrix_sum(A, B);
    matrix_print(A);

    C = matrix_init(3, 3);
    matrix_fill(&C);
    D = matrix_init(3, 3);
    matrix_fill(&D);
    C = matrix_exponent(C, 3);
    matrix_print(C);

    matrix_print(D);

    return 0;
}