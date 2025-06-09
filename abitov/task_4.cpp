#include <iostream>
#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <cstdint>

enum Matrix_message_level {ERROR, WARNING, INFO, DEBUG};


void matrix_exception(const enum Matrix_message_level level,const char* msg)
{
    if(level == ERROR) {
        printf("\nERROR: %s\n", msg);
    }

    if(level == WARNING) {
        printf("\nWARNING: %s\n", msg);
    }
    
    if(level == INFO) {
        printf("\nINFO: %s\n", msg);
    }
        
    if(level == DEBUG) {
        printf("\nDEBUG: %s\n", msg);
    }
}

class MatrixException : public std::domain_error {
public:
    explicit MatrixException(const std::string& message) : domain_error(message) {} 
};

class Matrix 
{
private:
    size_t rows, cols;
    double* data;   
     
public:
    // Конструкторы
    Matrix();
    Matrix(size_t n); 
    Matrix(size_t r, size_t c);
    Matrix(const Matrix& M);
    Matrix(Matrix&& M) noexcept;
    ~Matrix();


    // Операторы
    Matrix& operator=(const Matrix& M);
    Matrix& operator=(Matrix&& M) noexcept;
    Matrix operator+(const Matrix& M) const;
    Matrix operator-(const Matrix& M) const;
    Matrix operator*(const Matrix& M);
    Matrix operator*=(const double count);

    // Функции
    void random();
    void print();
    Matrix unit();
    Matrix power(size_t exp);
    Matrix transp();
    Matrix exponent(size_t procedure);
    double determinant();
    
};
//Конструкторы
Matrix::Matrix() : rows(0), cols(0), data(nullptr){}

Matrix::Matrix(size_t n) : rows(n), cols(n) 
{
    if (n == 0) {
        data = nullptr;
        return;
    }

    if (n > SIZE_MAX / n) {     
        throw MatrixException("OVERFLOW: Переполнение выделенной памяти");
    }
    data = new double[n * n];
}


Matrix::Matrix(size_t r, size_t c): rows(r), cols(c) 
{

    if (rows == 0 || cols == 0) {
        matrix_exception(INFO, "Матрица распределена");
        throw MatrixException("Матрица содержит 0 столбцов или 0 строк");
        data = nullptr;
        return;
    }   

    if (rows > SIZE_MAX / cols) { 
        matrix_exception(ERROR, "Матрица распределена");
        rows = 0;
        cols = 0;
        data = nullptr;
        throw MatrixException("OVERFLOW: Переполнение количества элементов.");
    }

    if (cols * rows > SIZE_MAX / sizeof(double)) {
        matrix_exception(ERROR, "Матрица распределена");
        rows = 0;
        cols = 0;
        data = nullptr;
        throw MatrixException("OVERFLOW: Переполнение выделенной памяти");
    }

    data = new double[cols * rows]();
}

Matrix::Matrix(const Matrix& M) : rows(M.rows), cols(M.cols) 
{
    data = new double[rows * cols];
    std::copy(M.data, M.data + rows * cols, data);
}

Matrix::Matrix(Matrix&& M) noexcept : rows(M.rows), cols(M.cols), data(M.data) 
{
    M.rows = 0;
    M.cols = 0;
    M.data = nullptr;
}

Matrix::~Matrix() {
    rows = 0;
    cols = 0;
    delete[] data;
}

//Операторы
Matrix& Matrix::operator=(const Matrix& M) //Оператор копирования
{
    if (this == &M) return *this;
    delete[] data;
    rows = M.rows;
    cols = M.cols;
    data = new double[rows * cols];
    std::copy(M.data, M.data + rows * cols, data);
    return *this;
}

Matrix& Matrix::operator=(Matrix&& M) noexcept //Оператор перемещения
{
    if (this == &M) return *this;
    delete[] data;
    rows = M.rows;
    cols = M.cols;
    data = M.data;
    M.rows = 0;
    M.cols = 0;
    M.data = nullptr;
    return *this;
}

Matrix Matrix::operator+(const Matrix& B) const // C = A + B
{
    if ((rows != B.rows) || (cols != B.cols)) 
    {
        matrix_exception(ERROR, "C = A + B");
        throw MatrixException("Allocated memory is not the same");
    }
    Matrix C(rows, cols);   
    for(size_t index = 0; index < rows * cols; index++) {
        C.data[index] = data[index] + B.data[index];
    }

    return C;
}

Matrix Matrix::operator-(const Matrix& B) const // C = A - B
{
    if ((rows != B.rows) || (cols != B.cols)) 
    {
        matrix_exception(ERROR, "C = A - B");
        throw MatrixException("Allocated memory is not the same");
    }
    Matrix C(rows, cols);   
    for(size_t index = 0; index < rows * cols; index++) {
        C.data[index] = data[index] - B.data[index];
    }

    return C;
}

Matrix Matrix::operator*(const Matrix& B)// C = A * B
{
    if (cols != B.rows) 
    {
        matrix_exception(ERROR, "C = A * B");
        throw MatrixException("incorrect matrix size");
    }

    Matrix C(rows, B.cols);
       
    for(size_t row = 0; row < C.rows; row++) {
        for(size_t col = 0; col < C.cols; col++) {
            C.data[row * C.cols + col] = 0;
            for (size_t index = 0; index < cols; index++) {
                C.data[row * C.cols + col] += data[row * cols + index] * B.data[index * B.cols + col];
            }
        }
    }
    

    return C;
}

Matrix Matrix::operator*=(const double count) // B = A * count
{
    for(size_t index = 0; index < rows * cols; index++) {
        data[index] *= count;
    }

    return *this;
}

//Функции
void Matrix::random() // Функция рандомайзера
{
    for (size_t row = 0; row < rows; ++row) 
    {
        for (size_t col = 0; col < cols; ++col) 
        {
            data[row * cols + col] = rand()%10;
        }
    }
}

void Matrix::print() // Функция выведения
{
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            printf("%.2f ", data[row * cols + col]);
        }
        printf("\n");
    }
    printf("\n");
}
Matrix Matrix::unit() //Единичная матрица
{
    Matrix A(rows, cols);
    for ( size_t i = 0; i < cols; ++i) 
    {
        A.data[i * cols + i] = 1.0;
    }
    return A;
}

Matrix Matrix::power(size_t exp)// возведение в степень A^p
{
    if (cols != rows) {
       matrix_exception(ERROR, "C = A^p");
       throw MatrixException("incorrect matrix size");
    }
    Matrix A = unit();
    Matrix B = *this;
    for(unsigned int index = 0; index < exp; ++index) {
        A = A * B;
    } 

    return A;
}

Matrix Matrix::transp()// транспонирование матрицы A
{
    Matrix B(cols, rows);
    for (size_t row = 0; row < B.rows; row++) 
    {
        for (size_t col = 0; col < cols; col++) {
            B.data[col *B.cols + row] = data[row * cols + col];
        }
    }

    return B;
}
                                                                                                           
Matrix Matrix::exponent(size_t procedure)// экспонента матрицы A  
{
    if (rows != cols) {
        matrix_exception(ERROR, "C = A^e");
        throw MatrixException("incorrect matrix size");
    }
    
    Matrix C = unit();
    Matrix tmp = unit(); 

    if (procedure == 1) {
        return C;
    }

    for (size_t inx = 1; inx < procedure; ++inx)
    {
        tmp = tmp * (*this) *= (1.0 / inx);
        C = C + tmp;
    }
    return C;
}

double Matrix::determinant()// определитель матрицы A
{
    double det;
    if (rows != cols) 
    {
        matrix_exception(ERROR, "incorrect matrix size");
        return NAN;
    }

    if (rows == 0) 
    {
        matrix_exception(ERROR, "Matrix Zero");
        return NAN;
    }

    if (rows == 1) 
    {
        det = data[0];
        return det;
    }

    if (rows == 2) 
    {
        det = data[0] * data[3] - data[1] * data[2];
        return det;
    }

    if (rows == 3) 
    {
        det = data[0] * data[4] * data[8] 
        + data[1] * data[5] * data[6] 
        + data[3] * data[7] * data[2] 
        - data[2] * data[4] * data[6] 
        - data[3] * data[1] * data[8] 
        - data[0] * data[5] * data[7];
        return det;
    }

    return NAN;
}

int main() 
{
    Matrix A(3,3);
    Matrix B(3, 3);

    A.random();
    B.random();

    // Печать исходных матриц
    printf("Матрица A:\n");
    A.print();

    printf("Матрица B:\n");
    B.print();

    // Сложение
    Matrix C = A + B;
    printf("Результат сложения:\n");
    C.print();

    // Вычитание
    Matrix D = A - B;
    printf("Результат вычитания:\n");
    D.print();

    // Умножение
    Matrix E = A * B;
    printf("Результат умножения:\n");
    E.print();

    //Единичная матрица
    Matrix O = A.unit();
    printf("Единичная матрица A:\n");
    O.print();

    // Транспонирование
    Matrix T = A.transp();
    printf("Транспонированная матрица A:\n");
    T.print();

    // Возведение в степень
    double counter = 3;
    Matrix F = A.power(counter);
    printf("Матрица A в степени %.2f\n", counter);
    F.print();

    // Умножение на число
    double number = 5;
    Matrix G = A;
    G *= number;
    printf("Матрица A умноженная на: %.2f\n", number);
    G.print();
    
    // Определитель
    Matrix P = A;
    printf("Определитель матрицы A: %.2f\n", A.determinant());
    //Матричная экспонента
    Matrix exponent_A = P.exponent(3);
    printf("Матричная экспонента от A:\n");
    exponent_A.print();
}