#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct Matrix {
    int row;
    int col;
    double *array;
} Matrix;

void give_matrix_memory(Matrix *matrix) {
    matrix->array = (double *) malloc(matrix->row * matrix->col * sizeof(double));
}

void free_matrix_memory(Matrix *matrix) {
    free(matrix->array);
}

void input_matrix(Matrix *matrix, int ROW, int COL, double input[][COL]) {
    matrix->row = ROW;
    matrix->col = COL;

    give_matrix_memory(matrix);

    for (int row = 0; row < matrix->row; row++) {
        for (int col = 0; col < matrix->col; col++)
            matrix->array[row * matrix->col + col] = input[row][col];
    }
}

void matrix_to_array(Matrix *matrix, double array[matrix->row][matrix->col]) {
    for (int row = 0; row < matrix->row; row++) {
        for (int col = 0; col < matrix->col; col++) {
           array[row][col] = matrix->array[row * matrix->col + col];
        }
    }
}

void fill_zero(Matrix *matrix) {
    for (int item = 0; item < (matrix->row * matrix->col); item++)
        matrix->array[item] = 0;
}

void print_matrix(const Matrix *matrix) {
    if (matrix == NULL)
        return;

    for (int item = 0; item < (matrix->row * matrix->col); item++) {
        if ((item != 0) && ((item % matrix->col) == 0)) {
            printf("\n");
        }
        printf("%6.2f ", matrix->array[item]);
    }
    printf("\n");
}

void exception(int error_code) {
    switch (error_code) {
        case 1:
            printf("\nMatrices have different sizes. Addition not possible\n");
            break;
        case 2:
            printf("\nMatrices have different sizes. Subtraction not possible\n");
            break;
        case 3:
            printf("\nThe number of columns doesn't match the number of rows. Multiplication not possible\n");
            break;
        case 4:
            printf("\nThe matrix isn't square. The determinant can't be calculated\n");
            break;
        default:
            printf("ERROR");
    }
}

// Prototype of the det_calc function to prevent compilation errors
double det_calc(int N, double array[N][N]);

// Defining the det_calc function to calculate the determinant of the matrix
double det_calc(int N, double array[N][N]) {
    double det = 0;
    
    if (N == 2) {
        return array[0][0] * array[1][1] - array[0][1] * array[1][0];
    }
    
    double minor[N - 1][N - 1];
    int offset = 0;

    for (int i = 0; i < N; i++) {
        for (int row = 0; row < N - 1; row++) {
            for (int col = 0; col < N; col++) {
                if (col == i) {
                    offset = 1;
                }
                minor[row][col] = array[row + 1][col + offset];
            }
            offset = 0;
        }
        det += pow(-1, i) * array[0][i] * det_calc(N - 1, minor);
    }
    return det;
}

Matrix *determinant_matrix(struct Matrix *matrix) {
    if (matrix->row != matrix->col) {
        exception(4);
        return NULL;
    }

    double array[matrix->row][matrix->col];
    matrix_to_array(matrix, array);

    static Matrix determinant;
    determinant.row = 1;
    determinant.col = 1;
    give_matrix_memory(&determinant);

    determinant.array[0] = det_calc(matrix->row, array);

    return &determinant;
}

// Utility function to extract a row as a separate array
void get_row_as_array(const Matrix *matrix, int row, double *row_array) {
    for (int col = 0; col < matrix->col; col++) {
        row_array[col] = matrix->array[row * matrix->col + col];
    }
}

// Utility function to extract a column as a separate array
void get_col_as_array(const Matrix *matrix, int col, double *col_array) {
    for (int row = 0; row < matrix->row; row++) {
        col_array[row] = matrix->array[row * matrix->col + col];
    }
}

void check() {
    Matrix matrix1;
    Matrix matrix2;

    double test3x3[3][3] = {{1, 2, 3},
                            {4, 5, 6},
                            {7, 8, 1}};

    double test2x2[2][2] = {{2, 4},
                            {6, 8}};

    // Test determinant function for 3x3 matrix
    input_matrix(&matrix1, 3, 3, test3x3);
    Matrix *det3x3 = determinant_matrix(&matrix1);
    printf("Determinant of 3x3 matrix:\n");
    print_matrix(det3x3);

    // Test determinant function for 2x2 matrix
    input_matrix(&matrix2, 2, 2, test2x2);
    Matrix *det2x2 = determinant_matrix(&matrix2);
    printf("Determinant of 2x2 matrix:\n");
    print_matrix(det2x2);

    // Free memory
    free_matrix_memory(&matrix1);
    free_matrix_memory(&matrix2);

    if (det3x3 != NULL)
        free_matrix_memory(det3x3);

    if (det2x2 != NULL)
        free_matrix_memory(det2x2);
}

int main() {
    check();
}
