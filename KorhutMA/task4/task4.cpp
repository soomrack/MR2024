#include <iostream>
#include <vector>
#include <stdexcept>

class Matrix
{
private:
    size_t rows;
    size_t cols;
    std::vector<double> data;

public:
    Matrix();
    Matrix(size_t rows, size_t cols);
    Matrix(double *new_data, size_t rows, size_t cols);
    Matrix(double number);
    Matrix(const Matrix &other);
    Matrix(Matrix &&other);

    Matrix &operator=(const Matrix &other);
    Matrix operator+(const Matrix &other) const;
    Matrix operator-(const Matrix &other) const;
    Matrix operator*(const Matrix &other) const;
    Matrix operator*(double scalar) const;
    
    Matrix pow(size_t n) const;
    Matrix transpose() const;
    double determinant() const;
    Matrix getMinorMatrix(size_t row, size_t col) const;
    Matrix identity(size_t size) const;
    Matrix exp(size_t iterations) const;

    void print() const;
    void fillRandom();
};

class MatrixException : public std::domain_error
{
public:
    explicit MatrixException(const std::string &message) : std::domain_error(message) {}
};
const MatrixException MEMORY_ERROR("Memory allocation failed!");
const MatrixException SIZE_ERROR("Matrices of different sizes!");
const MatrixException MULTIPLY_ERROR("A.cols and B.rows aren't equal!");
const MatrixException EMP_ERROR("Matrix is empty!");
const MatrixException SQR_ERROR("Determinant can only be calculated for square matrices!");
const MatrixException ZER_ERROR("Determinant can't be calculated for zero matrice!");

Matrix::Matrix() : rows(0), cols(0) {}

Matrix::Matrix(size_t rows, size_t cols) 
    : rows(rows), cols(cols), data(rows * cols, 0.0) {}

Matrix::Matrix(double *new_data, size_t rows, size_t cols) 
    : rows(rows), cols(cols), data(new_data, new_data + rows * cols) {}

Matrix::Matrix(double number) 
    : rows(1), cols(1), data(1, number) {}

Matrix::Matrix(const Matrix &other)
    : rows(other.rows), cols(other.cols), data(other.data){}

Matrix::Matrix(Matrix &&other) : rows(other.rows), cols(other.cols), data(move(other.data))
{
    other.rows = 0;
    other.cols = 0;
}

void Matrix::print() const
{
    for (size_t idx = 0; idx < rows; ++idx){
        for (size_t jdx = 0; jdx < cols; ++jdx){
            std::cout << data[idx * cols + jdx] << " ";
        }
        std::cout << std::endl;
    }
}

Matrix Matrix::identity(size_t size) const
{
    Matrix result(size, size);
    for (size_t i = 0; i < size; ++i){
        result.data[i * size + i] = 1.0;
    }
    return result;
}

void Matrix::fillRandom()
{
    srand(time(0));
    for (size_t idx = 0; idx < rows * cols; ++idx){
        int sight = (rand() % 2) * 2 - 1;
        data[idx] = (rand() % 10) * sight;
    }
}

Matrix &Matrix::operator=(const Matrix &other)
{
    if (this == &other){
        return *this;
    }

    rows = other.rows;
    cols = other.cols;
    data = other.data;

    return *this;
}

Matrix Matrix::operator+(const Matrix &other) const
{
    if (rows != other.rows || cols != other.cols){
        throw SIZE_ERROR;
    }
    Matrix result(rows, cols);
    for (size_t idx = 0; idx < rows * cols; ++idx){
        result.data[idx] = data[idx] + other.data[idx];
    }
    return result;
}

Matrix Matrix::operator-(const Matrix &other) const
{
    if (rows != other.rows || cols != other.cols){
        throw SIZE_ERROR;
    }
    Matrix result(rows, cols);
    for (size_t idx = 0; idx < rows * cols; ++idx){
        result.data[idx] = data[idx] - other.data[idx];
    }
    return result;
}

Matrix Matrix::operator*(const Matrix &other) const
{
    if (cols != other.rows){
        throw MULTIPLY_ERROR;
    }
    Matrix result(rows, other.cols);
    for (size_t idx = 0; idx < rows; ++idx){
        for (size_t jdx = 0; jdx < other.cols; ++jdx){
            result.data[idx * other.cols + jdx] = 0;
            for (size_t k = 0; k < cols; ++k){
                result.data[idx * other.cols + jdx] += data[idx * cols + k] * other.data[k * other.cols + jdx];
            }
        }
    }
    return result;
}

Matrix Matrix::operator*(double scalar) const
{
    Matrix result(rows, cols);
    for (size_t idx = 0; idx < rows * cols; ++idx){
        result.data[idx] = data[idx] * scalar;
    }
    return result;
}

Matrix Matrix::pow(size_t n) const
{
    if (rows != cols){
        throw SIZE_ERROR;
    }

    Matrix result(*this);
    Matrix base(*this);

    if (n == 0) {
        result = identity (rows);
        return result;
    }

    for (size_t idx = 1; idx < n; ++idx){
        result = result * base;
    }

    return result;
}

Matrix Matrix::transpose() const
{
    Matrix result(cols, rows);

    for (size_t idx = 0; idx < rows; ++idx){
        for (size_t jdx = 0; jdx < cols; ++jdx){
            result.data[jdx * rows + idx] = data[idx * cols + jdx];
        }
    }

    return result;
}

double Matrix::determinant() const
{
    if (rows == 0 && cols == 0){
        throw ZER_ERROR;
    }
    if (rows != cols){
        throw SIZE_ERROR;
    }

    if (rows == 1){
        return data[0];
    }

    if (rows == 2){
        return data[0] * data[3] - data[1] * data[2];
    }

    double det = 0;
    for (size_t jdx = 0; jdx < cols; ++jdx){
        Matrix minorMatrix = getMinorMatrix(0, jdx);
        det += (jdx % 2 == 0 ? 1 : -1) * data[jdx] * minorMatrix.determinant();
    }

    return det;
}

Matrix Matrix::getMinorMatrix(size_t row, size_t col) const
{
    Matrix minor(rows - 1, cols - 1);

    size_t minorIdx = 0;
    for (size_t idx = 0; idx < rows; ++idx){
        if (idx == row)
            continue;
        size_t minorJdx = 0;
        for (size_t jdx = 0; jdx < cols; ++jdx){
            if (jdx == col)
                continue;
            minor.data[minorIdx * (cols - 1) + minorJdx] = data[idx * cols + jdx];
            minorJdx++;
        }
        minorIdx++;
    }

    return minor;
}

Matrix Matrix::exp(size_t iterations = 20) const
{
    if (rows != cols){
        throw SIZE_ERROR;
    }

    Matrix result = identity(rows);
    Matrix term = identity(rows);

    for (size_t idx = 1; idx <= iterations; ++idx){
        term = term * (*this) * (1.0 / idx);
        result = result + term;
    }

    return result;
}

int main()
{
    double arr1[] = {1, 2, 3, 4, 5, 6, 73, 8, 9};
    Matrix m1(arr1, 3, 3);
    m1.print();
    double det = m1.determinant();
    printf("Определитель матрицы: %lf\n", det);
    Matrix expon = m1.exp();
    expon.print();

    return 0;
}
