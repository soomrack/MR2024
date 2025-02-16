#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstring>

using namespace std;

typedef double MatrixItem;

class Matrix {
private:
    size_t rows; 
    size_t cols; 
    vector<MatrixItem> data; 

public:
    Matrix(); 
    Matrix(size_t rows, size_t cols, const MatrixItem* src_data); // Конструктор с инициализацией данными
    Matrix(size_t rows, size_t cols); // Конструктор с заданием размеров
    Matrix(const Matrix& A); // Конструктор копирования
    Matrix(Matrix&& A) noexcept; // Конструктор перемещения

    // Деструктор
    ~Matrix() = default; 

    // Операторы
    Matrix& operator=(const Matrix& A); 
    Matrix operator+(const Matrix& A) const; 
    void operator+=(const Matrix& A); // Оператор сложения с присваиванием
    Matrix operator-(const Matrix& A) const; 
    Matrix operator*(const Matrix& A) const; 
    Matrix operator*(double a) const; 
    void operator*=(const Matrix& A); // Оператор умножения с присваиванием
    void operator*=(double a); // Оператор умножения на скаляр с присваиванием
    void operator-=(const Matrix& A); 

    // Методы для работы с матрицей
    void set_zeros(); 
    void pow(int n); // Возведение в степень 
    void ident(); 
    void transp(); 
    double det() const; 
    Matrix exp(int n) const; // Экспоненциальная матрица АЙАЙАЙ
    Matrix inverse() const; 
    void print() const; 

    // Метод для проверки состояния матрицы
    bool is_empty() const { return rows == 0 || cols == 0; } 
};

// Исключения для обработки ошибок
class MatrixException : public domain_error {
public:
    explicit MatrixException(const string& message) : domain_error(message) {} 
};

// Определяем сообщения об ошибках
const MatrixException MEMORY_ERROR("Memory allocation failed!");
const MatrixException SIZE_ERROR("Matrices of different sizes!");
const MatrixException MULTIPLY_ERROR("A.cols and B.rows aren't equal!");
const MatrixException EMP_ERROR("Matris is empty!");
const MatrixException SQR_ERROR("Determinant can only be calculated for square matrices!");


Matrix::Matrix() : rows(0), cols(0) {}

Matrix::Matrix(size_t rows, size_t cols, const MatrixItem* src_data)
    : rows(rows), cols(cols), data(rows * cols) {
    if (src_data) {
        memcpy(data.data(), src_data, sizeof(MatrixItem) * rows * cols); // Копируем данные из массива в вектор+++++++++++++
    }
}

// Конструктор с заданием размеров
Matrix::Matrix(size_t rows, size_t cols)
    : rows(rows), cols(cols), data(rows * cols) {}

// Конструктор копирования
Matrix::Matrix(const Matrix& A)
    : rows(A.rows), cols(A.cols), data(A.data) {}

// Конструктор перемещения
Matrix::Matrix(Matrix&& A) noexcept
    : rows(A.rows), cols(A.cols), data(std::move(A.data)) {
    A.rows = 0;
    A.cols = 0;
}

// Оператор присваивания
Matrix& Matrix::operator=(const Matrix& A) {
    if (this != &A) { 
        rows = A.rows;
        cols = A.cols;
        data = A.data; 
    }
    return *this;
}

// Оператор сложения двух матриц
Matrix Matrix::operator+(const Matrix& A) const {
    if (rows != A.rows || cols != A.cols) { 
        throw SIZE_ERROR; 
    }

    Matrix result(rows, cols); 
    for (size_t idx = 0; idx < rows * cols; idx++) {
        result.data[idx] = data[idx] + A.data[idx]; // Сложение элементов матриц
    }
    return result; // Возвращаем результат сложения
}

// Оператор сложения с присваиванием
void Matrix::operator+=(const Matrix& A) {
    if (rows != A.rows || cols != A.cols) {
        throw SIZE_ERROR; 
    }

    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] += A.data[idx]; 
    }
}

// Оператор вычитания двух матриц
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

// Оператор умножения двух матриц
Matrix Matrix::operator*(const Matrix& A) const {
    if (cols != A.rows) {
        throw MULTIPLY_ERROR; 
    }

    Matrix result(rows, A.cols); 
    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < A.cols; col++) {
            for (size_t idx = 0; idx < cols; ++idx) {
                result.data[row * A.cols + col] += data[row * cols + idx] * A.data[idx * A.cols + col]; 
               
            }
        }
    }
    return result; 
}

// Оператор умножения на скаляр
Matrix Matrix::operator*(double a) const {
    Matrix result(rows, cols); 
    for (size_t idx = 0; idx < result.rows * result.cols; idx++) {
        result.data[idx] = data[idx] * a; 
    }
    return result; 
}

// Оператор умножения с присваиванием (умножение на другую матрицу)
void Matrix::operator*=(const Matrix& A) {
    *this = *this * A; 
}

// Оператор умножения на скаляр с присваиванием
void Matrix::operator*=(double a) {
    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] *= a; 
    }
}

// Оператор вычитания с присваиванием
void Matrix::operator-=(const Matrix& A) {
    if (rows != A.rows || cols != A.cols) {
        throw SIZE_ERROR; 
    }

    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] -= A.data[idx]; 
    }
}

// Метод для печати матрицы на экран
void Matrix::print() const {
    if (is_empty()) { 
        throw EMP_ERROR;
        return;
    }

    for (size_t row = 0; row < rows; row++) { 
        for (size_t col = 0; col < cols; col++) { 
            cout << data[row * cols + col] << " "; 
        }
        cout << endl;
    }
}

// Установка всех элементов в 0
void Matrix:: set_zeros() {
    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] = 0;
    }
}

// Создание единичной матрицы
void Matrix:: ident() {
    Matrix result(rows, cols); 
    for (size_t idx = 0; idx < rows * cols; idx++) {
        result.data[idx] = 1;
    }
    result.print();
}

// Транспонирование матрицы
void Matrix:: transp() {
    Matrix result(rows, cols); 
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            result.data[j * rows + i] = data[i * cols + j];
        }
    }
    result.print();
}

double Matrix::det() const {
    if (rows != cols) {
        throw SQR_ERROR;  // Ошибка, если матрица не квадратная
    }
    if (rows == 1) {
        return data[0];  // Определитель 1x1 матрицы
    }
    if (rows == 2) {
        return data[0] * data[3] - data[1] * data[2];  // Определитель 2x2 матрицы
    }

    double det = 0;
    for (size_t p = 0; p < cols; p++) {
        Matrix submatrix(rows - 1, cols - 1);
        for (size_t i = 1; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                if (j < p) {
                    submatrix.data[(i - 1) * (cols - 1) + j] = data[i * cols + j];  // Копируем элементы до p
                } else if (j > p) {
                    submatrix.data[(i - 1) * (cols - 1) + (j - 1)] = data[i * cols + j];  // Копируем элементы после p
                }
            }
        }
        // Рекурсивный вызов для подматрицы
        det += (p % 2 == 0 ? 1 : -1) * data[p] * submatrix.det();
    }
    return det; // Убедитесь, что эта строка находится в конце функции
}
/*
void Matrix:: exp(int n) const{
    if (rows != cols) {
        throw SQR_ERROR;
    }
    Matrix result(rows, cols); 
    Matrix result = result.ident();
    Matrix term = *this;

    for (int n = 1; n <= 10; n++) { // Ограничиваем число членов ряда
        result += term / factorial(n);
        term *= *this; // Умножаем на саму себя для следующего члена ряда
    }

    return result;
}
*/

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
            // Вычисляем определитель подматрицы и сохраняем в аджойнт
            adjoint.data[j * cols + i] = ((i + j) % 2 == 0 ? 1 : -1) * submatrix.det(); 
        }
    }
    adjoint=adjoint * (1.0 / detValue);
    adjoint.print();
    return adjoint; // Возвращаем обратную матрицу
}

int main() {
    double arr1[] = {5, 6, 7, 8}; // Исходные данные для матрицы A
    double arr2[] = {1, 2, 3, 4}; // Исходные данные для матрицы B 

    Matrix A(2, 2, arr1); 
    Matrix B(2, 2, arr2); 

    cout << "Matrix A:" << endl;
    A.print(); 

    cout << "Matrix B:" << endl;
    B.print(); 

    // Сложение матриц
    Matrix C = A + B; 
    cout << "Matrix C (A + B):" << endl;
    C.print(); 

    // Вычитание матриц
    cout << "Matrix C (A - B):" << endl;
    Matrix D = A - B; 
    D.print(); 

    // Умножение матриц
    cout << "Matrix C (A * B):" << endl;
    Matrix E = A * B; 
    E.print(); 

    // Умножение матрицы на скаляр
    cout << "Matrix C (A * k):" << endl;
    Matrix F = A * 5; 
    F.print(); 

    // Печать единичной матрицы
    cout << "Matrix 0:" << endl;
    A.ident();

    // Транспонирование матрицы
    cout << "Matrix T:" << endl;
    B.transp();

    // Вычисление и вывод определителя матрицы A
    cout << "Determinant of Matrix A:" << endl;
    double res = A.det(); 
    cout << res << endl; 

    cout << "Matrix inv:" << endl;
    B.inverse();
    

    return 0;
}