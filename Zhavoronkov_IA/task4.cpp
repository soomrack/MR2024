#include <iostream>
using namespace std;
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <stdexcept>


enum MatrixExceptionLevel {ERROR, WARNING, INFO, DEBUG};

// Сообщение об ошибке
void matrix_error(const enum MatrixExceptionLevel level, const char* location)
{
    if (level == ERROR) {
        printf("\nERROR\nLoc: %s\n", location);
    }

    if (level == WARNING) {
        printf("\nWARNING\nLoc: %s\n", location);
    }

    if (level == INFO) {
        printf("\nINFO\nLoc: %s\n", location);
    }

    if (level == DEBUG) {
        printf("\nDEBUG\nLoc: %s\n", location);
    }
}


class Matrix {
private:
    size_t rows;
    size_t cols;
    double* data;    
public:
    Matrix(size_t des_rows, size_t des_cols) 
    {
        rows = des_rows;
        cols = des_cols;

        if (cols == 0 || rows == 0) {
            matrix_error(INFO, "matrix_alloc");
            printf("Matrix is ix0 or 0xj\n");
            data = nullptr;
        }   

        if (rows > SIZE_MAX / cols) { // Проверка на переполнение cols * rows
            matrix_error(ERROR, "matrix_alloc");
            rows = 0;
            cols = 0;
            data = nullptr;
            throw std::runtime_error("Cols * rows exceeds SIZE_MAX");
        }

        if (cols * rows > SIZE_MAX / sizeof(double)) { // Проверка на общий объем памяти
            matrix_error(ERROR, "matrix_alloc");
            rows = 0;
            cols = 0;
            data = nullptr;
            throw std::runtime_error("Memory allocation size exceeds SIZE_MAX");
        }

        data = new double[cols * rows]();

        if (data == nullptr) {
            matrix_error(ERROR, "matrix_alloc");
            rows = 0;
            cols = 0;
            throw std::runtime_error("No memory allocated");
        }
    }

    ~Matrix() 
    {
        if (data) {
            delete[] data;
        }
    }

public: // Сервисные методы (функции)
    void print() // Вывод матрицы
    {
        printf("\n_____________________________\n");
        for (size_t row_i = 0; row_i < rows; ++row_i) {
            for (size_t col_j = 0; col_j < cols; ++col_j) {
                printf("%.2f ", data[row_i * cols + col_j]);
            }
            printf("\n");
        }
        printf("\n");
    }

    void set_zero() // Заполнение матрицы нулями
    {
        if (data == nullptr) {
            matrix_error(ERROR, "set_zero");
            throw std::runtime_error("No memory allocated");
        }
        
        memset(data, 0, cols * rows * sizeof(double));
    }

    void set_identity() // Превращение матрицы в "единичную" по диагонали
    {   
        if (data == nullptr) {
            matrix_error(ERROR, "set_identity");
            throw std::runtime_error("No memory allocated");
        }
    
        set_zero();
        for (size_t idx = 0; idx < rows * cols; idx += cols + 1) {
            data[idx] = 1.0;
        }
    }

    void fillWithIndices() // Заполнение матрицы индексами элементов
    {
        if (data == nullptr) {
            matrix_error(ERROR, "fillWithIndices");
            throw std::runtime_error("No memory allocated");
        }

        for (size_t row_i = 0; row_i < rows; ++row_i) {
               for (size_t col_j = 0; col_j < cols; ++col_j) {
                data[row_i * cols + col_j] = row_i * cols + col_j; 
            }
        }
    }

    void set_value(const unsigned int idx, const double value) // Заполнение элемента матрицы по индексу
    {
        if (data == nullptr) {
            matrix_error(ERROR, "set_value");
            throw std::runtime_error("No memory allocated");
        }

        if (idx >= cols * rows) {
            matrix_error(ERROR, "set_value");
            throw std::runtime_error("The entered index exceeds the maximum for this matrix");
        }

        data[idx] = value;
    }

    double determinant() // Определитель
    {
        double det;
        if (rows != cols) {
            printf("A non-square matrix\n");
            return NAN;
        }

        if (rows == 0) {
            return NAN;
        }

        if (rows == 1) {
            det = data[0];
            return det;
        }

        if (rows == 2) {
            det = data[0] * data[3] - data[1] * data[2];
            return det;
        }

        if (rows == 3) {
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

    Matrix transp() // B = A ^ T
    {
        if (data == nullptr) {
            matrix_error(ERROR, "transp");
            throw std::runtime_error("No memory allocated");
        }

        Matrix result (cols, rows);

        for (size_t row = 0; row < rows; ++row) {
            for (size_t col = 0; col < cols; ++col) {
                result.data[row + rows * col] = data[col + cols * row];
            }
        }

        return result;
    }

    Matrix exp(const unsigned int targ_num) // E = e ^ A
    {
        if (rows != cols) {
            matrix_error(ERROR, "matrix_pow");
            throw std::runtime_error("A non-square matrix");
        }

        if (data == nullptr) {
            matrix_error(ERROR, "exp");
            throw std::runtime_error("No memory allocated");
        }

        Matrix E (cols,rows);
        E.set_identity();

        if (targ_num == 1) {
            return E;
        }

        Matrix tmp (rows, cols);
        Matrix buf_mult (rows, cols);

        tmp = E;

        for (size_t num = 1; num < targ_num; ++num) { 

            buf_mult = tmp * *this;
            buf_mult * (1.0/num);

            E += buf_mult;
            tmp = buf_mult;
        }
        return E;
    }


public: // Переопределение операторов
    double& operator[](unsigned int idx)
    {
        if (data == nullptr) {
            matrix_error(ERROR, "matrix_idx");
            throw std::runtime_error("No memory allocated for matrix");
        }

        if (idx >= cols * rows) {
            matrix_error(ERROR, "matrix_idx");
            throw std::runtime_error("The entered index exceeds the maximum for this matrix");
        }

        return data[idx];
    }

    Matrix& operator=(const Matrix& B) // A := B
    {
        if ((cols != B.cols) || (rows != B.rows )) {
            matrix_error(ERROR, "matrix_copy");
            throw std::runtime_error("Matrices of different dimensions");
        }
        
        if (data == nullptr || B.data == nullptr) {
            matrix_error(ERROR, "matrix_copy");
            throw std::runtime_error("No memory allocated");
        }

        memcpy(data, B.data, cols * rows * sizeof(double));
        return *this;
    }

    Matrix& operator+=(const Matrix& B) // A += B
    {
        if ((cols != B.cols) || (rows != B.rows )) {
            matrix_error(ERROR, "A = A + B");
            throw std::runtime_error("Matrices of different dimensions");
        }

        if (data == nullptr || B.data == nullptr) {
            matrix_error(ERROR, "A += B");
            throw std::runtime_error("No memory allocated");
        }
    
        for (size_t idx = 0; idx < cols * rows; ++idx) {
            data[idx] += B.data[idx];
        }

        return *this;
    }

    Matrix& operator*(const double k) // A *= k
    {
        if (data == nullptr) {
            matrix_error(ERROR, "matrix_coef");
            throw std::runtime_error("No memory allocated");
        }

        for (size_t idx = 0; idx < cols * rows; idx++) {
            data[idx] *= k;
        }
        return *this;
    }

    Matrix operator+(const Matrix& B) // C = A + B
    {
        if ((cols != B.cols) || (rows != B.rows))
        {
            matrix_error(ERROR, "C = A + B");
            throw std::runtime_error("Matrices of different dimensions");
        }

        if (data == nullptr || B.data == nullptr) {
            matrix_error(ERROR, "C = A + B");
            throw std::runtime_error("No memory allocated");
        }

        Matrix result (rows, cols);

        result = B;
        result += *this;

        return result;
    }

    Matrix operator-(const Matrix& B) // C = A - B
    {
        if ((cols != B.cols) || (rows != B.rows))
        {
            matrix_error(ERROR, "C = A - B");
            throw std::runtime_error("Matrices of different dimensions");
        }

        if (data == nullptr || B.data == nullptr) {
            matrix_error(ERROR, "C = A - B");
            throw std::runtime_error("No memory allocated");
        }

        Matrix result (rows, cols);

        for (size_t idx = 0; idx < cols * rows; ++idx) {
            result.data[idx] = data[idx] - B.data[idx];
        }

        return result;
    }

    Matrix operator*(const Matrix& B) // C = A * B
    {
        if (cols != B.rows)
        {
            matrix_error(ERROR, "C = A * B");
            throw std::runtime_error("Incorrect matrix size");
        }

        if (data == nullptr || B.data == nullptr) {
            matrix_error(ERROR, "C = A * B");
            throw std::runtime_error("No memory allocated");
        }

        Matrix result (rows, B.cols);

        for (size_t row = 0; row < result.rows; ++row) {
            for (size_t col = 0; col < result.cols; ++col) {
                result.data[row * (result.cols) + col] = 0;
                    
                for (size_t idx = 0; idx < cols; idx++) {
                    result.data[row * result.cols + col] += data[row * cols + idx] * B.data[idx * B.cols + col];
                }
            }
        }

        return result;
    }

    Matrix operator^(const unsigned int targ_power) // B = A ^ n
    {
        if (data == nullptr) {
            matrix_error(ERROR, "B = A ^ n");
            throw std::runtime_error("No memory allocated");
        }

        if (rows != cols) {
            matrix_error(WARNING, "matrix_pow");
            throw std::runtime_error("A non-square matrix");
        }

        Matrix result (rows, cols);

        if (targ_power == 0) {
            result.set_identity();
            return result;
        }

        result = *this;

        for (unsigned int pow = 1; pow < targ_power; ++pow) {
            result = result * (*this);
        }
        return result;
    }

};


int main()
{
    double res;
    Matrix A(3,3);
    A.print();
    Matrix B(3,3);
    B.fillWithIndices();
    A = B;
    A.print();
    A * 2;
    A.print();
    res = A[2];
    printf("%.2f\n", res);
    A.set_value(0, 2.3);
    A.print();
    printf("%.2f", A.determinant());
    Matrix C = B - A;
    A.print();
    B.print();
    C.print();

    Matrix D(2,3);
    D.fillWithIndices();
    Matrix E(3,4);
    E.fillWithIndices();
    D.print();
    E.print();
    printf("above");
    Matrix F = D * E;
    F.print();
    printf("end");
    Matrix G = F.transp();
    G.print();

    printf("Matrix_pow test\n");
    B.print();
    Matrix J = B ^ 2;
    J.print();

    printf("Matrix_exp test\n");
    B.print();
    Matrix I = B.exp(4);
    I.print();

    Matrix Z (0,5);

    printf("\n------------\nEnd of main\n------------\n");
}
