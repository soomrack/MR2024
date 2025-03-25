#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <cstring>
#include <math.h>
#include <cstdlib>

using namespace std;
typedef double elem;

class Matrix_exception: public std::exception
{
    private:
        std::string message;
        public:
        Matrix_exception(std::string message): message(message) {};
    
};

class Matrix 
{
    private:
        size_t rows;
        size_t cols;
    
        elem* data;

    public:
        Matrix();  // конструктор по умолчанию
        Matrix(size_t rows, size_t cols);  //конструктор с значениями
        Matrix(Matrix& other);  //кноструктор копий
        Matrix(Matrix&& other);
        ~Matrix();

    public:
        Matrix operator+ (const Matrix& other);  // перегрузка оператора сложения
        Matrix operator- (const Matrix& other);  // перегрузка оператора вычитания
        Matrix operator* (const Matrix& other);  // умножение матрицы на матрицу
        Matrix& operator= (const Matrix& other);  // перегрузка оператора присваивания
        Matrix operator/(const elem k);
        Matrix& operator-= (const Matrix& other);
        Matrix& operator+= (const Matrix& other);
        Matrix& operator/=(const elem k);
        Matrix& operator=(Matrix&& other) noexcept;

    public:  

        void set_zero();
        void set_one();
        void print();
        Matrix& transp();
        double matrix_det();
        Matrix exp(const Matrix& other);
       
};

Matrix::Matrix() : rows(0), cols(0), data(nullptr) {
    rows = 0;
	cols = 0;
	data = nullptr;
}

Matrix::Matrix(const size_t rows, const size_t cols):
rows(rows), cols(cols), data(nullptr)
{
    if (cols == 0 || rows == 0) {
        throw Matrix_exception("null matrix");
    };

    if (rows >= SIZE_MAX / sizeof(elem) / cols){  
        throw Matrix_exception("Size");
    }

    data = new elem[cols * rows];
}

Matrix::Matrix(Matrix& other):
rows(other.rows), cols(other.cols), data(nullptr)
{
    if (!other.data)
    {
        data = nullptr;
        return;
    }

    data = new elem[rows * cols];

    std::memcpy(data, other.data, rows * cols * sizeof(elem));
}

Matrix::Matrix(Matrix&& other):
rows(other.rows), cols(other.cols), data(nullptr)
{
    other.rows = 0;
    other.cols = 0;
    other.data = nullptr;
}

Matrix::~Matrix() {
    delete[] data;
}

void Matrix::set_zero() {
    if (data == nullptr) return;
    std::memset(data, 0, sizeof(elem) * rows * cols);
}

void Matrix::set_one()
{
    set_zero();

    for (size_t idx = 0; idx < rows * cols; idx += cols + 1)
        data[idx] = 1.0;
}


void Matrix::print()
{
    if (data == nullptr) throw Matrix_exception("Error");
   
    for (size_t row = 0; row < rows; ++row) 
    {
        for (size_t col = 0; col < cols; ++col) {
            printf("%2.3f ", data[row * cols + col]);
        }
    }
}


Matrix& Matrix::operator=(const Matrix& other)
{
    if (&other == this) return *this;

    cols = other.cols;
    rows = other.rows;

    if (other.data == nullptr){
        data = nullptr;
        return *this;
    }
        
    delete[] data;

    data = new elem[rows * cols];

    memcpy(this->data, other.data, rows * cols * sizeof(elem));

    return *this;
}

Matrix& Matrix::operator=(Matrix&& other) noexcept
{
    delete[] data;
    rows = other.rows;
    cols = other.cols;
    data = other.data;
    other.rows = 0;
    other.cols = 0;
    other.data = nullptr;
    return *this;
};

// B += A
Matrix& Matrix::operator+=(const Matrix& other) 
{
    if (rows != other.rows || cols != other.cols) 
    {
        throw Matrix_exception("Wrong size");
    }

    for (size_t idx = 0; idx < other.cols * other.rows; ++idx)
        data[idx] += other.data[idx];

    return *this;
}


// S = B + A
Matrix Matrix::operator+(const Matrix& other) 
{
    if (other.cols != cols || other.rows != rows)
    {
        throw Matrix_exception("Wrong size");
    }  

    Matrix s = Matrix(*this);
    s += other;         
  
    return s;
}

//C = B - A
Matrix Matrix::operator-(const Matrix& other) 
{
    if (other.cols != cols || other.rows != rows)
    {
        throw Matrix_exception("Wrong size");
    }  

    Matrix v = Matrix(*this);
    v -= other;    
    return v;
}

// C -= B 
Matrix& Matrix::operator-=(const Matrix& other) 
{
    if (rows != other.rows || cols != other.cols) 
    {
        throw Matrix_exception("Wrong size");
    }

    for (size_t idx = 0; idx < other.cols * other.rows; ++idx)
        data[idx] -= other.data[idx];

    return *this;
}

// C = B * A
Matrix Matrix::operator*(const Matrix& other)
{
    if (rows != other.rows || cols != other.cols) 
    {
        throw Matrix_exception("Wrong size");
    }

    Matrix v = Matrix(*this);

    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < other.cols; ++col){
            v.data[row * cols + col] = 0;
            for (size_t k = 0; k < other.cols; k++) {
                 v.data[row * cols + col] 
                 += other.data[row * other.cols + k] * data[k * cols + col];
            }
        }
    }
    return v;
}

// A /= k
Matrix& Matrix::operator/=(const elem k)
{
    for (size_t idx = 0; idx < cols * rows; ++idx)
        data[idx] = data[idx] / k;
    return *this;
}

// C = A/k
Matrix Matrix::operator/(const elem k)
{
    Matrix v = Matrix(*this);

    for (size_t idx = 0; idx < cols * rows; ++idx)
        v.data[idx] = data[idx] / k;
    return v;
}

// A^T
Matrix& Matrix::transp()
{
    Matrix* trans = new Matrix(rows, cols);
    
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            trans->data[col * cols + row] 
            = data[row * cols + col];
        }
    }

    return *trans;
}

// det (A)
double Matrix::matrix_det()
{

    if (rows != cols) {
        throw Matrix_exception("error");
    }

    double det = 0.0;
    if (rows == 1) {
        det = data[0];
    }

    if (rows == 2) {
        det = data[0] * data[3] - data[1] * data[2];
    }

    if (rows == 3) {
        det = data[0] * data[4] * data[8]
            + data[1] * data[5] * data[6]
            + data[2] * data[7] * data[3]
            - data[2] * data[4] * data[6]
            - data[0] * data[5] * data[7]
            - data[1] * data[8] * data[3];
    }

    else throw Matrix_exception("For matrix 1x1,2x2,3x3");

    return det;
}

//exp
Matrix Matrix::exp(const Matrix& other)
{
    if (rows != cols) {
        throw Matrix_exception("Error");
    }

    if (other.cols == 0)
    throw Matrix_exception("error");

    Matrix exp(*this);
    exp.set_one();

    Matrix temp(*this);

    for (int idx = 1; idx <= 80; idx++) {
       temp = temp * (*this) / idx;
       exp += temp;
    }
    return exp;
}

int main() {

     return 0;
}