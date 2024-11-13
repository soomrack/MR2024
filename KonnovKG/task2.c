#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

struct Matrix{
    size_t rows;
    size_t cols;
    double *data;
};
typedef struct Matrix Matrix;


Matrix MATRIX_ZERO = {0, 0, NULL};

enum MatrixException {ERROR, WARNING, INFO, DEBUG};
typedef enum MatrixException MatrixException;


void matrix_error(const enum MatrixException error_level, const char *message)
{
    if (error_level == ERROR) {
        fprintf(stderr, "ERROR: %s" , message);
    }
    if (error_level == WARNING) {
        fprintf(stderr, "WARNING: %s", message);
    }
}


Matrix matrix_allocate(const Matrix frame)
{
    Matrix matrix = MATRIX_ZERO;

    if (frame.rows == 0 || frame.cols == 0) {
        return (Matrix){frame.rows, frame.cols, NULL};
    }

    if (frame.rows * frame.cols > SIZE_MAX / sizeof(double)) {
        matrix_error(ERROR, "Memory allocation failed: size_t overflow.\n");
        return MATRIX_ZERO;
    }

    matrix.rows = frame.rows;
    matrix.cols = frame.cols;
    matrix.data = malloc((matrix.rows * matrix.cols) * sizeof(double));

    if (matrix.data == NULL) {
        matrix_error(ERROR, "Memory allocation failed.\n");
        matrix = MATRIX_ZERO;
        return matrix;
    }

    return matrix;
}


void matrix_pass_array(Matrix *matrix, double array_name[])
{
    *matrix = matrix_allocate(*matrix);
    memcpy(matrix->data, array_name, matrix->rows * matrix->cols * sizeof(double));
}


void memory_free(Matrix *matrix)
{
    matrix->rows = 0;
    matrix->cols = 0;
    if (matrix->data != NULL) {
        free(matrix->data);
    }
    matrix->data = NULL;
}


void matrix_print(const Matrix *matrix, const char *message)
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


void matrix_copy(Matrix destination, const Matrix source) {

    if (destination.rows != source.rows || destination.cols != source.cols) {
        matrix_error(ERROR, "Unable to copy matrixes of different sizes.\n");
        return;
    }

    for (size_t row = 0; row < source.rows; ++row) {
        for (size_t col = 0; col < source.cols; ++col) {
            destination.data[row * destination.cols + col] = source.data[row * source.cols + col];
        }
    }
}


Matrix matrix_sum (const Matrix A, const Matrix B)
{
    if (A.cols != B.cols || A.rows != B.rows) {
        matrix_error(ERROR, "Unable to add matrixes of different sizes.\n");
        return MATRIX_ZERO;
    }

    Matrix C = {A.rows, A.cols, NULL};
    C = matrix_allocate(C);

    for (size_t idx = 0; idx < C.rows * C.cols; idx++) {
        C.data[idx] = A.data[idx] + B.data[idx];
    }

    return C;
}


Matrix matrix_subtract (const Matrix A, const Matrix B)
{
    if (A.cols != B.cols || A.rows != B.rows) {
        matrix_error(ERROR, "Unable to subtract matrixes of different sizes.\n");
        return MATRIX_ZERO;
    }

    Matrix C = {A.rows, A.cols, NULL};
    C = matrix_allocate(C);

    for (size_t idx = 0; idx < C.rows * C.cols; idx++) {
        C.data[idx] = A.data[idx] - B.data[idx];
    }

    return C;
}


Matrix matrix_multiply (const Matrix A, const Matrix B)
{
    if (A.cols != B.rows) {
        matrix_error(ERROR, "Unable to multiply matrixes of given sizes.\n");
        return MATRIX_ZERO;
    }

    Matrix C = {A.rows, B.cols, NULL};
    C = matrix_allocate(C);

    for (size_t row = 0; row < C.rows; row++) {
        for (size_t col = 0; col < C.cols; col++) {
            C.data[row * C.cols + col] = 0;
            for (size_t k = 0; k < A.cols; k++) {
                C.data[row * C.cols + col] += A.data[row * A.cols + k] * B.data[k * B.cols + col];
            }
        }
    }

    return C;
}


Matrix matrix_multi_by_number (const Matrix A, const double number)
{
    Matrix C = {A.cols, A.rows, NULL};
    C = matrix_allocate(C);

    for (size_t idx = 0; idx < C.rows * C.cols; idx++) {
        C.data[idx] = A.data[idx] * number;
    }

    return C;
}


Matrix matrix_transpose (const Matrix A)
{
    Matrix T = {A.cols, A.rows, NULL};
    T = matrix_allocate(T);

    for (size_t row = 0; row < T.rows; row++) {
        for (size_t col = 0; col < T.cols; col++) {
            T.data[row * T.cols + col] = A.data[col * T.rows + row];
        }
    }
    return T;
}


Matrix matrix_get_submatrix(const Matrix A, size_t row_exclude, size_t col_exclude) {
    Matrix submatrix = {A.rows - 1, A.cols - 1, NULL};
    submatrix = matrix_allocate(submatrix);

    size_t sub_row = 0;
    for (size_t row = 0; row < A.rows; row++) {
        if (row == row_exclude) continue;
        size_t sub_col = 0;
        for (size_t col = 0; col < A.cols; col++) {
            if (col == col_exclude) continue;
            submatrix.data[sub_row * submatrix.cols + sub_col] = A.data[row * A.cols + col];
            sub_col++;
        }
        sub_row++;
    }
    return submatrix;
}

double matrix_determinant (const Matrix A)
{
    if ((A.cols != A.rows) || A.rows == 0 || A.cols == 0) {
        return NAN;
    }
    if (A.rows == 1) {
        return A.data[0];
    }
    if (A.rows == 2) {
        return A.data[0] * A.data[3] - A.data[1] * A.data[2]; 
    }
    double det = 0;
    double sign = 1.;
    for (size_t col = 0; col < A.cols; col++) {

        Matrix submatrix = matrix_get_submatrix(A, 0, col); 

        if (submatrix.data == NULL) {
            return NAN;
        }

        if (matrix_determinant(submatrix) == NAN) {
            memory_free(&submatrix);
            return NAN;
        }

        det += sign * A.data[col] * matrix_determinant(submatrix);
        sign = -sign;

        memory_free(&submatrix);
    }
    return det;

}


Matrix matrix_inverse (const Matrix A)
{
    double det_A = matrix_determinant(A);

    if (abs(det_A) < 0.00000001) {
        matrix_error(ERROR, "Unable to calculate inverse of given matrix. \n");
        return MATRIX_ZERO;
    }

    Matrix cofactor_matrix = {A.rows, A.cols};
    cofactor_matrix = matrix_allocate(cofactor_matrix);

    for (size_t row = 0; row < A.rows; row++) {
        for (size_t col = 0; col < A.cols; col++) {

            Matrix submatrix = matrix_get_submatrix(A, row, col);

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


Matrix matrix_identity(const size_t rows, const size_t cols) {

    Matrix identity_matrix = {rows, cols};
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


Matrix matrix_power(const Matrix A, const unsigned long long int n)
{
    if (A.rows != A.cols || n < 0) {
        matrix_error(ERROR, "Unable to calculate power of given matrix.\n");
        return MATRIX_ZERO;
    }

    if (n == 0) {
        return matrix_identity(A.rows, A.cols);;
    }

    Matrix matrix_powered_to_n = A;
    matrix_powered_to_n = matrix_allocate(matrix_powered_to_n);

    matrix_copy(matrix_powered_to_n, A);

    for (unsigned long int power = 1; power < n; power++) {
        Matrix temp = matrix_multiply(matrix_powered_to_n, A);
        memory_free(&matrix_powered_to_n);
        matrix_powered_to_n = temp;
    }

    return matrix_powered_to_n;
}


Matrix matrix_exponent(const Matrix A, const unsigned long long int n)
{
    if (A.rows != A.cols || n < 0) {
        matrix_error(ERROR, "Unable to calculate exponent of given matrix.\n");
        return MATRIX_ZERO;
    }

    Matrix matrix_exponent_term = matrix_identity(A.rows, A.cols);
    Matrix matrix_exponent_result = matrix_identity(A.rows, A.cols);
    
    if (n == 0) {
        return matrix_exponent_term;
    }

    double factorial = 1.0;

    for (long long int idx = 1; idx <= n; idx++) {
        Matrix temp = matrix_multiply(matrix_exponent_term, A);
        memory_free(&matrix_exponent_term);

        matrix_exponent_term = temp;
        factorial *= idx;

        Matrix added = matrix_multi_by_number(matrix_exponent_term, 1.0 / factorial);
        Matrix newResult = matrix_sum(matrix_exponent_result, added);

        memory_free(&matrix_exponent_result);
        matrix_exponent_result = newResult;
        memory_free(&added);

    }

    memory_free(&matrix_exponent_term);
    return matrix_exponent_result;
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

    Matrix exponent_A = matrix_exponent(A, 3);
    matrix_print(&exponent_A, "Result of Matrix A Exponent: \n");
    Matrix exponent_B = matrix_exponent(B, 20);
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
