#include <stdio.h>
#include <stdlib.h>

typedef struct {
    size_t rows;
    size_t cols;
    double* data;
} Matrix;

Matrix Zero = { 0, 0, NULL };

// Функция для выделения памяти под матрицу
void matrix_allocate(Matrix* matrix)
{
    matrix->data = (double*)malloc(matrix->rows * matrix->cols * sizeof(double));
    if (matrix->data == NULL) {
        fprintf(stderr, "Ошибка: Не удалось выделить память для матрицы.\n");
        exit(EXIT_FAILURE);
    }
}

void memory_free(Matrix* matrix)
{
    free(matrix->data);
    matrix->data = NULL;
    matrix->rows = 0;
    matrix->cols = 0;
}

// Функция для передачи массива в матрицу
void matrix_trans_array(Matrix* matrix, double array_name[])
{
    matrix_allocate(matrix);
    for (size_t i = 0; i < matrix->rows * matrix->cols; i++) {
        matrix->data[i] = array_name[i];
    }
}

void matrix_print(Matrix* matrix)
{
    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = 0; j < matrix->cols; j++) {
            printf("%lf ", matrix->data[i * matrix->cols + j]);
        }
        printf("\n");
    }
}

Matrix matrix_add(Matrix A, Matrix B)
{
    if (A.rows != B.rows || A.cols != B.cols) {
        fprintf(stderr, "Ошибка: размеры матриц должны совпадать для сложения.\n");
        exit(EXIT_FAILURE);
    }

    Matrix result = { A.rows, A.cols, NULL };
    matrix_allocate(&result);
    for (size_t i = 0; i < A.rows * A.cols; i++) {
        result.data[i] = A.data[i] + B.data[i];
    }
    return result;
}

Matrix matrix_subtract(Matrix A, Matrix B)
{
    if (A.rows != B.rows || A.cols != B.cols) {
        fprintf(stderr, "Ошибка: размеры матриц должны совпадать для вычитания.\n");
        exit(EXIT_FAILURE);
    }

    Matrix result = { A.rows, A.cols, NULL };
    matrix_allocate(&result);
    for (size_t i = 0; i < A.rows * A.cols; i++) {
        result.data[i] = A.data[i] - B.data[i];
    }
    return result;
}

Matrix matrix_multiply(Matrix A, Matrix B)
{
    if (A.cols != B.rows) {
        fprintf(stderr, "Ошибка: количество столбцов первой матрицы должно совпадать с количеством строк второй матрицы.\n");
        exit(EXIT_FAILURE);
    }

    Matrix result = { A.rows, B.cols, NULL };
    matrix_allocate(&result);
    for (size_t i = 0; i < A.rows; i++) {
        for (size_t j = 0; j < B.cols; j++) {
            result.data[i * B.cols + j] = 0;
            for (size_t k = 0; k < A.cols; k++) {
                result.data[i * B.cols + j] += A.data[i * A.cols + k] * B.data[k * B.cols + j];
            }
        }
    }
    return result;
}

Matrix matrix_multiply_number(Matrix A, double number)
{
    Matrix result = { A.rows, A.cols, NULL };
    matrix_allocate(&result);
    for (size_t i = 0; i < A.rows * A.cols; i++) {
        result.data[i] = A.data[i] * number;
    }
    return result;
}

Matrix matrix_transpose(Matrix A)
{
    Matrix result = { A.cols, A.rows, NULL };
    matrix_allocate(&result);
    for (size_t i = 0; i < A.rows; i++) {
        for (size_t j = 0; j < A.cols; j++) {
            result.data[j * A.rows + i] = A.data[i * A.cols + j];
        }
    }
    return result;
}

double matrix_determinant(Matrix A)
{
    if (A.rows != A.cols) {
        fprintf(stderr, "Ошибка: определитель может быть вычислен только для квадратных матриц.\n");
        exit(EXIT_FAILURE);
    }
    // Рекурсивное вычисление определителя
    if (A.rows == 1) {
        return A.data[0];
    }
    if (A.rows == 2) {
        return A.data[0] * A.data[3] - A.data[1] * A.data[2];
    }

    double det = 0.0;
    for (size_t i = 0; i < A.rows; i++) {
        Matrix submatrix = { A.rows - 1, A.cols - 1, NULL };
        matrix_allocate(&submatrix);
        for (size_t j = 1; j < A.rows; j++) {
            for (size_t k = 0; k < A.cols; k++) {
                if (k < i) {
                    submatrix.data[(j - 1) * (A.cols - 1) + k] = A.data[j * A.cols + k];
                }
                else if (k > i) {
                    submatrix.data[(j - 1) * (A.cols - 1) + (k - 1)] = A.data[j * A.cols + k];
                }
            }
        }
        det += (i % 2 == 0 ? 1 : -1) * A.data[i] * matrix_determinant(submatrix);
        memory_free(&submatrix);
    }
    return det;
}

// Функция для вычисления обратной матрицы
Matrix matrix_inverse(Matrix A)
{
    double det = matrix_determinant(A);
    if (det == 0) {
        fprintf(stderr, "Ошибка: матрица не имеет обратной.\n");
        exit(EXIT_FAILURE);
    }

    Matrix result = { A.rows, A.cols, NULL };
    matrix_allocate(&result);
    // Создание матрицы кофакторов
    for (size_t i = 0; i < A.rows; i++) {
        for (size_t j = 0; j < A.cols; j++) {
            Matrix submatrix = { A.rows - 1, A.cols - 1, NULL };
            matrix_allocate(&submatrix);
            for (size_t sub_i = 0; sub_i < A.rows; sub_i++) {
                for (size_t sub_j = 0; sub_j < A.cols; sub_j++) {
                    if (sub_i != i && sub_j != j) {
                        submatrix.data[(sub_i < i ? sub_i : sub_i - 1) * (A.cols - 1) + (sub_j < j ? sub_j : sub_j - 1)] = A.data[sub_i * A.cols + sub_j];
                    }
                }
            }
            result.data[j * A.cols + i] = ((i + j) % 2 ? -1 : 1) * matrix_determinant(submatrix) / det;
            memory_free(&submatrix);
        }
    }
    return result;
}

// Функция для возведения матрицы в степень
Matrix matrix_power(Matrix A, long long int n)
{
    if (A.rows != A.cols) {
        fprintf(stderr, "Ошибка: возведение в степень возможно только для квадратных матриц.\n");
        exit(EXIT_FAILURE);
    }
    Matrix result = { A.rows, A.cols, NULL };
    matrix_allocate(&result);

    // Инициализация результатирующей матрицы как единичной
    for (size_t i = 0; i < A.rows; i++) {
        for (size_t j = 0; j < A.cols; j++) {
            result.data[i * A.cols + j] = (i == j) ? 1.0 : 0.0;
        }
    }

    while (n > 0) {
        if (n % 2 == 1) {
            Matrix temp = matrix_multiply(result, A);
            memory_free(&result);
            result = temp;
        }
        A = matrix_multiply(A, A);
        n /= 2;
    }

    return result;
}
/*
// Функция для вычисления матричной экспоненты
Matrix matrix_exponent(Matrix A, long long int n)
{
    Matrix result = { A.rows, A.cols, NULL };
    matrix_allocate(&result);

    // Инициализация результатирующей матрицы как единичной
    for (size_t i = 0; i < A.rows; i++) {
        for (size_t j = 0; j < A.cols; j++) {
            result.data[i * A.cols + j] = (i == j) ? 1.0 : 0.0;
        }
    }

    Matrix term = { A.rows, A.cols, NULL };
    matrix_allocate(&term);
    // Инициализация первого члена ряда (A^0)
    for (size_t i = 0; i < A.rows; i++) {
        for (size_t j = 0; j < A.cols; j++) {
            term.data[i * A.cols + j] = (i == j) ? 1.0 : 0.0;
        }
    }

    for (long long int k = 1; k <= n; k++) {
        // Умножаем текущий член на A (A^k)
        Matrix next_term = matrix_multiply(term, A);
        // Добавляем текущий член к результату
        for (size_t i = 0; i < result.rows * result.cols; i++) {
            result.data[i] += next_term.data[i] / k;  // делим на k для ряда Тейлора
        }
        memory_free(&term);
        term = next_term;
    }

    memory_free(&term);
    return result;
}
*/

// Функция для выполнения вычислений
void make_calculations()
{
    double array1[] = { 1, 2, 3, 4 };
    Matrix A = { 2, 2, NULL };
    matrix_trans_array(&A, array1);

    double array2[] = { 5, 6, 7, 8 };
    Matrix B = { 2, 2, NULL };
    matrix_trans_array(&B, array2);

    printf("Matrix A:\n");
    matrix_print(&A);
    printf("Matrix B:\n");
    matrix_print(&B);

    Matrix C = matrix_add(A, B);
    printf("A + B:\n");
    matrix_print(&C);
    memory_free(&C);

    Matrix D = matrix_subtract(A, B);
    printf("A - B:\n");
    matrix_print(&D);
    memory_free(&D);

    Matrix E = matrix_multiply(A, B);
    printf("A * B:\n");
    matrix_print(&E);
    memory_free(&E);

    Matrix E1 = matrix_multiply_number(A, 5);
    printf("A * 5:\n");
    matrix_print(&E1);
    memory_free(&E1);

    Matrix F = matrix_transpose(A);
    printf("Transpose of A:\n");
    matrix_print(&F);
    memory_free(&F);

    double detA = matrix_determinant(A);
    printf("Determinant of A: %lf\n", detA);

    Matrix inverseA = matrix_inverse(A);
    printf("Inverse of A:\n");
    matrix_print(&inverseA);
    memory_free(&inverseA);

    Matrix powerA = matrix_power(A, 3);
    printf("A to the power of 3:\n");
    matrix_print(&powerA);
    memory_free(&powerA);

    /*
    Matrix expA = matrix_exponent(A, 10);
    printf("Matrix Exponential of A:\n");
    matrix_print(&expA);
    memory_free(&expA);
    */

    memory_free(&A);
    memory_free(&B);
}

int main() {
    make_calculations();
    return 0;
}