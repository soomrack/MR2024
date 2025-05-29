#include <iostream>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <locale>

#ifdef _WIN32
#include <windows.h>
#endif

// ����� ���������� ��� ������, ��������� � ���������
class MatrixException : public std::runtime_error {
public:
    explicit MatrixException(const std::string& msg) : std::runtime_error(msg) {}
};

// ����� ��� ������ � ��������� 
class Matrix {
    size_t rows_, cols_;          
    std::vector<double> data_;    
public:
    // �����������: ������ ������� ��������� ������� � �������������� ������
    Matrix(size_t rows, size_t cols)
        : rows_(rows), cols_(cols), data_(rows* cols, 0.0) {
        if (rows == 0 || cols == 0)
            throw MatrixException("������� ������� ������ ���� ����������");
    }

    // �������� ����� �����
    size_t rows() const { return rows_; }
    // �������� ����� ��������
    size_t cols() const { return cols_; }

    // ������ � �������� ������� � ��������� �������� (��� ������)
    double& at(size_t row, size_t col) {
        if (row >= rows_ || col >= cols_)
            throw MatrixException("������ ��� ���������");
        return data_[row * cols_ + col];
    }

    // ������ � �������� ������� � ��������� �������� (��� ������)
    double at(size_t row, size_t col) const {
        if (row >= rows_ || col >= cols_)
            throw MatrixException("������ ��� ���������");
        return data_[row * cols_ + col];
    }

    // ��������� ������� ����������, ������� �������� ��������� (���������)
    void fill_with_indices() {
        for (size_t i = 0; i < rows_; ++i)
            for (size_t j = 0; j < cols_; ++j)
                at(i, j) = i * cols_ + j;
    }

    // ��������� ������� ������
    void set_zero() {
        std::fill(data_.begin(), data_.end(), 0.0);
    }

    // ������� ������� ��������� (1 �� ���������, 0 � ��������� ������)
    void set_identity() {
        set_zero();
        size_t n = min(rows_, cols_);
        for (size_t i = 0; i < n; ++i)
            at(i, i) = 1.0;
    }

    // ���������� �������� �� �������� ������� � ������� 
    void set_value(size_t idx, double value) {
        if (idx >= data_.size())
            throw MatrixException("������ ��� ���������");
        data_[idx] = value;
    }

    // ��������� ������������ ������� 
    double determinant() const {
        if (rows_ != cols_)
            throw MatrixException("������������ �������� ������ ��� ���������� ������");
        if (rows_ == 1)
            return data_[0];
        if (rows_ == 2)
            return at(0, 0) * at(1, 1) - at(0, 1) * at(1, 0);
        if (rows_ == 3)
            return at(0, 0) * at(1, 1) * at(2, 2) + at(0, 1) * at(1, 2) * at(2, 0) + at(0, 2) * at(1, 0) * at(2, 1)
            - at(0, 2) * at(1, 1) * at(2, 0) - at(0, 0) * at(1, 2) * at(2, 1) - at(0, 1) * at(1, 0) * at(2, 2);
        throw MatrixException("������������ �� ���������� ��� ������ ������ 3x3");
    }

    // ��������������� ������� 
    Matrix transpose() const {
        Matrix result(cols_, rows_);
        for (size_t i = 0; i < rows_; ++i)
            for (size_t j = 0; j < cols_; ++j)
                result.at(j, i) = at(i, j);
        return result;
    }

    
    double& operator[](size_t idx) { return data_.at(idx); }
    double operator[](size_t idx) const { return data_.at(idx); }

    // ������������ ��������
    Matrix& operator+=(const Matrix& other) {
        if (rows_ != other.rows_ || cols_ != other.cols_)
            throw MatrixException("������� ������ ������ ��������� ��� ��������");
        for (size_t i = 0; i < data_.size(); ++i)
            data_[i] += other.data_[i];
        return *this;
    }

    // ��������� ���� ��������� ������� �� ����� � �������������
    Matrix& operator*=(double k) {
        for (auto& v : data_) v *= k;
        return *this;
    }

    // �������� �������� 
    Matrix operator+(const Matrix& other) const {
        Matrix res = *this;
        res += other;
        return res;
    }

    // �������� ��������� 
    Matrix operator-(const Matrix& other) const {
        if (rows_ != other.rows_ || cols_ != other.cols_)
            throw MatrixException("������� ������ ������ ��������� ��� ���������");
        Matrix res(rows_, cols_);
        for (size_t i = 0; i < data_.size(); ++i)
            res.data_[i] = data_[i] - other.data_[i];
        return res;
    }

    // �������� ��������� ������
    Matrix operator*(const Matrix& other) const {
        if (cols_ != other.rows_)
            throw MatrixException("������� ������ �� �������� ��� ���������");
        Matrix res(rows_, other.cols_);
        for (size_t i = 0; i < rows_; ++i)
            for (size_t j = 0; j < other.cols_; ++j)
                for (size_t k = 0; k < cols_; ++k)
                    res.at(i, j) += at(i, k) * other.at(k, j);
        return res;
    }

    // ���������� ������� � ������������� ������� 
    Matrix pow(unsigned int p) const {
        if (rows_ != cols_)
            throw MatrixException("���������� � ������� ���������� ������ ��� ���������� ������");
        Matrix res(*this);
        if (p == 0) {
            res.set_identity();
            return res;
        }
        for (unsigned int i = 1; i < p; ++i)
            res = res * (*this);
        return res;
    }

    // ������ ������� � ����� ������
    void print(std::ostream& os = std::cout) const {
        for (size_t i = 0; i < rows_; ++i) {
            for (size_t j = 0; j < cols_; ++j)
                os << std::setw(8) << at(i, j) << ' ';
            os << '\n';
        }
        os << '\n';
    }

    // ��������� ��������
    void print_with_label(const std::string& label, std::ostream& os = std::cout) const {
        os << label << ":\n";
        print(os);
    }
};

int main() {
#ifdef _WIN32
    
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    std::locale::global(std::locale(""));
    std::cout.imbue(std::locale());

    try {
        // �������� � ���������� ������
        Matrix A(3, 3);
        Matrix B(3, 3);
        B.fill_with_indices(); // ���������� B ���������
        A = B;                 // ����������� B � A
        A.print_with_label("������� A (����� B)");

        A *= 2;                // ��������� A �� 2
        A.print_with_label("������� A ����� ��������� �� 2");

        double res = A[2];     // ��������� �������� �� �������� �������
        std::cout << "������� A[2]: " << res << "\n\n";

        A.set_value(0, 2.3);   // ��������� �������� 2.3 � ������ �������
        A.print_with_label("������� A ����� ��������� �������� 2.3 � ������ 0");

        std::cout << "������������ ������� A: " << A.determinant() << "\n\n";

        Matrix C = B - A;      // ��������� ������
        C.print_with_label("������� C = B - A");

        Matrix D(2, 3), E(3, 4);
        D.fill_with_indices();
        E.fill_with_indices();
        D.print_with_label("������� D (2x3) � ���������");
        E.print_with_label("������� E (3x4) � ���������");

        Matrix F = D * E;      // ��������� ������
        F.print_with_label("������� F = D * E");

        Matrix G = F.transpose(); // ���������������� �������
        G.print_with_label("������� G = ����������������� F");

        Matrix J = B.pow(2);   // ���������� ������� B � �������
        J.print_with_label("������� J = B � ��������");

        Matrix P(2, 2), Q(2, 2), T(2, 2);
        P.fill_with_indices();
        Q.fill_with_indices();
        T.fill_with_indices();
        Matrix U = T.transpose();
        Matrix R = P + Q * U;  // �������� � ��������� ������
        R.print_with_label("������� R = P + Q * U (U = ����������������� T)");
    }
    catch (const MatrixException& ex) {
        std::cerr << "������ �������: " << ex.what() << std::endl;
    }
}
