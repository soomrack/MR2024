#include <stdio.h>
#include <stdlib.h>


typedef struct {
    size_t rows;
    size_t cols;
    double *data;
} Matrix;

Matrix Zero = {0, 0, 0};

void matrix_allocate(Matrix *matrix)
{
    // Matrix matrix = Zero;

    // if (frame.rows == 0 || frame.cols == 0) {
    //     printf("Message: You have inserted zero matrix.\n");
    //     return matrix;
    // }

    // matrix.rows = frame.rows;
    // matrix.cols = frame.cols;
    // matrix.data = malloc((matrix.rows * matrix.cols) * sizeof(double));

    // if (matrix.data == NULL) {
    //     printf("Message: Memory allocation failed.\n");
    //     matrix = Zero;
    //     return matrix;
    // }

    matrix->data = malloc((matrix->rows * matrix->cols) * sizeof(double));

    // printf("Message: Memory allocated successfully.\n");
}


void matrix_pass_array(Matrix *matrix, double array_name[])
{

    matrix_allocate(matrix);

    for (size_t idx = 0; idx < matrix->rows * matrix->cols; idx++) {
        matrix->data[idx] = array_name[idx];
    }

}


void memory_free(Matrix *matrix)
{
    matrix->rows = 0;
    matrix->cols = 0;
    matrix->data = 0;
    free(matrix->data);
}


void matrix_print(Matrix *matrix)
{
    for (size_t row = 0; row < matrix->rows; row++) {
        for(size_t col = 0; col < matrix->cols; col++) {
            printf("%.2f ", matrix->data[row * matrix->cols + col]);
        }
        printf("\n");
    }
    printf("\n");
}


Matrix matrix_add (Matrix A, Matrix B)
{
    if (A.cols != B.cols || A.rows != B.rows) {
        printf("Matrix sizes are not equal!\n");
        return Zero;
    }

    Matrix new_matrix_add = {A.rows, A.cols};
    matrix_allocate(&new_matrix_add);

    for (size_t row = 0; row < new_matrix_add.rows; row++) {
        for (size_t col = 0; col < new_matrix_add.cols; col++) {
                new_matrix_add.data[row * new_matrix_add.cols + col] = A.data[row * A.cols + col] + B.data[row * B.cols + col];
        }
    }
    return new_matrix_add;
}


Matrix matrix_subtract (Matrix A, Matrix B)
{
    if (A.cols != B.cols || A.rows != B.rows) {
        printf("Matrix sizes are not equal!\n");
        return Zero;
    }

    Matrix new_matrix_subtract = {2, 2};
    matrix_allocate(&new_matrix_subtract);

    for (size_t row = 0; row < new_matrix_subtract.rows; row++) {
        for (size_t col = 0; col < new_matrix_subtract.cols; col++) {
            new_matrix_subtract.data[row * new_matrix_subtract.cols + col] = A.data[row * A.cols + col] - B.data[row * B.cols + col];
        }
    }
    return new_matrix_subtract;
}


Matrix matrix_multiply (Matrix A, Matrix B)
{
    if (A.cols != B.rows) {
        printf("Incorrect matrix sizes for multiplication!");
        return Zero;
    }

    Matrix new_matrix_multi = {A.rows, B.cols};
    matrix_allocate(&new_matrix_multi);

    for (size_t row = 0; row < new_matrix_multi.rows; row++) {
        for (size_t col = 0; col < new_matrix_multi.cols; col++) {
            for (size_t k = 0; k < A.cols; k++) {
                new_matrix_multi.data[row * new_matrix_multi.cols + col] += A.data[row * A.cols + k] * B.data[k * B.cols + col];
            }
        }
    }
    return new_matrix_multi;

}

Matrix matrix_multi_by_number (Matrix A, double number)
{
    // printf("Multiplication by number:\n");

    Matrix new_matrix_multi_by_number = {A.cols, A.rows};
    matrix_allocate(&new_matrix_multi_by_number);

    for (size_t row = 0; row < new_matrix_multi_by_number.rows; row++) {
        for (size_t col = 0; col < new_matrix_multi_by_number.cols; col++) {
            new_matrix_multi_by_number.data[row * new_matrix_multi_by_number.cols + col] = A.data[row * new_matrix_multi_by_number.cols + col] * number;
        }
    }
    // matrix_print(&new_matrix_multi_by_number);
    return new_matrix_multi_by_number;
}




Matrix matrix_transpose (Matrix A)
{
    // printf("Matrix Transposed:\n");

    Matrix new_matrix_transpose = {A.cols, A.rows};
    matrix_allocate(&new_matrix_transpose);

    for (size_t row = 0; row < new_matrix_transpose.rows; row++) {
        for (size_t col = 0; col < new_matrix_transpose.cols; col++) {
            new_matrix_transpose.data[row * new_matrix_transpose.cols + col] = A.data[col * new_matrix_transpose.rows + row];
        }
    }
    return new_matrix_transpose;
}


double matrix_determinant (Matrix A)
{
    if (A.cols != A.rows) {
        printf("Incorrect matrix sizes for determinant calculation!");
        return 0;
    }

    if (A.rows == 1) {
        return A.data[0];
    } else if (A.rows == 2) {
        return A.data[0] * A.data[3] - A.data[1] * A.data[2];
    } else {
        double det = 0;
        int sign = 1;
        for (size_t col = 0; col < A.cols; col++) {
            Matrix submatrix = {A.rows - 1, A.cols - 1};
            matrix_allocate(&submatrix);

            size_t sub_row = 0;
            for (size_t row = 1; row < A.rows; row++) {
                size_t sub_col = 0;
                for (size_t j = 0; j < A.cols; j++) {
                    if (j != col) {
                        submatrix.data[sub_row * submatrix.cols + sub_col] = A.data[row * A.cols + j];
                        sub_col++;
                    }
                }
                sub_row++;
            }

            det += sign * A.data[col] * matrix_determinant(submatrix);
            sign = -sign;

            memory_free(&submatrix);
        }
        return det;
    }
}


Matrix matrix_inverse (Matrix A)
{
    // printf("Matrix Inverse:\n");
    double det_A = matrix_determinant(A);

    if (det_A == 0) {
        printf("Incorrect matrix sizes for inversion!\n");
        return Zero;
    }

    Matrix cofactor_matrix = {A.rows, A.cols};
    matrix_allocate(&cofactor_matrix);

    for (size_t row = 0; row < A.rows; row++) {
        for (size_t col = 0; col < A.cols; col++) {
            Matrix submatrix = {A.rows - 1, A.cols - 1};
            matrix_allocate(&submatrix);

            size_t sub_row = 0;
            for (size_t i = 0; i < A.rows; i++) {
                if (i == row) {
                    continue;
                }
                size_t sub_col = 0;
                for (size_t j = 0; j < A.cols; j++) {
                    if (j == col) {
                        continue;
                    }
                    submatrix.data[sub_row * submatrix.cols + sub_col] = A.data[i * A.cols + j];
                    sub_col++;
                }
                sub_row++;
            }

            cofactor_matrix.data[row * cofactor_matrix.cols + col] = ((row + col) % 2 == 0 ? 1 : -1) * matrix_determinant(submatrix);

            memory_free(&submatrix);
        }
    }

    Matrix transposed_cofactor_matrix = matrix_transpose(cofactor_matrix);
    memory_free(&cofactor_matrix);

    Matrix inverse_matrix = matrix_multi_by_number(transposed_cofactor_matrix, 1 / det_A);
    memory_free(&transposed_cofactor_matrix);

    // matrix_print(&inverse_matrix);
    
    return inverse_matrix;
}


Matrix matrix_power (Matrix A, long long int n)
{
    if (A.rows != A.cols || n < 0) {
        printf("Incorrect matrix sizes or power!\n");
        return Zero;
    }

    Matrix matrix_powered_to_n = A;
    matrix_allocate(&matrix_powered_to_n);
    matrix_powered_to_n = A;

    for (long long int power = 1; power < n; power++) {
        matrix_powered_to_n = matrix_multiply(matrix_powered_to_n, A);
    }

    return matrix_powered_to_n;
}


long long int factorial(int n) {
  if (n < 0) {
    return -1;
  } else if (n == 0) {
    return 1;
  } else {
    return n * factorial(n - 1);
  }
}


Matrix matrix_exponent (Matrix A, long long int n)
{
    if (A.rows != A.cols || n < 1) {
        printf("Incorrect matrix sizes for calculating exponent!\n");
        return Zero;
    }

    Matrix matrix_exponent_to_n = {A.rows, A.cols};
    matrix_allocate(&matrix_exponent_to_n);

    for (long long int idx = 1; idx <= n; idx++) {
        Matrix step_matrix = A;
        matrix_allocate(&step_matrix);
        step_matrix = matrix_multi_by_number(matrix_power(A, idx), 1/factorial(idx));
        matrix_exponent_to_n = matrix_add(matrix_exponent_to_n, step_matrix);
        memory_free(&step_matrix);
    }

    return matrix_exponent_to_n;
}


void make_calculations()
{
    Matrix A = {2, 2};
    Matrix B = {2, 2};

    matrix_pass_array(&A, (double[]){1, 3, 5, 7});
    matrix_pass_array(&B, (double[]){1, 2, 3, 4});

    Matrix add_A_B = matrix_add(A, B);
    Matrix subtract_A_B = matrix_subtract(A, B);

    Matrix multi_A_B = matrix_multiply(A, B);

    Matrix transposed_A = matrix_transpose(A); 
    Matrix transposed_B = matrix_transpose(B); 

    Matrix multi_by_numbder_A = matrix_multi_by_number(A, 2);
    Matrix multi_by_numbder_B = matrix_multi_by_number(B, 2);

    Matrix inverse_A = matrix_inverse(A);
    Matrix inverse_B = matrix_inverse(B);

    Matrix power_A = matrix_power(A, 2);
    Matrix power_B = matrix_power(B, 3);

    Matrix exponent_A = matrix_exponent(A, 5);
    Matrix exponent_B = matrix_exponent(B, 5);

    matrix_print(&transposed_A);
    // matrix_print(&transposed_B);

    matrix_print(&multi_by_numbder_A);
    // matrix_print(&multi_by_numbder_B);
    
    matrix_print(&inverse_A);
    // matrix_print(&inverse_B);

    matrix_print(&multi_A_B);
    // matrix_print(&subtract_A_B);
    // matrix_print(&add_A_B);

    matrix_print(&power_A);
    // matrix_print(&power_B);

    matrix_print(&exponent_A);
    // matrix_print(&exponent_B);

    double det_A = matrix_determinant(A);
    printf("Determinant of A: %.2f\n", det_A);

    double det_B = matrix_determinant(B);
    printf("Determinant of B: %.2f\n", det_B);

    memory_free(&transposed_A);
    memory_free(&transposed_B);
    memory_free(&multi_by_numbder_A);
    memory_free(&multi_by_numbder_B);
    memory_free(&inverse_A);
    memory_free(&inverse_B);
    memory_free(&multi_A_B);
    memory_free(&subtract_A_B);
    memory_free(&add_A_B);
    memory_free(&power_A);
    memory_free(&power_B);
    memory_free(&exponent_A);
    memory_free(&exponent_B);
    memory_free(&A);
    memory_free(&B);
}


int main()
{
    make_calculations();
    return 0;
}
