#include <iostream>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <cmath>
#include <algorithm>

class MatrixException : public std::runtime_error {
public:
    explicit MatrixException(const std::string& msg) : std::runtime_error(msg) {}
};

class Matrix {
    size_t rows_, cols_;
    std::vector<double> data_;
public:
    Matrix(size_t rows, size_t cols)
        : rows_(rows), cols_(cols), data_(rows* cols, 0.0) {
        if (rows == 0 || cols == 0)
            throw MatrixException("Matrix dimensions must be non-zero");
    }

    size_t rows() const { return rows_; }
    size_t cols() const { return cols_; }

    double& at(size_t row, size_t col) {
        if (row >= rows_ || col >= cols_)
            throw MatrixException("Index out of bounds");
        return data_[row * cols_ + col];
    }

    double at(size_t row, size_t col) const {
        if (row >= rows_ || col >= cols_)
            throw MatrixException("Index out of bounds");
        return data_[row * cols_ + col];
    }

    void fill_with_indices() {
        for (size_t i = 0; i < rows_; ++i)
            for (size_t j = 0; j < cols_; ++j)
                at(i, j) = i * cols_ + j;
    }

    void set_zero() {
        std::fill(data_.begin(), data_.end(), 0.0);
    }

    void set_identity() {
        set_zero();
        size_t n = std::min(rows_, cols_);
        for (size_t i = 0; i < n; ++i)
            at(i, i) = 1.0;
    }

    void set_value(size_t idx, double value) {
        if (idx >= data_.size())
            throw MatrixException("Index out of bounds");
        data_[idx] = value;
    }

    double determinant() const {
        if (rows_ != cols_)
            throw MatrixException("Determinant is defined for square matrices only");
        if (rows_ == 1)
            return data_[0];
        if (rows_ == 2)
            return at(0, 0) * at(1, 1) - at(0, 1) * at(1, 0);
        if (rows_ == 3)
            return at(0, 0) * at(1, 1) * at(2, 2) + at(0, 1) * at(1, 2) * at(2, 0) + at(0, 2) * at(1, 0) * at(2, 1)
            - at(0, 2) * at(1, 1) * at(2, 0) - at(0, 0) * at(1, 2) * at(2, 1) - at(0, 1) * at(1, 0) * at(2, 2);
        throw MatrixException("Determinant not implemented for matrices larger than 3x3");
    }

    Matrix transpose() const {
        Matrix result(cols_, rows_);
        for (size_t i = 0; i < rows_; ++i)
            for (size_t j = 0; j < cols_; ++j)
                result.at(j, i) = at(i, j);
        return result;
    }

    // Операторы
    double& operator[](size_t idx) { return data_.at(idx); }
    double operator[](size_t idx) const { return data_.at(idx); }

    Matrix& operator+=(const Matrix& other) {
        if (rows_ != other.rows_ || cols_ != other.cols_)
            throw MatrixException("Matrix dimensions must match for addition");
        for (size_t i = 0; i < data_.size(); ++i)
            data_[i] += other.data_[i];
        return *this;
    }

    Matrix& operator*=(double k) {
        for (auto& v : data_) v *= k;
        return *this;
    }

    Matrix operator+(const Matrix& other) const {
        Matrix res = *this;
        res += other;
        return res;
    }

    Matrix operator-(const Matrix& other) const {
        if (rows_ != other.rows_ || cols_ != other.cols_)
            throw MatrixException("Matrix dimensions must match for subtraction");
        Matrix res(rows_, cols_);
        for (size_t i = 0; i < data_.size(); ++i)
            res.data_[i] = data_[i] - other.data_[i];
        return res;
    }

    Matrix operator*(const Matrix& other) const {
        if (cols_ != other.rows_)
            throw MatrixException("Matrix dimensions do not allow multiplication");
        Matrix res(rows_, other.cols_);
        for (size_t i = 0; i < rows_; ++i)
            for (size_t j = 0; j < other.cols_; ++j)
                for (size_t k = 0; k < cols_; ++k)
                    res.at(i, j) += at(i, k) * other.at(k, j);
        return res;
    }

    Matrix pow(unsigned int p) const {
        if (rows_ != cols_)
            throw MatrixException("Power is defined for square matrices only");
        Matrix res(*this);
        if (p == 0) {
            res.set_identity();
            return res;
        }
        for (unsigned int i = 1; i < p; ++i)
            res = res * (*this);
        return res;
    }

    void print(std::ostream& os = std::cout) const {
        for (size_t i = 0; i < rows_; ++i) {
            for (size_t j = 0; j < cols_; ++j)
                os << std::setw(8) << at(i, j) << ' ';
            os << '\n';
        }
        os << '\n';
    }
};

int main() {
    try {
        Matrix A(3, 3);
        Matrix B(3, 3);
        B.fill_with_indices();
        A = B;
        A.print();
        A *= 2;
        A.print();
        double res = A[2];
        std::cout << res << std::endl;
        A.set_value(0, 2.3);
        A.print();
        std::cout << "Determinant: " << A.determinant() << std::endl;
        Matrix C = B - A;
        C.print();

        Matrix D(2, 3), E(3, 4);
        D.fill_with_indices();
        E.fill_with_indices();
        D.print();
        E.print();
        Matrix F = D * E;
        F.print();
        Matrix G = F.transpose();
        G.print();

        Matrix J = B.pow(2);
        J.print();

        Matrix P(2, 2), Q(2, 2), T(2, 2);
        P.fill_with_indices();
        Q.fill_with_indices();
        T.fill_with_indices();
        Matrix U = T.transpose();
        Matrix R = P + Q * U;
        R.print();
    }
    catch (const MatrixException& ex) {
        std::cerr << "Matrix error: " << ex.what() << std::endl;
    }
}
