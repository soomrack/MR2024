#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <cstring>

typedef double MatrixItem;

class Matrix {
private:
    size_t rows;
    size_t cols;
    double *data;
public:
    Matrix() : rows(0), cols(0), data(nullptr) {};
    Matrix(const size_t rows, const size_t cols);
    Matrix(const size_t rows, const size_t cols, const MatrixItem* data);
    Matrix(const Matrix &A);
    Matrix(Matrix &&A);
    ~Matrix() { rows = 0; cols = 0; delete[] data; printf("\ndestructor\n"); };
public:
    Matrix& operator=(const Matrix& A);
    Matrix& operator=(Matrix&& A); 
    Matrix operator+(const Matrix& A);
    Matrix& operator+=(const Matrix& A);
    Matrix operator-(const Matrix& A);
    Matrix operator*(const Matrix& A);
    Matrix operator*(const double a);
    Matrix& operator*=(const Matrix& A);
    Matrix& operator*=(const double a);
    Matrix& operator-=(const Matrix& A);
public:
    void insert_arr(const MatrixItem* arr);
    void set_zeros();
    void pow(const int n);
    void ident();
    void transp();
    double det();
    Matrix exp(const int n);
    Matrix inverse();
    void print();
private:
    Matrix get_submatrix(const size_t row_to_exclude, const size_t col_to_exclude);
};


class MatrixException : public std::domain_error
{
public:
    MatrixException(const std::string message) : domain_error {message} { };
};


MatrixException MEMORY_ERROR("Memory allocation failed\n");
MatrixException DATA_ERROR("Matrix.data is nullptr\n");
MatrixException SIZE_ERROR("Matrixes of different sizes\n");
MatrixException SQUARE_ERROR("Matrix isn't square\n");
MatrixException DETERMINANT_ERROR("Determinant is NaN\n");
MatrixException DET_ZERO_ERROR("Determinant is zero\n");


Matrix::Matrix(const size_t rows, const size_t cols, const MatrixItem* src_data) :
    rows(rows), cols(cols)
{
    printf("\ncreation constructor\n");
    if (cols == 0 || rows == 0) {
        printf("Warning: Initialized matrix with 0 rows/cols\n");
        return;
    };
    
    if (sizeof(MatrixItem) * rows * cols >= SIZE_MAX) {
        throw MEMORY_ERROR;
    };

    
    data = new MatrixItem[rows * cols];
    memcpy(data, src_data, sizeof(MatrixItem) * rows * cols);
}


Matrix::Matrix(const size_t rows, const size_t cols) :
    rows(rows), cols(cols), data(nullptr)
{
    if (cols == 0 || rows == 0) {
        printf("Warning: Initialized matrix with 0 rows/cols\n");
	return;
    };

    if (sizeof(MatrixItem) * rows * cols >= SIZE_MAX) {
        throw MEMORY_ERROR;
    };

    data = new MatrixItem[rows * cols];
}


Matrix::Matrix(const Matrix& A) 
{
    printf("\nCopy constructor\n");
    rows = A.rows;
    cols = A.cols;
    data = new MatrixItem[rows * cols];
    memcpy(data, A.data, sizeof(MatrixItem) * rows * cols);
}


Matrix::Matrix(Matrix&& A) : rows(A.rows), cols(A.cols), data(A.data) 
{
    printf("\nMove constructor\n");
    A.data = nullptr;
    A.cols = 0;
    A.rows = 0;
}


Matrix& Matrix::operator=(const Matrix& A) 
{
    printf("\ncopy assignment\n");
    if (this == &A) {return *this;};
    
    if (cols == A.cols && rows == A.rows) {
        memcpy(data, A.data, sizeof(MatrixItem) * rows * cols);
        return *this;
    }

    delete[] data;
    rows = A.rows;
    cols = A.cols;
    data = new MatrixItem[rows * cols];
    memcpy(data, A.data, sizeof(MatrixItem) * rows * cols);
    return *this;
}


Matrix& Matrix::operator=(Matrix&& A)
{
    printf("\nmove assignment\n");
    
    rows = A.rows;
    cols = A.cols;
    data = A.data;
    A.data = nullptr;
    A.rows = 0;
    A.cols = 0;
    return *this;
}


Matrix& Matrix::operator+=(const Matrix& A)
{
    if (rows != A.rows || cols != A.cols) {
        throw SIZE_ERROR;
    }

    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] += A.data[idx];
    }

    return *this;
}


Matrix& Matrix::operator*=(const Matrix& A)
{
    if (cols != A.rows) {
        throw SIZE_ERROR;
    }

    Matrix multy(rows, A.cols);
    for (size_t row = 0; row < multy.rows; row++) {
		for (size_t col = 0; col < multy.cols; col++) {
			for (size_t idx = 0; idx < cols; ++idx) {
				multy.data[row * multy.cols + col] += data[row * cols + idx] * A.data[idx * A.cols + col];
			}
		}
	}

    *this = multy;
    return *this;
}


Matrix& Matrix::operator*=(const double a)
{
    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] *= a;
    }

    return *this;
}


Matrix& Matrix::operator-=(const Matrix& A)
{
    if (rows != A.rows || cols != A.cols) {
        throw SIZE_ERROR;
    }

    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] -= A.data[idx];
    }

    return *this;
}


Matrix Matrix::operator-(const Matrix& A)
{
    Matrix sub(*this);
    sub -= A;
    return sub;
}


Matrix Matrix::operator+(const Matrix& A)
{
    Matrix sum(*this);
    sum += A;
    return sum;
}


Matrix Matrix::operator*(const Matrix& A)
{
    Matrix mult(*this);
    mult *= A;
    return mult;
}


Matrix Matrix::operator*(const double a)
{
    Matrix mult(*this);
    mult *= a;
    return mult;
}


void Matrix::insert_arr(const MatrixItem* arr)
{
    if (data == nullptr || arr == nullptr) {
        throw DATA_ERROR;
    } 

    memcpy(data, arr, sizeof(MatrixItem) * rows * cols);
}


void Matrix::set_zeros()
{
    if (data == nullptr) {
        throw DATA_ERROR;
    }
    
    memset(data, 0, cols * rows * sizeof(MatrixItem));
}


void Matrix::ident()
{
    if (rows != cols) {
        throw SQUARE_ERROR;
    }

    set_zeros();

    for (size_t idx = 0; idx < rows * cols; idx += cols + idx) {
        data[idx] = 1;
    }
}


void Matrix::pow(const int n)
{
    if (rows != cols) {
        throw SQUARE_ERROR;
    }
    
    if (n == 0) {
        ident();
    }

    for (int i = 1; i < n; i++) {
        *this *= *this;
    }
}


Matrix Matrix::get_submatrix(const size_t row_to_exclude, const size_t col_to_exclude)
{
    Matrix submatrix(rows - 1, cols - 1);

    if (submatrix.data == nullptr) {
        throw MEMORY_ERROR;
	}

    size_t sub_row = 0;
    for (size_t row = 0; row < rows; row++) {
        if (row == row_to_exclude) continue;
        size_t sub_col = 0;
        for (size_t col = 0; col < cols; col++)
        {
            if (col == col_to_exclude) continue;
            submatrix.data[sub_row * submatrix.cols + sub_col] = data[row * cols + col];
            sub_col++;
        }
        sub_row++;  
    }
    return submatrix;
}


double Matrix::det()
{
    if (cols != rows) {
        throw SQUARE_ERROR;
    }

	if (cols == 1) {
		return data[0];
	}

	if (cols == 2) {
		double det = (data[0]) * (data[3]) - (data[1]) * (data[2]);
		return det;
	}
	
    double det = 0.;
    double sign = 1;
    for (size_t col = 0; col < cols; col++) {
        Matrix minor = this->get_submatrix(0, col);

        if (minor.data == NULL) {
            return NAN;
        }

        det += sign * data[col] * minor.det();
        sign *= -1;
    }
    return det;
}


void Matrix::transp()
{
    Matrix B(cols, rows);

	for (size_t row = 0; row < B.rows; row++) {
		for (size_t col = 0; col < B.cols; col++) {
			B.data[rows * col + row] = data[col + row * cols];
		}
	}
	*this = B;
}


Matrix Matrix::exp(int n)
{
    if (rows != cols) {
        throw SQUARE_ERROR;
    }

    Matrix matrix_exp_res(rows, cols);
    matrix_exp_res.ident();
    
    if (n == 0) {
        return matrix_exp_res;
    }

    Matrix matrix_exp_temp = matrix_exp_res;
    Matrix to_calc_pow = matrix_exp_res;
    double factorial = 1.;

    for (size_t idx = 1; idx <= n; idx++) {
        factorial *= idx;
        to_calc_pow = matrix_exp_temp * *this;
        matrix_exp_temp = to_calc_pow;

        matrix_exp_temp *= 1 / factorial;

        matrix_exp_res += matrix_exp_temp;
    }

    return matrix_exp_res;
}


Matrix Matrix::inverse()
{
    double det = this->det();

    if (det == NAN) {
        throw DETERMINANT_ERROR;
    }

    if (abs(det) < 0.00000001) {
        throw DET_ZERO_ERROR;
    }

    Matrix temp_matrix(rows, cols);

    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++) {
            Matrix submatrix = this->get_submatrix(row, col);
            temp_matrix.data[row * temp_matrix.cols + col] = 
                ((row + col) % 2 == 0 ? 1 : -1) * submatrix.det();
        }
    }

    temp_matrix.transp();
    temp_matrix *= 1 / det;

    return temp_matrix;
}


void Matrix::print()
{
    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++)
            printf("%.f ", data[row * cols + col]);
        printf("\n");
    } 
    printf("\n");
}


int main() 
{
    double src_data[9] = {0., 1., 2., 3., 4., 5., 6., 7., 8.};
    Matrix A(3, 3, src_data);

    A.print();
    Matrix B = A * A;
    B = Matrix(3, 2);
}