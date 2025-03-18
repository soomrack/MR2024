#include <iostream>
#include <vector>
#include <stdexcept>
#include <cmath>

class Matrix {
private:
    size_t rows;
    size_t cols;
    std::vector<double> data;

public:
    
    Matrix() : rows(0), cols(0), data() { }
    Matrix(size_t rows, size_t cols) ;
    Matrix(std::vector<double> data, size_t rows, size_t cols);
    Matrix(const Matrix&) ;
    Matrix(Matrix&& other);
 

   
    void print() const;
    Matrix transpose() const;
    double determinant() const;
    Matrix exp(unsigned int terms = 20) const;
    static Matrix identity(size_t size);



    Matrix& operator=(const Matrix& );
    Matrix operator+(const Matrix&);
    Matrix operator*(double k) const;
    Matrix operator*(const Matrix&)const; 
    Matrix operator^(const unsigned int degree);   
      
};
class MatrixException : public std::exception { 
    private:
        std::string message;
    public:
        explicit MatrixException(const std::string& msg) : message(msg) {} 
        const char* what() const noexcept override{
            return message.c_str();
        }
    };

 Matrix::Matrix(size_t rows, size_t cols) : rows(rows), cols(cols) {
    if (rows == 0 || cols == 0) {
        throw MatrixException("Matrix dimensions cannot be zero");
    }
    
    if(__SIZE_MAX__ / rows / cols / sizeof(double) == 0) {
        throw MatrixException("Overflow, sizes of matrix is big");
    }
    if(rows != 0 && cols != 0) {
        data = std::vector<double> (rows * cols, 0);
    }
}

Matrix::Matrix(std::vector<double> new_data, size_t rows, size_t cols): rows(rows), cols(cols) {
    if (rows == 0 || cols == 0) {
        throw MatrixException("Matrix dimensions cannot be zero");
    }
    
    if(__SIZE_MAX__ / rows / cols / sizeof(double) == 0) {
        throw MatrixException("Overflow, sizes of matrix is big");
    }
    if(rows != 0 && cols != 0) {
        data = new_data;
    }
}

//конструктор копирования 
Matrix::Matrix(const Matrix& other) : rows(other.rows), cols(other.cols), data(other.data) {}

// Конструктор перемещения
Matrix::Matrix(Matrix&& other) 
        : rows(other.rows), cols(other.cols), data(std::move(other.data)) { 
    }


Matrix& Matrix::operator=(const Matrix& other) {
    if (this != &other) {
        rows = other.rows;
        cols = other.cols;
        data = other.data;
    }
    return *this;
}

//result = this + B
 Matrix Matrix::operator+(const Matrix& B)  {
    if (rows != B.rows || cols != B.cols) {
        throw MatrixException("Matrix dimensions do not match for addition");
    }
    Matrix result(rows, cols);
    for (size_t idx = 0; idx < data.size(); idx++) {
        result.data[idx] = data[idx] + B.data[idx];
    }
    return result;
}

// result = this * k
Matrix Matrix::operator*(double k) const {
    Matrix result(rows, cols);
    for (size_t idx = 0; idx < data.size(); idx++) {
        result.data[idx] = data[idx] * k;
    }
    return result;
}

// result = this * other 
Matrix Matrix::operator*(const Matrix& other) const {
    if (cols != other.rows) {
        throw MatrixException("Matrix dimensions do not match for multiplication");
    }
    Matrix result(rows, other.cols);
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < other.cols; ++col) {
            double sum = 0.0;
            for (size_t idx = 0; idx < cols; ++idx) {
                sum += data[row * cols + idx] * other.data[idx * other.cols + col];
            }
            result.data[row * result.cols + col] = sum;
        }
    }
    return result;
}

// Возведение матрицы в степень
Matrix Matrix::operator^(const unsigned int degree) {
    if (rows != cols) {
        throw MatrixException("Matrix must be square to compute power");
    }
    if (degree == 0) {
        return identity(rows);
    }
    Matrix result = *this;
    for (unsigned int i = 1; i < degree; ++i) {
        result = result * (*this);
    }
    return result;
}

// Печать матрицы
void Matrix::print() const {
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            std::cout << data[row * cols + col] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

// Транспонирование матрицы
Matrix Matrix::transpose() const {
    Matrix result(cols, rows);
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            result.data[col * result.cols + row] = data[row * cols + col];
        }
    }
    return result;
}

// Вычисление определителя
double Matrix::determinant() const {
    if (rows != cols) {
        throw MatrixException("Matrix must be square to compute determinant");
    }
    if (rows == 1) return data[0];
    if (rows == 2) return data[0] * data[3] - data[1] * data[2];

    double det = 0.0;
    for (size_t col = 0; col < cols; ++col) {
        Matrix submatrix(rows - 1, cols - 1);
        for (size_t subrow = 1; subrow < rows; ++subrow) {
            size_t subcol = 0;
            for (size_t c = 0; c < cols; ++c) {
                if (c == col) continue;
                submatrix.data[(subrow - 1) * submatrix.cols + subcol] = data[subrow * cols + c];
                ++subcol;
            }
        }
        det += (col % 2 == 0 ? 1 : -1) * data[col] * submatrix.determinant();
    }
    return det;
}



// Единичная матрица
 Matrix Matrix::identity(size_t size) {
    Matrix result(size, size);
    for (size_t i = 0; i < size; ++i) {
        result.data[i * size + i] = 1.0;
    }
    return result;
}

// Экспонента матрицы
Matrix Matrix::exp(unsigned int terms = 20) const {
    if (rows != cols) {
        throw MatrixException("Matrix must be square to compute exponential");
    }
    Matrix result = identity(rows);
    Matrix term = identity(rows);
    for (unsigned int n = 1; n <= terms; ++n) {
        term = term * (*this) * (1.0 / n);
        result = result + term;
    }
    return result;
}

int main() {
    Matrix A({3., 4., 5., 7., 8., 9., 6., 5., 3.}, 3, 3);
    

    Matrix B({1., 0., 1., 0., 1., 1., 1., 1., 1.}, 3, 3);
   

    Matrix C = A + B;
    C.print();

    Matrix D = A * B;
    D.print();

    Matrix E = A.transpose();
    E.print();

    double det = A.determinant();
    std::cout << "Determinant of A: " << det << std::endl;

    Matrix F = A^2;
    F.print();

    Matrix G = A.exp();
    G.print();

    return 0; 
}