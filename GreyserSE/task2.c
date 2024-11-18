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


Matrix matrix_init(const size_t rows, const size_t cols)
{
    if (rows == 0 || cols == 0) {
        return MATRIX_NULL;    
    }

    if (rows >= SIZE_MAX / sizeof(MatrixItem) / cols) {
		return MATRIX_NULL;
    }
    
    Matrix A;
    A.rows = rows; 
    A.cols = cols;
    A.data = (MatrixItem*) malloc(sizeof(MatrixItem) * A.rows * A.cols);
    
    if(A.data == NULL) {
		return MATRIX_NULL;
	}

    return A;
}


int matrix_free(Matrix* matrix)
{
	if (matrix->data == NULL)
		return 0;

	free(matrix->data);
	*matrix = MATRIX_NULL;
	return 0;
}


int matrix_fill(Matrix A, const MatrixItem array[])
{
    if (A.rows == 0) {
        return 1;    
    }
    
    memcpy(A.data, array, A.cols * A.rows * sizeof(MatrixItem));
    return 0;
}


// Заполняет матрицу случайными значениями от 0 до 99
int matrix_fill_random(Matrix A)
{
    if (A.data == NULL) {
        return 1;    
    }

    for (int idx = 0; idx < A.cols * A.rows; ++idx) {
        A.data[idx] = (double) (rand() % 100);
    }
    return 0;
}


// B = A
int matrix_copy(Matrix B, const Matrix A) 
{
    if ((A.cols != B.cols) || (A.rows != B.rows)) {
        return 1;
    }

    memcpy(B.data, A.data, A.cols * A.rows * sizeof(double));
    return 0;
}


int matrix_set_zeros(Matrix A)
{
    if (A.data == NULL) {
        return 1;    
    }

    memset(A.data, 0, A.cols * A.rows * sizeof(double));
    return 0;
}


Matrix matrix_ident(size_t rows, size_t cols)
{
    Matrix A = matrix_init(rows, cols);

    if (A.data == NULL) {
        return MATRIX_NULL;
    }   


    if (A.rows != A.cols) {
        return MATRIX_NULL;
    } 

    matrix_set_zeros(A);  

    for (int row = 0; row < A.rows; row++)
    {
        A.data[row * A.rows + row] = 1.;
    }
    return A;
}


// A += B
int matrix_add(Matrix A, const Matrix B)
{
   if (A.rows != B.rows || A.cols != B.cols) {
        return 1;
    }

    if (A.data == NULL || B.data == NULL)
		return 1;

    for (size_t idx = 0; idx < A.cols * A.rows; idx++)
    {
        A.data[idx] += B.data[idx];
    }
    return 0; 
}


//  C = A + B
Matrix matrix_sum(const Matrix A, const Matrix B)
{
    if (A.rows != B.rows || A.cols != B.cols) {
        return MATRIX_NULL;
    } 

    if (A.data == NULL || B.data == NULL)
		return MATRIX_NULL;
    
    Matrix C = matrix_init(A.rows, A.cols);

    for (int i = 0; i < A.rows * A.cols; i++) {
        C.data[i] = A.data[i] + B.data[i];
    return C;  
    }
}

// C = A - B
Matrix matrix_sub(const Matrix A, const Matrix B)
{
    if (A.cols != B.cols || A.rows != B.rows)
		return MATRIX_NULL;

	Matrix C = matrix_init(A.cols, A.rows);
	
    if (C.data == NULL)
		return C;

	for (size_t idx = 0; idx < A.cols * A.rows; ++idx)
		C.data[idx] = A.data[idx] - B.data[idx];
	return C;
}


// A *= coeff
int matrix_coeff_multi(Matrix A, const double coeff)
{
	if (A.data == NULL)
		return 1;

	for (size_t idx = 0; idx < A.cols * A.rows; ++idx)
		A.data[idx] *= coeff;

	return 0;
}


// C = A * B
Matrix matrix_multi(const Matrix A, const Matrix B)
{
	if (A.cols != B.rows)
		return MATRIX_NULL;

	Matrix C = matrix_init(A.cols, A.rows);

	if (C.data == NULL)
	{
		return MATRIX_NULL;
	}

	for (size_t rowA = 0; rowA < A.rows; ++rowA) {
		for (size_t colB = 0; colB < B.cols; ++colB) {
			C.data[rowA * A.cols + colB] = 0;
			for (size_t idx = 0; idx < A.cols; ++idx) {
				C.data[rowA * A.cols + colB] += A.data[rowA * A.cols + idx] * B.data[idx * B.cols + colB];
			}
		}
	}
	return C;
}


// B = A^T
Matrix matrix_transp(const Matrix A)
{
	Matrix B = matrix_init(A.cols, A.rows);

	if (B.data == NULL) {
		return MATRIX_NULL;
	}

	for (size_t rowA = 0; rowA < A.rows; ++rowA) {
		for (size_t colA = 0; colA < A.cols; ++colA) {
			B.data[A.rows * colA + rowA] = A.data[colA + rowA * A.cols];
		}
	}
	return B;
}


Matrix matrix_get_submatrix(Matrix A, size_t row_to_exclude, size_t col_to_exclude)
{
    Matrix submatrix = matrix_init(A.rows - 1, A.cols - 1);

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
		return NAN;
    }

	if (A.cols == 1)
	{
		return A.data[0];
	}

	if (A.cols == 2)
	{
		double det = (A.data[0]) * (A.data[3]) - (A.data[1]) * (A.data[2]);
		return det;
	}
	
    double det = 0.;
    double sign = 1;
    for (size_t col = 0; col < A.cols; col++)
    {
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
Matrix matrix_pow(const Matrix A, const size_t n) 
{
    if (A.cols != A.rows) {
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
    }

    return to_power_on_n;
}


// B = exp(A * n)
Matrix matrix_exp(const Matrix A, const size_t n)
{
    if (A.rows != A.cols) {
        return MATRIX_NULL;
    }

    Matrix matrix_exp_res = matrix_ident(A.rows, A.cols);
    
    if (n == 0) {
        return matrix_exp_res;
    }

    Matrix matrix_exp_temp = matrix_ident(A.rows, A.cols);
    double factorial = 1.;

    for (size_t idx = 1; idx <= n; idx++) {
        factorial *= idx;
        Matrix to_calc_pow = matrix_multi(matrix_exp_temp, A);
        matrix_free(&matrix_exp_temp);
        matrix_exp_temp = to_calc_pow;

        double coeff_to_multy = (n ^ idx) / factorial;
        matrix_coeff_multi(matrix_exp_temp, coeff_to_multy);

        matrix_add(matrix_exp_res, matrix_exp_temp);
    }
    return matrix_exp_res;
}


// B = A^-1
Matrix matrix_inverse(Matrix A)
{
    double det_A = matrix_det(A);

    if (abs(det_A) < 0.00000001) {
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
