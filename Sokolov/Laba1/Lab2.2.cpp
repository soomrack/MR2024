#include <iostream>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <iomanip>

class Matrix {
private:
    size_t rows, cols;
    std::vector<double> data;

public:
    Matrix(size_t rows, size_t cols) : rows(rows), cols(cols), data(rows* cols, 0.0) {
        if (rows == 0 || cols == 0) {
            throw std::invalid_argument("Matrix dimensions cannot be zero");
        }
    }

    double& operator()(size_t row, size_t col) {
        return data[row * cols + col];
    }

    double operator()(size_t row, size_t col) const {
        return data[row * cols + col];
    }

    Matrix operator+(const Matrix& other) const {
        if (rows != other.rows || cols != other.cols) {
            throw std::invalid_argument("Matrix dimensions must match for addition");
        }
        Matrix result(rows, cols);
        for (size_t i = 0; i < data.size(); ++i) {
            result.data[i] = data[i] + other.data[i];
        }
        return result;
    }

    Matrix operator*(const Matrix& other) const {
        if (cols != other.rows) {
            throw std::invalid_argument("Matrix dimensions must be valid for multiplication");
        }
        Matrix result(rows, other.cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < other.cols; ++j) {
                for (size_t k = 0; k < cols; ++k) {
                    result(i, j) += (*this)(i, k) * other(k, j);
                }
            }
        }
        return result;
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

    void identity() {
        if (rows != cols) {
            throw std::invalid_argument("Matrix must be square for identity operation");
        }
        std::fill(data.begin(), data.end(), 0.0);
        for (size_t i = 0; i < rows; ++i) {
            (*this)(i, i) = 1.0;
        }
    }

    void print(const std::string& title) const {
        std::cout << title << ":\n";
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                std::cout << std::setw(6) << (*this)(i, j) << " ";
            }
            std::cout << "\n";
        }
    }
};

int main() {
    try {
        Matrix A(2, 2);
        A(0, 0) = 1.0; A(0, 1) = 2.0;
        A(1, 0) = 3.0; A(1, 1) = 4.0;

        Matrix B(2, 2);
        B(0, 0) = 5.0; B(0, 1) = 6.0;
        B(1, 0) = 7.0; B(1, 1) = 8.0;

        Matrix sum = A + B;
        sum.print("Sum of A and B");

        Matrix product = A * B;
        product.print("Product of A and B");

        Matrix transposed = A.transpose();
        transposed.print("Transpose of A");
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << '\n';
    }
    return 0;
}
