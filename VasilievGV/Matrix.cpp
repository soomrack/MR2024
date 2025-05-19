#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cstdlib>
#include <cstdint>

class MatrixError : public std::runtime_error {
public:
    explicit MatrixError(const std::string& msg) : std::runtime_error(msg) {}
};

MatrixError ERR_MEM("Allocation failed.");
MatrixError ERR_DIM("Incompatible dimensions.");
MatrixError ERR_DATA("Matrix buffer is null.");

class Matrix {
private:
    size_t rows_, cols_;
    double* data_;

    void ensure_same_shape(const Matrix& other) const {
        if (rows_ != other.rows_ || cols_ != other.cols_)
            throw ERR_DIM;
    }

public:
    Matrix();
    Matrix(size_t r, size_t c);
    Matrix(size_t r, size_t c, const double* src);
    Matrix(const Matrix& other);
    Matrix(Matrix&& other) noexcept;
    ~Matrix();

    Matrix& operator=(const Matrix& rhs);
    Matrix& operator=(Matrix&& rhs) noexcept;

    Matrix operator+(const Matrix& rhs) const;
    Matrix operator-(const Matrix& rhs) const;
    Matrix operator*(const Matrix& rhs) const;
    Matrix operator*(double scalar) const;

    Matrix& operator+=(const Matrix& rhs);
    Matrix& operator-=(const Matrix& rhs);
    Matrix& operator*=(const Matrix& rhs);
    Matrix& operator*=(double scalar);

    void print() const;

    void randomize();
    void reset();
    void identity();

    double determinant() const;

    void power(int exponent);
    Matrix transpose() const;
    Matrix exp(unsigned long long terms) const;
};

Matrix::Matrix() : rows_(0), cols_(0), data_(nullptr) {}

Matrix::Matrix(size_t r, size_t c) : rows_(r), cols_(c), data_(nullptr) {
    if (r == 0 || c == 0) throw ERR_DIM;

    if (r > SIZE_MAX / sizeof(double) / c)
        throw ERR_MEM;

    data_ = new double[r * c];
}

Matrix::Matrix(size_t r, size_t c, const double* src) : rows_(r), cols_(c) {
    if (!src) throw ERR_DATA;

    data_ = new double[rows_ * cols_];
    std::memcpy(data_, src, sizeof(double) * rows_ * cols_);
}

Matrix::Matrix(const Matrix& other) : rows_(other.rows_), cols_(other.cols_) {
    data_ = new double[rows_ * cols_];
    std::memcpy(data_, other.data_, sizeof(double) * rows_ * cols_);
}

Matrix::Matrix(Matrix&& other) noexcept : rows_(other.rows_), cols_(other.cols_), data_(other.data_) {
    other.data_ = nullptr;
    other.rows_ = other.cols_ = 0;
}

Matrix::~Matrix() {
    delete[] data_;
}

Matrix& Matrix::operator=(const Matrix& rhs) {
    if (this == &rhs) return *this;

    delete[] data_;

    rows_ = rhs.rows_;
    cols_ = rhs.cols_;

    data_ = new double[rows_ * cols_];
    std::memcpy(data_, rhs.data_, sizeof(double) * rows_ * cols_);

    return *this;
}

Matrix& Matrix::operator=(Matrix&& rhs) noexcept {
    if (this != &rhs) {
        delete[] data_;

        rows_ = rhs.rows_;
        cols_ = rhs.cols_;
        data_ = rhs.data_;

        rhs.data_ = nullptr;
        rhs.rows_ = rhs.cols_ = 0;
    }
    return *this;
}

void Matrix::print() const {
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j)
            std::cout << data_[i * cols_ + j] << " ";
        std::cout << "\n";
    }
}

void Matrix::randomize() {
    for (size_t i = 0; i < rows_ * cols_; ++i)
        data_[i] = rand() % 10;
}

void Matrix::reset() {
    if (!data_) throw ERR_DATA;
    std::memset(data_, 0, sizeof(double) * rows_ * cols_);
}

void Matrix::identity() {
    if (rows_ != cols_) throw ERR_DIM;

    reset();

    for (size_t i = 0; i < rows_; ++i)
        data_[i * cols_ + i] = 1.0;
}

double Matrix::determinant() const {
    if (rows_ != cols_) throw ERR_DIM;

    if (rows_ == 1)
        return data_[0];

    if (rows_ == 2)
        return data_[0] * data_[3] - data_[1] * data_[2];

    if (rows_ == 3) {
        return data_[0] * (data_[4] * data_[8] - data_[5] * data_[7])
             - data_[1] * (data_[3] * data_[8] - data_[5] * data_[6])
             + data_[2] * (data_[3] * data_[7] - data_[4] * data_[6]);
    }

    throw ERR_DIM;
}

Matrix Matrix::transpose() const {
    Matrix res(cols_, rows_);

    for (size_t i = 0; i < rows_; ++i)
        for (size_t j = 0; j < cols_; ++j)
            res.data_[j * rows_ + i] = data_[i * cols_ + j];

    return res;
}

Matrix Matrix::exp(unsigned long long terms) const {
    if (rows_ != cols_) throw ERR_DIM;

    Matrix result(rows_, cols_);
    result.identity();

    Matrix term(*this);

    for (unsigned long long k = 1; k <= terms; ++k) {
        term = term * (*this) * (1.0 / k);
        result += term;
    }

    return result;
}

void Matrix::power(int exponent) {
    if (rows_ != cols_) throw ERR_DIM;

    Matrix result(rows_, cols_);
    result.identity();

    for (int i = 0; i < exponent; ++i)
        result *= *this;

    *this = result;
}

Matrix& Matrix::operator+=(const Matrix& rhs) {
    ensure_same_shape(rhs);

    for (size_t i = 0; i < rows_ * cols_; ++i)
        data_[i] += rhs.data_[i];

    return *this;
}

Matrix& Matrix::operator-=(const Matrix& rhs) {
    ensure_same_shape(rhs);

    for (size_t i = 0; i < rows_ * cols_; ++i)
        data_[i] -= rhs.data_[i];

    return *this;
}

Matrix& Matrix::operator*=(const Matrix& rhs) {
    if (cols_ != rhs.rows_) throw ERR_DIM;

    Matrix result(rows_, rhs.cols_);

    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < rhs.cols_; ++j) {
            double sum = 0;

            for (size_t k = 0; k < cols_; ++k)
                sum += data_[i * cols_ + k] * rhs.data_[k * rhs.cols_ + j];

            result.data_[i * rhs.cols_ + j] = sum;
        }
    }

    *this = result;
    return *this;
}

Matrix& Matrix::operator*=(double scalar) {
    for (size_t i = 0; i < rows_ * cols_; ++i)
        data_[i] *= scalar;
    return *this;
}

Matrix Matrix::operator+(const Matrix& rhs) const {
    Matrix result(*this);
    result += rhs;
    return result;
}

Matrix Matrix::operator-(const Matrix& rhs) const {
    Matrix result(*this);
    result -= rhs;
    return result;
}

Matrix Matrix::operator*(const Matrix& rhs) const {
    Matrix result(*this);
    result *= rhs;
    return result;
}

Matrix Matrix::operator*(double scalar) const {
    Matrix result(*this);
    result *= scalar;
    return result;
}

int main() {
    double vals[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    Matrix m(3, 3, vals);

    m.print();

    std::cout << "\nScaled by 10:\n";
    Matrix scaled = m * 10;
    scaled.print();

    std::cout << "\nDeterminant: " << scaled.determinant() << "\n";
    return 0;
}
