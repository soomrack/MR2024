#include <iostream>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <algorithm>
#include <locale>

#ifdef _WIN32
#include <windows.h>
#endif

class MatrixException : public std::runtime_error {
public:
    explicit MatrixException(const std::string& msg);
};

class Matrix {
    size_t rowsn, colsn;
    std::vector<double> datan;

    Matrix(size_t rows, size_t cols, std::vector<double>&& data);
public:
    friend Matrix create_matrix(size_t rows, size_t cols);

    size_t rows() const;
    size_t cols() const;

    double& at(size_t row, size_t col);
    double at(size_t row, size_t col) const;

    void fill_with_indices();
    void set_zero();
    void set_identity();
    void set_value(size_t idx, double value);

    double determinant() const;
    Matrix transpose() const;

    double& operator[](size_t idx);
    double operator[](size_t idx) const;

    Matrix& operator+=(const Matrix& other);
    Matrix& operator*=(double k);
    Matrix operator+(const Matrix& other) const;
    Matrix operator-(const Matrix& other) const;
    Matrix operator*(const Matrix& other) const;
    Matrix pow(unsigned int p) const;

    void print(std::ostream& os = std::cout) const;
    void print_with_label(const std::string& label, std::ostream& os = std::cout) const;
};


MatrixException::MatrixException(const std::string& msg) : std::runtime_error(msg) {}

Matrix::Matrix(size_t rows, size_t cols, std::vector<double>&& data)
    : rowsn(rows), colsn(cols), datan(std::move(data)) {
}

size_t Matrix::rows() const { return rowsn; }
size_t Matrix::cols() const { return colsn; }

double& Matrix::at(size_t row, size_t col) {
    if (row >= rowsn || col >= colsn)
        throw MatrixException("Индекс вне диапазона");
    return datan[row * colsn + col];
}

double Matrix::at(size_t row, size_t col) const {
    if (row >= rowsn || col >= colsn)
        throw MatrixException("Индекс вне диапазона");
    return datan[row * colsn + col];
}

void Matrix::fill_with_indices() {
    for (size_t i = 0; i < rowsn; ++i)
        for (size_t j = 0; j < colsn; ++j)
            at(i, j) = i * colsn + j;
}

void Matrix::set_zero() {
    std::fill(datan.begin(), datan.end(), 0.0);
}

void Matrix::set_identity() {
    set_zero();
    size_t n = min(rowsn, colsn);
    for (size_t i = 0; i < n; ++i)
        at(i, i) = 1.0;
}

void Matrix::set_value(size_t idx, double value) {
    if (idx >= datan.size())
        throw MatrixException("Индекс вне диапазона");
    datan[idx] = value;
}

// Определитель (для 1x1, 2x2, 3x3)
double Matrix::determinant() const {
    if (rowsn != colsn)
        throw MatrixException("Определитель определён только для квадратных матриц");
    if (rowsn == 1)
        return datan[0];
    if (rowsn == 2)
        return at(0, 0) * at(1, 1) - at(0, 1) * at(1, 0);
    if (rowsn == 3)
        return at(0, 0) * at(1, 1) * at(2, 2) + at(0, 1) * at(1, 2) * at(2, 0) + at(0, 2) * at(1, 0) * at(2, 1)
        - at(0, 2) * at(1, 1) * at(2, 0) - at(0, 0) * at(1, 2) * at(2, 1) - at(0, 1) * at(1, 0) * at(2, 2);
    throw MatrixException("Определитель не реализован для матриц больше 3x3");
}

// Транспонирование
Matrix Matrix::transpose() const {
    Matrix result(colsn, rowsn, std::vector<double>(rowsn * colsn, 0.0));
    for (size_t i = 0; i < rowsn; ++i)
        for (size_t j = 0; j < colsn; ++j)
            result.at(j, i) = at(i, j);
    return result;
}

double& Matrix::operator[](size_t idx) { return datan.at(idx); }
double Matrix::operator[](size_t idx) const { return datan.at(idx); }

// Операторы сложения и умножения
Matrix& Matrix::operator+=(const Matrix& other) {
    if (rowsn != other.rowsn || colsn != other.colsn)
        throw MatrixException("Размеры матриц должны совпадать для сложения");
    for (size_t i = 0; i < datan.size(); ++i)
        datan[i] += other.datan[i];
    return *this;
}

Matrix& Matrix::operator*=(double k) {
    for (auto& v : datan) v *= k;
    return *this;
}

Matrix Matrix::operator+(const Matrix& other) const {
    Matrix res = *this;
    res += other;
    return res;
}

Matrix Matrix::operator-(const Matrix& other) const {
    if (rowsn != other.rowsn || colsn != other.colsn)
        throw MatrixException("Размеры матриц должны совпадать для вычитания");
    Matrix res(rowsn, colsn, std::vector<double>(rowsn * colsn, 0.0));
    for (size_t i = 0; i < datan.size(); ++i)
        res.datan[i] = datan[i] - other.datan[i];
    return res;
}

Matrix Matrix::operator*(const Matrix& other) const {
    if (colsn != other.rowsn)
        throw MatrixException("Размеры матриц не подходят для умножения");
    Matrix res(rowsn, other.colsn, std::vector<double>(rowsn * other.colsn, 0.0));
    for (size_t i = 0; i < rowsn; ++i)
        for (size_t j = 0; j < other.colsn; ++j)
            for (size_t k = 0; k < colsn; ++k)
                res.at(i, j) += at(i, k) * other.at(k, j);
    return res;
}

// Возведение в степень 
Matrix Matrix::pow(unsigned int p) const {
    if (rowsn != colsn)
        throw MatrixException("Возведение в степень определено только для квадратных матриц");
    Matrix res = *this;
    if (p == 0) {
        res.set_identity();
        return res;
    }
    for (unsigned int i = 1; i < p; ++i)
        res = res * (*this);
    return res;
}

// Вывод матрицы
void Matrix::print(std::ostream& os) const {
    for (size_t i = 0; i < rowsn; ++i) {
        for (size_t j = 0; j < colsn; ++j)
            os << std::setw(8) << at(i, j) << ' ';
        os << '\n';
    }
    os << '\n';
}

void Matrix::print_with_label(const std::string& label, std::ostream& os) const {
    os << label << ":\n";
    print(os);
}

Matrix create_matrix(size_t rows, size_t cols) {
    if (rows == 0 || cols == 0)
        throw MatrixException("Размеры матрицы должны быть ненулевыми");
    std::vector<double> data(rows * cols, 0.0);
    return Matrix(rows, cols, std::move(data));
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    std::locale::global(std::locale(""));
    std::cout.imbue(std::locale());

    try {
        Matrix A = create_matrix(3, 3);
        Matrix B = create_matrix(3, 3);
        B.fill_with_indices();
        A = B;
        A.print_with_label("Матрица A (копия B)");

        A *= 2;
        A.print_with_label("Матрица A после умножения на 2");

        double res = A[2];
        std::cout << "Элемент A[2]: " << res << "\n\n";

        A.set_value(0, 2.3);
        A.print_with_label("Матрица A после установки значения 2.3 в индекс 0");

        std::cout << "Определитель матрицы A: " << A.determinant() << "\n\n";

        Matrix C = B - A;
        C.print_with_label("Матрица C = B - A");

        Matrix D = create_matrix(2, 3);
        Matrix E = create_matrix(3, 4);
        D.fill_with_indices();
        E.fill_with_indices();
        D.print_with_label("Матрица D (2x3) с индексами");
        E.print_with_label("Матрица E (3x4) с индексами");

        Matrix F = D * E;
        F.print_with_label("Матрица F = D * E");

        Matrix G = F.transpose();
        G.print_with_label("Матрица G = транспонированная F");

        Matrix J = B.pow(2);
        J.print_with_label("Матрица J = B в квадрате");

        Matrix P = create_matrix(2, 2);
        Matrix Q = create_matrix(2, 2);
        Matrix T = create_matrix(2, 2);
        P.fill_with_indices();
        Q.fill_with_indices();
        T.fill_with_indices();
        Matrix U = T.transpose();
        Matrix R = P + Q * U;
        R.print_with_label("Матрица R = P + Q * U (U = транспонированная T)");
    }
    catch (const MatrixException& ex) {
        std::cerr << "Ошибка матрицы: " << ex.what() << std::endl;
    }
}
