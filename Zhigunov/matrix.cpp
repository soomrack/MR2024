
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct Matrix {
    unsigned int cols;
    unsigned int rows;
    double* values;
}Matrix;

const Matrix EMPTY = { 0, 0, NULL };

Matrix init_matrix(const unsigned int cols, const unsigned int rows) {
    Matrix matrix;
    matrix.cols = cols;
    matrix.rows = rows;
    unsigned int n_values = matrix.cols * matrix.rows;
    matrix.values = (double*)malloc(n_values * sizeof(double));
    return matrix;
}

void free_matrix(Matrix* matrix) {
    free(matrix->values);
}

void random_matrix(Matrix* matrix) {
    for (unsigned int index=0; index < matrix->cols * matrix->rows; index++) {
//        matrix->values[index] = index+1; 
        matrix->values[index] = (double)rand();
    }
}

int null_or_not(const Matrix matrix) {
    return matrix.cols == 0 && matrix.rows == 0 ? 1 : 0;
}

void print_matrix(const Matrix matrix) {
    if (null_or_not(matrix)) {
        printf("Matrix does not exist");
        return;
    }
    for (unsigned int row = 0; row < matrix.rows; ++row) {
        for (unsigned int col = 0; col < matrix.cols; ++col) {
            printf("%.2f ", matrix.values[row * matrix.cols + col]);
        }
        printf("\n");
    }
    printf("\n");
}

Matrix summ(const Matrix matrix1, const Matrix matrix2) {
    if (matrix1.rows!=matrix2.rows || matrix1.cols != matrix2.cols) {
        printf("Summ is impossible, because of different matrix sizes");
        return EMPTY;
    }
    Matrix result_matrix = init_matrix(matrix1.cols, matrix1.rows);
    unsigned int n_values = result_matrix.cols * result_matrix.rows;
    for (unsigned int index = 0; index < n_values; ++index) {
        result_matrix.values[index] = matrix1.values[index] + matrix2.values[index];
    }
    return result_matrix;
}

Matrix sub(const Matrix matrix1, const Matrix matrix2) {
    if (matrix1.rows != matrix2.rows || matrix1.cols != matrix2.cols) {
        printf("Substraction is impossible, because of different matrix sizes");
        return EMPTY;
    }
    Matrix result_matrix = init_matrix(matrix1.cols, matrix1.rows);
    unsigned int n_values = result_matrix.cols * result_matrix.rows;
    for (unsigned int index = 0; index < n_values; ++index) {
        result_matrix.values[index] = matrix1.values[index] - matrix2.values[index];
    }
    return result_matrix;
}

Matrix multiply_by_double(const Matrix matrix, double number) {
    Matrix result_matrix = init_matrix(matrix.cols, matrix.rows);
    unsigned int n_values = result_matrix.cols * result_matrix.rows;
    for (unsigned int index = 0; index < n_values; ++index) {
        result_matrix.values[index] = matrix.values[index] * number;
    }
    return result_matrix;
}

Matrix multiply(const Matrix matrix1, const Matrix matrix2) {
    
    if (matrix1.cols!=matrix2.rows) {
        printf("Multiply is impissible, because of not suitable sizes");
        return EMPTY;
    }
    unsigned int n_cols = matrix2.cols;
    unsigned int n_rows = matrix1.rows;
    Matrix result_matrix = init_matrix(n_cols, n_rows);
    for (unsigned int row = 0; row < n_rows; ++row) {
        for (unsigned int col = 0; col < n_cols; ++col) {
            double summa = 0.0;
            for (unsigned int k = 0; k < matrix1.cols; ++k) {
                summa += 
                    matrix1.values[row * matrix1.cols + k] * matrix2.values[k * matrix2.cols + col];
            }
            result_matrix.values[row * n_cols + col] = summa;
        }
    }
    return result_matrix;
}

double deter(const Matrix matrix) {
    if (matrix.cols != matrix.rows) {
        printf("Getting determinant is impossible, because of matrix do not square");
        return 0;
    }
    double result = 0;
    unsigned int n = matrix.cols;
    if (n == 1) {
        result = matrix.values[0];
        return result;
    }
    for (unsigned int row = 0; row < n; ++row) {
        unsigned int col = 0;
        Matrix submatrix = init_matrix(n - 1, n - 1);
        unsigned int row_offset = 0;
        unsigned int col_offset = 0;
        for (unsigned int sub_row = 0; sub_row < n - 1; ++sub_row) {
            for (unsigned int sub_col = 0; sub_col < n - 1; ++sub_col) {
                if (sub_row == row) { row_offset = 1; }
                if (sub_col == col) { col_offset = 1; }
                submatrix.values[sub_row * (n - 1) + sub_col] =
                    matrix.values[(sub_row + row_offset) * n + (sub_col + col_offset)];
            }
        }
        result += pow(-1, row + col) * matrix.values[row * n + col] * deter(submatrix);
        free_matrix(&submatrix);
    }
    return result;
}

Matrix transpose(const Matrix matrix) {
    Matrix result_matrix = init_matrix(matrix.rows, matrix.cols);
    for (unsigned int row = 0; row < result_matrix.rows; ++row) {
        for (unsigned int col = 0; col < result_matrix.cols; ++col) {
            result_matrix.values[row * result_matrix.cols + col] = 
                matrix.values[col * result_matrix.rows + row];
        }
    }
    return result_matrix;
}

Matrix unit_matrix(unsigned int sizes) {
    Matrix result_matrix = init_matrix(sizes, sizes);
    for (unsigned int row = 0; row < result_matrix.rows; ++row) {
        for (unsigned int col = 0; col < result_matrix.cols; ++col) {
            result_matrix.values[row * result_matrix.cols + col] = 
                (row == col) ? 1. : 0.;
        }
    }
    return result_matrix;
}

Matrix invert(const Matrix matrix) {
    if (matrix.cols != matrix.rows) {
        printf("Getting inverible matrix is impossible, because of matrix do not square");
        return EMPTY;
    }
    Matrix transponent = transpose(matrix);
    Matrix result = init_matrix(matrix.cols, matrix.rows);
    unsigned int n = transponent.rows;
    for (unsigned int row = 0; row < n; ++row) {
        for (unsigned int col = 0; col < n; ++col) {
            Matrix submatrix = init_matrix(n - 1, n - 1);
            unsigned int row_offset = 0;
            unsigned int col_offset = 0;
            for (unsigned int sub_row = 0; sub_row < n - 1; ++sub_row) {
                if (sub_row == row) { row_offset = 1; }
                for (unsigned int sub_col = 0; sub_col < n - 1; ++sub_col) {
                    if (sub_col == col) { col_offset = 1; }
                    submatrix.values[sub_row * (n - 1) + sub_col] =
                        transponent.values[(sub_row + row_offset) * n + (sub_col + col_offset)];
                }
            }
            result.values[row * matrix.cols + col] = pow(-1, row + col) * deter(submatrix);
            free_matrix(&submatrix);
        }
    }
    free_matrix(&transponent);
    Matrix multiplied_result = multiply_by_double(result, 1 / deter(result));
    free_matrix(&result);
    return multiplied_result;

}

Matrix copy(const Matrix matrix) {
    Matrix result = init_matrix(matrix.cols, matrix.rows);
    for (unsigned int index = 0; index < matrix.cols * matrix.rows; ++index) {
        result.values[index] = matrix.values[index];
    }
    return result;
}

Matrix exp(const Matrix matrix, int accuracy) {
    if (matrix.cols != matrix.rows) {
        printf("Exp is impossible, because of matrix do not square");
        return EMPTY;
    }
    Matrix new_result, new_powered, multiplied;
    Matrix result = unit_matrix(matrix.rows);
    Matrix powered = matrix;
    int factorial = 1;
    for (int acc = 1; acc <= accuracy; ++acc) {
        factorial *= acc;
        new_powered = multiply(powered, matrix);
        powered = copy(new_powered);
        free_matrix(&new_powered);
        multiplied = multiply_by_double(powered, 1./factorial);
        new_result = summ(result, multiplied);
        result = copy(new_result);
        free_matrix(&new_result);
        free_matrix(&multiplied);
    }
    free_matrix(&powered);
    return result;
}

int main() {
    Matrix m1, m2;

    m1 = init_matrix(2, 2);
    random_matrix(&m1);
    print_matrix(m1);

    m2 = init_matrix(2, 2);
    random_matrix(&m2);
    print_matrix(m2);

    Matrix addition;
    addition = summ(m1,m2);
    print_matrix(addition);
    free_matrix(&addition);

    Matrix substruction;
    substruction = sub(m1, m2);
    print_matrix(substruction);
    free_matrix(&substruction);

    double determinant;
    determinant = deter(m1);
    printf("%.2f\n\n", determinant);

    Matrix multiplication1;
    multiplication1 = multiply_by_double(m1, 5.);
    print_matrix(multiplication1);
    free_matrix(&multiplication1);

    Matrix multiplication2;
    multiplication2 = multiply(m1, m2);
    print_matrix(multiplication2);
    free_matrix(&multiplication2);

    Matrix trans;
    trans = transpose(m1);
    print_matrix(trans);
    free_matrix(&trans);

    Matrix invertible;
    invertible = invert(m1);
    print_matrix(invertible);
    free_matrix(&invertible);

    Matrix exponenta;
    exponenta = exp(m1, 3);
    print_matrix(exponenta);
    free_matrix(&exponenta);

    free_matrix(&m1);
    free_matrix(&m2);
}

