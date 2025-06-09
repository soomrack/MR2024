#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

class Matrix {
private:
    size_t rows;
    size_t cols;
    vector<double> data;

public:
    // Конструкторы
    Matrix(size_t rows, size_t cols) : rows(rows), cols(cols), data(rows * cols, 0.0) {}
    
    Matrix(size_t rows, size_t cols, const vector<double>& values) : rows(rows), cols(cols), data(values) {
        if (values.size() != rows * cols) {
            throw invalid_argument("Размер списка значений не соответствует размерам матрицы");
        }
    }
    
    // Конструктор копирования
    Matrix(const Matrix& other) = default;
    
    // Конструктор перемещения
    Matrix(Matrix&& other) noexcept = default;
    
    // Операторы присваивания
    Matrix& operator=(const Matrix& other) = default;
    Matrix& operator=(Matrix&& other) noexcept = default;
    
    // Методы доступа
    size_t getRows() const { return rows; }
    size_t getCols() const { return cols; }
    
    // Доступ к элементам
    double& operator()(size_t row, size_t col) {
        if (row >= rows || col >= cols) {
            throw out_of_range("Индексы матрицы выходят за допустимые пределы");
        }
        return data[row * cols + col];
    }
    
    const double& operator()(size_t row, size_t col) const {
        if (row >= rows || col >= cols) {
            throw out_of_range("Индексы матрицы выходят за допустимые пределы");
        }
        return data[row * cols + col];
    }
    
    // Операции с матрицами
    Matrix operator+(const Matrix& other) const {
        if (rows != other.rows || cols != other.cols) {
            throw invalid_argument("Размеры матриц не совпадают для сложения");
        }
        
        Matrix result(rows, cols);
        for (size_t i = 0; i < data.size(); ++i) {
            result.data[i] = data[i] + other.data[i];
        }
        return result;
    }
    
    Matrix operator-(const Matrix& other) const {
        if (rows != other.rows || cols != other.cols) {
            throw invalid_argument("Размеры матриц не совпадают для вычитания");
        }
        
        Matrix result(rows, cols);
        for (size_t i = 0; i < data.size(); ++i) {
            result.data[i] = data[i] - other.data[i];
        }
        return result;
    }
    
    Matrix operator*(const Matrix& other) const {
        if (cols != other.rows) {
            throw invalid_argument("Размеры матриц несовместимы для умножения");
        }
        
        Matrix result(rows, other.cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < other.cols; ++j) {
                result(i, j) = 0;
                for (size_t k = 0; k < cols; ++k) {
                    result(i, j) += (*this)(i, k) * other(k, j);
                }
            }
        }
        return result;
    }
    
    Matrix operator*(double scalar) const {
        Matrix result(rows, cols);
        for (size_t i = 0; i < data.size(); ++i) {
            result.data[i] = data[i] * scalar;
        }
        return result;
    }
    
    friend Matrix operator*(double scalar, const Matrix& matrix) {
        return matrix * scalar;
    }
    
    Matrix transpose() const {
        Matrix result(cols, rows);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result(j, i) = (*this)(i, j);
            }
        }
        return result;
    }
    
    Matrix getSubmatrix(size_t ex_row, size_t ex_col) const {
        if (ex_row >= rows || ex_col >= cols) {
            throw invalid_argument("Исключаемые строка или столбец выходят за пределы матрицы");
        }
        
        Matrix result(rows - 1, cols - 1);
        size_t new_row = 0;
        for (size_t row = 0; row < rows; ++row) {
            if (row == ex_row) continue;
            
            size_t new_col = 0;
            for (size_t col = 0; col < cols; ++col) {
                if (col == ex_col) continue;
                
                result(new_row, new_col) = (*this)(row, col);
                ++new_col;
            }
            ++new_row;
        }
        return result;
    }
    
    double determinant() const {
        if (rows != cols) {
            throw invalid_argument("Матрица должна быть квадратной для вычисления определителя");
        }
        
        if (rows == 1) {
            return data[0];
        }
        
        if (rows == 2) {
            return data[0] * data[3] - data[1] * data[2];
        }
        
        double det = 0;
        double sign = 1.0;
        
        for (size_t col = 0; col < cols; ++col) {
            Matrix submatrix = getSubmatrix(0, col);
            det += sign * (*this)(0, col) * submatrix.determinant();
            sign *= -1;
        }
        
        return det;
    }
    
    Matrix inverse() const {
        double det = determinant();
        const double eps = 1e-10; // Маленькое значение вместо numeric_limits
        if (abs(det) < eps) {
            throw runtime_error("Матрица вырожденная (определитель равен нулю)");
        }
        
        Matrix cofactor(rows, cols);
        for (size_t row = 0; row < rows; ++row) {
            for (size_t col = 0; col < cols; ++col) {
                Matrix submatrix = getSubmatrix(row, col);
                double sign = ((row + col) % 2 == 0) ? 1.0 : -1.0;
                cofactor(row, col) = sign * submatrix.determinant();
            }
        }
        
        Matrix adjugate = cofactor.transpose();
        return adjugate * (1.0 / det);
    }
    
    static Matrix identity(size_t size) {
        Matrix result(size, size);
        for (size_t i = 0; i < size; ++i) {
            result(i, i) = 1.0;
        }
        return result;
    }
    
    Matrix power(unsigned long long n) const {
        if (rows != cols) {
            throw invalid_argument("Матрица должна быть квадратной для возведения в степень");
        }
        
        if (n == 0) {
            return identity(rows);
        }
        
        Matrix result = *this;
        for (unsigned long long i = 1; i < n; ++i) {
            result = result * (*this);
        }
        return result;
    }
    
    Matrix exponent(unsigned long long terms) const {
        if (rows != cols) {
            throw invalid_argument("Матрица должна быть квадратной для вычисления экспоненты");
        }
        
        Matrix result = identity(rows);
        Matrix term = identity(rows);
        double factorial = 1.0;
        
        for (unsigned long long n = 1; n <= terms; ++n) {
            term = term * (*this);
            factorial *= n;
            result = result + term * (1.0 / factorial);
        }
        
        return result;
    }
    
    void print() const {
        for (size_t row = 0; row < rows; ++row) {
            for (size_t col = 0; col < cols; ++col) {
                printf("%8.4f ", (*this)(row, col));
            }
            printf("\n");
        }
    }
};

int main() {
    try {
        Matrix A(2, 2, {1, 3, 5, 7});
        Matrix B(2, 2, {1, 2, 3, 4});
        
        printf("Матрица A:\n");
        A.print();
        printf("\nМатрица B:\n");
        B.print();
        
        printf("\nРезультат сложения матриц:\n");
        (A + B).print();
        
        printf("\nРезультат вычитания матриц:\n");
        (A - B).print();
        
        printf("\nРезультат умножения матриц:\n");
        (A * B).print();
        
        printf("\nРезультат умножения матрицы A на число 2:\n");
        (A * 2).print();
        
        printf("\nРезультат умножения матрицы B на число 2:\n");
        (B * 2).print();
        
        printf("\nРезультат транспонирования матрицы A:\n");
        A.transpose().print();
        
        printf("\nРезультат транспонирования матрицы B:\n");
        B.transpose().print();
        
        printf("\nОпределитель матрицы A: %.4f\n", A.determinant());
        printf("Определитель матрицы B: %.4f\n\n", B.determinant());
        
        printf("Обратная матрица для A:\n");
        A.inverse().print();
        
        printf("\nОбратная матрица для B:\n");
        B.inverse().print();
        
        printf("\nМатрица A в степени 3:\n");
        A.power(3).print();
        
        printf("\nМатрица B в степени 2:\n");
        B.power(2).print();
        
        printf("\nЭкспонента матрицы A (3 члена ряда):\n");
        A.exponent(3).print();
        
        printf("\nЭкспонента матрицы B (20 членов ряда):\n");
        B.exponent(20).print();
        
    } catch (const exception& e) {
        printf("Ошибка: %s\n", e.what());
        return 1;
    }
    
    return 0;
}