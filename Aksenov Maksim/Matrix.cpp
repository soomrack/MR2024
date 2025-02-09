#include <iostream>
using namespace std;

class Matrix {
private:
    size_t rows;
    size_t cols;
    double* data;

public:
    // Конструкторы
    Matrix(int r, int c) : rows(r), cols(c) {
        data = new double[rows * cols];
        for(int i = 0; i < rows * cols; i++) data[i] = 0;
    }

    Matrix(int r, int c, const double* arr) : rows(r), cols(c) {
        data = new double[rows * cols];
        for(int i = 0; i < rows * cols; i++) data[i] = arr[i];
    }

    // Копирующий конструктор
    Matrix(const Matrix& other) : rows(other.rows), cols(other.cols) {
        data = new double[rows * cols];
        for(int i = 0; i < rows * cols; i++) data[i] = other.data[i];
    }

    // Деструктор
    ~Matrix() {
        delete[] data;
    }

    // Оператор присваивания
    Matrix& operator=(const Matrix& other) {
        if(this == &other) return *this;
        
        delete[] data;
        rows = other.rows;
        cols = other.cols;
        
        data = new double[rows * cols];
        for(int i = 0; i < rows * cols; i++) data[i] = other.data[i];
        
        return *this;
    }

    // Арифметические операции
    Matrix operator+(const Matrix& other) const {
        if(rows != other.rows || cols != other.cols)
            throw "Size mismatch!";
        
        Matrix result(rows, cols);
        for(int i = 0; i < rows * cols; i++) 
            result.data[i] = data[i] + other.data[i];
        return result;
    }

    Matrix operator-(const Matrix& other) const {
        if(rows != other.rows || cols != other.cols)
            throw "Size mismatch!";
        
        Matrix result(rows, cols);
        for(int i = 0; i < rows * cols; i++) 
            result.data[i] = data[i] - other.data[i];
        return result;
    }

    Matrix operator*(const Matrix& other) const {
        if(cols != other.rows) throw "Size mismatch!";
        
        Matrix result(rows, other.cols);
        for(int i = 0; i < rows; i++) {
            for(int j = 0; j < other.cols; j++) {
                double sum = 0;
                for(int k = 0; k < cols; k++) {
                    sum += data[i * cols + k] * other.data[k * other.cols + j];
                }
                result.data[i * result.cols + j] = sum;
            }
        }
        return result;
    }

    Matrix operator*(double scalar) const {
        Matrix result(rows, cols);
        for(int i = 0; i < rows * cols; i++) 
            result.data[i] = data[i] * scalar;
        return result;
    }

    Matrix operator/(double scalar) const {
        if(scalar == 0) throw "Division by zero!";
        return *this * (1.0 / scalar);
    }

    // Специальные операции
    Matrix transpose() const {
        Matrix result(cols, rows);
        for(int i = 0; i < rows; i++) {
            for(int j = 0; j < cols; j++) {
                result.data[j * rows + i] = data[i * cols + j];
            }
        }
        return result;
    }

    Matrix power(int n) const {
        if(rows != cols) throw "Not a square matrix!";
        
        Matrix result = identity(rows);
        Matrix temp = *this;
        
        for(int i = 0; i < n; i++) {
            result = result * temp;
        }
        return result;
    }

    double determinant() const {
        if(rows != cols) throw "Not a square matrix!";
        
        if(rows == 1) return data[0];
        if(rows == 2) return data[0] * data[3] - data[1] * data[2];
        if(rows == 3) {
            return data[0]*(data[4]*data[8] - data[5]*data[7]) -
                   data[1]*(data[3]*data[8] - data[5]*data[6]) +
                   data[2]*(data[3]*data[7] - data[4]*data[6]);
        }
        throw "Determinant cannot be calculated!";
    }

    // Статические методы
    static Matrix identity(int size) {
        Matrix result(size, size);
        for(int i = 0; i < size; i++) 
            result.data[i * size + i] = 1.0;
        return result;
    }

    // Вывод матрицы
    friend ostream& operator<<(ostream& os, const Matrix& m) {
        for(int i = 0; i < m.rows; i++) {
            for(int j = 0; j < m.cols; j++) {
                os << m.data[i * m.cols + j] << "\t";
            }
            os << endl;
        }
        return os;
    }
};


int main() {
    try {
        double A[9] = {3,1,7,0,5,7,2,5,8};
        double B[9] = {5,0,8,1,9,6,3,2,1};
        
        Matrix matA(3, 3, A);
        Matrix matB(3, 3, B);

        cout << "Matrix A:\n" << matA << endl;
        cout << "Matrix B:\n" << matB << endl;
        cout << "A + B:\n" << matA + matB << endl;
        cout << "A - B:\n" << matA - matB << endl;
        cout << "A * B:\n" << matA * matB << endl;
        cout << "A * 5:\n" << matA * 5 << endl;
        cout << "Transposed A:\n" << matA.transpose() << endl;
        cout << "A^3:\n" << matA.power(3) << endl;
        cout << "Determinant of A: " << matA.determinant() << endl;
        
    } catch(const char* msg) {
        cerr << "Error: " << msg << endl;
    }
    return 0;
}
