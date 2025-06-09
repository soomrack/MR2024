#include <iostream>

class MatrixException : public std::domain_error {
public:
    explicit MatrixException(const std::string& message) : domain_error(message) {}
};

class Matrix {
private:
    size_t rows;
    size_t cols;
    double* data;

public:
    Matrix(const size_t row, const size_t col); // Конструктор
    ~Matrix(); // Деструктор
    Matrix& set(const double* values); // Заполнение матрицы
    int print(); // Вывод матрицы
    Matrix(const Matrix& A); // Конструктор копирования
    Matrix(Matrix&& B) noexcept; // Конструктор переноса
    Matrix& operator=(const Matrix& A); // Копирование
    Matrix& operator=(Matrix&& B); // Перенос
    Matrix& identity(); // Единичная матрица

    Matrix& operator+=(const Matrix& B); // Добавление
    Matrix& operator-=(const Matrix& B); // Вычитание
    Matrix operator+(const Matrix& B); // Сумма
    Matrix operator-(const Matrix& B); // Разность
    Matrix& operator*=(const double k); // Умножение на число
    Matrix operator*(const Matrix& B); // Умножение матриц
    Matrix& T(const Matrix& A); // Транспонирование
    double det(); // Детерминант
    Matrix operator^(const unsigned int n); // Матрица в степени
    Matrix& exp(const unsigned int target); // Экспонента в степени матрицы
};

enum MatrixExceptionLevel { ERROR, WARNING, INFO, DEBUG };


int matrix_exception(const enum MatrixExceptionLevel level, const char* msg)
{
    if (level == ERROR) {
        printf("ERROR: %s", msg);
    }
    if (level == WARNING) {
        printf("WARNING: %s", msg);
    }
    if (level == INFO) {
        printf("INFO: %s", msg);
    }
    if (level == DEBUG) {
        printf("DEBUG: %s", msg);
    }
    return 0;
}

Matrix::Matrix(const size_t row, const size_t col)
{
    rows = row;
    cols = col;
    if (row == 0 || col == 0) {
        matrix_exception(INFO, "no matrix");
        throw MatrixException("");
    }
    if (SIZE_MAX / col / row / sizeof(double) < 1) {
        matrix_exception(ERROR, "overflow");
        throw MatrixException("");
    }

    data = new double[rows * cols];

    if (data == NULL) {
        matrix_exception(ERROR, "Allocation memory fail");
        throw MatrixException("");
    }
}

Matrix::~Matrix()
{
    delete[] data;
}

Matrix& Matrix::set(const double* values)
{
    memcpy(data, values, rows * cols * sizeof(double));
    return *this;
}

int Matrix::print()
{
    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++) {
            printf(" %.2f", data[row * cols + col]);
        }
        printf("\n");
    }
    printf("\n");
    return 0;
}

// E
Matrix& Matrix::identity() {
    if (rows != cols) {
        matrix_exception(WARNING, "not square");
        throw MatrixException("");
    }
    for (size_t idx = 0; idx < cols * rows; idx++) {
        data[idx] = 0.0;
    }
    for (size_t idx = 0; idx < cols * rows; idx += cols + 1) {
        data[idx] = 1.0;
    }
    return *this;
}

Matrix::Matrix(const Matrix& A)
{
    memcpy(data, A.data, A.cols * A.rows * sizeof(double));
}

Matrix::Matrix(Matrix&& B) noexcept : rows(B.rows), cols(B.cols), data(B.data)
{
    data = B.data;

    B.cols = 0;
    B.rows = 0;
    B.data = 0;
}

// B := A
Matrix& Matrix::operator=(const Matrix& A)
{
    if ((A.cols != cols) || (A.rows != rows)) {
        matrix_exception(ERROR, "different sizes");
        throw MatrixException("");
    }
    if (data == NULL) {
        matrix_exception(ERROR, "memory is not allocated");
        throw MatrixException("");
    }
    
    memcpy(data, A.data, A.cols * A.rows * sizeof(double));
    return *this;
}

// A = B, B = 0
Matrix& Matrix::operator=(Matrix&& B)
{
    if (this == &B) {
        return *this;
    }
    if (rows != B.rows || cols != B.cols) {
        matrix_exception(ERROR, "different sizes");
        throw MatrixException("");
    }
    
    data = B.data;

    B.cols = 0;
    B.rows = 0;
    B.data = nullptr;

    return *this;
}

// A += B
Matrix& Matrix::operator+=(const Matrix& B)
{
    if ((cols != B.cols) || (rows != B.rows)) {
        matrix_exception(ERROR, "different sizes");
        throw MatrixException("");
    }

    for (size_t idx = 0; idx < cols * rows; idx++) {
        data[idx] += B.data[idx];
    }
    return *this;
}

// A -= B
Matrix& Matrix::operator-=(const Matrix& B)
{
    if ((cols != B.cols) || (rows != B.rows)) {
        matrix_exception(ERROR, "different sizes");
        throw MatrixException("");
    }

    for (size_t idx = 0; idx < cols * rows; idx++) {
        data[idx] -= B.data[idx];
    }
    return *this;
}

// C = A + B
Matrix Matrix::operator+(const Matrix& B)
{
    if ((cols != B.cols) || (rows != B.rows)) {
        matrix_exception(ERROR, "different sizes");
        throw MatrixException("");
    }
    
    Matrix res(cols, rows);
    
    for (size_t idx = 0; idx < cols * rows; idx++) {
        res.data[idx] = data[idx] + B.data[idx];
    }
    return res;
}

// C = A - B
Matrix Matrix::operator-(const Matrix& B)
{
    if ((cols != B.cols) || (rows != B.rows)) {
        matrix_exception(ERROR, "different sizes");
        throw MatrixException("");
    }

    Matrix res(cols, rows);

    for (size_t idx = 0; idx < cols * rows; idx++) {
        res.data[idx] = data[idx] - B.data[idx];
    }
    return res;
}

// A *= k
Matrix& Matrix::operator*=(const double k)
{
    for (size_t idx = 0; idx < cols * rows; idx++) {
        data[idx] *= k;
    }
    return *this;
}

// C = A * B
Matrix Matrix::operator*(const Matrix& B)
{
    if (cols != B.rows) {
        matrix_exception(ERROR, "incorrect sizes");
        throw MatrixException("");
    }
    
    Matrix res(cols, rows);
    
    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++) {
            res.data[row * cols + col] = 0;
            for (size_t idx = 0; idx < cols; idx++) {
                res.data[row * cols + col] += data[row * cols + idx] * B.data[idx * B.cols + col];
            }
        }
    }
    return res;
}

// B = A ^ T
Matrix& Matrix::T(const Matrix& A)
{
    if ((A.cols != cols) || (A.rows != rows)) {
        matrix_exception(ERROR, "different sizes");
        throw MatrixException("");
    }

    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++) {
            data[row + A.rows * col] = A.data[col + A.cols * row];
        }
    }
    return *this;
}

double Matrix::det()
{
    double det;
    if (cols != rows) {
        matrix_exception(ERROR, "not square");
        throw MatrixException("");
    }

    if (rows == 0) {
        matrix_exception(ERROR, "no matrix");
        throw MatrixException("");
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
}

// B = A ^ n
Matrix Matrix::operator^(const unsigned int n)
{
    if (rows != cols) {
        matrix_exception(WARNING, "not square");
        throw MatrixException("");
    }

    Matrix res(rows, cols);

    if (n == 0) {
        res.identity();
        return res;
    }

    if (n == 1) {
        res = *this;
        return res;
    }

    Matrix tmp(rows, cols);
    if (tmp.data == NULL) {
        matrix_exception(ERROR, "no memory allocated");
        throw MatrixException("");
    }

    res = *this;

    for (unsigned int pow = 1; pow < n; ++pow) {
        res = tmp * res;
        tmp = res;
    }

    return res;
}

//e ^ A
Matrix& Matrix::exp(const unsigned int target)
{
    if (rows != cols) {
        matrix_exception(WARNING, "not square");
        throw MatrixException("");
    }

    Matrix temp(rows, cols);
    temp.identity();

    double factorial = 1.0;
    
    for (size_t idx = 1; idx < target + 1; idx++) {
        Matrix tmp(rows, cols);
        tmp = *this ^ idx;
        factorial *= idx;
        tmp*=(1 / factorial);
        temp+=tmp;
    }
    *this = temp;
    return *this;
}

int main()
{
    Matrix A(2, 2);
    Matrix B(2, 2);
    Matrix C(2, 2);
    Matrix Y(2, 2);
    
    double valuesA[] = { 2.0, 3.0, 1.0, -1.0 };
    A.set(valuesA);
    double valuesB[] = { 1., 2.,2., 1. };
    B.set(valuesB);
    double valuesY[] = {2., 0., 0., -1.};
    Y.set(valuesY);
    A.print();
    B.print();
    Y.print();


    printf("    A+=B\n");
    A+=B;
    A.print();

    printf("    A-=B\n");
    A-=B;
    A.print();

    printf("    C = A + B\n");
    C = A + B;
    C.print();

    printf("    C = A - B\n");
    C = A - B;
    C.print();

    printf("    A *= k\n");
    A*=5.78;
    A.print();

    printf("    C = A * B\n");
    C = A * B;
    C.print();

    printf("    B = A ^ T\n");
    C.T(A);
    C.print();

    printf("    det\n");
    double det = A.det();
    printf("%f\n\n", det);

    printf("    B = A ^ n\n");
    C = A ^ 1;
    C.print();

    printf("    e ^ A\n");
    Y.exp(2);
    Y.print();

    return 0;
}