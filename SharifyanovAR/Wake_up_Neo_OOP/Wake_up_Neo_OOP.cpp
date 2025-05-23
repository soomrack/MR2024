#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <cmath>
#include <cstring>  
#include <sstream>


class MatrixException : public std::exception {
private:
    std::string message;
public:
    explicit MatrixException(const std::string& msg);
    virtual const char* what() const noexcept override;
};


MatrixException::MatrixException(const std::string& msg) : message(msg) {}


const char* MatrixException::what() const noexcept {
    return message.c_str();
}


class Matrix {
private:
    size_t rows;
    size_t cols;
    double* data;

    void allocateMemory();

public:
    Matrix();                                  // empty construct
    Matrix(size_t rows, size_t cols);          // parametric construct
    Matrix(const Matrix& other);               // copying construct
    Matrix(Matrix&& other) noexcept;           // move construct

    // kill matrix
    ~Matrix();


    Matrix& operator=(const Matrix& other);
    Matrix& operator=(Matrix&& other) noexcept;

    void matrix_set(const double* values);
    void matrix_identity();
    double matrix_determinant() const;

    Matrix operator+(const Matrix& other) const;
    Matrix& operator+=(Matrix& other);

    Matrix operator*(const Matrix& other) const;
    //Matrix& operator*=(const Matrix&& other) const;
    Matrix operator*(double scalar) const;
    //Matrix& operator*=(double scalar) const;

    Matrix matrix_transpose() const;
    Matrix matrix_exponent(unsigned long k = 10) const;
    void matrix_print(const std::string& title, double scalar_result = NAN) const;

    size_t getRows() const;
    size_t getCols() const;
};


void Matrix::allocateMemory() {
    if (rows == 0 || cols == 0)
        throw MatrixException("Matrix dimensions cannot be zero");

    if (rows > SIZE_MAX / cols)
        throw MatrixException("Matrix dimensions are too large");

    data = new double[rows * cols];

    memset(data, 0, cols * rows * sizeof(double));
}


//Matrix::Matrix() : rows(0), cols(0), data(nullptr) {}


Matrix::Matrix(size_t r, size_t c) : rows(r), cols(c), data(nullptr) {
    allocateMemory();
}


Matrix::Matrix(const Matrix& other) : rows(other.rows), cols(other.cols), data(nullptr) {
    if (other.data == nullptr || rows * cols == 0) return; //check if matrix nan

    allocateMemory();
    std::memcpy(data, other.data, rows * cols * sizeof(double));
}


Matrix::Matrix(Matrix&& other) noexcept: rows(other.rows), cols(other.cols), data(other.data) {
    // Just move poiner with no copying data
    other.rows = 0;
    other.cols = 0;
    other.data = nullptr;
}


Matrix::~Matrix() {
    delete[] data;
}


Matrix& Matrix::operator=(const Matrix& other) {
    if (this == &other)
        return *this;
    if (rows != other.rows || cols != other.cols)
        throw MatrixException("Assignment failed: matrix sizes do not match");
    std::memcpy(data, other.data, rows * cols * sizeof(double));
    return *this;
}


Matrix& Matrix::operator=(Matrix&& other) noexcept {
    delete[] data;
    rows = other.rows;
    cols = other.cols;
    data = other.data;
    other.rows = 0;
    other.cols = 0;
    other.data = nullptr;
    return *this;
}


void Matrix::matrix_set(const double* values) {
    if (values == nullptr || rows * cols == 0)
        return;
    std::memcpy(data, values, rows * cols * sizeof(double));
}


void Matrix::matrix_identity() {
    if (rows != cols)
        throw MatrixException("Matrix must be square for identity operation");

   //for (size_t i = 0; i < rows * cols; ++i)
   //    data[i] = 0.0;

   //Matrix::matrix_set(0);

    memset(data, 0, cols * rows * sizeof(double)); //тут не совсем понял как сделать этот блок полностью через memset

    for (size_t i = 0; i < rows; ++i)
        data[i * cols + i] = 1.0;
}


double Matrix::matrix_determinant() const {
    if (rows != cols)
        throw MatrixException("Matrix must be square to calculate determinant");

    if (rows == 1) {
        return data[0];
    }

    if (rows == 2) {
        return data[0] * data[3] - data[1] * data[2];
    }

    if (rows == 3) {
        return    data[0] * data[4] * data[8]
                + data[1] * data[5] * data[6]
                + data[3] * data[7] * data[2]
                - data[2] * data[4] * data[6]
                - data[3] * data[1] * data[8]
                - data[0] * data[5] * data[7];
    }
    else throw MatrixException("Determinant is not implemented for matrices larger than 3x3");
}


// overloaded add operator (same to matrix_add)
Matrix Matrix::operator+(const Matrix& other) const {
    if (rows != other.rows || cols != other.cols)
        throw MatrixException("Matrix dimensions must match for addition");

    Matrix result(rows, cols);

    for (size_t i = 0; i < rows * cols; ++i)
        result.data[i] = data[i] + other.data[i];
    return result;
}



Matrix& Matrix::operator+=(Matrix& other) {
    if (rows != other.rows || cols != other.cols)
        throw MatrixException("Matrix dimensions must match for addition");

    //Matrix result(rows, cols);

    for (size_t i = 0; i < rows * cols; ++i)
        this->data[i] = data[i] + other.data[i];
    return *this;
}





// overloaded multiplier operator (same to matrix_multiply).
Matrix Matrix::operator*(const Matrix& other) const {
    if (cols != other.rows)
        throw MatrixException("Matrix dimensions are invalid for multiplication");
    Matrix result(rows, other.cols);
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < other.cols; ++col) {
            double sum = 0.0;
            for (size_t k = 0; k < cols; ++k)
                sum += data[row * cols + k] * other.data[k * other.cols + col];
            result.data[row * other.cols + col] = sum;
        }
    }
    return result;
}


/*
Matrix& Matrix::operator*=(const Matrix&& other) const {
    if (cols != other.rows)
        throw MatrixException("Matrix dimensions are invalid for multiplication");
    Matrix result(rows, other.cols);
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < other.cols; ++col) {
            double sum = 0.0;
            for (size_t k = 0; k < cols; ++k)
                sum += data[row * cols + k] * other.data[k * other.cols + col];
            result.data[row * other.cols + col] = sum;
        }
    }
    return result;
}
*/


// overloaded multiplier on scalar operator (same to matrix_scalar_multiply).
Matrix Matrix::operator*(double scalar) const {
    Matrix result(rows, cols);
    for (size_t i = 0; i < rows * cols; ++i)
        result.data[i] = data[i] * scalar;
    return result;
}


/*
Matrix& Matrix::operator*=(double scalar) const {
    Matrix result(rows, cols);
    for (size_t i = 0; i < rows * cols; ++i)
        result.data[i] = data[i] * scalar;
    return result;
}
*/



// transpose function from 5th semester (procedure C matrix code)
Matrix Matrix::matrix_transpose() const {
    Matrix result(cols, rows);
    for (size_t row = 0; row < rows; ++row)
        for (size_t col = 0; col < cols; ++col)
            result.data[col * rows + row] = data[row * cols + col];
    return result;
}


//e^A
Matrix Matrix::matrix_exponent(unsigned long k) const {
    if (rows != cols)
        throw MatrixException("Matrix must be square for exponentiation");

    Matrix result(rows, cols);
    result.matrix_identity();

    Matrix term(result);

    for (unsigned long i = 1; i <= k; ++i) {
        term = (*this * term) * (1.0 / i);
        result += term;
    }
    return result;
}



/*// e ^ A
Matrix Matrix::matrix_exponent() const {
    if (rows != cols)
        throw MatrixException("Matrix must be square for exponentiation");

    Matrix result(rows, cols);
    result.matrix_identity();

    Matrix tmp(result);
    Matrix buf_mult(rows, cols);

    for (size_t num = 1; num <= 10; ++num) {
        buf_mult = tmp * (*this);

        buf_mult *= (1.0 / num);

        result += buf_mult;

        tmp = buf_mult;
    }
    return result;
}*/





void Matrix::matrix_print(const std::string& title, double scalar_result) const {
    std::cout << "\n" << title << ":\n";
    if (data != nullptr) {
        for (size_t row = 0; row < rows; ++row) {
            for (size_t col = 0; col < cols; ++col)
                std::cout << std::setw(6) << data[row * cols + col] << " ";
            std::cout << "\n";
        }
    }
    else {
        std::cout << scalar_result << "\n";
    }
}


size_t Matrix::getRows() const { 
    return rows; 
}


size_t Matrix::getCols() const { 
    return cols; 
}


int main() {

    Matrix A(2, 2);
    double valuesA[] = { 2.0, 0.0, 7.0, 7.0 };
    A.matrix_set(valuesA);


    Matrix B(2, 2);
    double valuesB[] = { 1.0, 3.0, 3.0, 7.0 };
    B.matrix_set(valuesB);

    Matrix A_scaled(A);

    A_scaled = A_scaled * 2.0;
    A_scaled.matrix_print("Matrix A multiplied by scalar 2");

    double detA = A_scaled.matrix_determinant();
    std::cout << "Determinant of A_scaled: " << detA << "\n\n";

    Matrix sumAB = A_scaled + B;
    sumAB.matrix_print("Sum of A_scaled and B");

    Matrix productAB = A_scaled * B;
    productAB.matrix_print("Product of A_scaled and B");

    Matrix expA = A_scaled.matrix_exponent(50);
    expA.matrix_print("Exponent of A_scaled (e^A)");

    Matrix transposeA = A_scaled.matrix_transpose();
    transposeA.matrix_print("Transpose of A_scaled");

    return 0;
}
