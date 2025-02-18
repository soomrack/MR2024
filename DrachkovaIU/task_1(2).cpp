#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <cstring>


typedef double MatrixItem;


class Matrix{
private:
    size_t rows;
    size_t cols;
    double* data;
public:
    Matrix();
    Matrix(const size_t rows, const size_t cols);
    Matrix(const size_t rows, const size_t cols, const MatrixItem* data);
    Matrix(const Matrix &A);
    Matrix(Matrix &&A);
    ~Matrix();
public:
    void print();
    void matrix_fill(const double* values);
    void matrix_trans();
    void matrix_submatrix(size_t row_exclude, size_t col_exclude);
    void matrix_determinant();
    void matrix_inverse();
    void matrix_exponent(int n);
private:
    void matrix_identity(const size_t size);
public:
    Matrix& operator+=(const Matrix& B);
    Matrix& operator-=(const Matrix& B);
    Matrix& operator*=(const Matrix& B);
    Matrix& operator*=(const double number);
    Matrix& operator^(unsigned int n);
    Matrix& operator=(const Matrix& B);
    Matrix& operator=(Matrix&& B);
};

//Конструкторы Matrix и деструктор
Matrix::Matrix() : rows(0), cols (0), data(nullptr) {
	rows = 0;
	cols = 0;
	data = nullptr;
}

Matrix::Matrix(const size_t rows, const size_t cols) : rows(rows), cols(cols), data(nullptr) {
    if (rows == 0 || cols == 0) {
        throw std::runtime_error("Нулевая матрица.");
    }
    if (sizeof(MatrixItem) * rows * cols >= SIZE_MAX) {
        throw std::runtime_error("Ошибка размера матрицы.");
    };

    data = new MatrixItem[rows * cols];
}

Matrix::Matrix(const size_t rows, const size_t cols, const MatrixItem* value) : rows(rows), cols(cols) {
    if (data == nullptr)
		throw std::runtime_error("Нулевая матрица.");

    if (sizeof(MatrixItem) * rows * cols >= SIZE_MAX) {
        throw std::runtime_error("Ошибка размера матрицы.");
    };

	data = new MatrixItem[rows * cols];
    
    std::memcpy(data, value, rows * cols * sizeof(MatrixItem));
}

Matrix::Matrix(const Matrix &A) {
    rows = A.rows;
    cols = A.cols;
    data = new MatrixItem[rows * cols];
    memcpy(data, A.data, sizeof(MatrixItem) * rows * cols);
}

Matrix::Matrix(Matrix &&A) {
    rows = A.rows;
    cols = A.cols;
    data = A.data;

    A.rows = 0;
    A.cols = 0;
    A.data = nullptr;
}

Matrix::~Matrix() {
    rows = 0;
    cols = 0;
    delete[] data;
}


//Операции

Matrix& Matrix::operator=(const Matrix& B) 
{
    if (this == &B) {return *this;};
    
    if (cols == B.cols && rows == B.rows) {
        memcpy(data, B.data, sizeof(MatrixItem) * rows * cols);
        return *this;
    }

    delete[] data;
    rows = B.rows;
    cols = B.cols;
    data = new MatrixItem[rows * cols];
    memcpy(data, B.data, sizeof(MatrixItem) * rows * cols);
    return *this;
}

Matrix& Matrix::operator=(Matrix&& B)
{
    rows = B.rows;
    cols = B.cols;
    data = B.data;
    B.data = nullptr;
    B.rows = 0;
    B.cols = 0;
    return *this;
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

void Matrix::matrix_fill(const double* values) 
{
    if (!values) {
        throw std::runtime_error("Передан недопустимый массив значений.");
    }

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            data[i * cols + j] = values[i * cols + j];
        }
    }
}


//Сложение
    Matrix& Matrix::operator+=(const Matrix& B)
    {
        if (cols != B.cols || rows != B.rows) {
            throw std::runtime_error("Размеры матриц не совпадают.");
        }

        for (size_t i = 0; i < rows * cols; i++) {
            data[i] += B.data[i]; 
        }

        return *this;
    }

//Вычитание
    Matrix& Matrix::operator-=(const Matrix& B)
    {
        if (cols != B.cols || rows != B.rows) {
            throw std::runtime_error("Размеры матриц не совпадают.");
        }

        for (size_t i = 0; i < rows * cols; i++) {
            data[i] -= B.data[i];  
        }

        return *this;
    }


//Умножение
    Matrix& Matrix::operator*=(const Matrix& B)
    {
        if (cols != B.rows) {
            throw std::runtime_error("Число столбцов не соответствует числу строк.");
        }

        Matrix C(rows, B.cols);
        std::memset(C.data, 0 , sizeof(MatrixItem) * rows * B.cols);

        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < B.cols; j++) {
                for (size_t k = 0; k < cols; k++) {
                    C.data[i * C.cols + j] += data[i * cols + k] * B.data[k * B.cols + j];
                }
            }
        }

        *this = C;
        return *this;
    }


//Умножение на число
    Matrix& Matrix::operator*=(const double number)
    {
        for (size_t i = 0; i < rows * cols; i++) {
        data[i] *= number;
    }

    return *this;
    }


//Транспонирование
    void Matrix::matrix_trans()
    {
        Matrix T(cols, rows);

        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                T.data[j * rows + i] = data[i * cols + j];
            }
        }
        *this = T;
    }


//Получение подматрицы
    void Matrix::matrix_submatrix(size_t row_exclude, size_t col_exclude) 
    {
        Matrix submatrix(rows-1, cols-1);
        size_t submatrix_i = 0, submatrix_j = 0;

        for (size_t i = 0; i < rows; i++) {
            if (i == row_exclude) continue;
            submatrix_j = 0;
            for (size_t j = 0; j < cols; j++) {
                if (j == col_exclude) continue;
                submatrix.data[submatrix_i * (cols - 1) + submatrix_j] = data[i * cols + j];
                submatrix_j++;
            }
            submatrix_i++;
        }
        *this = submatrix;
    }


//Определитель
    double Matrix::matrix_determinant()
{
    if (rows != cols) {
        throw std::runtime_error("Матрица не квадратная.");
    }

    if (rows == 1) {
        return data[0];
    }

    if (rows == 2) {
        return data[0] * data[3] - data[1] * data[2];
    }

    double det = 0.0;
    double sign = 1.0;

    for (size_t j = 0; j < cols; j++) {
        Matrix submatrix(rows - 1, cols - 1);
        size_t sub_i = 0;

        for (size_t i = 1; i < rows; i++) {
            size_t sub_j = 0;
            for (size_t k = 0; k < cols; k++) {
                if (k == j) continue;
                submatrix.data[sub_i * (cols - 1) + sub_j] = data[i * cols + k];
                sub_j++;
            }
            sub_i++;
        }

        det += sign * data[j] * submatrix.matrix_determinant();
        sign = -sign;
    }

    return det;
}

//Единичная матрица
    Matrix& Matrix::matrix_identity(const size_t size) 
    {
        Matrix identity_matrix(size, size);

        for (size_t i = 0; i < size; ++i) {
            identity_matrix.data[i* size + i] = 1;
        }

        return identity_matrix;
    }


//Возведение в степень
    Matrix& Matrix::operator^(unsigned int n)
    {
        if (rows != cols) {
            throw std::runtime_error("Матрица не квадратная.");
        }

        *this = matrix_identity(rows);
        if (n == 0) return *this;                    //Нулевая степень

        Matrix result = matrix_identity(rows);
        Matrix temp = *this;

        while (n > 0) {
            if (n % 2 == 1) {
                result *= temp;
            }
            temp *= temp;
            n /= 2;
        }

        *this = result;
        return *this;
    }


//Инверсия
    Matrix Matrix::matrix_inverse() const
{
    double det = matrix_determinant();
    if (fabs(det) < 0.000000001) {
        throw std::runtime_error("Определитель = 0, матрица не обратима.");
    }

    if (rows == 1) {
        Matrix inv(1, 1);
        inv.data[0] = 1.0 / data[0];
        return inv;
    }

    Matrix additional_matrix(rows, cols);
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            Matrix submatrix = *this;
            submatrix.matrix_submatrix(i, j);
            additional_matrix.data[i * rows + j] = ((i + j) % 2 == 0 ? 1 : -1) * submatrix.matrix_determinant();
        }
    }

    Matrix inverse_matrix = additional_matrix.matrix_trans();
    inverse_matrix *= (1.0 / det);

    return inverse_matrix;
}


//Экспонента
    Matrix::matrix_exponent(int n) const
    {
        if (rows != cols) {
            throw runtime_error("Матрица не квадратная.");
        }

        Matrix result = matrix_identity(rows);
        Matrix temp(rows, cols);
        Matrix temp2(rows, cols);
        Matrix exp_result = matrix_identity(rows);
        double factorial = 1.0;

        for (int i = 1; i <= n; i++) {
            temp = *this ^ i;  // temp = A^i
            factorial *= i; 
            temp2 = temp * (1.0 / factorial);  // temp2 = (A^i) / i!
            exp_result = exp_result + temp2;  // exp_result += temp2
        }

        return exp_result;
    }

//Итоговые вычисления
void calculations ()
{
    Matrix A(2, 2);
    Matrix B(2, 2);

    A.matrix_fill({1, 2, 3, 4});
    B.matrix_fill({5, 6, 7, 8});

    printf("Matrix A:\n");
    A.print();

    printf("Matrix B:\n");
    B.print();

    printf("Addition:\n");
    (A + B).print();

    printf("Subtraction:\n");
    (A - B).print();

    printf("Multiplication:\n");
    (A * B).print();

    printf("Multiplication by 2:\n");
    (A * 2).print();

    printf("Transpose A:\n");
    A.matrix_trans().print();

    printf("Determinant of A: ");
    A.matrix_determinant();

    printf("A ^ 2:\n");
    (A ^ 2).print();

    printf("Inverse of A:\n");
    A.matrix_inverse().print();

    printf("Matrix Exponential of A:\n");
    A.matrix_exponent().print();

}


int main()
{
    calculations();
    return 0;
}
