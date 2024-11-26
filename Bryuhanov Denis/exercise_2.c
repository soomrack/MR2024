#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


struct Matrix
{
    size_t rows; 
    size_t cols;
    double *data;
};
typedef struct Matrix Matrix;

enum MatrixExceptionType {ERROR, WARNING, INFO};

const Matrix NULL_MATRIX = {0, 0, NULL};  // Нулевая матрица



void matrix_exception(enum MatrixExceptionType type, char *message)  // Сообщение об ошибке
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


void matrix_free(Matrix* M)  // Функция для освобождения памяти матриц
{  
    if (M == NULL){
        matrix_exception(ERROR, "Обращение к недопутимой области памяти");
        return;
    }
    
    free(M->data);
    *M = NULL_MATRIX;
}


Matrix matrix_init(const size_t rows, const size_t cols)  // Инициализация матрицы
{
    if (rows == 0 || cols == 0) {
        matrix_exception(INFO, "Матрица без строк и столбцов - Инициализация матрицы");
        return (Matrix){rows, cols, NULL};
    }
    
    size_t size = rows * cols;
    if (size / rows  != cols) {
        matrix_exception(ERROR, "OVERFLOW - Переполнение количества элементов - Инициализация матрицы");
        return NULL_MATRIX;
    }

    size_t size_in_bytes = size * sizeof(double);
    if (size_in_bytes / sizeof(double) != size) {
        matrix_exception(ERROR, "OVERFLOW: Переполнение выделенной памяти - Инициализация матрицы");
        return NULL_MATRIX;
    }
    Matrix A;
    A.data = (double *)malloc(rows * cols * sizeof(double));

    if (A.data == NULL) {
        matrix_exception(ERROR, "Сбой выделения памяти - Инициализация матрицы");
        return NULL_MATRIX;
    }
    
    A.rows = rows;
    A.cols = cols;
    
    return A;
}


void matrix_create_zero(const Matrix M)  // Создание нулевой матрицы
{
    memset(M.data, 0, M.cols * M.rows * sizeof(double));    
}


Matrix matrix_create_unit(const size_t rows)  // Создание единичной матрицы
{
    if (rows == 0) {
        return NULL_MATRIX;
    }

    Matrix I = matrix_init(rows, rows);
    if (I.data == NULL) {
        matrix_exception(ERROR, "Ошибка выделения памяти - создания единичной матрицы");
        return NULL_MATRIX;
    }
    matrix_create_zero(I);

    for (size_t i = 0; i < I.cols; i ++) {  // через нулевую матрицу 
        I.data[i * I.cols + i] = 1.0;
    }

    return I;
}

void matrix_fill_serial_ariphmetics(Matrix *A)  // Заполнение матрицы арифмитической последовательностью 
{
    for (size_t i = 0; i < A->rows * A->cols; i++) {
        A->data[i] = (i + 1);
    }
}


void matrix_fill_serial_ariphmetics_power(Matrix *A, int power) //Заполенение матрицы арифмитической прогрессией power-порядка
{
    for (size_t i = 0; i < A->rows * A->cols; i++) {
        A->data[i] = pow((i + 1), power);
    }
}

Matrix matrix_clone(Matrix A) // Клонирование матрицы
{
    Matrix result = matrix_init(A.rows, A.cols);

    double data_A[9] = {3, 1, 7, 0, 5, 7, 2, 5, 8};

    if (result.data == NULL) {
        matrix_exception(ERROR, "Ошибка выделения памяти - Клонирование матриц");
        return NULL_MATRIX;
    }

    memcpy(result.data, A.data, result.cols * result.rows * sizeof(double));

    return result;     
}


Matrix matrix_sum(Matrix A, Matrix B) // Суммирование матриц
{   
    if (A.rows != B.rows && A.cols != B.cols){
        matrix_exception(ERROR, "Разные размеры матриц - Сложение матриц");
        return NULL_MATRIX;
    }

    Matrix result = matrix_init(A.rows, A.cols);

    if (result.data == NULL) {
        matrix_exception(ERROR, "Ошибка выделения памяти - Суммирование матриц");
        return NULL_MATRIX;
    }
    
    // Бежать по колонкам result
    for(size_t i = 0; i < result.rows * result.cols; i++) {
        result.data[i] = A.data[i] + B.data[i];   
    }

    return result;
}


Matrix matrix_sub(Matrix A, Matrix B) // Вычитание матриц
{   
    if (A.rows != B.rows && A.cols != B.cols){
        matrix_exception(ERROR, "Разные размеры матриц - Вычитание матриц");
        return NULL_MATRIX;
    }

    Matrix result = matrix_init(A.rows, A.cols);
    
    if (result.data == NULL) {
        matrix_exception(ERROR, "Ошибка выделения памяти - Вычитание матриц");
        return NULL_MATRIX;
    }

    if (A.rows == B.rows & A.cols == B.cols) {
        for (size_t i = 0; i < result.rows * result.cols; i++) {
            result.data[i] = A.data[i] - B.data[i];   
        }
    }

    return result;
}


Matrix matrix_mul_num(Matrix A, double num) // Умножение матрицы на число
{
    Matrix result = matrix_init(A.rows, A.cols);

    if (result.data == NULL) {
        matrix_exception(ERROR, "Ошибка выделения памяти - Умножение матрицы на число");
        return NULL_MATRIX;
    }

    for (size_t i = 0; i < result.rows * result.cols; i++) {
        result.data[i] = A.data[i] * num;
    }

    return result;
}


Matrix matrix_div_num(Matrix A, double num) // Деление матрицы на число
{
    Matrix result = matrix_init(A.rows, A.cols);

    if (result.data == NULL) {
        matrix_exception(ERROR, "Ошибка выделения памяти - Делениематрицы на число");
        return NULL_MATRIX;
    }

    for (size_t i = 0; i < result.rows * result.cols; i++) {
        result.data[i] = (double)A.data[i] / (double)num;
    }

    return result;
}


Matrix matrix_mul(Matrix A, Matrix B) // Умножение матрицы
{
    if (A.cols != B.rows) {
        matrix_exception(ERROR, "Количество столбцов первой матрицы не равно числу строк второй матрицы - Умножение матриц");
        return NULL_MATRIX;
    }
    
    Matrix result = matrix_init(A.rows, A.cols);;
    if (result.data == NULL) {
        matrix_exception(ERROR, "Ошибка выделения памяти - Умножение матриц");
        return NULL_MATRIX;
    }

    result = matrix_init(A.rows, B.cols);
    for (size_t row = 0; row < result.rows; row++) {
        for (size_t col = 0; col < result.cols; col++) {
            result.data[row * result.cols + col] = 0;
            for (size_t i = 0; i < result.cols; i++) {
                result.data[row * result.cols + col] += (double)A.data[row * A.cols + i] * B.data[i * B.cols + col];
            }
        }
    }
    return result;
}


Matrix matrix_pow(Matrix A, unsigned int power) // Возведение матрицы в степень
{
    if (A.cols != A.rows) {
        matrix_exception(ERROR, "Матрица должна быть квадратной - Возведение в степень");
        return NULL_MATRIX;
    }

    if (power == 0) {
        return matrix_create_unit(A.rows);
    }

    Matrix result = matrix_clone(A);

    if (result.data == NULL) {
        matrix_exception(ERROR, "Ошибка выделения памяти - Возведение матрицы в степень");
        return NULL_MATRIX;
    }

    for (size_t i = 1; i < power; i++) {
        Matrix temporary = matrix_mul(result, A);
        matrix_free(&result);
        result = temporary; 
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
    Matrix M = matrix_clone(*A);
    if (M.data == NULL) {
        matrix_exception(ERROR, "Ошибка выделения памяти - Транспонирование матрицы");
    }
    
    *A = matrix_init(A->cols, A->rows);

    for (size_t i = 0; i < M.cols * M.rows; i++) {
        A->data[M.rows * (i % M.cols) + i / M.cols] = M.data[i];
    }
    matrix_free(&M);
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
    for (size_t col_nul = 0; col_nul < B.cols; col_nul++) {
        for (size_t row = col_nul + 1; row < B.rows; row++) {
            double proportion = (double)B.data[row * B.cols + col_nul] / B.data[col_nul * B.cols + col_nul];
            for (size_t col = col_nul; col < B.cols; col++) {
                B.data[row * B.cols + col] -= (double)B.data[col_nul * B.cols + col] * proportion;
            }
        }
        result *= (double)B.data[col_nul * B.cols + col_nul];
    }

    matrix_free(&B);
    return result;
}


Matrix matrix_reverse(Matrix A) // Нахождение обратной матрицы
{
    
    if(A.data == NULL) {
        matrix_exception(ERROR, "Матрица не должна быть пустой - Нахождение обратной матрицы");
        return NULL_MATRIX;
    }
    if(A.cols != A.rows) {
        matrix_exception(ERROR, "Матрица должна быть квадратной - Нахождение обратной матрицы");
        return NULL_MATRIX;
    }
    if(matrix_det(A) == 0) {
        matrix_exception(WARNING, "Определитель матрицы равен 0 - Нахождение обратной матрицы");
        return NULL_MATRIX;
    }

    Matrix result = matrix_create_unit(A.rows);
    if (result.data == NULL) {
        matrix_exception(ERROR, "Ошибка выделения памяти - Нахождение обратной матрицы");
        return NULL_MATRIX;
    }

    Matrix B = matrix_clone(A);
   
    if(B.rows == B.cols) {  
        for (size_t col_nul = 0; col_nul <= result.cols; col_nul++) {
            for (size_t row = col_nul + 1; row < result.rows; row++) {
                double proportion = (double)result.data[row * result.cols + col_nul] / result.data[col_nul * B.cols + col_nul];
                for (size_t col = 0; col < result.cols; col++) {
                    B.data[row * B.cols + col] -= (double)B.data[col_nul * B.cols + col] * proportion;
                    result.data[row * result.cols + col] -= (double) result.data[col_nul * result.cols + col] * proportion;
                }
            }
        }
        for (size_t row = 0; row < result.rows; row++){
            double koef = B.data[row * B.cols + row];
            for (size_t col = 0; col < result.cols; col++){
                B.data[row * B.cols + col] /= koef;
                result.data[row * result.cols + col] /= koef;
            }
        }
        for (size_t col_nul = result.cols - 1; col_nul >= 0; col_nul--) {
            for (size_t row = col_nul - 1; row >= 0; row--) {
                double proportion = (double)B.data[row * B.cols + col_nul] / B.data[col_nul * B.cols + col_nul];
                for (size_t col = result.cols - 1; col >= 0; col--) {
                    B.data[row * B.cols + col] -= (double)B.data[col_nul * B.cols + col] * proportion;
                    result.data[row * result.cols + col] -= (double) result.data[col_nul * result.cols + col] * proportion;
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
        return NULL_MATRIX;
    }
    if (A.cols != B.cols && A.rows != B.rows) {
        matrix_exception(ERROR, "Количество столбцов первой матрицы не равно числу строк второй матрицы - Деление матриц");
        return NULL_MATRIX;
    }

    Matrix result = matrix_init(A.rows, A.cols);

    if (result.data == NULL) {
        matrix_exception(ERROR, "Ошибка выделения памяти - Деление матриц");
        return NULL_MATRIX;
    }
    
    Matrix M = matrix_reverse(B);
    result = matrix_mul(A, M);
    matrix_free(&M);

    return result;
}

double factorial(const unsigned int num) // Нахождение факториала
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
        return NULL_MATRIX;
    }

    Matrix E = matrix_create_unit(A.rows);

    if (E.data == NULL) {
        matrix_exception(ERROR, "Сбой выделения памяти - Экспонента матрицы");
        return NULL_MATRIX;
    }

    for (size_t cur_num = 1; cur_num < num; cur_num++)
    {
        Matrix temporary = matrix_pow(A, cur_num);
        Matrix temporary2 = matrix_mul_num(temporary, (double) 1.0 / factorial(cur_num));
        matrix_free(&temporary);
        temporary = temporary2;
        Matrix E_copy = matrix_sum(E, temporary);
        matrix_free(&E);
        matrix_free(&temporary);
        E = E_copy;
        matrix_free(&E_copy);
        matrix_free(&temporary2); 
    }
    
    return E;
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

    matrix_fill_serial_ariphmetics(&A);
    matrix_fill_serial_ariphmetics_power(&B, 2);
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

    matrix_fill_serial_ariphmetics_power(&C, 3);
    matrix_fill_serial_ariphmetics_power(&D, 2);

    //Вывод матриц C и D
    printf("Матрица C:\n");
    matrix_print(C);
    printf("Матрица D:\n");
    matrix_print(D);

    //Вычитание матриц
    C = matrix_sub(C, D);
    printf("Вычитание матриц C и D:\n");
    matrix_print(C);

    matrix_fill_serial_ariphmetics_power(&E, 1);
    matrix_fill_serial_ariphmetics_power(&F, 2);
    matrix_fill_serial_ariphmetics_power(&G, 2);

    E = matrix_exponent(E, 3);
    printf("Экспонента матрицы:\n");
    matrix_print(E);

    return 0;
}