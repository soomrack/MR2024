#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>


typedef double MatrixItem; 


typedef struct {
    size_t rows;
    size_t cols;
    MatrixItem *data;
} Matrix;


const Matrix MATRIX_NULL = {0, 0, NULL};

enum MatrixException {ERROR, WARNING, INFO, DEBUG};
typedef enum MatrixException MatrixException;


void matrix_exception(const MatrixException level, char *msg)
{
    if(level == ERROR) {
        printf("ERROR: %s", msg);
    }

    if(level == WARNING) {
        printf("WARNING: %s", msg);
    }
}


Matrix matrix_init(const size_t rows, const size_t cols)
{
    if (rows == 0 || cols == 0) {
        matrix_exception(WARNING, "Initialized matrix with 0 rows/cols \n");
        return (Matrix) {rows, cols, NULL};   
    }

    if (rows >= SIZE_MAX / sizeof(MatrixItem) / cols) {
		matrix_exception(ERROR, "Memory allocation failed \n");
        return MATRIX_NULL;
    }
    
    Matrix A;
    A.rows = rows; 
    A.cols = cols;
    A.data = (MatrixItem*)malloc(sizeof(MatrixItem) * A.rows * A.cols);
    
    if(A.data == NULL) {
        matrix_exception(ERROR, "Memory allocation failed \n");
		return MATRIX_NULL;
	}

    return A;
}


void matrix_free(Matrix* matrix)
{
	
    free(matrix->data);
	*matrix = MATRIX_NULL;
}


void matrix_fill(const Matrix A, const MatrixItem array[])
{   
    memcpy(A.data, array, A.cols * A.rows * sizeof(MatrixItem));
}


// Заполняет матрицу случайными значениями от 0 до 99
void matrix_fill_random(const Matrix A)
{
    for (size_t idx = 0; idx < A.cols * A.rows; ++idx) {
        A.data[idx] = (double) (rand() % 100);
    }
}


// B = A
void matrix_copy(const Matrix B, const Matrix A) 
{
    if ((A.cols != B.cols) || (A.rows != B.rows)) {
        matrix_exception(ERROR, "Unable to copy matrixes with different size \n");
        return;
    }

    if (A.data == NULL) {
        matrix_exception(ERROR, "Unable to copy: Data of source matrix is NULL \n");
        return;
    }

    memcpy(B.data, A.data, A.cols * A.rows * sizeof(double));
}


void matrix_set_zeros(const Matrix A)
{
    if (A.data == NULL) {
        return;    
    }

    memset(A.data, 0, A.cols * A.rows * sizeof(double));
}


Matrix matrix_ident(size_t rows, size_t cols)
{
    if (rows != cols) {
        matrix_exception(ERROR, "Unable to make identity matrix: rows is not equal to cols \n");
        return MATRIX_NULL;
    } 
    
    Matrix A = matrix_init(rows, cols); 

    matrix_set_zeros(A);  

    for (size_t idx = 0; idx < A.rows * A.cols; idx += A.cols + 1) {
        A.data[idx] = 1.;
    }
    return A;
}


// A += B
void matrix_add(Matrix A, const Matrix B)
{
   if (A.rows != B.rows || A.cols != B.cols) {
        matrix_exception(ERROR, "Unable to add: Matrixes of different sizes \n");
        return;
    }

    if (A.data == NULL || B.data == NULL) {
        matrix_exception(ERROR, "Unable to add: Matrix data is NULL \n");
		return;
    }

    for (size_t idx = 0; idx < A.cols * A.rows; idx++) {
        A.data[idx] += B.data[idx];
    }
}


//  C = A + B
Matrix matrix_sum(const Matrix A, const Matrix B)
{
    if (A.rows != B.rows || A.cols != B.cols) {
        matrix_exception(ERROR, "Unable to sum: Matrixes of different sizes \n");
        return MATRIX_NULL;
    } 

    if (A.data == NULL || B.data == NULL) {
		matrix_exception(ERROR, "Unable to sum: Matrix data is NULL \n");
        return MATRIX_NULL;
    }

    Matrix C = matrix_init(A.rows, A.cols);

    for (size_t idx = 0; idx < C.rows * C.cols; idx++) {
        C.data[idx] = A.data[idx] + B.data[idx];  
    }
    return C;
}

// C = A - B
Matrix matrix_sub(const Matrix A, const Matrix B)
{
    if (A.cols != B.cols || A.rows != B.rows) {
		matrix_exception(ERROR, "Unable to sub: Matrixes of different sizes \n");
        return MATRIX_NULL;
    }

	if (A.data == NULL || B.data == NULL) {
		matrix_exception(ERROR, "Unable to sub: Matrix data is NULL \n");
        return MATRIX_NULL;
    }

    Matrix C = matrix_init(A.cols, A.rows);

	for (size_t idx = 0; idx < C.cols * C.rows; ++idx) {
		C.data[idx] = A.data[idx] - B.data[idx];
    }
	return C;
}


// A *= coeff
void matrix_coeff_multi(const Matrix A, const double coeff)
{
	if (A.data == NULL) {
		matrix_exception(ERROR, "Unable to coeff_multi: Matrix data is NULL \n");
        return;
    }

	for (size_t idx = 0; idx < A.cols * A.rows; ++idx)
		A.data[idx] *= coeff;
}


// C = A * B
Matrix matrix_multi(const Matrix A, const Matrix B)
{
	if (A.cols != B.rows) {
        matrix_exception(ERROR, "Unable to multi: Matrix A cols is not equal Matrix B rows \n");
		return MATRIX_NULL;
    }

	Matrix C = matrix_init(A.cols, B.rows);

	for (size_t row = 0; row < C.rows; ++row) {
		for (size_t col = 0; col < C.cols; ++col) {
			C.data[row * C.cols + col] = 0;
			for (size_t idx = 0; idx < A.cols; ++idx) {
				C.data[row * C.cols + col] += A.data[row * A.cols + idx] * B.data[idx * B.cols + col];
			}
		}
	}
	return C;
}


// B = A^T
Matrix matrix_transp(const Matrix A)
{
	if (A.data == NULL) {
        matrix_exception(ERROR, "Unable to transpond: Matrix data is NULL \n");
		return MATRIX_NULL;
	}

    Matrix B = matrix_init(A.cols, A.rows);

	for (size_t rowA = 0; rowA < B.rows; ++rowA) {
		for (size_t colA = 0; colA < B.cols; ++colA) {
			B.data[A.rows * colA + rowA] = A.data[colA + rowA * A.cols];
		}
	}
	return B;
}


Matrix matrix_get_submatrix(Matrix A, size_t row_to_exclude, size_t col_to_exclude)
{
    Matrix submatrix = matrix_init(A.rows - 1, A.cols - 1);

    if (submatrix.data == NULL) {
        matrix_exception(ERROR, "Unable to get submatrix: Matrix data is NULL \n");
		return MATRIX_NULL;
	}

    size_t sub_row = 0;
    for (size_t row = 0; row < A.rows; row++) {
        if (row == row_to_exclude) continue;
        size_t sub_col = 0;
        for (size_t col = 0; col < A.cols; col++)
        {
            if (col == col_to_exclude) continue;
            submatrix.data[sub_row * submatrix.cols + sub_col] = A.data[row * A.cols + col];
            sub_col++;
        }
        sub_row++;  
    }
    return submatrix;
}


// determinant
double matrix_det(const Matrix A)
{
	if (A.cols != A.rows) {
        matrix_exception(ERROR, "Unable to get determinant: Matrix is not square \n");
        return NAN;
    }

	if (A.cols == 1) {
		return A.data[0];
	}

	if (A.cols == 2) {
		double det = (A.data[0]) * (A.data[3]) - (A.data[1]) * (A.data[2]);
		return det;
	}
	
    double det = 0.;
    double sign = 1;
    for (size_t col = 0; col < A.cols; col++) {
        Matrix minor = matrix_get_submatrix(A, 0, col);

        if (minor.data == NULL) {
            return NAN;
        }

        det += sign * A.data[col] * matrix_det(minor);
        sign *= -1;
        matrix_free(&minor); 
    }
    return det;
}


// B = A^n
Matrix matrix_pow(const Matrix A, const unsigned long n) 
{
    if (A.cols != A.rows) {
        matrix_exception(ERROR, "Unable to power: Matrix is not square \n");
		return MATRIX_NULL;
    }

    if (n == 0) {
        return matrix_ident(A.rows, A.cols);
    }

    Matrix to_power_on_n = matrix_init(A.rows, A.cols);
    matrix_copy(to_power_on_n, A);

    for (size_t pow = 1; pow < n; pow++) {
        Matrix temp = matrix_multi(to_power_on_n, A);
        matrix_free(&to_power_on_n);
        to_power_on_n = temp;
        matrix_free(&temp);
    }

    return to_power_on_n;
}


// B = exp(A * n)
Matrix matrix_exp(const Matrix A, const size_t n)
{
    if (A.rows != A.cols) {
        matrix_exception(ERROR, "Unable to exp: Matrix is not square \n");
        return MATRIX_NULL;
    }

    Matrix matrix_exp_res = matrix_ident(A.rows, A.cols);
    
    if (n == 0) {
        return matrix_exp_res;
    }

    Matrix matrix_exp_temp = matrix_ident(A.rows, A.cols);
    Matrix to_calc_pow = matrix_ident(A.rows, A.cols);
    double factorial = 1.;

    for (size_t idx = 1; idx <= n; idx++) {
        factorial *= idx;
        to_calc_pow = matrix_multi(matrix_exp_temp, A);
        matrix_free(&matrix_exp_temp);
        matrix_exp_temp = to_calc_pow;

        double coeff_to_multy = (n ^ idx) / factorial;
        matrix_coeff_multi(matrix_exp_temp, coeff_to_multy);

        matrix_add(matrix_exp_res, matrix_exp_temp);
    }
    matrix_free(&to_calc_pow);
    matrix_free(&matrix_exp_temp);
    return matrix_exp_res;
}


// B = A^-1
Matrix matrix_inverse(Matrix A)
{
    double det_A = matrix_det(A);

    if (det_A == NAN) {
        matrix_exception(ERROR, "Unable to inverse: determinant is NAN");
        return MATRIX_NULL;
    }

    if (abs(det_A) < 0.00000001) {
        matrix_exception(ERROR, "Unable to inverse: determinant is equal to 0 \n");
        return MATRIX_NULL;
    }

    Matrix cofactor_matrix = matrix_init(A.rows, A.cols);

    for (size_t row = 0; row < A.rows; row++) {
        for (size_t col = 0; col < A.cols; col++) {

            Matrix submatrix = matrix_get_submatrix(A, row, col);

            cofactor_matrix.data[row * cofactor_matrix.cols + col] = ((row + col) % 2 == 0 ? 1 : -1) * matrix_det(submatrix);

            matrix_free(&submatrix);
        }
    }

    Matrix transp_cofactor_matrix = matrix_transp(cofactor_matrix);
    matrix_free(&cofactor_matrix);

    matrix_coeff_multi(transp_cofactor_matrix, 1 / det_A);
    return transp_cofactor_matrix;
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


void calculations()
{
    Matrix A = matrix_init(3, 3);
    matrix_fill_random(A);

    printf("Матрица, заполненная случайными значениями \n");
    matrix_print(A);
    Matrix B = matrix_init(3, 3);
    matrix_copy(B, A);

    printf("Скопированная матрица \n");
    matrix_print(B);

    B = matrix_pow(A, 2);
    printf("Матрица в квадрате \n");
    matrix_print(B);
    
    double det = matrix_det(A);
    printf("Определитель матрицы \n");
    printf("%.f \n", det);
    printf("\n");
    
    Matrix exp = matrix_exp(B, 3);
    printf("Экспонента матрицы \n");
    matrix_print(exp);
}


int main() 
{
    calculations();
    return 0;
}
