
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


typedef struct Matrix {
    size_t cols;
    size_t rows;
    double* data;
}Matrix;


const Matrix MATRIX_NULL = { 0, 0, NULL };


enum MatrixExceptionLevel {ERROR, WARNING, INFO, DEBUG};


void matrix_exception(const enum MatrixExceptionLevel level, char *msg) 
{
    if (level == ERROR) {
        printf("ERROR: %s", msg);
    }

    if (level == WARNING) {
        printf("WARNING: %s", msg);
    }

    if (level == INFO) {
        printf("INFO: %s", msg);
    }
}


Matrix matrix_allocate(const size_t cols, const size_t rows) 
{
    Matrix A = {0, 0, NULL};

    if (rows == 0 || cols == 0) {
        return A = { rows, cols, NULL };
    }

    A.data = (double*)malloc(cols * rows * sizeof(double));
    if (A.data == NULL) {
        matrix_exception(ERROR, (char *)"0 столбцов или строк.");
        return A;
    }

    if (A.data == NULL) {
        matrix_exception(ERROR, (char*)"память не выделена");
    }

    size_t size = rows * cols;
    if (size / rows != cols) {
        matrix_exception(ERROR, (char*)"OVERFLOW: Переполнение элементов.");
        return MATRIX_NULL;
    }

    size_t mem_size = size * sizeof(double);
    if (mem_size / sizeof(double) != size) {
        matrix_exception(ERROR, (char*)"OVERFLOW: Переполнение памяти");
    }

    A.cols = cols;
    A.rows = rows;
    return A;
}


void matrix_free(Matrix* A) 
{
    if (A == NULL)return;

    free(A->data);
    *A = {0,0,NULL};
}


void matrix_random(Matrix* A) 
{
    for (size_t index=0; index < A->cols * A->rows; index++) {
//        A->data[index] = index+1; 
        A->data[index] = (double)rand();
    }
}


void matrix_print(const Matrix A) 
{
    for (size_t row = 0; row < A.rows; ++row) {
        for (size_t col = 0; col < A.cols; ++col) {
            printf("%.2f ", A.data[row * A.cols + col]);
        }
        printf("\n");
    }
    printf("\n");
}


Matrix summ(const Matrix A, const Matrix B) 
{
    if (A.rows!=B.rows || A.cols != B.cols) {
        matrix_exception(ERROR, (char*)"не соответствующие размеры матрицы.");
        return MATRIX_NULL;
    }

    Matrix result_matrix = matrix_allocate(A.cols, A.rows);

    size_t n_data = result_matrix.cols * result_matrix.rows;
    for (size_t index = 0; index < n_data; ++index) {
        result_matrix.data[index] = A.data[index] + B.data[index];
    }
    return result_matrix;
}


Matrix sub(const Matrix A, const Matrix B) 
{
    if (A.rows != B.rows || A.cols != B.cols) {
        matrix_exception(ERROR, (char*)"не соответствующие размеры матрицы.");
        return MATRIX_NULL;
    }

    Matrix result_matrix = matrix_allocate(A.cols, A.rows);

    size_t n_data = result_matrix.cols * result_matrix.rows;
    for (size_t index = 0; index < n_data; ++index) {
        result_matrix.data[index] = A.data[index] - B.data[index];
    }
    return result_matrix;
}


Matrix multiply_by_double(const Matrix A, double number) 
{
    Matrix result_matrix = matrix_allocate(A.cols, A.rows);

    size_t n_data = result_matrix.cols * result_matrix.rows;
    for (size_t index = 0; index < n_data; ++index) {
        result_matrix.data[index] = A.data[index] * number;
    }
    return result_matrix;
}


Matrix multiply(const Matrix A, const Matrix B) 
{
    
    if (A.cols!=B.rows) {
        matrix_exception(ERROR, (char*)"не соответствующие размеры матрицы.");
        return MATRIX_NULL;
    }

    size_t n_cols = B.cols;
    size_t n_rows = A.rows;
    Matrix result_matrix = matrix_allocate(n_cols, n_rows);

    for (size_t row = 0; row < n_rows; ++row) {
        for (size_t col = 0; col < n_cols; ++col) {
            double summa = 0.0;
            for (size_t k = 0; k < A.cols; ++k) {
                summa += 
                    A.data[row * A.cols + k] * B.data[k * B.cols + col];
            }
            result_matrix.data[row * n_cols + col] = summa;
        }
    }
    return result_matrix;
}


double deter(const Matrix A) 
{
    if (A.cols != A.rows) {
        matrix_exception(ERROR, (char*)"не соответствующие размеры матрицы.");
        return 0;
    }

    double result = 0;
    size_t n = A.cols;
    if (n == 1) {
        result = A.data[0];
        return result;
    }

    for (size_t row = 0; row < n; ++row) {

        size_t col = 0;
        Matrix submatrix = matrix_allocate(n - 1, n - 1);
        size_t row_offset = 0;
        size_t col_offset = 0;

        for (size_t sub_row = 0; sub_row < n - 1; ++sub_row) {
            for (size_t sub_col = 0; sub_col < n - 1; ++sub_col) {
                if (sub_row == row) { row_offset = 1; }

                if (sub_col == col) { col_offset = 1; }

                submatrix.data[sub_row * (n - 1) + sub_col] =
                    A.data[(sub_row + row_offset) * n + (sub_col + col_offset)];
            }
        }
        result += pow(-1, row + col) * A.data[row * n + col] * deter(submatrix);
        matrix_free(&submatrix);
    }
    return result;
}


Matrix transpose(const Matrix A) 
{
    Matrix result_matrix = matrix_allocate(A.rows, A.cols);

    for (size_t row = 0; row < result_matrix.rows; ++row) {
        for (size_t col = 0; col < result_matrix.cols; ++col) {
            result_matrix.data[row * result_matrix.cols + col] = 
                A.data[col * result_matrix.rows + row];
        }
    }
    return result_matrix;
}


Matrix matrix_idenity(size_t sizes) 
{
    Matrix result_matrix = matrix_allocate(sizes, sizes);

    for (size_t row = 0; row < result_matrix.rows; ++row) {
        for (size_t col = 0; col < result_matrix.cols; ++col) {
            result_matrix.data[row * result_matrix.cols + col] = 
                (row == col) ? 1. : 0.;
        }
    }
    return result_matrix;
}


Matrix invert(const Matrix A) 
{
    if (A.cols != A.rows) {
        matrix_exception(ERROR, (char*)"не соответствующие размеры матрицы.");
        return MATRIX_NULL;
    }

    Matrix transponent = transpose(A);

    Matrix result = matrix_allocate(A.cols, A.rows);

    size_t n = transponent.rows;
    for (size_t row = 0; row < n; ++row) {
        for (size_t col = 0; col < n; ++col) {

            Matrix submatrix = matrix_allocate(n - 1, n - 1);

            size_t row_offset = 0;
            size_t col_offset = 0;
            for (size_t sub_row = 0; sub_row < n - 1; ++sub_row) {
                if (sub_row == row) { row_offset = 1; }
                for (size_t sub_col = 0; sub_col < n - 1; ++sub_col) {
                    if (sub_col == col) { col_offset = 1; }

                    submatrix.data[sub_row * (n - 1) + sub_col] =
                        transponent.data[(sub_row + row_offset) * n + (sub_col + col_offset)];
                }
            }
            result.data[row * A.cols + col] = pow(-1, row + col) * deter(submatrix);
            matrix_free(&submatrix);
        }
    }
    matrix_free(&transponent);
    Matrix multiplied_result = multiply_by_double(result, 1 / deter(result));

    matrix_free(&result);
    return multiplied_result;

}


Matrix copy(const Matrix A) 
{
    Matrix result = matrix_allocate(A.cols, A.rows);

    memcpy(result.data, A.data, A.cols * A.rows * sizeof(double));
    return result;
}


double diagonal(const Matrix A) 
{
    double summ = A.data[0];
    for (size_t row = 0; row < A.rows; ++row) {
        for (size_t col = 0; col < A.cols; ++col) {
            if (row == col) {
                summ += A.data[row * A.cols + col];
            }
        }
    }
    return summ;
}


Matrix exp(const Matrix A, int accuracy) 
{
    if (A.cols != A.rows || A.cols==0) {
        matrix_exception(ERROR, (char*)"не соответствующие размеры матрицы.");
        return MATRIX_NULL;
    }

    Matrix new_result = { 0, 0, NULL }, new_powered = { 0, 0, NULL }, multiplied, powered = { 0, 0, NULL };

    Matrix result = matrix_idenity(A.rows);

    powered = copy(A);
    int factorial = 1;
    for (int acc = 1; acc <= accuracy; ++acc) {
        factorial *= acc;

        matrix_free(&new_powered);

        matrix_free(&multiplied);
        multiplied = multiply_by_double(powered, 1./factorial);

        matrix_free(&new_result);
        new_result = summ(result, multiplied);

        matrix_free(&result);
        result = copy(new_result);

        new_powered = multiply(powered, A);

        matrix_free(&powered);
        powered = copy(new_powered);

        
        
    }
    matrix_free(&powered);
    return result;
}


int main() 
{
    Matrix m1, m2;

    m1 = matrix_allocate(2, 2);
    matrix_random(&m1);
    matrix_print(m1);

    m2 = matrix_allocate(2, 2);
    matrix_random(&m2);
    matrix_print(m2);

    Matrix addition;
    addition = summ(m1,m2);
    matrix_print(addition);
    matrix_free(&addition);

    Matrix substruction;
    substruction = sub(m1, m2);
    matrix_print(substruction);
    matrix_free(&substruction);

    double determinant;
    determinant = deter(m1);
    printf("%.2f\n\n", determinant);

    Matrix multiplication1;
    multiplication1 = multiply_by_double(m1, 5.);
    matrix_print(multiplication1);
    matrix_free(&multiplication1);

    Matrix multiplication2;
    multiplication2 = multiply(m1, m2);
    matrix_print(multiplication2);
    matrix_free(&multiplication2);

    Matrix trans;
    trans = transpose(m1);
    matrix_print(trans);
    matrix_free(&trans);

    Matrix invertible;
    invertible = invert(m1);
    matrix_print(invertible);
    matrix_free(&invertible);

    Matrix exponenta;
    exponenta = exp(m1, 3);
    matrix_print(exponenta);
    matrix_free(&exponenta);

    matrix_free(&m1);
    matrix_free(&m2);
}

