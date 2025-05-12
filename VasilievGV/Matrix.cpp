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

class Mat {
private:
    size_t h_, w_;
    double* buffer_;

    void check_dimensions(const Mat& other) const {
        if (h_ != other.h_ || w_ != other.w_)
            throw ERR_DIM;
    }

public:
    Mat();
    Mat(size_t rows, size_t cols);
    Mat(size_t rows, size_t cols, const double* source);
    Mat(const Mat& other);
    Mat(Mat&& other) noexcept;
    ~Mat();

    Mat& operator=(const Mat& rhs);
    Mat& operator=(Mat&& rhs) noexcept;

    Mat operator+(const Mat& rhs) const;
    Mat operator-(const Mat& rhs) const;
    Mat operator*(const Mat& rhs) const;
    Mat operator*(double scalar) const;

    Mat& operator+=(const Mat& rhs);
    Mat& operator-=(const Mat& rhs);
    Mat& operator*=(const Mat& rhs);
    Mat& operator*=(double scalar);

    void print() const;
    void fillRandom();
    void toZero();
    void toIdentity();
    double determinant() const;
    void raiseTo(int power);
    Mat transpose() const;
    Mat exp(unsigned long long terms) const;
};

// --- Implementations ---

Mat::Mat() : h_(0), w_(0), buffer_(nullptr) {}

Mat::Mat(size_t r, size_t c) : h_(r), w_(c), buffer_(nullptr) {
    if (r == 0 || c == 0) return;
    if (r > SIZE_MAX / sizeof(double) / c) throw ERR_MEM;
    buffer_ = new double[r * c];
}

Mat::Mat(size_t r, size_t c, const double* source) : h_(r), w_(c) {
    if (!source) throw ERR_DATA;
    buffer_ = new double[h_ * w_];
    std::memcpy(buffer_, source, sizeof(double) * h_ * w_);
}

Mat::Mat(const Mat& other) : h_(other.h_), w_(other.w_) {
    buffer_ = new double[h_ * w_];
    std::memcpy(buffer_, other.buffer_, sizeof(double) * h_ * w_);
}

Mat::Mat(Mat&& other) noexcept : h_(other.h_), w_(other.w_), buffer_(other.buffer_) {
    other.buffer_ = nullptr;
    other.h_ = other.w_ = 0;
}

Mat::~Mat() {
    delete[] buffer_;
}

Mat& Mat::operator=(const Mat& rhs) {
    if (this == &rhs) return *this;
    delete[] buffer_;
    h_ = rhs.h_;
    w_ = rhs.w_;
    buffer_ = new double[h_ * w_];
    std::memcpy(buffer_, rhs.buffer_, sizeof(double) * h_ * w_);
    return *this;
}

Mat& Mat::operator=(Mat&& rhs) noexcept {
    if (this != &rhs) {
        delete[] buffer_;
        h_ = rhs.h_;
        w_ = rhs.w_;
        buffer_ = rhs.buffer_;
        rhs.buffer_ = nullptr;
        rhs.h_ = rhs.w_ = 0;
    }
    return *this;
}

void Mat::print() const {
    for (size_t i = 0; i < h_; ++i) {
        for (size_t j = 0; j < w_; ++j) {
            std::cout << buffer_[i * w_ + j] << " ";
        }
        std::cout << "\n";
    }
}

void Mat::fillRandom() {
    for (size_t i = 0; i < h_ * w_; ++i)
        buffer_[i] = rand() % 10;
}

void Mat::toZero() {
    if (!buffer_) throw ERR_DATA;
    std::memset(buffer_, 0, sizeof(double) * h_ * w_);
}

void Mat::toIdentity() {
    if (h_ != w_) throw ERR_DIM;
    toZero();
    for (size_t i = 0; i < h_; ++i)
        buffer_[i * w_ + i] = 1.0;
}

double Mat::determinant() const {
    if (h_ != w_) throw ERR_DIM;
    if (h_ == 1) return buffer_[0];
    if (h_ == 2) return buffer_[0] * buffer_[3] - buffer_[1] * buffer_[2];
    if (h_ == 3) {
        return buffer_[0] * (buffer_[4] * buffer_[8] - buffer_[5] * buffer_[7])
             - buffer_[1] * (buffer_[3] * buffer_[8] - buffer_[5] * buffer_[6])
             + buffer_[2] * (buffer_[3] * buffer_[7] - buffer_[4] * buffer_[6]);
    }
    throw ERR_DIM;
}

Mat Mat::transpose() const {
    Mat res(w_, h_);
    for (size_t i = 0; i < h_; ++i)
        for (size_t j = 0; j < w_; ++j)
            res.buffer_[j * h_ + i] = buffer_[i * w_ + j];
    return res;
}

Mat Mat::exp(unsigned long long terms) const {
    if (h_ != w_) throw ERR_DIM;
    Mat result(h_, w_);
    result.toIdentity();
    Mat term(*this);
    for (unsigned long long k = 1; k <= terms; ++k) {
        term = term * (*this) * (1.0 / k);
        result += term;
    }
    return result;
}

void Mat::raiseTo(int power) {
    if (h_ != w_) throw ERR_DIM;
    Mat res(h_, w_);
    res.toIdentity();
    for (int i = 0; i < power; ++i)
        res *= *this;
    *this = res;
}

Mat& Mat::operator+=(const Mat& rhs) {
    check_dimensions(rhs);
    for (size_t i = 0; i < h_ * w_; ++i)
        buffer_[i] += rhs.buffer_[i];
    return *this;
}

Mat& Mat::operator-=(const Mat& rhs) {
    check_dimensions(rhs);
    for (size_t i = 0; i < h_ * w_; ++i)
        buffer_[i] -= rhs.buffer_[i];
    return *this;
}

Mat& Mat::operator*=(const Mat& rhs) {
    if (w_ != rhs.h_) throw ERR_DIM;
    Mat result(h_, rhs.w_);
    for (size_t i = 0; i < h_; ++i) {
        for (size_t j = 0; j < rhs.w_; ++j) {
            double sum = 0;
            for (size_t k = 0; k < w_; ++k) {
                sum += buffer_[i * w_ + k] * rhs.buffer_[k * rhs.w_ + j];
            }
            result.buffer_[i * rhs.w_ + j] = sum;
        }
    }
    *this = result;
    return *this;
}

Mat& Mat::operator*=(double scalar) {
    for (size_t i = 0; i < h_ * w_; ++i)
        buffer_[i] *= scalar;
    return *this;
}

Mat Mat::operator+(const Mat& rhs) const {
    Mat result(*this);
    result += rhs;
    return result;
}

Mat Mat::operator-(const Mat& rhs) const {
    Mat result(*this);
    result -= rhs;
    return result;
}

Mat Mat::operator*(const Mat& rhs) const {
    Mat result(*this);
    result *= rhs;
    return result;
}

Mat Mat::operator*(double scalar) const {
    Mat result(*this);
    result *= scalar;
    return result;
}

// --- Example usage ---

int main() {
    double values[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
    Mat mat(3, 3, values);
    mat.print();

    std::cout << "\nAfter multiplying by 10:\n";
    Mat scaled = mat * 10;
    scaled.print();

    return 0;
}
