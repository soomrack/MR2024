#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>


typedef struct Matrix{
    size_t rows;
    size_t cols;
    double *data;
}Matrix;


Matrix MATRIX_ZERO = {0, 0, NULL};

enum MatrixException {ERROR, WARNING, INFO, DEBUG};
typedef enum MatrixException MatrixException;

void matrix_exception (const enum MatrixException level, const char *result)
{
    if (level == ERROR) {
        printf("ERROR: %s" , result);
    }
    if (level == WARNING) {
        printf ("WARNING: %s", result);
    }
}

Matrix matrix_allocate(const size_t rows, const size_t cols)
{
    Matrix matrix = MATRIX_ZERO;

    if (rows == 0 || cols == 0) {
        matrix_exception(WARNING, "Initialized matrix with 0 rows/cols \n");
        return (Matrix) {rows, cols, NULL};   
    }

    if (((double)SIZE_MAX / cols / rows / sizeof(double)) < 1) {
		matrix_exception(ERROR, "Memory allocation failed \n");
        return MATRIX_ZERO;
    }
    
    Matrix A;
    A.rows = rows; 
    A.cols = cols;
    A.data = malloc((A.rows * A.cols) * sizeof(double));
    
    if(A.data == NULL) {
        matrix_exception(ERROR, "Memory allocation failed \n");
		return MATRIX_ZERO;
	}

    return A;
}

void memory_free(Matrix *A)
{
    if (A == NULL) return;

    free(A->data);
     A->rows = 0;
     A->cols = 0;
    A->data = NULL;
}

void matrix_fill(const Matrix A, const double array[])
{   
    memcpy(A.data, array, A.cols * A.rows * sizeof(double));
}

void matrix_print(Matrix A)
{
    for (int row = 0; row < A.rows; row++) {
        for (int col = 0; col < A.cols; col++)
            printf("%.f ", A.data[row * A.cols + col]);
        printf("\n");
    } 
    printf("\n");
}

Matrix matrix_copy(const Matrix A) 
{
    Matrix copy = matrix_allocate(A.rows, A.cols);

    if (A.data == NULL) {
        matrix_exception(ERROR, "Unable to copy: Data of source matrix is NULL \n");
    }

    memcpy(copy.data, A.data, A.cols * A.rows * sizeof(double));

    return copy;

}

Matrix matrix_sum(const Matrix A, const Matrix B)
{
    if (A.cols != B.cols || A.rows != B.rows) {
        matrix_exception(ERROR, "Unable to add matrixes of different sizes\n");
        return MATRIX_ZERO;
    }

    Matrix C = matrix_allocate(A.rows, A.cols);

    for (size_t idx = 0; idx < C.rows * C.cols; idx++) {
        C.data[idx] = A.data[idx] + B.data[idx];
    }

    return C;
}

Matrix matrix_subtract(const Matrix A, const Matrix B)
{
    if (A.cols != B.cols || A.rows != B.rows) {
        matrix_exception(ERROR, "Unable to subtract matrixes of different sizes\n");
        return MATRIX_ZERO;
    }

    Matrix C = matrix_allocate(A.rows, A.cols);

    for (size_t idx = 0; idx < C.rows * C.cols; idx++) {
        C.data[idx] = A.data[idx] - B.data[idx];
    }

    return C;
}

Matrix matrix_multiply(const Matrix A, const Matrix B)
{
    if (A.cols != B.rows) {
        matrix_exception(ERROR, "Unable to multi: Matrix A cols is not equal Matrix B rows\n");
        return MATRIX_ZERO;
    }

     Matrix C = matrix_allocate(A.rows, A.cols);

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

Matrix matrix_multiplying_by_number(const Matrix A, const double number)
{
    Matrix C = matrix_allocate(A.rows, A.cols);

    for (size_t idx = 0; idx < C.rows * C.cols; idx++) {
        C.data[idx] = A.data[idx] * number;
    }

    return C;
}

Matrix matrix_trans(const Matrix A)
{
    Matrix T = matrix_allocate(A.rows, A.cols);

    for (size_t row = 0; row < T.rows; row++) {
        for (size_t col = 0; col < T.cols; col++) {
            T.data[row * T.cols + col] = A.data[col * T.rows + row];
        }
    }
    return T;
}

Matrix matrix_submatrix(const Matrix A, size_t row_exclude, size_t col_exclude) {

    Matrix submatrix = matrix_allocate(A.rows-1, A.cols-1);

    if (submatrix.data == NULL) {
        matrix_exception(ERROR, "Unable to calculate inverse of given matrix \n");
        return MATRIX_ZERO;
    }

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

double matrix_determinant(const Matrix A)
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
    double sign = 1;
    for (size_t col = 0; col < A.cols; col++) {

        Matrix submatrix = matrix_submatrix(A, 0, col); 

        if (submatrix.data == NULL) {
            return NAN;
        }

        det += sign * A.data[col] * matrix_determinant(submatrix);
        sign = -sign;

        memory_free(&submatrix);
    }
    return det;
}

Matrix matrix_identity(const size_t rows, const size_t cols) {

    Matrix identity_matrix = matrix_allocate(rows, cols);

    if (identity_matrix.data == NULL) {
        matrix_exception(ERROR, "Unable to calculate inverse of given matrix \n");
        return MATRIX_ZERO;
    }
  
    memset(identity_matrix.data, 0, identity_matrix.rows * identity_matrix.cols * sizeof(double));

    for (size_t idx = 0; idx < identity_matrix.rows * identity_matrix.cols; idx+=identity_matrix.cols+1) {
        identity_matrix.data[idx] = 1.0;
    }

    return identity_matrix;
}

Matrix matrix_power(const Matrix A, const unsigned long long int n)
{
    if (A.rows != A.cols) {
        matrix_exception(ERROR, "Unable to calculate power of given matrix\n");
        return MATRIX_ZERO;
    }

    if (n == 0) {
        return matrix_identity(A.rows, A.cols);;
    }

    Matrix matrix_powered_to_n = matrix_copy(A);
    Matrix temp = MATRIX_ZERO;

    if (matrix_powered_to_n.data == NULL) {
        matrix_exception(ERROR, "Unable to calculate power of given matrix\n");
        return MATRIX_ZERO;
    } 

    for (unsigned long long int pow = 1; pow < n; pow++) {
        memory_free(&temp);
        temp = matrix_multiply(matrix_powered_to_n, A);

        memory_free(&matrix_powered_to_n);
        matrix_powered_to_n = temp;
        temp = MATRIX_ZERO; 

    }

    return matrix_powered_to_n;
}

Matrix matrix_inverse(const Matrix A)
{
    double det_A = matrix_determinant(A);

    if (abs(det_A) < 0.00000001) {
        matrix_exception(ERROR, "Unable to inverse: determinant is equal to 0 \n");
        return MATRIX_ZERO;
    }

    Matrix additional_matrix = matrix_allocate(A.rows, A.cols);

    for (size_t row = 0; row < A.rows; row++) {
        for (size_t col = 0; col < A.cols; col++) {

            Matrix submatrix = matrix_submatrix(A, row, col);

            additional_matrix.data[row * additional_matrix.cols + col] = ((row + col) % 2 == 0 ? 1 : -1) * matrix_determinant(submatrix);

            memory_free(&submatrix);
        }
    }

    Matrix transposed_additional_matrix = matrix_trans(additional_matrix);
    memory_free(&additional_matrix);

    Matrix inverse_matrix = matrix_multiplying_by_number(transposed_additional_matrix, 1 / det_A);
    memory_free(&transposed_additional_matrix);
    
    return inverse_matrix;
}

Matrix matrix_exponent(const Matrix A, const unsigned long long int n)
{
    if (A.rows != A.cols) {
        matrix_exception(ERROR, "Unable to calculate exponent\n");
        return MATRIX_ZERO;
    }

    Matrix matrix_exp_result = matrix_identity(A.rows, A.cols);
    Matrix temp = MATRIX_ZERO;
    Matrix temp2 = MATRIX_ZERO;
    Matrix exp_result = MATRIX_ZERO;
    double factorial = 1.0;

    for (unsigned long long int idx = 1; idx <= n; idx++) {
        memory_free(&temp);
        temp = matrix_power(A, idx);
        
        factorial *= idx;

        memory_free(&temp2);
        temp2 = matrix_multiplying_by_number(temp, 1.0 / factorial);

        memory_free(&exp_result);
        exp_result = matrix_sum(matrix_exp_result, temp2);
        
        memory_free(&matrix_exp_result);
        matrix_exp_result = exp_result;
        exp_result = MATRIX_ZERO;

    }

    memory_free(&temp);
    memory_free(&temp2);
    memory_free(&exp_result);
    return matrix_exp_result;
}


void calculations ()
{
    Matrix A = matrix_allocate(2, 2);
    Matrix B = matrix_allocate(2, 2);

    matrix_fill(A, (double[]){1, 2, 3, 4});
    matrix_fill(B, (double[]){5, 6, 7, 8});

    Matrix add_A_B = matrix_sum(A, B);
    printf("Addition\n");
    matrix_print(add_A_B);

    Matrix subtract_A_B = matrix_subtract(A, B);
    printf("Subtraction\n");
    matrix_print(subtract_A_B);

    Matrix multi_A_B = matrix_multiply(A, B);
    printf("Multiplication\n");
    matrix_print(multi_A_B);

    Matrix multi_by_numbder_A = matrix_multiplying_by_number(A, 2);
    printf("Multiplication by 2\n");
    matrix_print(multi_by_numbder_A);
    Matrix multi_by_numbder_B = matrix_multiplying_by_number(B, 2);
    matrix_print(multi_by_numbder_B);

    Matrix transposed_A = matrix_trans(A);
    printf("Transposition\n");
    matrix_print(transposed_A);
    Matrix transposed_B = matrix_trans(B); 
    matrix_print(transposed_B);

    Matrix sub = matrix_submatrix(A, 2, 0);
    matrix_print(sub);

    double det_A = matrix_determinant(A);
    printf("Matrix determinant:%lf\n", det_A);

    Matrix power_A = matrix_power(A, 2);
    printf("Matrix powered to 2\n");
    matrix_print(power_A);

    Matrix inverse_A = matrix_inverse(A);
    printf("Matrix A inverse\n");
    matrix_print(inverse_A);

    Matrix exponent_A = matrix_exponent(A, 3);
    printf("Matrix A exponent\n");
    matrix_print(exponent_A);

    memory_free(&A);
    memory_free(&B);
    memory_free(&add_A_B);
    memory_free(&subtract_A_B);
    memory_free(&multi_A_B);
    memory_free(&multi_by_numbder_A);
    memory_free(&multi_by_numbder_B);
    memory_free(&transposed_A);
    memory_free(&transposed_B);
    memory_free(&sub);
    memory_free(&power_A);
    memory_free(&exponent_A);
    memory_free(&inverse_A);

}

Matrix Matrix_doubl_transp(const Matrix A, const Matrix B) {
    Matrix C = matrix_multiply(A, B);

    if (C.data == NULL) {
        return MATRIX_ZERO;
    }
	
    Matrix T = matrix_transpose(C);
    matrix_free(&C);
    printf("Transposition\n");
    matrix_print(transposed_T);

    return T;
}


int main()
{
    calculations();
    return 0;
}
