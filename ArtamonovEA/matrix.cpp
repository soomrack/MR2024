#include <exception>
#include <iostream>
#include <math.h>
#include <memory>
#include <optional>
#include <stdio.h>
#include <string>
#include <vector>

#include "logging.cpp"


// ------------------- Matrix ------------------- //


class Matrix {
    protected:
        size_t rows;
        size_t cols;
        std::vector<double> data;

    public:
        // Constructors
        Matrix() : rows(0), cols(0) {}
        Matrix(size_t rows, size_t cols): rows(rows), cols(cols), data(rows * cols, 0.0) {}
        Matrix(const Matrix &M) : rows(M.rows), cols(M.cols), data(M.data) {}
        Matrix(Matrix &&M) noexcept : rows(M.rows), cols(M.cols), data(std::move(M.data)) {}

        // Destructor
        ~Matrix() = default;

        double& operator()(const size_t row_num,const size_t col_num);
        const double& operator()(const size_t row_num, const size_t col_num) const;

        // Getters
        size_t get_rows() const noexcept { return rows; }
        size_t get_cols() const noexcept { return rows; }

        // Arithmetic operations with other matrices
        Matrix operator+(const Matrix &Other);
        Matrix operator-(const Matrix &Other);
        Matrix operator*(const Matrix &Other);
        Matrix operator/(const Matrix &Other)  { throw UndefinedOperationException(); };

        // Arithmetic operations with scalars
        Matrix operator+(const double &Scalar);
        Matrix operator-(const double &Scalar);
        Matrix operator*(const double &Scalar);
        Matrix operator/(const double &Scalar);
        
        // Assignment operations with other matrices
        Matrix& operator=(const Matrix &Other);
        Matrix& operator+=(const Matrix &Other);
        Matrix& operator-=(const Matrix &Other);
        Matrix& operator*=(const Matrix &Other);
        Matrix& operator/=(const Matrix &Other)  { throw UndefinedOperationException(); };

        // Assignment operations with scalars
        Matrix& operator+=(const double &Scalar);
        Matrix& operator-=(const double &Scalar);
        Matrix& operator*=(const double &Scalar);
        Matrix& operator/=(const double &Scalar);

        // Relational operations
        bool equal_size(const Matrix &Other) const noexcept;
        bool operator==(const Matrix &Other);
        bool operator!=(const Matrix &Other);  

        // Other operations
        std::optional<Matrix> transpose(const bool &inplace = false);
        void print() const noexcept;

        // Friend classes
        friend class SquareMatrix;
        friend class ZeroMatrix;
};


double& Matrix::operator()(const size_t row_num, const size_t col_num) {
    if (row_num >= rows || col_num >= cols) {
        throw SizeMismatchException();
    }
    return data[row_num * cols + col_num];
}


const double& Matrix::operator()(const size_t row_num, const size_t col_num) const
{
    if (row_num >= rows || col_num >= cols) {
        throw SizeMismatchException();
    }

    return data[row_num * cols + col_num];
}


Matrix Matrix::operator+(const Matrix &Other) {
    if (!equal_size(Other)) {
        throw SizeMismatchException();
    }

    Matrix result(rows, cols);
    
    for (size_t idx = 0; idx < rows * cols; ++idx) {
        result.data[idx] = data[idx] + Other.data[idx];
    }

    return result;
}


Matrix Matrix::operator-(const Matrix &Other) 
{
    if (!equal_size(Other)) {
        throw SizeMismatchException();
    }

    Matrix result(rows, cols);
    
    for (size_t idx = 0; idx < rows * cols; ++idx) {
        result.data[idx] = data[idx] - Other.data[idx];
    }

    return result;
}


Matrix Matrix::operator*(const Matrix &Other) 
{
    if (cols != Other.rows) {
        throw SizeMismatchException(
            "The number of columns in the first matrix must be equal to the number of rows in the second matrix. "
            "Got " + std::to_string(cols) + " and " + std::to_string(Other.rows) + " respectively."
        );
    }

    Matrix result(rows, Other.cols);

    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < Other.cols; ++col) {
            result.data[row * Other.cols + col] = 0;
            for (size_t idx = 0; idx < cols; ++idx) {
                result.data[row * Other.cols + col] += data[row * cols + idx] * Other.data[idx * Other.cols + col];
            }
        }
    }

    return result;
}


Matrix Matrix::operator+(const double &Scalar) 
{
    Matrix result(rows, cols);
    
    for (size_t idx = 0; idx < rows * cols; ++idx) {
        result.data[idx] = data[idx] + Scalar;
    }

    return result;
}


Matrix Matrix::operator-(const double &Scalar) 
{
    Matrix result(rows, cols);
    
    for (size_t idx = 0; idx < rows * cols; ++idx) {
        result.data[idx] = data[idx] - Scalar;
    }

    return result;
}


Matrix Matrix::operator*(const double &Scalar) 
{
    Matrix result(rows, cols);
    
    for (size_t idx = 0; idx < rows * cols; ++idx) {
        result.data[idx] = data[idx] * Scalar;
    }

    return result;
}


Matrix Matrix::operator/(const double &Scalar) 
{
    if (Scalar == 0) {
        throw DivisionByZeroException();
    }

    Matrix result(rows, cols);
    
    for (size_t idx = 0; idx < rows * cols; ++idx) {
        result.data[idx] = data[idx] / Scalar;
    }

    return result;
}


Matrix& Matrix::operator=(const Matrix &Other) 
{
    if (this == &Other) {
        return *this;
    }
    
    rows = Other.rows;
    cols = Other.cols;
    data = Other.data;

    return *this;
}


Matrix& Matrix::operator+=(const Matrix &Other) 
{
    if (rows != Other.rows || cols != Other.cols) {
        throw SizeMismatchException();
    }

    for (size_t idx = 0; idx < rows * cols; ++idx) {
        data[idx] += Other.data[idx];
    }

    return *this;
}


Matrix& Matrix::operator-=(const Matrix &Other) 
{
    if (rows != Other.rows || cols != Other.cols) {
        throw SizeMismatchException();
    }

    for (size_t idx = 0; idx < rows * cols; ++idx) {
        data[idx] -= Other.data[idx];
    }

    return *this;
}


Matrix& Matrix::operator*=(const Matrix &Other) 
{
    if (cols != Other.rows) {
        throw SizeMismatchException();
    }

    Matrix result(rows, Other.cols);

    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < Other.cols; ++col) {
            result.data[row * Other.cols + col] = 0;
            for (size_t idx = 0; idx < cols; ++idx) {
                result.data[row * Other.cols + col] += data[row * cols + idx] * Other.data[idx * Other.cols + col];
            }
        }
    }

    *this = result;
    return *this;
}


Matrix& Matrix::operator+=(const double &Scalar) 
{
    for (size_t idx = 0; idx < rows * cols; ++idx) {
        data[idx] += Scalar;
    }

    return *this;
}


Matrix& Matrix::operator-=(const double &Scalar) 
{
    for (size_t idx = 0; idx < rows * cols; ++idx) {
        data[idx] -= Scalar;
    }

    return *this;
}


Matrix& Matrix::operator*=(const double &Scalar) 
{    
    for (size_t idx = 0; idx < rows * cols; ++idx) {
        data[idx] *= Scalar;
    }

    return *this;
}


Matrix& Matrix::operator/=(const double &Scalar) 
{
    for (size_t idx = 0; idx < rows * cols; ++idx) {
        data[idx] /= Scalar;
    }

    return *this;
}


bool Matrix::equal_size(const Matrix &Other) const noexcept 
{
    return rows == Other.rows && cols == Other.cols;
}


bool Matrix::operator==(const Matrix &Other) 
{
    if (!equal_size(Other)) {
        return false;
    }

    for (size_t idx = 0; idx < rows * cols; ++idx) {
        if (data[idx] != Other.data[idx]) {
            return false;
        }
    }

    return true;
}


bool Matrix::operator!=(const Matrix &Other) 
{
    return !(*this == Other);
}


std::optional<Matrix> Matrix::transpose(const bool &inplace) 
{
    Matrix result(cols, rows);

    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            result.data[col * rows + row] = data[row * cols + col];
        }
    }

    if (inplace) {
        *this = result;
        return std::nullopt;
    }

    return result;
}


void Matrix::print() const noexcept 
{
    if (rows == 0 || cols == 0) {
        std::cout << "Empty matrix." << std::endl << std::endl;
        return;
    }

    for (size_t row_idx = 0; row_idx < rows; ++row_idx) {
        for (size_t col_idx = 0; col_idx < cols; ++col_idx) {
            std::cout << data[row_idx * cols + col_idx];
            col_idx == cols - 1 ? std::cout << std::endl : std::cout << '\t';
        }
    }
    std::cout << std::endl;
}

// ---------------- Zero Matrix ----------------- //


class ZeroMatrix : public Matrix {
    public:
        ZeroMatrix(): Matrix() {}
        ZeroMatrix(size_t rows, size_t cols): Matrix(rows, cols) {std::fill(data.begin(), data.end(), 0.0);}
        ZeroMatrix(const Matrix &M): Matrix(M) {std::fill(data.begin(), data.end(), 0.0);}
};


// --------------- Square Matrix ---------------- //


class SquareMatrix : public Matrix {
    public:
        SquareMatrix(size_t size): Matrix(size, size) {}
        SquareMatrix(const Matrix &M);

        SquareMatrix identity(const bool &inplace = false);
        SquareMatrix minor(const size_t &row_exclude, const size_t &col_exclude);

        double determinant();

        std::optional<SquareMatrix> inverse(const bool &inplace = false);
        std::optional<SquareMatrix> pow(unsigned int power, const bool &inplace = false);
        std::optional<SquareMatrix> exp(const unsigned int &iterations, const bool &inplace = false);

        SquareMatrix operator/(SquareMatrix &Other);
        SquareMatrix& operator/=(SquareMatrix &Other);

        SquareMatrix operator/(const double &Scalar) const;
        SquareMatrix& operator/=(const double &Scalar);

};


SquareMatrix::SquareMatrix(const Matrix &M): Matrix(M)
{
    if (M.rows != M.cols) {
        throw SizeMismatchException("Matrix is not square.");
    }
}


SquareMatrix SquareMatrix::identity(const bool &inplace) 
{
    SquareMatrix identity_matrix(rows);

    for (size_t idx = 0; idx < rows * rows; idx++) {
        identity_matrix.data[idx] = 1.0;
    }

    if (inplace) {
        *this = identity_matrix;
        return *this;
    }

    return identity_matrix;
}


SquareMatrix SquareMatrix::minor(const size_t &row_exclude, const size_t &col_exclude) 
{
    SquareMatrix minor(rows - 1);

    size_t sub_row = 0;

    for (size_t row = 0; row < rows; ++row) {
        if (row == row_exclude) { continue; }
        size_t sub_col = 0;

        for (size_t col = 0; col < cols; ++col) {
            if (col == col_exclude) { continue; }
            minor(sub_row, sub_col) = (*this)(row, col);
            ++sub_col;
        }

        ++sub_row;
    }

    return minor;
}


double SquareMatrix::determinant() 
{
    if (rows == 0 || cols == 0) {
        throw SizeMismatchException("Matrix is not square.");
    }

    if (rows == 1) {
        return data[0];
    }

    if (rows == 2) {
        return data[0] * data[3] - data[1] * data[2];
    }

    double det = 0;

    for (size_t col = 0; col < cols; ++col) {
        SquareMatrix minor_matrix = minor(0, col);
        det += (col % 2 == 0 ? 1 : -1) * data[col] *  minor_matrix.determinant();
    }

    return det;
}


std::optional<SquareMatrix> SquareMatrix::inverse(const bool &inplace) 
{
    double det = determinant();

    if (fabs(det) < 1e-12) {
        throw MatrixException("Matrix's determinant too is small.");
    }

    if (isnan(det)) {
        throw MatrixException("Matrix's determinant is NaN.");
    }

    SquareMatrix cofactor_matrix(rows);

    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            SquareMatrix minor_matrix = minor(row, col);
            double minor_det = minor_matrix.determinant();

            cofactor_matrix(row, col) = ((row + col) % 2 == 0 ? 1 : -1) * minor_det;
        }
    }

    cofactor_matrix.transpose(true);
    cofactor_matrix /= det;

    if (inplace) {
        *this = cofactor_matrix;
        return std::nullopt;
    }
    return cofactor_matrix;
}


std::optional<SquareMatrix> SquareMatrix::pow(unsigned int power, const bool &inplace) 
{
    if (power == 0) { return identity(inplace); }

    SquareMatrix result(*this);
    SquareMatrix base(*this);

    for (uint idx = 1; idx < power; ++idx) {
        result *= base;
    }

    if (inplace) {
        *this = result;
        return *this;
    }

    return result;
}


std::optional<SquareMatrix> SquareMatrix::exp(const unsigned int &iterations, const bool &inplace) 
{
    if (iterations == 0) {
        return *this;
    }

    SquareMatrix matrix_exponent_term = identity();
    SquareMatrix matrix_exponent_result = identity();

    double factorial = 1.0;

    for (unsigned long int idx = 1; idx <= iterations; ++idx) {
        matrix_exponent_term *= *this;
        factorial *= idx;

        matrix_exponent_result += matrix_exponent_term / factorial;
    }

    if (inplace) {
        *this = matrix_exponent_result;
        return *this;
    }

    return matrix_exponent_result;
}


SquareMatrix SquareMatrix::operator/(SquareMatrix &Other) 
{
    SquareMatrix result = *this;
    result *= Other.inverse() ? *Other.inverse() : SquareMatrix(Other.rows);
    return result;
}


SquareMatrix& SquareMatrix::operator/=(SquareMatrix &Other) 
{
    *this = *this / Other;
    return *this;
}

SquareMatrix& SquareMatrix::operator/=(const double &Scalar) {
    if (Scalar == 0) {
        throw DivisionByZeroException();
    }
    for (size_t idx = 0; idx < data.size(); ++idx) {
        data[idx] /= Scalar;
    }
    return *this;
}

SquareMatrix SquareMatrix::operator/(const double &Scalar) const {
    if (Scalar == 0) {
        throw DivisionByZeroException();
    }
    SquareMatrix result(*this);
    for (size_t idx = 0; idx < result.data.size(); ++idx) {
        result.data[idx] /= Scalar;
    }
    return result;
}