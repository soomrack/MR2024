#include <iostream>
#include <cstring>
#include <cmath>
#include <stdexcept>


class MatrixException : public std::domain_error {
    public:
        explicit MatrixException(const std::string& message) : std::domain_error(message) {} 
};
    
const MatrixException MEMORY_ERROR("Memory allocation error");
const MatrixException SIZE_ERROR("Matrices have different sizes");
const MatrixException MULTIPLY_ERROR("Number of columns in A does not match number of rows in B");
const MatrixException EMP_ERROR("Matrix is empty");
const MatrixException SQR_ERROR("Determinant can only be calculated for square matrices");
const MatrixException POWER_ERROR("Matrix must be square to raise to a power");


class Matrix {
private:
    size_t rows;
    size_t cols;
    double* data;

    void check_non_empty() const 
    {
        if (rows == 0 || cols == 0 || data == nullptr){
            throw EMP_ERROR;
        }
    }


public:
    // Конструкторы
    Matrix();
    Matrix(const Matrix& M);
    Matrix(Matrix&& M);
    Matrix(const size_t n);
    Matrix(const size_t row, const size_t col);
    ~Matrix();

    // Операторы
    Matrix& operator= (Matrix&& M);
    Matrix& operator= (const Matrix& M);
    
    Matrix operator+ (const Matrix& M) const;
    Matrix operator- (const Matrix& M) const;
    Matrix operator* (const Matrix& M) const;
    Matrix operator* (const double k) const;

    // Функции
    void enter_data(); // Ввод значений в матрицу
    void set_identity(); // Создание единичной матрицы
    void print() const; // Вывод значений матрицы
    double determinant() const; // Нахождение определителя матрицы
    Matrix power(size_t n) const; // Возведение матрицы в степень
    Matrix transpose() const; // Транспонирование матрицы
    Matrix exp(size_t steps) const; // Нахождение экспоненты матрицы
};


Matrix::Matrix(): rows(0), cols(0), data(nullptr) {}


Matrix::Matrix(const Matrix& M): rows(M.rows), cols(M.cols), data(nullptr)
{
     
    if (rows == 0 || cols == 0) return;

    data = new double[rows * cols];
    memcpy(data, M.data, rows * cols * sizeof(double));

}


Matrix::Matrix(Matrix&& M): rows(M.rows), cols(M.cols), data(M.data)
{
    M.rows = 0;
    M.cols = 0;
    M.data = nullptr;
}


Matrix::Matrix(const size_t n): rows(n), cols(n), data(nullptr)
{
    if(n == 0) return;
    data = new double[n * n]; 
}


Matrix::Matrix(const size_t row, const size_t col): rows(row), cols(col), data(nullptr)
{
    if (rows == 0 || cols == 0) return;
    data = new  double[row * col];
}


Matrix::~Matrix()
{
    delete[] data;
}


// Операторы
Matrix& Matrix::operator=(const Matrix& M) 
{
    if (this == &M) return *this;

    delete[] data;
    rows = M.rows;
    cols = M.cols;
    if (rows == 0 || cols == 0) return;
    data = new double[rows * cols];
    memcpy(data, M.data, cols * rows * sizeof(double));

    return *this;
}


Matrix& Matrix::operator=(Matrix&& M) 
{
    if (this == &M) return *this;

    delete[] data;

    rows = M.rows;
    cols = M.cols;
    data = M.data;

    M.cols = 0;
    M.rows = 0;
    M.data = nullptr;

    return *this;
}


Matrix Matrix::operator+(const Matrix& M) const
{
    check_non_empty();
    M.check_non_empty();

    if ((rows != M.rows) || (cols != M.cols)) {
        throw SIZE_ERROR;
    }
    Matrix result(rows, cols);
    for (size_t i = 0; i < rows * cols; ++i) {
        result.data[i] = data[i] + M.data[i];
    }
    return result;
}


Matrix Matrix::operator-(const Matrix& M) const
{
    check_non_empty();
    M.check_non_empty();

    if ((rows != M.rows) || (cols != M.cols)) {
        throw SIZE_ERROR;
    }
    Matrix result(rows, cols);
    for (size_t i = 0; i < rows * cols; ++i) {
        result.data[i] = data[i] - M.data[i];
    }
    return result;
}


Matrix Matrix::operator*(const double k) const
{
    check_non_empty();
    
    Matrix result(rows, cols);
    for (size_t i = 0; i < rows * cols; ++i) {
        result.data[i] = data[i] * k;
    }
    return result;
}


Matrix Matrix::operator*(const Matrix& M) const 
{
    check_non_empty();
    M.check_non_empty();

    if ((cols != M.rows)) {
        throw MULTIPLY_ERROR;
    }
    
    Matrix result(rows, M.cols);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < M.cols; ++j) {
            double sum = 0;
            for (size_t k = 0; k < cols; ++k) {
                sum += data[i*cols + k] * M.data[k * M.cols + j];
            } 
            result.data[i * result.cols + j] = sum;
        }
    }
    return result;
}

// Функции
void Matrix::enter_data()
{
    check_non_empty();

    for(size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++) {
            std::cin >> data[row * cols + col];
        }
    }
}


void Matrix::set_identity()
{
    check_non_empty();

    memset(data, 0, rows * cols * sizeof(double));
    for (size_t i = 0; i < rows; i++) {
        data[i * cols + i] = 1.0;
    }
}


void Matrix::print() const 
{
    check_non_empty();

    for(size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++) {
            printf("%.2f\t", data[row * cols + col]);
        }
        printf("\n");
    }
}


double Matrix::determinant() const
{ 
    check_non_empty();

    if(rows != cols) {
        throw SQR_ERROR;
    }

    Matrix temp = *this;
    double result = 1;
    for (size_t i = 0; i < cols; i++) { 
        for (size_t row = i + 1; row < rows; row++) { // Приводим матрицу к треугольному виду
            double proportion = (double)temp.data[row * cols + i] / temp.data[i * cols + i]; // Считаем коэффициент
            for (size_t col = i; col < cols; col++) { 
                // Вычитаем из всей строки i-ую строку * коэффициент, чтобы обнулить значения i-ого элемента строки
                temp.data[row * cols + col] -= temp.data[i * cols + col] * proportion;
            }
        }
        result *= temp.data[i * cols + i]; // Умножаем число на i-ый элемент главное диагонали 
    }

    return result;
}


Matrix Matrix::power(size_t n) const 
{
    check_non_empty();

    if (rows != cols) {
        throw POWER_ERROR;
    }

    Matrix result(rows, cols);
    result.set_identity();
    Matrix temp = *this;

    for(int i = 1; i < n; i++) {
        result = result * temp;
    }

    return result;
}


Matrix Matrix::transpose() const 
{
    check_non_empty();

    Matrix result(cols, rows);
    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++) {
            result.data[col * rows + row] = data[row * cols + col];
        }
    }
    return result;
}

Matrix Matrix::exp(size_t n) const 
{
    if (rows != cols) {
        throw SQR_ERROR;
    }
    
    Matrix result(rows, cols);
    result.set_identity();

    Matrix temp = result;
    for (size_t i = 1; i <= n; i++) {
        temp = temp * (*this) * (1.0 / tgamma(i + 1));
        result = result + temp;
    }
    return result;
}

int main() {

    printf("Matrix A\n");
    size_t rows, cols;
    printf("Enter row count: ");
    scanf("%zu", &rows);
    printf("Enter colums count: ");
    scanf("%zu", &cols);
    Matrix A(rows, cols);
    printf("Enter data: \n");
    A.enter_data();

    printf("Matrix B\n");
    printf("Enter row count: ");
    scanf("%zu", &rows);
    printf("Enter colums count: ");
    scanf("%zu", &cols);
    Matrix B = Matrix(rows, cols);
    printf("Enter B data: \n");
    B.enter_data();

    printf("Sum of matrices A and B:\n");
    (A + B).print();

    printf("Difference of matrices A and B:\n");
    (A - B).print();

    printf("Multiplication of matrices A and B:\n");
    (A * B).print();

    printf("Transpose of matrix A:\n");
    (A.transpose()).print();

    printf("Matrix A squared:\n");
    (A.power(2)).print();

    printf("Matrix A multiplied by 3:\n");
    (A * 3).print();

    printf("Exponential of matrix A of order 3:\n");
    A.exp(3).print();

    printf("Determinant of matrix A: %.2f\n", A.determinant());
}
