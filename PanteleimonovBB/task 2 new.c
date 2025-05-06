#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <math.h>


struct Matrix {
    size_t rows;
    size_t cols;
    double* data;
};

typedef struct Matrix Matrix;


const Matrix MATRIX_NULL = {0, 0, NULL};


enum Matrix_message_level {ERROR, WARNING, INFO, DEBUG};


void print_message(const enum Matrix_message_level level, char *msg)
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
        
    if(level == DEBUG) {
        printf("DEBUG: %s", msg);
    }
}


Matrix matrix_memory_alloc(const size_t rows, const size_t cols)
{
    Matrix M;

    if (rows == 0 || cols == 0) {
        print_message(WARNING, "В матрице 0 строк или 0 столбцов\n");
        return (Matrix) {rows, cols, NULL};
    }

    size_t size = rows * cols;

    if (size / rows != cols) {
        print_message(ERROR, "Переполнение количества элементов\n");
        return MATRIX_NULL;
    }

       size_t size_bytes = size * sizeof(double);

    if (size_bytes / sizeof(double) != size) {
        print_message(ERROR, "Переполнение выделенной памяти\n");
        return MATRIX_NULL;
    }

       M.data = malloc(rows * cols * sizeof(double));

    if (M.data == NULL) {
        print_message(ERROR, "Сбой выделенния памяти\n");
        return MATRIX_NULL;
    }

    M.rows = rows; 
    M.cols = cols; 

    return M;
}


void matrix_memory_free(struct Matrix* M)
{
    if (M == NULL) {
        print_message(ERROR, "Обращение к недопутимой области памяти\n");
        return;
    }
    
    free(M->data);

    *M = MATRIX_NULL;
}

void matrix_copy(const Matrix B, const Matrix A)
{  
    if ((A.cols != B.cols) || (A.rows != B.rows )) {
        print_message(ERROR, "Выделенная память не одинакова\n");
        return;
    }

    if (B.data == NULL) {
        print_message(ERROR, "Обращение к недопутимой области памяти\n");
        return;
    }
   
    memcpy(B.data, A.data, A.cols * A.rows * sizeof(double));    
}

void matrix_print(const Matrix M)
{
    for (size_t row = 0; row < M.rows; row++) {
        for (size_t col = 0; col < M.cols; col++) {
            printf("%.2f ", M.data[row * M.cols + col]);
        }
        printf("\n");
    }
}


Matrix matrix_enter()
{
    size_t rows, cols;

    printf("Введите количество строк: ");
    scanf("%zu", &rows);

    printf("Введите количество столбцов: ");
    scanf("%zu", &cols);

    Matrix C = matrix_memory_alloc(rows, cols);

    printf("Введите элементы матрицы, разделённые пробелами и переходами на новую строку:");
    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++) {
            scanf("%lf", &C.data[row * C.cols + col]);
        }
    }
    matrix_print(C);
    return C;       
}


// создание нулевой матрицы
Matrix matrix_zero(const size_t rows, const size_t cols) {
    if (rows == 0 || cols == 0) {
        print_message(ERROR, "Некорректные размеры матрицы.\n");
        return MATRIX_NULL;
    }

    Matrix C = matrix_memory_alloc(rows, cols);
    if (C.data == NULL) {
        print_message(ERROR, "Ошибка выделения памяти для матрицы.\n");
        return MATRIX_NULL;
    }

    memset(C.data, 0, rows * cols * sizeof(double));

    return C;
}


// создание единичной матрицы
Matrix matrix_unit(const size_t rows, const size_t cols) {
    if (rows == 0 || cols == 0) {
        print_message(ERROR, "Некорректные размеры матрицы.");
        return MATRIX_NULL; 
    }

        Matrix C = matrix_memory_alloc(rows, cols);
    if (C.data == NULL) {
        print_message(ERROR, "Ошибка выделения памяти при создании матрицы.");
        return MATRIX_NULL;
    }
    for ( size_t i = 0; i < rows && i < cols; i++) {
        C.data[i * cols + i] = 1.0;
    }

    return C;
}

// умножение матриц A * B
Matrix matrix_multiplication(const Matrix A, const Matrix B)
{
    if (A.cols != B.rows) {
        print_message(WARNING, "Умножение невозможно, так как количество столбцов матрицы A не равно количеству строк матрицы B\n");
        return MATRIX_NULL;
    }

    Matrix C = matrix_memory_alloc(A.rows, B.cols);
       
    for(size_t row = 0; row < C.rows; row++) {
        for(size_t col = 0; col < C.cols; col++) {
            C.data[row * C.cols + col] = 0;
            for (size_t index = 0; index < A.cols; index++) {
                C.data[row * C.cols + col] += A.data[row * A.cols + index] * B.data[index * B.cols + col];
            }
        }
    }

    return C;
}


// возведение в степень A^p
Matrix matrix_power(const Matrix A, size_t power)
{
    if (A.cols != A.rows) {
        print_message(WARNING, "Возведение невозможно, так как матрица не квадратная\n");
        return MATRIX_NULL;
    }
    Matrix tmp;

    if (power == 0) {
        return matrix_unit(A.rows, A.cols);
    }

    if (power == 1) {
        Matrix C = matrix_memory_alloc(A.rows, A.cols);
        if (C.data == NULL) {
            print_message(ERROR, "Ошибка выделения памяти для копии матрицы\n");
            return MATRIX_NULL;
        }
        memcpy(C.data, A.data, A.rows * A.cols * sizeof(double));
        return C;
    }

    Matrix C = matrix_memory_alloc(A.rows, A.cols);
    if (C.data == NULL) {
        print_message(ERROR, "Ошибка выделения памяти\n");
        return MATRIX_NULL;
    }
    memcpy(C.data, A.data, A.rows * A.cols * sizeof(double));

    for (size_t index = 1; index < power; index++) {
        Matrix tmp = matrix_multiplication(C, A);
        if (tmp.data == NULL) {
            print_message(ERROR, "Ошибка при умножении матриц\n");
            matrix_memory_free(&C);
            return MATRIX_NULL;
        }
        matrix_memory_free(&C);
        C = tmp;
    }
    matrix_memory_free(&tmp);
    return C;
}


// умножение матрицы на число A * r
Matrix matrix_multiplication_ratio(const Matrix A, double ratio)
{
    Matrix C = matrix_memory_alloc(A.rows, A.cols);
       
    for(size_t index = 0; index < C.rows * C.cols; index++) {
        C.data[index] = A.data[index] * ratio;
    }

    return C;
}


// сложение матриц A + B
Matrix matrix_sum(const Matrix A, const Matrix B)
{
    if (A.rows != B.rows || A.cols != B.cols) {
        print_message(WARNING, "Сложение невозможно, так как у матриц разная размерность\n");
        return MATRIX_NULL;
    }

    Matrix C = matrix_memory_alloc(A.rows, A.cols);
       
    for(size_t index = 0; index < C.rows * C.cols; index++) {
        C.data[index] = A.data[index] + B.data[index];
    }

    return C;
}


// вычитание матриц A - B
Matrix matrix_difference(const Matrix A, const Matrix B)
{
    if ((A.rows != B.rows) || (A.cols != B.cols)) {
        print_message(WARNING, "Разность невозможна, так как у матриц разная размерность\n");
        return MATRIX_NULL;
    }

    Matrix C = matrix_memory_alloc(A.rows, A.cols);
       
    for(size_t index = 0; index < C.rows * C.cols; index++) {
        C.data[index] = A.data[index] - B.data[index];
    }

    return C;
}


// транспонирование матрицы A
Matrix matrix_transp(const Matrix A)
{
    Matrix C = matrix_memory_alloc(A.cols, A.rows);

    for (size_t row = 0; row < C.rows; row++) {
        for (size_t col = 0; col < A.cols; col++) {
            C.data[row * C.cols + col] = A.data[col * A.cols + row];
        }
    }

    return C;
}


double factorial (const unsigned int f) 
{
    unsigned long long int res = 1;
    for (unsigned int idx = 1; idx <= f; idx++) {
        res *= idx;
    }

    return res;
}


// экспонента матрицы A                                                                                                             
Matrix matrix_exponent(const Matrix A, const size_t order)
{
    if (A.rows != A.cols) {
        print_message(WARNING, "Матрица должна быть квадратной для вычисления экспоненты");
        return MATRIX_NULL;
    }
    
    Matrix C = matrix_unit(A.rows, A.cols); 
    Matrix tmp = MATRIX_NULL; 
    Matrix tmp_factorial = MATRIX_NULL;

    for (size_t cur_num = 1; cur_num < order; ++cur_num) {
        matrix_memory_free(&tmp); 
        tmp = matrix_power(A, cur_num);
        if (tmp.data == NULL) {
            matrix_memory_free(&C);
            return MATRIX_NULL;
        }

        matrix_memory_free(&tmp_factorial); 
        tmp_factorial = matrix_multiplication_ratio(tmp, 1.0 / factorial(cur_num));
        if (tmp_factorial.data == NULL) {
            matrix_memory_free(&C);
            matrix_memory_free(&tmp);
            return MATRIX_NULL;
        }

        Matrix new_C = matrix_sum(C, tmp_factorial); 
        if (new_C.data == NULL) {
            matrix_memory_free(&C);
            matrix_memory_free(&tmp);
            matrix_memory_free(&tmp_factorial);
            return MATRIX_NULL;
        }

        matrix_memory_free(&C); 
        C = new_C;
    }
    
    matrix_memory_free(&tmp);
    matrix_memory_free(&tmp_factorial);
    return C;
}



// определитель матрицы A
float matrix_determinant(const Matrix A)
{
    double det;
    if (A.rows != A.cols) {
        print_message(WARNING, "Поиск определителя невозможен, так как матрица не квадратная\n");
        return NAN;
    }

    if (A.rows == 0) {
        print_message(WARNING, "Матрица нулевая\n");
        return NAN;
    }

    if (A.rows == 1) {
        det = A.data[0];
    }

    if (A.rows == 2) {
        det = A.data[0] * A.data[3] - A.data[1] * A.data[2];
    }

    if (A.rows == 3) {
        det = A.data[0] * A.data[4] * A.data[8] 
        + A.data[1] * A.data[5] * A.data[6] 
        + A.data[3] * A.data[7] * A.data[2] 
        - A.data[2] * A.data[4] * A.data[6] 
        - A.data[3] * A.data[1] * A.data[8] 
        - A.data[0] * A.data[5] * A.data[7];
    }

    return det;
}


void matrix_operation(size_t number, const Matrix A, const Matrix B)
{

    if (number == 2 || number == 1) {
        Matrix C = matrix_sum(A, B);
        printf("Результат сложения матриц A и B :\n");

        matrix_print(C);

        matrix_memory_free(&C);
    }

    if (number == 3 || number == 1) {
        Matrix D = matrix_difference(A, B);
        printf("Результат разности матрицы A и B:\n");

        matrix_print(D);

        matrix_memory_free(&D);
    }
       
    if (number == 4 || number == 1) {
        Matrix E = matrix_multiplication(A, B);
        printf("Результат умножения матрицы A на B:\n");

        matrix_print(E);

        matrix_memory_free(&E);
    }
       
       
    if (number == 5 || number == 1) {
        Matrix G = matrix_transp(A);
        printf("Транспонированная матрица A:\n");

        matrix_print(G);

        matrix_memory_free(&G);
    }
       
    if (number == 6 || number == 1) {
        size_t power;
        printf("Введите целое неотрицательное число p"); 
        scanf("%zu", &power);
        Matrix H = matrix_power(A, power);
        printf("Результат возведения матрицы A в степень %zu:\n", power);

        matrix_print(H);

        matrix_memory_free(&H);
    }
       
    if (number == 7 || number == 1) {
        double ratio;
        printf("Введите число r"); 
        scanf("%lf", &ratio);
        Matrix I = matrix_multiplication_ratio(A, ratio);
        printf("Результат умножения матрицы A на число %.2f:\n", ratio);

        matrix_print(I);

        matrix_memory_free(&I);
    }

    if (number == 8 || number == 1) {
        size_t order;
        printf("Введите целое неотрицательное число o"); 
        scanf("%zu", &order);
        Matrix J = matrix_exponent(A, order);
        printf("Экспонента матрицы A порядка %zu:\n", order);

        matrix_print(J);

        matrix_memory_free(&J);
    }

    if (number == 9 || number == 1) {
        printf("Определитель матрицы A: %2.f\n", matrix_determinant(A));
    }
}       

int main() 
{
    printf("Укажите номер операции, которую вы хотите выполнить:\n");
    printf("1. Выполнить все доступные операции\n");
    printf("2. Сложение матриц A и B\n");
    printf("3. Разность матриц A и B\n");
    printf("4. Умножение матриц A и B\n");
    printf("5. Транспонированная матрицы A\n");
    printf("6. Возведения матрицы A в степень p\n");
    printf("7. Умножения матрицы A на число r\n");
    printf("8. Экспонента матрицы A порядка o\n");
    printf("9. Вычисление определителя матрицы A\n");

    size_t number;
    scanf("%zu", &number);

    if (number < 1 || number > 9) {
        printf("Такой операции нет\n");
        return EXIT_FAILURE;
    }

    printf("Ввод матрицы A\n");
    Matrix A = matrix_enter();
    Matrix B;
    if (number < 6) {
        printf("Ввод матрицы B\n");
        Matrix B = matrix_enter();
    }
    else {
        Matrix B = MATRIX_NULL;
    }

    matrix_operation(number, A, B);

    matrix_memory_free(&A);
    matrix_memory_free(&B);

    return 0;
}
