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
    Matrix& operator*=(const double number);      ///!!!
    Matrix operator^(unsigned int n);
//=
//=&& (A+B=C
};

//Конструкторы Matrix и деструктор
//!!!
Matrix::Matrix() : rows(0), cols (0), data(nullptr) {
	rows = 0;
	cols = 0;
	data = nullptr;
}

Matrix::Matrix(const size_t rows, const size_t cols) {
    if (rows == 0 || cols == 0) {
        throw std::runtime_error("Нулевая матрица.");
    }
    if (sizeof(MatrixItem) * rows * cols >= SIZE_MAX) {
        throw std::runtime_error("Ошибка размера матрицы.");
    };

    data = new MatrixItem[rows * cols];
}

Matrix::Matrix(const size_t rows, const size_t cols, const MatrixItem* value) {
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

        for (size_t i = 0; i < rows; i++) {
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
    Matrix& Matrix::operator*=(const double number) const
    {
        for (size_t i = 0; i < rows * cols; i++) {
        data[i] *= number;
    }

    return *this;
    }


//Транспонирование
    Matrix::matrix_trans() const          ///Возвращает матрицу!
    {
        Matrix T(cols, rows);

        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                T.data[j][i] = data[i][j];
            }
        }
        return T;
    }


//Получение подматрицы
    Matrix::matrix_submatrix(size_t row_exclude, size_t col_exclude) const         ///Возвращает матрицу!!!
    {
        Matrix submatrix(rows-1, cols-1);
        size_t submatrix_i = 0, submatrix_j = 0;

        for (size_t i = 0; i < rows; i++) {
            if (i == row_exclude) continue;
            submatrix_j = 0;
            for (size_t j = 0; j < cols; j++) {
                if (j == col_exclude) continue;
                submatrix.data[submatrix_i][submatrix_j] = data[i][j];
                submatrix_j++;
            }
            submatrix_i++;
        }
        return submatrix;
    }


//Определитель
    double Matrix::matrix_determinant() const
    {
        if ((cols != rows) || rows == 0 || cols == 0) {
            throw runtime_error("Не квадратная матрица.");
        }
        if (rows == 1) {
            return data[0][0];
        }
        if (rows == 2) {
            return data[0][0] * data[1][1] - data[0][1] * data[1][0]; 
        }

        double det = 0;
        double sign = 1;

        for (size_t j = 0; j < cols; j++) {

            Matrix submatrix = matrix_submatrix(0, j);

            det += sign * data[0][j] * submatrix.matrix_determinant();
            sign = -sign;
        }
        return det;
    }


//Единичная матрица
    static Matrix::matrix_identity(const size_t size) 
    {
        Matrix identity_matrix(size, size);

        for (size_t i = 0; i < size; ++i) {
            identity_matrix.data[i][i] = 1;
        }

        return identity_matrix;
    }


//Возведение в степень
    Matrix& Matrix::operator^(unsigned int n)
    {
        if (rows != cols) {
            throw runtime_error("Матрица не квадратная.");
        }

        if (n == 0) return matrix_identity(rows);                    //Нулевая степень

        Matrix matrix_powered_to_n = matrix_identity(rows);
        Matrix temp = *this;

        while (n > 0) {
            if (n % 2 == 1) {
                matrix_powered_to_n = matrix_powered_to_n * temp;
            }
            temp = temp * temp;
            n /= 2;
        }

        return matrix_powered_to_n;
    }


//Инверсия
    Matrix::matrix_inverse() const
    {
        double det = matrix_determinant();

        if (abs(det) < 0.00000001) {
            throw runtime_error("Определитель = 0.");
        }

        if (rows == 1) {                                               //1 на 1
            return Matrix(1, 1).matrix_fill({1.0 / data[0][0]});
        }

        Matrix additional_matrix(rows, cols);

        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                Matrix submatrix = matrix_submatrix(i, j);
                additional_matrix.data[i][j] = (i + j) % 2 == 0 ? submatrix.matrix_determinant() : -submatrix.matrix_determinant();
            }
        }

        Matrix inverse_matrix = additional_matrix * (1.0 / det);

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
    A.matrix_print();

    cprintf("Matrix B:\n");
    B.matrix_print();

    printf("Addition:\n");
    (A + B).matrix_print();

    printf("Subtraction:\n");
    (A - B).matrix_print();

    printf("Multiplication:\n");
    (A * B).matrix_print();

    printf("Multiplication by 2:\n");
    (A * 2).matrix_print();

    printf("Transpose A:\n");
    A.matrix_trans().matrix_print();

    printf("Determinant of A: ");
    A.matrix_determinant();

    printf("A ^ 2:\n");
    (A ^ 2).matrix_print();

    printf("Inverse of A:\n");
    A.matrix_inverse().matrix_print();

    printf("Matrix Exponential of A:\n");
    A.matrix_exponent().matrix_print();

}


int main()
{
    calculations();
    return 0;
}
