#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstring>

using namespace std;

// Определяем тип для элементов матрицы
typedef double MatrixItem;

class Matrix {
private:
    size_t rows; 
    size_t cols; 
    vector<MatrixItem> data; // Данные матрицы хранятся в векторе

public:
    // Конструкторы
    Matrix(); // Конструктор по умолчанию
    Matrix(size_t rows, size_t cols, const MatrixItem* src_data); // Конструктор с инициализацией данными
    Matrix(size_t rows, size_t cols); // Конструктор с заданием размеров
    Matrix(const Matrix& A); // Конструктор копирования

    // Деструктор
    ~Matrix() = default; // vector сам управляет памятью

    // Операторы
    Matrix operator+(const Matrix& A) const; // +
    Matrix operator-(const Matrix& A) const; // -
    Matrix operator=(const Matrix& B) const; // =

    // Методы для работы с матрицей
    void print() const; // Печать матрицы
    void copy() const; // Печать матрицы

    // Метод для проверки состояния матрицы
    bool is_empty() const { return rows == 0 || cols == 0; } // Проверка на пустую матрицу
};

// Исключения для обработки ошибок
class MatrixException : public domain_error {
public:
    explicit MatrixException(const string& message) : domain_error(message) {} // Конструктор для исключений
};

// Определяем сообщения об ошибках
const MatrixException MEMORY_ERROR("Memory allocation failed");
const MatrixException SIZE_ERROR("Matrices of different sizes");
const MatrixException MULTIPLY_ERROR("A.cols and B.rows aren't equal");

// Конструктор по умолчанию
Matrix::Matrix() : rows(0), cols(0) {}

// Конструктор с инициализацией данными
Matrix::Matrix(size_t rows, size_t cols, const MatrixItem* src_data)
    : rows(rows), cols(cols), data(rows * cols) {
    if (src_data) {
        memcpy(data.data(), src_data, sizeof(MatrixItem) * rows * cols); // Копируем данные из массива в вектор
    }
}

// Конструктор с заданием размеров
Matrix::Matrix(size_t rows, size_t cols)
    : rows(rows), cols(cols), data(rows * cols) {}

// Конструктор копирования
Matrix::Matrix(const Matrix& A)
    : rows(A.rows), cols(A.cols), data(A.data) {}

// Оператор сложения двух матриц
Matrix Matrix::operator+(const Matrix& A) const {
    if (rows != A.rows || cols != A.cols) { // Проверка на совпадение размеров
        throw SIZE_ERROR; 
    }

    Matrix result(rows, cols); 
    for (size_t idx = 0; idx < rows * cols; idx++) {
        result.data[idx] = data[idx] + A.data[idx]; // Сложение элементов матриц
    }
    return result; 
}

// Оператор вычитания двух матриц
Matrix Matrix::operator-(const Matrix& A) const {
    if (rows != A.rows || cols != A.cols) {
        throw SIZE_ERROR; // Проверка на совпадение размеров
    }

    Matrix result(rows, cols); 
    for (size_t idx = 0; idx < rows * cols; idx++) {
        result.data[idx] = data[idx] - A.data[idx]; // Вычитание элементов матриц
    }
    return result; 
}

// Метод для печати матрицы на экран
void Matrix::print() const {
    if (is_empty()) { 
        cout << "Empty matrix!" << endl; 
        return;
    }

    for (size_t row = 0; row < rows; row++) { 
        for (size_t col = 0; col < cols; col++) { 
            cout << data[row * cols + col] << " "; 
        }
        cout << endl;
    }
}

// Основная функция для демонстрации работы класса Matrix
int main() {
    double arr1[] = {7, 7, 7, 7}; // Исходные данные для первой матрицы
    double arr2[] = {3, 3, 3, 3}; 

    Matrix A(2, 2, arr1); 
    Matrix B(2, 2, arr2); 

    cout << "Matrix A:" << endl;
    A.print();

    cout << "Matrix B:" << endl;
    B.print(); 

    Matrix C = A + B; 
    cout << "Matrix C (A + B):" << endl;
    C.print(); 

    Matrix D = A - B; // Изменил имя переменной для результата вычитания
    cout << "Matrix D (A - B):" << endl;
    D.print(); 

    return 0;
}