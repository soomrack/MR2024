#include <stdio.h>
#include <stdlib.h>
#include <math.h>


typedef struct {
    size_t rows;
    size_t cols;
    double *data;
} Matrix;


Matrix MATRIX_ZERO = {0, 0, NULL};


void matrix_error(int error_level, const char *message)
{
    if (error_level == 1) {
        fprintf(stderr, "Message: %s\n" , message);
    }
    if (error_level == 2) {
        fprintf(stderr, "Message: %s\n", message);
    }
}


Matrix matrix_allocate(Matrix frame)
{
    Matrix matrix = MATRIX_ZERO;

    if (frame.rows == 0 || frame.cols == 0) {
        matrix_error(1, "You have inserted zero matrix.\n");
        return matrix;
    }

    matrix.rows = frame.rows;
    matrix.cols = frame.cols;
    matrix.data = malloc((matrix.rows * matrix.cols) * sizeof(double));

    if (matrix.data == NULL) {
        matrix_error(2, "Memory allocation failed.\n");
        matrix = MATRIX_ZERO;
        return matrix;
    }

    return matrix;
}


void matrix_pass_array(Matrix *matrix, double array_name[])
{
    *matrix = matrix_allocate(*matrix);

    for (size_t idx = 0; idx < matrix->rows * matrix->cols; idx++) {
        matrix->data[idx] = array_name[idx];
    }
}


void memory_free(Matrix *matrix)
{
    matrix->rows = 0;
    matrix->cols = 0;
    free(matrix->data);
    matrix->data = NULL;
}


void matrix_print(Matrix *matrix, const char *message)
{
    printf("%s", message);
    if (matrix->data == NULL) {
        printf("Operation is not possible.\n\n");
    } else {
        for (size_t row = 0; row < matrix->rows; row++) {
            for(size_t col = 0; col < matrix->cols; col++) {
                printf("%.2f ", matrix->data[row * matrix->cols + col]);
            }
            printf("\n");
        }
        printf("\n");
    }
}


void matrix_copy(Matrix destination, Matrix source) {
    for (size_t row = 0; row < source.rows; ++row) {
        for (size_t col = 0; col < source.cols; ++col) {
            destination.data[row * destination.cols + col] = source.data[row * source.cols + col];
        }
    }
}


Matrix matrix_sum (Matrix A, Matrix B)
{
    if (A.cols != B.cols || A.rows != B.rows) {
        return MATRIX_ZERO;
    }

    Matrix new_matrix_add = {A.rows, A.cols};
    new_matrix_add = matrix_allocate(new_matrix_add);

    if (new_matrix_add.data == NULL) {
        matrix_error(2, "Memory allocation failed.\n");
        return MATRIX_ZERO;
    }

    for (size_t idx = 0; idx < new_matrix_add.rows * new_matrix_add.cols; idx++) {
        new_matrix_add.data[idx] = A.data[idx] + B.data[idx];
    }

    return new_matrix_add;
}


Matrix matrix_subtract (Matrix A, Matrix B)
{
    if (A.cols != B.cols || A.rows != B.rows) {
        return MATRIX_ZERO;
    }

    Matrix new_matrix_subtract = {A.rows, A.cols};
    new_matrix_subtract = matrix_allocate(new_matrix_subtract);

    if (new_matrix_subtract.data == NULL) {
        matrix_error(2, "Memory allocation failed.\n");
        return MATRIX_ZERO;
    }

    for (size_t idx = 0; idx < new_matrix_subtract.rows * new_matrix_subtract.cols; idx++) {
        new_matrix_subtract.data[idx] = A.data[idx] - B.data[idx];
    }

    return new_matrix_subtract;
}


Matrix matrix_multiply (Matrix A, Matrix B)
{
    if (A.cols != B.rows) {
        return MATRIX_ZERO;
    }

    Matrix new_matrix_multi = {A.rows, B.cols};
    new_matrix_multi = matrix_allocate(new_matrix_multi);

    if (new_matrix_multi.data == NULL) {
        matrix_error(2, "Memory allocation failed.\n");
        return MATRIX_ZERO;
    }

    for (size_t row = 0; row < new_matrix_multi.rows; row++) {
        for (size_t col = 0; col < new_matrix_multi.cols; col++) {
            new_matrix_multi.data[row * new_matrix_multi.cols + col] = 0;
            for (size_t k = 0; k < A.cols; k++) {
                new_matrix_multi.data[row * new_matrix_multi.cols + col] += A.data[row * A.cols + k] * B.data[k * B.cols + col];
            }
        }
    }
    return new_matrix_multi;

}


Matrix matrix_multi_by_number (Matrix A, double number)
{
    Matrix new_matrix_multi_by_number = {A.cols, A.rows};
    new_matrix_multi_by_number = matrix_allocate(new_matrix_multi_by_number);

    if (new_matrix_multi_by_number.data == NULL) {
        matrix_error(2, "Memory allocation failed.\n");
        return MATRIX_ZERO;
    }

    for (size_t idx = 0; idx < new_matrix_multi_by_number.rows * new_matrix_multi_by_number.cols; idx++) {
        new_matrix_multi_by_number.data[idx] = A.data[idx] * number;
    }

    return new_matrix_multi_by_number;
}


Matrix matrix_transpose (Matrix A)
{
    Matrix new_matrix_transpose = {A.cols, A.rows};
    new_matrix_transpose = matrix_allocate(new_matrix_transpose);

    if (new_matrix_transpose.data == NULL) {
        matrix_error(2, "Memory allocation failed.\n");
        return MATRIX_ZERO;
    }

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
        return NAN;
    }

    if (A.rows == 1) {
        return A.data[0];
    }
    if (A.rows == 2) {
        return A.data[0] * A.data[3] - A.data[1] * A.data[2]; 
    } else {
        double det = 0;
        double sign = 1.;
        for (size_t col = 0; col < A.cols; col++) {

            Matrix submatrix = {A.rows - 1, A.cols - 1};
            submatrix = matrix_allocate(submatrix);
            if (submatrix.data == NULL) {
                matrix_error(2, "Memory allocation failed.\n");
                return NAN;
            }

            size_t sub_row = 0;
            for (size_t row = 1; row < A.rows; row++) {
                size_t sub_col = 0;
                for (size_t idx = 0; idx < A.cols; idx++) {
                    if (idx != col) {
                        continue;
                    }
                    submatrix.data[sub_row * submatrix.cols + sub_col] = A.data[row * A.cols + idx];
                    sub_col++;
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
    double det_A = matrix_determinant(A);

    if (det_A == 0) {
        return MATRIX_ZERO;
    }

    Matrix cofactor_matrix = {A.rows, A.cols};
    cofactor_matrix = matrix_allocate(cofactor_matrix);
    if (cofactor_matrix.data == NULL) {
        matrix_error(2, "Memory allocation failed.\n");
        return MATRIX_ZERO;
    }

    for (size_t row = 0; row < A.rows; row++) {
        for (size_t col = 0; col < A.cols; col++) {

            Matrix submatrix = {A.rows - 1, A.cols - 1};
            submatrix = matrix_allocate(submatrix);
            if (submatrix.data == NULL) {
                matrix_error(2, "Memory allocation failed.\n");
                return MATRIX_ZERO;
            }

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
    
    return inverse_matrix;
}


Matrix matrix_power(Matrix A, unsigned long int n)
{
    if (A.rows != A.cols || n < 0) {
        return MATRIX_ZERO;
    }

    if (n == 0) {
        Matrix identity_matrix = {A.rows, A.cols};
        identity_matrix = matrix_allocate(identity_matrix);
        if (identity_matrix.data == NULL) {
            matrix_error(2, "Memory allocation failed.\n");
            return MATRIX_ZERO;
        }

        for (size_t row = 0; row < identity_matrix.rows; ++row) {
            for (size_t col = 0; col < identity_matrix.cols; ++col) {
                identity_matrix.data[row * identity_matrix.cols + col] = (row == col) ? 1.0 : 0.0;
            }
        }
        return identity_matrix;

    }

    Matrix matrix_powered_to_n = A;
    matrix_powered_to_n = matrix_allocate(matrix_powered_to_n);
    if (matrix_powered_to_n.data == NULL) {
        matrix_error(2, "Memory allocation failed.\n");
        return MATRIX_ZERO;
    }

    for (size_t idx = 0; idx < A.rows * A.cols; ++idx) {
        matrix_powered_to_n.data[idx] = A.data[idx];
    }

    for (unsigned long int power = 1; power < n; power++) {
        Matrix temp = matrix_multiply(matrix_powered_to_n, A);
        memory_free(&matrix_powered_to_n);
        matrix_powered_to_n = temp;
    }

    return matrix_powered_to_n;
}


Matrix matrix_exponent(Matrix A, long long int n)
{
    if (A.rows != A.cols || n < 0) {
        return MATRIX_ZERO;
    }

    Matrix matrix_exponent_to_n = {A.rows, A.cols};
    matrix_exponent_to_n = matrix_allocate(matrix_exponent_to_n);
    if (matrix_exponent_to_n.data == NULL) {
        matrix_error(2, "Memory allocation failed.\n");
        return MATRIX_ZERO;
    }

    for (size_t row = 0; row < matrix_exponent_to_n.rows; ++row) {
        for (size_t col = 0; col < matrix_exponent_to_n.cols; ++col) {
            matrix_exponent_to_n.data[row * matrix_exponent_to_n.cols + col] = (row == col) ? 1.0 : 0.0;
        }
    }

    if (n == 0) {
        return matrix_exponent_to_n;
    }

    Matrix power_of_A = {A.rows, A.cols};
    power_of_A = matrix_allocate(power_of_A);
    if (power_of_A.data == NULL) {
        matrix_error(2, "Memory allocation failed.\n");
        return MATRIX_ZERO;
    }

    matrix_copy(power_of_A, A);

    for (long long int idx = 1; idx <= n; idx++) {
        Matrix term = matrix_multi_by_number(power_of_A, 1.0 / idx);
        matrix_exponent_to_n = matrix_sum(matrix_exponent_to_n, term);

        Matrix next_power_of_A = matrix_multiply(power_of_A, A);
        matrix_copy(power_of_A, next_power_of_A);

        memory_free(&term);
        memory_free(&next_power_of_A);
    }

    memory_free(&power_of_A);
    return matrix_exponent_to_n;
}


void make_calculations()
{
    Matrix A = {2, 2};
    Matrix B = {2, 2};

    matrix_pass_array(&A, (double[]){1, 3, 5, 7});
    matrix_pass_array(&B, (double[]){1, 2, 3, 4});

    Matrix add_A_B = matrix_sum(A, B);
    matrix_print(&add_A_B, "Result of Matrix Addition: \n");

    Matrix subtract_A_B = matrix_subtract(A, B);
    matrix_print(&subtract_A_B, "Result of Matrix Subtraction: \n");

    Matrix multi_A_B = matrix_multiply(A, B);
    matrix_print(&multi_A_B, "Result of Matrix Multiplication: \n");

    Matrix multi_by_numbder_A = matrix_multi_by_number(A, 2);
    matrix_print(&multi_by_numbder_A, "Result of Matrix A Multiplication by Number: \n");
    Matrix multi_by_numbder_B = matrix_multi_by_number(B, 2);
    matrix_print(&multi_by_numbder_B, "Result of Matrix B Multiplication by Number: \n");
    
    Matrix transposed_A = matrix_transpose(A); 
    matrix_print(&transposed_A, "Result of Matrix A Transposition: \n");
    Matrix transposed_B = matrix_transpose(B); 
    matrix_print(&transposed_B, "Result of Matrix B Transposition: \n");

    double det_A = matrix_determinant(A);
    printf("Determinant of A: %.2f\n", det_A);

    double det_B = matrix_determinant(B);
    printf("Determinant of B: %.2f\n\n", det_B);

    Matrix inverse_A = matrix_inverse(A);
    matrix_print(&inverse_A, "Result of Matrix A Inversion: \n");
    Matrix inverse_B = matrix_inverse(B);
    matrix_print(&inverse_B, "Result of Matrix B Inversion: \n");

    Matrix power_A = matrix_power(A, 3);
    matrix_print(&power_A, "Result of Matrix A Power: \n");
    Matrix power_B = matrix_power(B, 2);
    matrix_print(&power_B, "Result of Matrix B Power: \n");

    Matrix exponent_A = matrix_exponent(A, 2);
    matrix_print(&exponent_A, "Result of Matrix A Exponent: \n");
    Matrix exponent_B = matrix_exponent(B, 2);
    matrix_print(&exponent_B, "Result of Matrix B Exponent: \n");

    memory_free(&add_A_B);
    memory_free(&subtract_A_B);
    memory_free(&multi_A_B);
    memory_free(&multi_by_numbder_A);
    memory_free(&multi_by_numbder_B);
    memory_free(&transposed_A);
    memory_free(&transposed_B);
    memory_free(&inverse_A);
    memory_free(&inverse_B);
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
