#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include <cmath>


enum MatrixExceptionLevel {ERROR, WARNING, INFO, DEBUG};


void matrix_exception(const enum MatrixExceptionLevel level, const char *msg)
{
    if(level == ERROR) {
        printf("ERROR: %s", msg);
    }

    if(level == WARNING) {
        printf("WARNING: %s", msg);
    }

    if(level == INFO) {
        printf("INFO: %s", msg);
    }

    if(level == DEBUG) {
        printf("DEBUG: %s", msg);
    }
}


class Matrix {
private:
    size_t rows;
    size_t cols;
    double* data;

public:
    Matrix(size_t rows, size_t cols);
    Matrix(const Matrix& A);
    Matrix(Matrix&& A);

    ~Matrix();

    void matrix_print() const;
    void matrix_free();
    double determinant();
    void matrix_identity();
    void matrix_zero();

    Matrix matrix_transp();
    Matrix matrix_exp(const unsigned int accuracy);

    Matrix operator=(const Matrix& B);
    Matrix operator+(const Matrix& B);
    Matrix operator-(const Matrix& B);
    Matrix operator*(const double k);
    Matrix operator*(const Matrix& B);
    Matrix operator^(const unsigned int power);
};


Matrix::Matrix(size_t A_rows, size_t A_cols)
{
    rows = A_rows;
    cols = A_cols;

    if (rows == 0  || cols == 0) { 
        matrix_exception(WARNING, "Матрица содержит 0 элементов!\n");
    }

    if (SIZE_MAX / cols < rows) { 
        matrix_exception(ERROR, "Число строк или столбцов равно 0\n");
        rows = 0;
        cols = 0;
        data = NULL;
    }

    if (SIZE_MAX / (cols * sizeof(double)) < rows) { 
        matrix_exception(ERROR, "Не хватит места для выделения памяти под строки и столбцы\n");
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
    data = new double[rows * cols];
    std::copy(B.data, B.data + rows * cols, data);
}


Matrix::Matrix(Matrix&& other) : rows(other.rows), cols(other.cols), data(other.data)
{
    other.rows = 0;
    other.cols = 0;
    other.data = nullptr;
}


void Matrix::matrix_print() const
{
    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++) {
            printf("%.2f ", data[row * cols + col]);
        }
        printf("\n");
    }
    printf("\n");
}


void Matrix::matrix_free()
{ 
    free(data);
}


double Matrix::determinant()
{
    if (rows != cols) {
        matrix_exception(WARNING, "Размеры матрицы не подходят для вычисления определителя");
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
    matrix_exception(INFO, "Matrix is too big, I do not want to do it");
    return NAN;
}


void Matrix::matrix_identity()
{
    matrix_zero();

    for (size_t idx = 0; idx < rows * cols; idx += cols + 1) {
        data[idx] = 1.0;
    }
}


void Matrix::matrix_zero()
{
    if (cols == 0 || rows == 0) {
        matrix_exception(INFO, "Матрица не содержит элементов");
    }   

    memset(data, 0, cols * rows * sizeof(double));
}


Matrix Matrix::matrix_transp()
{
    Matrix B(cols, rows);

    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++) {
            B.data[col * rows + row] = data[row * cols + col];
        }
    }
    return B;
}


Matrix Matrix::matrix_exp(const unsigned int accuracy)
{
    if (rows != cols) {
        matrix_exception(WARNING, "Размеры матрицы не подходят для вычисления экспоненты");
        return MATRIX_NULL;
    }

    double fact = 1.0;

    Matrix B(*this);
    B.matrix_identity();

    for (int n = 1; n <= accuracy; n++) {
        fact *= n;
        Matrix P = *this ^ n;
        Matrix M = P * (1/fact);
        P.matrix_free();
        Matrix C = B + M;
        B.matrix_free();
        B = C;
        M.matrix_free();
    }

    return B;
}


Matrix Matrix::operator=(const Matrix& B)
{
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


Matrix Matrix::operator+(const Matrix& B)
{
    if (rows != B.rows || cols != B.cols) {
        matrix_exception(WARNING, "Размеры матриц не подходят для сложения");
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
        matrix_exception(WARNING, "Размеры матриц не подходят для вычитания");
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
        matrix_exception(WARNING, "Размеры матриц не подходят для умножения");
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
        matrix_exception(WARNING, "Размеры матрицы не подходят для возведения в степень");
        return MATRIX_NULL;
    }

    Matrix B(*this);
    
    for (unsigned int pow = 1; pow < power; ++pow) {
        B = std::move(B) * (*this);
    }

    return B;
}


int main()
{
    Matrix A(3,3);
    printf("A =\n");
    A.matrix_print();

    Matrix B(3,3);
    A = B;
    printf("A = B =\n");
    A.matrix_print();

    B = A * 2;
    printf("B = A *2 =\n");
    B.matrix_print();

    printf("det(A) = \n");
    printf("%.2f", A.determinant());

    Matrix C = B - A;
    printf("C = B - A\n");
    C.matrix_print();

    Matrix D(2,3);
    Matrix E(3,4);
    printf("D =\n");
    D.matrix_print();
    printf("E =\n");
    E.matrix_print();
   
    Matrix F = D * E;
    printf("F = D * E\n");
    F.matrix_print();

    Matrix G = F.matrix_transp();
    printf("G = transp(F)\n");
    G.matrix_print();

    printf("B =\n");
    B.matrix_print();
    Matrix J = B ^ 2;
    printf("J = B ^ 2\n");
    J.matrix_print();
    
    Matrix I = B.matrix_exp(4);
    printf("I = exp(B,4)\n");
    I.matrix_print();
}
