#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstring>

typedef double MatrixItem;

class Matrix {
private:
    size_t rows; 
    size_t cols; 
    std::vector<MatrixItem> data; 

public:
    Matrix(); 
    Matrix(size_t rows, size_t cols, const MatrixItem* src_data); // Конструктор с инициализацией данными
    Matrix(size_t rows, size_t cols); // Конструктор с заданием размеров
    Matrix(const Matrix& A); // Конструктор копирования
    Matrix(Matrix&& A) noexcept; // Конструктор перемещения

    // Деструктор
    ~Matrix() = default; 

    // Операторы
    Matrix& operator=(const Matrix& A); //копирование
    Matrix& operator=(Matrix&& A) noexcept;  //перенос
    Matrix operator+(const Matrix& A) const; 
    void operator+=(const Matrix& A); 
    Matrix operator-(const Matrix& A) const; 
    Matrix operator*(const Matrix& A) const; 
    Matrix operator*(double a) const; 
    void operator*=(const Matrix& A); 
    void operator*=(double a); 
    void operator-=(const Matrix& A); 

    // Методы для работы с матрицей
    void set_zeros(); 
    void pow(int n); 
    void ident();
    Matrix transp() const; 
    double det() const; 
    Matrix exp(int n) const; 
    Matrix inverse() const; 
    void print() const; 

    // Метод для проверки состояния матрицы
    bool is_empty() const { return rows == 0 || cols == 0; } 
};

// Исключения для обработки ошибок
class MatrixException : public std::domain_error {
public:
    explicit MatrixException(const std::string& message) : std::domain_error(message) {} 
};

// Определяем сообщения об ошибках
const MatrixException MEMORY_ERROR("Memory allocation failed!");
const MatrixException SIZE_ERROR("Matrices of different sizes!");
const MatrixException MULTIPLY_ERROR("A.cols and B.rows aren't equal!");
const MatrixException EMP_ERROR("Matrix is empty!");
const MatrixException SQR_ERROR("Determinant can only be calculated for square matrices!");

Matrix::Matrix() : rows(0), cols(0) {}

Matrix::Matrix(size_t rows, size_t cols, const MatrixItem* src_data)
    : rows(rows), cols(cols), data() {
    if (src_data) {
        data.assign(src_data, src_data + rows * cols); // Копируем данные из массива в вектор
    } else {
        data.resize(rows * cols, 0.0); // Инициализируем нулями, если данные не предоставлены
    }
}

Matrix::Matrix(size_t rows, size_t cols)
    : rows(rows), cols(cols), data(rows * cols) {}

Matrix::Matrix(const Matrix& A)
    : rows(A.rows), cols(A.cols), data(A.data) {}

Matrix::Matrix(Matrix&& A) noexcept
    : rows(A.rows), cols(A.cols), data(std::move(A.data)) {
    A.rows = 0;
    A.cols = 0;
}

Matrix& Matrix::operator=(const Matrix& A) {
    if (this != &A) {
        rows = A.rows;
        cols = A.cols;
        data = A.data;
    }
    return *this;
}

Matrix& Matrix::operator=(Matrix&& A) noexcept {
    if (this != &A) {
        rows = A.rows;
        cols = A.cols;
        data = std::move(A.data);
        A.rows = 0;
        A.cols = 0;
    }
    return *this;
}

Matrix Matrix::operator+(const Matrix& A) const {
    if (rows != A.rows || cols != A.cols) { 
        throw SIZE_ERROR; 
    }

    Matrix result(rows, cols); 
    for (size_t idx = 0; idx < rows * cols; idx++) {
        result.data[idx] = data[idx] + A.data[idx];
    }
    return result;
}

void Matrix::operator+=(const Matrix& A) {
    if (rows != A.rows || cols != A.cols) {
        throw SIZE_ERROR; 
    }

    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] += A.data[idx]; 
    }
}

Matrix Matrix::operator-(const Matrix& A) const {
    if (rows != A.rows || cols != A.cols) {
        throw SIZE_ERROR; 
    }

    Matrix result(rows, cols); 
    for (size_t idx = 0; idx < rows * cols; idx++) {
        result.data[idx] = data[idx] - A.data[idx]; 
    }
    return result; 
}

Matrix Matrix::operator*(const Matrix& A) const {
    if (cols != A.rows) {
        throw MULTIPLY_ERROR; 
    }

    Matrix result(rows, A.cols); 
    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < A.cols; col++) {
            result.data[row * A.cols + col] = 0; // Зануляем элемент перед вычислением
            for (size_t idx = 0; idx < cols; ++idx) {
                result.data[row * A.cols + col] += data[row * cols + idx] * A.data[idx * A.cols + col];
            }
        }
    }
    return result; 
}

Matrix Matrix::operator*(double a) const {
    Matrix result(rows, cols); 
    for (size_t idx = 0; idx < result.rows * result.cols; idx++) {
        result.data[idx] = data[idx] * a; 
    }
    return result; 
}

void Matrix::operator*=(const Matrix& A) {
    *this = *this * A; 
}

void Matrix::operator*=(double a) {
    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] *= a; 
    }
}

void Matrix::operator-=(const Matrix& A) {
    if (rows != A.rows || cols != A.cols) {
        throw SIZE_ERROR; 
    }

    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] -= A.data[idx]; 
    }
}

void Matrix::print() const {
    if (is_empty()) { 
        throw EMP_ERROR;
    }

    for (size_t row = 0; row < rows; row++) { 
        for (size_t col = 0; col < cols; col++) { 
            std::cout << data[row * cols + col] << " "; 
        }
        std::cout << std::endl;
    }
}

void Matrix::set_zeros() {
    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] = 0;
    }
}

Matrix Matrix::transp() const {
    Matrix result(cols, rows); 
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            result.data[j * rows + i] = data[i * cols + j];
        }
    }
    return result;
}

double Matrix::det() const {
    if (rows != cols) {
        throw SQR_ERROR; 
    }
    if (rows == 1) {
        return data[0];  
    }
    if (rows == 2) {
        return data[0] * data[3] - data[1] * data[2];  
    }

    double det = 0;
    for (size_t p = 0; p < cols; p++) {
        Matrix submatrix(rows - 1, cols - 1);
        for (size_t i = 1; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                if (j < p) {
                    submatrix.data[(i - 1) * (cols - 1) + j] = data[i * cols + j];  
                } else if (j > p) {
                    submatrix.data[(i - 1) * (cols - 1) + (j - 1)] = data[i * cols + j]; 
                }
            }
        }
        det += (p % 2 == 0 ? 1 : -1) * data[p] * submatrix.det();
    }
    return det; 
}

void Matrix::ident() {
    if (rows != cols) {
        throw SQR_ERROR; 
    }
    set_zeros();
    for (size_t i = 0; i < rows; i++) {
        data[i * cols + i] = 1.0; 
    }
}

void Matrix::pow(int n) {
    if (rows != cols) {
        throw SQR_ERROR; 
    }
    if (n == 0) {
        ident(); 
        return;
    }
    Matrix result = *this;
    for (int i = 1; i < n; i++) {
        result *= *this;
    }
    *this = result;
}

Matrix Matrix::exp(int n) const {
    if (rows != cols) {
        throw SQR_ERROR;
    }

    Matrix result(rows, cols);
    result.ident(); 

    Matrix term = *this; 
    double factorial = 1.0; 

    for (int k = 1; k <= n; ++k) {
        factorial *= k; 
        result += term * (1.0 / factorial); 
        term *= (*this); 
    }

    return result;
}

Matrix Matrix::inverse() const {
    if (rows != cols) {
        throw std::runtime_error("Inverse can only be calculated for square matrices");
    }

    double detValue = det(); // Вычисляем определитель
    if (detValue == 0) {
        throw std::runtime_error("Matrix is singular and cannot be inverted");
    }

    Matrix adjoint(rows, cols);
    
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            Matrix submatrix(rows - 1, cols - 1);
            for (size_t m = 0; m < rows; m++) {
                for (size_t n = 0; n < cols; n++) {
                    if (m != i && n != j) {
                        submatrix.data[(m < i ? m : m - 1) * (cols - 1) + (n < j ? n : n - 1)] = data[m * cols + n];
                    }
                }
            }
            adjoint.data[j * cols + i] = ((i + j) % 2 == 0 ? 1 : -1) * submatrix.det(); 
        }
    }
    return adjoint * (1.0 / detValue);
}

int main() {
    double arr1[] = {5, 6, 7, 8}; 
    double arr2[] = {1, 2, 3, 4}; 

    Matrix A(2, 2, arr1); 
    Matrix B(2, 2, arr2); 

    std::cout << "Matrix A:" << std::endl;
    A.print(); 

    std::cout << "Matrix B:" << std::endl;
    B.print(); 

    // Сложение матриц
    Matrix C = A + B; 
    std::cout << "Matrix C (A + B):" << std::endl;
    C.print(); 

    // Вычитание матриц
    std::cout << "Matrix C (A - B):" << std::endl;
    Matrix D = A - B; 
    D.print(); 

    // Умножение матриц
    std::cout << "Matrix C (A * B):" << std::endl;
    Matrix E = A * B; 
    E.print(); 

    // Умножение матрицы на скаляр
    std::cout << "Matrix C (A * k):" << std::endl;
    Matrix F = A * 5; 
    F.print(); 

    // Создание единичной матрицы
    std::cout << "Identity Matrix:" << std::endl;
    Matrix I(2, 2);
    I.ident();
    I.print();

    // Транспонирование матрицы
    std::cout << "Transposed Matrix B:" << std::endl;
    Matrix T = B.transp();
    T.print();

    // Вычисление и вывод определителя матрицы A
    std::cout << "Determinant of Matrix A:" << std::endl;
    double res = A.det(); 
    std::cout << res << std::endl; 

    // Вычисление обратной матрицы
    std::cout << "Inverse of Matrix B:" << std::endl;
    Matrix invB = B.inverse();
    invB.print();

    // Вычисление экспоненты матрицы A
    std::cout << "Exponential of Matrix A:" << std::endl;
    Matrix expA = A.exp(10);
    expA.print();

    // Возведение матрицы A в степень 2
    std::cout << "Matrix A raised to the power of 2:" << std::endl;
    A.pow(2);
    A.print();

    return 0;
}