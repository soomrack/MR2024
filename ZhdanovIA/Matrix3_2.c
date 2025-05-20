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

    Matrix(Matrix &&A); // Конструктор перемещения (перехватывает ресурсы)

    
    ~Matrix() {  // Деструктор - освобождает память
        rows = 0; 
        cols = 0; 
        delete[] data;  
        printf("\ndestructor\n"); 
    };

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

// Класс для обработки исключений матрицы
class MatrixException : public std::domain_error {
public:
    MatrixException(const std::string message) : domain_error {message} { };
};

// Определения исключений
MatrixException MEMORY_ERROR("Memory allocation failed\n");
MatrixException DATA_ERROR("Matrix.data is nullptr\n");
MatrixException SIZE_ERROR("Matrixes of different sizes\n");
MatrixException SQUARE_ERROR("Matrix isn't square\n");
MatrixException DETERMINANT_ERROR("Determinant is NaN\n");
MatrixException DET_ZERO_ERROR("Determinant is zero\n");

Matrix::Matrix(const size_t rows, const size_t cols, const MatrixItem* src_data) : // Реализация конструктора с размерами и данными
    rows(rows), cols(cols) 
{
    printf("\ncreation constructor\n");
    if (cols == 0 || rows == 0) {
        printf("Warning: Initialized matrix with 0 rows/cols\n");
        return;
    };
    if (sizeof(MatrixItem) * rows * cols >= SIZE_MAX) {             // Проверка на переполнение памяти
        throw MEMORY_ERROR;
    };
    data = new MatrixItem[rows * cols];      // Выделение памяти и копирование данных
    memcpy(data, src_data, sizeof(MatrixItem) * rows * cols);
}

Matrix::Matrix(const size_t rows, const size_t cols) :  // Реализация конструктора с размерами
    rows(rows), cols(cols), data(nullptr)
{
    if (cols == 0 || rows == 0) {
        printf("Warning: Initialized matrix with 0 rows/cols\n");
        return;
    };

    if (sizeof(MatrixItem) * rows * cols >= SIZE_MAX) {    // Проверка на переполнение памяти
        throw MEMORY_ERROR;
    };

    data = new MatrixItem[rows * cols];  // Выделение памяти
}

Matrix::Matrix(const Matrix& A)  // Реализация конструктора копирования
{
    printf("\nCopy constructor\n");
    rows = A.rows;
    cols = A.cols;
    data = new MatrixItem[rows * cols];      // Выделение новой памяти и копирование данных
    memcpy(data, A.data, sizeof(MatrixItem) * rows * cols);
}

Matrix::Matrix(Matrix&& A) : rows(A.rows), cols(A.cols), data(A.data) // Реализация конструктора перемещения
{
    printf("\nMove constructor\n");
    A.data = nullptr;      // Обнуляем исходный объект
    A.cols = 0;
    A.rows = 0;
}

Matrix& Matrix::operator=(const Matrix& A)    // Реализация оператора присваивания (копирование)
{
    printf("\ncopy assignment\n");
    if (this == &A) {return *this;};     // Проверка на самоприсваивание

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

Matrix& Matrix::operator=(Matrix&& A)   // Реализация оператора присваивания (перемещение)
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

Matrix& Matrix::operator+=(const Matrix& A)  // Реализация оператора +=
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

    Matrix multy(rows, A.cols);   // Создание временной матрицы для результата
    for (size_t row = 0; row < multy.rows; row++) {
        for (size_t col = 0; col < multy.cols; col++) {
            for (size_t idx = 0; idx < cols; ++idx) {
                multy.data[row * multy.cols + col] += 
                    data[row * cols + idx] * A.data[idx * A.cols + col];
            }
        }
    }
    *this = multy;
    return *this;
}

Matrix& Matrix::operator*=(const double a)  // Реализация оператора *= (умножение на скаляр)
{
    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] *= a;
    }

    return *this;
}

Matrix& Matrix::operator-=(const Matrix& A)  // Реализация оператора -=
{
    if (rows != A.rows || cols != A.cols) {
        throw SIZE_ERROR;
    }

    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] -= A.data[idx];
    }

    return *this;
}

Matrix Matrix::operator-(const Matrix& A)   // Реализация оператора -
{
    Matrix sub(*this);  // Создаем копию
    sub -= A;          
    return sub;
}

Matrix Matrix::operator+(const Matrix& A)  // Реализация оператора +
{
    Matrix sum(*this);  
    sum += A;          
    return sum;
}

Matrix Matrix::operator*(const Matrix& A)  // Реализация оператора * (умножение матриц)
{
    Matrix mult(*this);  
    mult *= A;           
    return mult;
}

Matrix Matrix::operator*(const double a)  // Реализация оператора * (умножение на скаляр)
{
    Matrix mult(*this);  
    mult *= a;           
    return mult;
}

void Matrix::insert_arr(const MatrixItem* arr)    // Заполнение матрицы данными из массива
{
    if (data == nullptr || arr == nullptr) {
        throw DATA_ERROR;
    } 
    memcpy(data, arr, sizeof(MatrixItem) * rows * cols);
}

void Matrix::set_zeros()   // Заполнение матрицы нулями
{
    if (data == nullptr) {
        throw DATA_ERROR;
    }
    memset(data, 0, cols * rows * sizeof(MatrixItem));
}

void Matrix::ident()  // Реализация метода ident (единичная матрица)
{
    if (rows != cols) {    
        throw SQUARE_ERROR;
    }
    set_zeros();  
    for (size_t idx = 0; idx < rows * cols; idx += cols + idx) {     // Заполняем диагональ единицами
        data[idx] = 1;
    }
}

void Matrix::pow(const int n) // Реализация метода ident (единичная матрица)
{
    if (rows != cols) {
        throw SQUARE_ERROR;
    }
    
    if (n == 0) {    // Случай степени 0 - единичная матрица
        ident();
    }

    for (int i = 1; i < n; i++) {
        *this *= *this;
    }
}

Matrix Matrix::get_submatrix(const size_t row_to_exclude, const size_t col_to_exclude)  // Реализация метода get_submatrix (получение подматрицы)
{
    Matrix submatrix(rows - 1, cols - 1);   // Создаем подматрицу меньшего размера

    if (submatrix.data == nullptr) {
        throw MEMORY_ERROR;
    }

    size_t sub_row = 0;
    for (size_t row = 0; row < rows; row++) {
        if (row == row_to_exclude) continue;
        size_t sub_col = 0;
        for (size_t col = 0; col < cols; col++) {
            if (col == col_to_exclude) continue;
            submatrix.data[sub_row * submatrix.cols + sub_col] = data[row * cols + col];
            sub_col++;
        }
        sub_row++;  
    }
    return submatrix;
}

double Matrix::det()  // Реализация метода det (определитель)
{
    if (cols != rows) {
        throw SQUARE_ERROR;
    }

    if (cols == 1) {
        return data[0];
    }

    if (cols == 2) {
        return data[0] * data[3] - data[1] * data[2];
    }
    
    double det = 0.;  
    double sign = 1;
    for (size_t col = 0; col < cols; col++) {  // Рекурсивное вычисление через разложение по первой строке
        Matrix minor = this->get_submatrix(0, col);

        if (minor.data == NULL) {
            return NAN;
        }

        det += sign * data[col] * minor.det();
        sign *= -1;
    }
    return det;
}

void Matrix::transp()   // Реализация метода transp (транспонирование)
{
    Matrix B(cols, rows);      // Создаем временную матрицу с обратными размерами

    for (size_t row_this = 0; row_this < B.rows; row_this++) {       // Копируем элементы с транспонированием
        for (size_t col_this = 0; col_this < B.cols; col_this++) {
            B.data[rows * col_this + row_this] = data[col_this + row_this * cols];
        }
    }
    *this = B;  // Заменяем текущую матрицу транспонированной
}

Matrix Matrix::exp(int n)  // Реализация метода exp (экспонента матрицы)
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

    for (size_t idx = 1; idx <= n; idx++) {   // Вычисление ряда Тейлора
        factorial *= idx;
        to_calc_pow = matrix_exp_temp * *this;
        matrix_exp_temp = to_calc_pow;

        matrix_exp_temp *= 1 / factorial;

        matrix_exp_res += matrix_exp_temp;
    }

    return matrix_exp_res;
}

Matrix Matrix::inverse()  // Реализация метода inverse (обратная матрица)
{
    double det = this->det();

    if (det == NAN) {
        throw DETERMINANT_ERROR;
    }

    if (abs(det) < 0.00000001) {      // Проверка на невырожденность
        throw DET_ZERO_ERROR;
    }

    Matrix temp_matrix(rows, cols);    // Создаем матрицу алгебраических дополнений

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
    double data_A[9] = {0., 1., 2., 3., 9., 5., 6., 7., 8.};
    double data_B[9] = {8., 7., 6., 5., 4., 3., 2., 1., 0.};

    Matrix A(3, 3, data_A);

    Matrix B(3, 3, data_B);

    // Демонстрируем основные операции с матрицами

    Matrix SUM = A + B;

    Matrix SUB = A - B;

    Matrix MULT = A * B;

    Matrix MULT_SCAL = A * 3.0;
    
    Matrix TRAN = A;
    TRAN.transp();
    
    // Вычисляем обратную матрицу с обработкой возможных ошибок
    Matrix INV;
    try {
        INV = A.inverse();
    } catch (const MatrixException& e) {
        std::cout << "Ошибка при вычислении обратной матрицы: " << e.what() << std::endl;
    }
    
    // Вычисляем матричную экспоненту
    Matrix EXP = A.exp(3);
    
    // Вычисляем определитель
    double DET = A.det();

    // Выводим все результаты
    std::cout << "Исходная матрица A:\n";
    A.print();
    
    std::cout << "Исходная матрица B:\n";
    B.print();
    
    std::cout << "Исходная матрица C:\n";
    C.print();
    
    std::cout << "Сумма матриц (A + B):\n";
    SUM.print();
    
    std::cout << "Разность матриц (A - B):\n";
    SUB.print();
    
    std::cout << "Произведение матриц (A * B):\n";
    MULT.print();
    
    std::cout << "Умножение на скаляр (A * 3):\n";
    MULT_SCAL.print();
    
    std::cout << "Транспонированная матрица A:\n";
    TRAN.print();
    
    std::cout << "Обратная матрица A:\n";
    INV.print();
    
    std::cout << "Экспонента матрицы A:\n";
    EXP.print();
    
    return 0;
}