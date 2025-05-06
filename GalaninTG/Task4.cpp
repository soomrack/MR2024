#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include <cmath>
#include <cstdint>


class MatrixException : public std::domain_error 
{
    public:
        explicit MatrixException(const std::string& message) : domain_error(message) {} 
};


class Matrix {
private:
    size_t rows;
    size_t cols;
    double* data;

public:
    Matrix();
    Matrix(size_t rows, size_t cols);
    Matrix(size_t rows, size_t cols, double* data);
    Matrix(const Matrix& A);
    Matrix(Matrix&& A);

    ~Matrix();

    void print() const;
    double determinant();
    void identity();
    void zero();
    void fill_with_indices();

    Matrix transp();
    Matrix exp(const unsigned int accuracy);

    Matrix& operator=(const Matrix& B);
    Matrix& operator=(Matrix&& B);
    Matrix operator+(const Matrix& B);
    Matrix operator-(const Matrix& B);
    Matrix operator*(const double k);
    Matrix operator*(const Matrix& B);
    Matrix operator^(const unsigned int power);
};


Matrix::Matrix() {}


Matrix::Matrix(size_t A_rows, size_t A_cols, double* A_data)
{
    rows = A_rows;
    cols = A_cols;
    data = A_data;

    /*if (rows == 0  || cols == 0) {
        throw MatrixException ("Матрица содержит 0 элементов\n");
    }*/

    if (SIZE_MAX / cols < rows) { 
        throw MatrixException ("Число строк или столбцов равно 0\n");
        rows = 0;
        cols = 0;
        data = NULL;
    }

    if (SIZE_MAX / (cols * sizeof(double)) < rows) { 
        throw MatrixException ("Не хватит места для выделения памяти под строки и столбцы\n");
        rows = 0;
        cols = 0;
        data = NULL;
    }
}


Matrix::Matrix(size_t A_rows, size_t A_cols)
{
    rows = A_rows;
    cols = A_cols;

    /*if (rows == 0  || cols == 0) {
        throw MatrixException ("Матрица содержит 0 элементов\n");
    }*/

    if (SIZE_MAX / cols < rows) { 
        throw MatrixException ("Число строк или столбцов равно 0\n");
        rows = 0;
        cols = 0;
        data = NULL;
    }

    if (SIZE_MAX / (cols * sizeof(double)) < rows) { 
        throw MatrixException ("Не хватит места для выделения памяти под строки и столбцы\n");
        rows = 0;
        cols = 0;
        data = NULL;
    }

    data = new double[cols * rows];
}


const Matrix MATRIX_NULL(0, 0);


Matrix::~Matrix()
{
    delete[] data;
}


Matrix::Matrix(const Matrix& B) : rows(B.rows), cols(B.cols)
{
    printf("Constructor copy\n");
    data = new double[rows * cols];
    std::copy(B.data, B.data + rows * cols, data);
}


Matrix::Matrix(Matrix&& other) : rows(other.rows), cols(other.cols), data(other.data)
{
    printf("Constructor move\n");
    other.rows = 0;
    other.cols = 0;
    other.data = nullptr;
}


void Matrix::print() const
{
    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++) {
            printf("%.2f ", data[row * cols + col]);
        }
        printf("\n");
    }
    printf("\n");
}


double Matrix::determinant()
{
    if (rows != cols) {
        throw MatrixException("Размеры матрицы не подходят для вычисления определителя\n");
        return NAN;
    }

    if (rows == 1) {
        return data[0];
    }
    
    if (rows == 2) {
        return data[0] * data[3] - data[1] * data[2];
    }

    if (rows == 3) {
        return data[0] * data[4] * data[5] +
               data[1] * data[5] * data[6] +
               data[2] * data[3] * data[7] -
               data[2] * data[4] * data[6] -
               data[1] * data[3] * data[8] -
               data[0] * data[5] * data[7];
    }
    throw MatrixException("Matrix is too big\n");
    return NAN;
}


void Matrix::identity()
{
    zero();

    for (size_t idx = 0; idx < rows * cols; idx += cols + 1) {
        data[idx] = 1.0;
    }
}


void Matrix::zero()
{
    if (cols == 0 || rows == 0) {
        throw MatrixException("Матрица не содержит элементов\n");
    }   

    memset(data, 0, cols * rows * sizeof(double));
}


void Matrix::fill_with_indices()
{
    for (size_t row_i = 0; row_i < rows; ++row_i) {
        for (size_t col_j = 0; col_j < cols; ++col_j) {
            data[row_i * cols + col_j] = row_i * cols + col_j; 
        }
    }
}


Matrix Matrix::transp()
{
    Matrix B(cols, rows);

    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++) {
            B.data[col * rows + row] = data[row * cols + col];
        }
    }
    return B;
}


Matrix Matrix::exp(const unsigned int accuracy)
{
    if (rows != cols) {
        throw MatrixException ("Размеры матрицы не подходят для вычисления экспоненты");
        return MATRIX_NULL;
    }

    Matrix B(*this);
    B.identity();

    Matrix term(rows, cols);
    term.identity();

    double fact = 1.0;

    for (unsigned int n = 1; n <= accuracy; n++) {
        fact *= n;
        term = term * (*this) * (1.0 / fact);
        B = B + term;
    }

    return B;
}


Matrix &Matrix::operator=(const Matrix& B)
{
    printf("\nOperator copy");
    
    if (this == &B) {
        return *this;
    }

    if (cols != B.cols || rows != B.rows) {
    delete[] data;     
    rows = B.rows;
    cols = B.cols;
    data = new double[rows * cols]; 
    }

    memcpy(data, B.data, cols * rows * sizeof(double));
    return *this;
}


Matrix &Matrix::operator=(Matrix&& B)
{
    printf("\nOperator move");

    if (this == &B) {
        return *this;
    }
    
    delete[] data;
    rows = B.rows;
    cols = B.cols;
    data = B.data;

    B.rows = 0;
    B.cols = 0;
    B.data = nullptr;

    return *this;
}


Matrix Matrix::operator+(const Matrix& B)
{
    if (rows != B.rows || cols != B.cols) {
        throw MatrixException ("Размеры матриц не подходят для сложения");
        return MATRIX_NULL;
    }

    Matrix C (rows, cols);

    for (size_t idx = 0; idx < C.rows * C.cols; idx++) {
        C.data[idx] = data[idx] + B.data[idx];
    }

    return C;
}


Matrix Matrix::operator-(const Matrix& B)
{
    if (rows != B.rows || cols != B.cols) {
        throw MatrixException ("Размеры матриц не подходят для вычитания");
        return MATRIX_NULL;
    }

    Matrix C (rows, cols);

    for (size_t idx = 0; idx < C.rows * C.cols; idx++) {
        C.data[idx] = data[idx] - B.data[idx];
    }

    return C;
}


Matrix Matrix::operator*(const double k)
{
    Matrix B(rows, cols);

    for (size_t idx = 0; idx < cols * rows; idx++) {
        B.data[idx] = data[idx] * k;
    }

    return B;
}


Matrix Matrix::operator*(const Matrix& B)
{
    if (cols != B.rows) {
        throw MatrixException ("Размеры матриц не подходят для умножения");
        return MATRIX_NULL;
    }

    Matrix C(rows, B.cols);
    
    for (size_t row = 0; row < C.rows; row++) {
        for (size_t col = 0; col < C.cols; col++) {
            C.data[row * B.cols + col] = 0;
            for (size_t idx = 0; idx < cols; idx++) {
                C.data[row * C.cols + col] += data[row * cols + idx] * B.data[idx * B.cols + col];
            }
        }
    }

    return C;
}


Matrix Matrix::operator^(const unsigned int power)
{
    if (rows != cols) {
        throw MatrixException ("Размеры матрицы не подходят для возведения в степень");
        return MATRIX_NULL;
    }

    Matrix B(*this);

    if (power == 0) {
        B.identity();
        return B;
    }
    
    for (unsigned int pow = 1; pow < power; ++pow) {
        B = std::move(B) * (*this);
    }

    return B;
}


int main()
{
    double arr1[] = {5, 6, 7, 8}; 
    double arr2[] = {1, 2, 3, 4}; 

    Matrix A(2, 2, arr1); 
    Matrix B(2, 2, arr2); 

    printf("A = \n");
    A.print(); 

    printf("B = \n");
    B.print(); 

    Matrix C = A + B * A; 
    printf("C = A + B =\n");
    C.print(); 

    printf("D = A - B =\n");
    Matrix D = A - B; 
    D.print(); 

    printf("E = A * B =\n");
    Matrix E = A * B; 
    E.print(); 

    printf("F = A * 5 =\n"); 
    Matrix F = A * 5; 
    F.print(); 

    printf("transp(A) =\n");
    Matrix T = A.transp();
    T.print();

    printf("det(A) = %.2f\n", A.determinant()); 

    printf("exp(A) = \n");
    Matrix expA = A.exp(10);
    expA.print();

    printf("A ^ 3 = \n");
    Matrix S = A^3;
    S.print();

    return 0;
}
