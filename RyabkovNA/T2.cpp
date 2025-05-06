#include <stdio.h>
#include <stdlib.h>
#include <math.h>


struct Matrix {
    size_t rows;
    size_t cols;
    double* data;
};

typedef struct Matrix Matrix;

Matrix MATRIX_ZERO = { 0, 0, NULL };


static void matrix_error(const char* message)
{
    printf("Ошибка: %s\n", message);
}


static int matrix_allocate(Matrix* matrix)
{
    matrix->data = (double*)malloc(matrix->rows * matrix->cols * sizeof(double));
    if (matrix->data == NULL) {
        matrix_error("Не удалось выделить память для матрицы.");
        return 0;
    }
    return 1;
}


static void memory_free(Matrix* matrix)
{
    if (matrix->data != NULL) {
        free(matrix->data);
        matrix->data = NULL;
    }
    matrix->rows = 0;
    matrix->cols = 0;
}


static void matrix_trans_array(Matrix* matrix, double array[])
{
    if (!matrix_allocate(matrix)) {
        return;
    }
    for (size_t index = 0; index < matrix->rows * matrix->cols; index++) {
        matrix->data[index] = array[index];
    }
}


static void matrix_print(const Matrix* matrix)
{
    for (size_t row = 0; row < matrix->rows; row++) {
        for (size_t col = 0; col < matrix->cols; col++) {
            printf("%lf ", matrix->data[row * matrix->cols + col]);
        }
        printf("\n");
    }
}


static Matrix matrix_single(size_t size)
{
    Matrix single = { size, size, NULL };
    if (!matrix_allocate(&single)) {
        return MATRIX_ZERO;
    }
    for (size_t row = 0; row < size; row++) {
        for (size_t col = 0; col < size; col++) {
            single.data[row * size + col] = (row == col) ? 1.0 : 0.0;
        }
    }
    return single;
}


static Matrix matrix_add(Matrix A, Matrix B)
{
    if (A.rows != B.rows || A.cols != B.cols) {
        matrix_error("Размеры матриц должны совпадать для сложения.");
        return MATRIX_ZERO;
    }

    Matrix result = { A.rows, A.cols, NULL };
    if (!matrix_allocate(&result)) {
        return MATRIX_ZERO;
    }
    for (size_t index = 0; index < A.rows * A.cols; index++) {
        result.data[index] = A.data[index] + B.data[index];
    }
    return result;
}


static Matrix matrix_subtract(Matrix A, Matrix B)
{
    if (A.rows != B.rows || A.cols != B.cols) {
        matrix_error("Размеры матриц должны совпадать для вычитания.");
        return MATRIX_ZERO;
    }

    Matrix result = { A.rows, A.cols, NULL };
    if (!matrix_allocate(&result)) {
        return MATRIX_ZERO;
    }
    for (size_t index = 0; index < A.rows * A.cols; index++) {
        result.data[index] = A.data[index] - B.data[index];
    }
    return result;
}


static Matrix matrix_multiply(Matrix A, Matrix B)
{
    if (A.cols != B.rows) {
        matrix_error("Количество столбцов первой матрицы должно совпадать с количеством строк второй матрицы.");
        return MATRIX_ZERO;
    }

    Matrix result = { A.rows, B.cols, NULL };
    if (!matrix_allocate(&result)) {
        return MATRIX_ZERO;
    }
    for (size_t row = 0; row < A.rows; row++) {
        for (size_t col = 0; col < B.cols; col++) {
            result.data[row * B.cols + col] = 0;
            for (size_t inner = 0; inner < A.cols; inner++) {
                result.data[row * B.cols + col] += A.data[row * A.cols + inner] * B.data[inner * B.cols + col];
            }
        }
    }
    return result;
}


static Matrix matrix_multiply_number(Matrix A, double number)
{
    Matrix result = { A.rows, A.cols, NULL };
    if (!matrix_allocate(&result)) {
        return MATRIX_ZERO;
    }
    for (size_t index = 0; index < A.rows * A.cols; index++) {
        result.data[index] = A.data[index] * number;
    }
    return result;
}


static Matrix matrix_transpose(Matrix A)
{
    Matrix result = { A.cols, A.rows, NULL };
    if (!matrix_allocate(&result)) {
        return MATRIX_ZERO;
    }
    for (size_t row = 0; row < A.rows; row++) {
        for (size_t col = 0; col < A.cols; col++) {
            result.data[col * A.rows + row] = A.data[row * A.cols + col];
        }
    }
    return result;
}


static double matrix_determinant(Matrix A)
{
    if (A.rows != A.cols) {
        matrix_error("Определитель может быть вычислен только для квадратных матриц.");
        return NAN;
    }


    if (A.rows == 1) {
        return A.data[0];
    }
    if (A.rows == 2) {
        return A.data[0] * A.data[3] - A.data[1] * A.data[2];
    }


    double det = 0.0;
    for (size_t row = 0; row < A.rows; row++) {
        Matrix submatrix = { A.rows - 1, A.cols - 1, NULL };
        if (!matrix_allocate(&submatrix)) {
            return NAN;
        }


        for (size_t sub_row = 1; sub_row < A.rows; sub_row++) {
            for (size_t col = 0; col < A.cols; col++) {
                if (col < row) {
                    submatrix.data[(sub_row - 1) * (A.cols - 1) + col] = A.data[sub_row * A.cols + col];
                }
                else if (col > row) {
                    submatrix.data[(sub_row - 1) * (A.cols - 1) + (col - 1)] = A.data[sub_row * A.cols + col];
                }
            }
        }

        double sub_det = matrix_determinant(submatrix);
        memory_free(&submatrix);

        if (isnan(sub_det)) {
            return NAN;
        }

        det += (row % 2 == 0 ? 1 : -1) * A.data[row] * sub_det;
    }
    return det;
}


static Matrix matrix_inverse(Matrix A)
{
    double det = matrix_determinant(A);
    if (det == 0) {
        matrix_error("Матрица не имеет обратной.");
        return MATRIX_ZERO;
    }

    Matrix result = { A.rows, A.cols, NULL };
    if (!matrix_allocate(&result)) {
        return MATRIX_ZERO;
    }

    // Создание матрицы кофакторов
    for (size_t row = 0; row < A.rows; row++) {
        for (size_t col = 0; col < A.cols; col++) {
            Matrix submatrix = { A.rows - 1, A.cols - 1, NULL };
            if (!matrix_allocate(&submatrix)) {
                memory_free(&result);
                return MATRIX_ZERO;
            }

            for (size_t sub_row = 0; sub_row < A.rows; sub_row++) {
                for (size_t sub_col = 0; sub_col < A.cols; sub_col++) {
                    if (sub_row != row && sub_col != col) {
                        submatrix.data[(sub_row < row ? sub_row : sub_row - 1) * (A.cols - 1) +
                            (sub_col < col ? sub_col : sub_col - 1)] = A.data[sub_row * A.cols + sub_col];
                    }
                }
            }

            result.data[col * A.cols + row] = ((row + col) % 2 ? -1 : 1) * matrix_determinant(submatrix) / det;
            memory_free(&submatrix);
        }
    }
    return result;
}


static Matrix matrix_power(Matrix A, long long int n)
{
    if (A.rows != A.cols) {
        matrix_error("Возведение в степень возможно только для квадратных матриц.");
        return MATRIX_ZERO;
    }

    Matrix result = matrix_single(A.rows);
    if (result.data == NULL) {
        return MATRIX_ZERO;
    }

    Matrix temp_A = { A.rows, A.cols, NULL };
    if (!matrix_allocate(&temp_A)) {
        memory_free(&result);
        return MATRIX_ZERO;
    }

    for (size_t i = 0; i < A.rows * A.cols; i++) {
        temp_A.data[i] = A.data[i];
    }

    while (n > 0) {
        if (n % 2 == 1) {
            Matrix temp = matrix_multiply(result, temp_A);
            if (temp.data == NULL) {
                memory_free(&result);
                memory_free(&temp_A);
                return MATRIX_ZERO;
            }
            memory_free(&result);
            result = temp;
        }

        Matrix new_A = matrix_multiply(temp_A, temp_A);
        if (new_A.data == NULL) {
            memory_free(&temp_A);
            memory_free(&result);
            return MATRIX_ZERO;
        }
        memory_free(&temp_A);
        temp_A = new_A;
        n /= 2;
    }

    memory_free(&temp_A);
    return result;
}


static Matrix matrix_identity(const size_t rows, const size_t cols)
{
    Matrix identity_matrix = { rows, cols, NULL };
    if (!matrix_allocate(&identity_matrix)) {
        return MATRIX_ZERO;
    }

    for (size_t i = 0; i < identity_matrix.rows * identity_matrix.cols; ++i) {
        identity_matrix.data[i] = 0.0;
    }

    for (size_t i = 0; i < identity_matrix.rows; ++i) {
        identity_matrix.data[i * identity_matrix.cols + i] = 1.0;
    }

    return identity_matrix;
}


static Matrix matrix_exponent(Matrix A, unsigned long long int terms)
{
    if (A.rows != A.cols) {
        matrix_error("Матрица должна быть квадратной для вычисления экспоненты.");
        return MATRIX_ZERO;
    }

    Matrix matrix_exponent_term = matrix_identity(A.rows, A.cols);
    Matrix matrix_exponent_result = matrix_identity(A.rows, A.cols);

    if (terms == 0) {
        return matrix_exponent_result;
    }

    double factorial = 1.0;

    for (long long int idx = 1; idx <= terms; idx++) {
        Matrix temp = matrix_multiply(matrix_exponent_term, A);
        memory_free(&matrix_exponent_term);

        matrix_exponent_term = temp;
        factorial *= idx;

        Matrix added = matrix_multiply_number(matrix_exponent_term, 1.0 / factorial);
        Matrix newResult = matrix_add(matrix_exponent_result, added);

        memory_free(&matrix_exponent_result);
        matrix_exponent_result = newResult;
        memory_free(&added);
    }

    memory_free(&matrix_exponent_term);
    return matrix_exponent_result;
}


void make_calculations() {
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

    Matrix expA = matrix_exponent(A, 20);
    printf("Matrix Exponential of A:\n");
    matrix_print(&expA);
    memory_free(&expA);

    memory_free(&A);
    memory_free(&B);
}

int main() {
    make_calculations();
    return 0;
}