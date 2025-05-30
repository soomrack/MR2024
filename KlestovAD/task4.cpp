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
    Matrix& matrix_set(const double* values); // Заполнение матрицы
    int matrix_print(); // Вывод матрицы
    Matrix& matrix_copy(const Matrix& A);

    Matrix& matrix_identity(); // Единичная матрица
    Matrix& matrix_add(const Matrix& B); // Добавление
    Matrix& matrix_negative_add(const Matrix& B); // Вычитание
    Matrix& matrix_sum(const Matrix& A, const Matrix& B); // Сумма
    Matrix& matrix_sub(const Matrix& A, const Matrix& B); // Разность
    Matrix& matrix_multiplication_k(const double k); // Умножение на число
    Matrix& matrix_multiplication(const Matrix& A, const Matrix& B); // Умножение матриц
    Matrix& matrix_transp(const Matrix& A); // Транспонирование
    double matrix_determinant(); // Детерминант
    Matrix& matrix_pow(const Matrix& A, const unsigned int n); // Матрица в степени
    Matrix& matrix_exp(const unsigned int target); // Экспонента в степени матрицы
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

Matrix& Matrix::matrix_set(const double* values)
{
    memcpy(data, values, rows * cols * sizeof(double));
    return *this;
}

int Matrix::matrix_print()
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
Matrix& Matrix::matrix_identity() {
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

// B := A
Matrix& Matrix::matrix_copy(const Matrix& A)
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

// A += B
Matrix& Matrix::matrix_add(const Matrix& B)
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
Matrix& Matrix::matrix_negative_add(const Matrix& B)
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
Matrix& Matrix::matrix_sum(const Matrix& A, const Matrix& B)
{
    if ((A.cols != B.cols) || (A.rows != B.rows) || (A.cols != cols) || (A.rows != rows) ||
        (cols != B.cols) || (rows != B.rows)) {
        matrix_exception(ERROR, "different sizes");
        throw MatrixException("");
    }

    for (size_t idx = 0; idx < A.cols * A.rows; idx++) {
        data[idx] = A.data[idx] + B.data[idx];
    }
    return *this;
}

// C = A - B
Matrix& Matrix::matrix_sub(const Matrix& A, const Matrix& B)
{
    if ((A.cols != B.cols) || (A.rows != B.rows) || (A.cols != cols) || (A.rows != rows) ||
        (cols != B.cols) || (rows != B.rows)) {
        matrix_exception(ERROR, "different sizes");
        throw MatrixException("");
    }

    for (size_t idx = 0; idx < A.cols * A.rows; idx++) {
        data[idx] = A.data[idx] - B.data[idx];
    }
    return *this;
}

// A *= k
Matrix& Matrix::matrix_multiplication_k(const double k)
{
    for (size_t idx = 0; idx < cols * rows; idx++) {
        data[idx] *= k;
    }
    return *this;
}

// C = A * B
Matrix& Matrix::matrix_multiplication(const Matrix& A, const Matrix& B)
{
    if ((A.cols != B.rows) || (cols != B.cols) || (rows != A.rows)) {
        matrix_exception(ERROR, "incorrect sizes");
        throw MatrixException("");
    }

    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++) {
            data[row * cols + col] = 0;
            for (size_t idx = 0; idx < A.cols; idx++) {
                data[row * cols + col] += A.data[row * A.cols + idx] * B.data[idx * B.cols + col];
            }
        }
    }
    return *this;
}

// B = A ^ T
Matrix& Matrix::matrix_transp(const Matrix& A)
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

double Matrix::matrix_determinant()
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
Matrix& Matrix::matrix_pow(const Matrix& A, const unsigned int n)
{
    if (A.rows != A.cols) {
        matrix_exception(WARNING, "not square");
        throw MatrixException("");
    }

    if ((A.cols != cols) || (A.rows != rows)) {
        matrix_exception(ERROR, "different sizes");
        throw MatrixException("");
    }

    if (n == 0) {
        matrix_identity();
    }

    if (n == 1) {
        matrix_copy(A);
        return *this;
    }

    Matrix tmp = Matrix::Matrix(A.rows, A.cols);
    if (tmp.data == NULL) {
        matrix_exception(ERROR, "no memory allocated");
        throw MatrixException("");
    }

    matrix_copy(A);

    for (unsigned int pow = 1; pow < n; ++pow) {
        matrix_multiplication(tmp, A);
        matrix_copy(tmp);
    }

    return *this;
}

//e ^ A
Matrix& Matrix::matrix_exp(const unsigned int target)
{
    if (rows != cols) {
        matrix_exception(WARNING, "not square");
        throw MatrixException("");
    }

    Matrix temp = Matrix::Matrix(rows, cols);
    temp.matrix_identity();

    double factorial = 1.0;
    
    for (size_t idx = 1; idx < target + 1; idx++) {
        Matrix tmp = Matrix::Matrix(rows, cols);
        tmp.matrix_pow(*this ,idx);
        factorial *= idx;
        tmp.matrix_multiplication_k(1 / factorial);
        temp.matrix_add(tmp);
    }
    matrix_copy(temp);

    return *this;
}

int main()
{
    Matrix A(2, 2);
    Matrix B(2, 2);
    Matrix C(2, 2);
    Matrix Y(2, 2);
    
    double valuesA[] = { 2.0, 3.0, 1.0, -1.0 };
    A.matrix_set(valuesA);
    double valuesB[] = { 1., 2.,2., 1. };
    B.matrix_set(valuesB);
    double valuesY[] = {2., 0., 0., -1.};
    Y.matrix_set(valuesY);
    A.matrix_print();
    B.matrix_print();
    Y.matrix_print();


    printf("    A+=B\n");
    A.matrix_add(B);
    A.matrix_print();

    printf("    A-=B\n");
    A.matrix_negative_add(B);
    A.matrix_print();

    printf("    C = A + B\n");
    C.matrix_sum(A, B);
    C.matrix_print();

    printf("    C = A - B\n");
    C.matrix_sub(A, B);
    C.matrix_print();

    printf("    A *= k\n");
    A.matrix_multiplication_k(5.78);
    A.matrix_print();

    printf("    C = A * B\n");
    C.matrix_multiplication(A, B);
    C.matrix_print();

    printf("    B = A ^ T\n");
    C.matrix_transp(A);
    C.matrix_print();

    printf("    det\n");
    double det = A.matrix_determinant();
    printf("%f\n\n", det);

    printf("    B = A ^ n\n");
    C.matrix_pow(A, 1);
    C.matrix_print();

    printf("    e ^ A\n");
    Y.matrix_exp(4);
    Y.matrix_print();

    return 0;
}