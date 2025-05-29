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

// Класс исключения для ошибок, связанных с матрицами
class MatrixException : public std::runtime_error {
public:
    explicit MatrixException(const std::string& msg) : std::runtime_error(msg) {}
};

// Класс для работы с матрицами 
class Matrix {
    size_t rows_, cols_;          
    std::vector<double> data_;    
public:
    // Конструктор: создаёт матрицу заданного размера и инициализирует нулями
    Matrix(size_t rows, size_t cols)
        : rows_(rows), cols_(cols), data_(rows* cols, 0.0) {
        if (rows == 0 || cols == 0)
            throw MatrixException("Размеры матрицы должны быть ненулевыми");
    }

    // Получить число строк
    size_t rows() const { return rows_; }
    // Получить число столбцов
    size_t cols() const { return cols_; }

    // Доступ к элементу матрицы с проверкой индексов (для записи)
    double& at(size_t row, size_t col) {
        if (row >= rows_ || col >= cols_)
            throw MatrixException("Индекс вне диапазона");
        return data_[row * cols_ + col];
    }

    // Доступ к элементу матрицы с проверкой индексов (для чтения)
    double at(size_t row, size_t col) const {
        if (row >= rows_ || col >= cols_)
            throw MatrixException("Индекс вне диапазона");
        return data_[row * cols_ + col];
    }

    // Заполнить матрицу значениями, равными индексам элементов (построчно)
    void fill_with_indices() {
        for (size_t i = 0; i < rows_; ++i)
            for (size_t j = 0; j < cols_; ++j)
                at(i, j) = i * cols_ + j;
    }

    // Заполнить матрицу нулями
    void set_zero() {
        std::fill(data_.begin(), data_.end(), 0.0);
    }

    // Сделать матрицу единичной (1 на диагонали, 0 в остальных местах)
    void set_identity() {
        set_zero();
        size_t n = min(rows_, cols_);
        for (size_t i = 0; i < n; ++i)
            at(i, i) = 1.0;
    }

    // Установить значение по плоскому индексу в векторе 
    void set_value(size_t idx, double value) {
        if (idx >= data_.size())
            throw MatrixException("Индекс вне диапазона");
        data_[idx] = value;
    }

    // Вычислить определитель матрицы 
    double determinant() const {
        if (rows_ != cols_)
            throw MatrixException("Определитель определён только для квадратных матриц");
        if (rows_ == 1)
            return data_[0];
        if (rows_ == 2)
            return at(0, 0) * at(1, 1) - at(0, 1) * at(1, 0);
        if (rows_ == 3)
            return at(0, 0) * at(1, 1) * at(2, 2) + at(0, 1) * at(1, 2) * at(2, 0) + at(0, 2) * at(1, 0) * at(2, 1)
            - at(0, 2) * at(1, 1) * at(2, 0) - at(0, 0) * at(1, 2) * at(2, 1) - at(0, 1) * at(1, 0) * at(2, 2);
        throw MatrixException("Определитель не реализован для матриц больше 3x3");
    }

    // Транспонировать матрицу 
    Matrix transpose() const {
        Matrix result(cols_, rows_);
        for (size_t i = 0; i < rows_; ++i)
            for (size_t j = 0; j < cols_; ++j)
                result.at(j, i) = at(i, j);
        return result;
    }

    
    double& operator[](size_t idx) { return data_.at(idx); }
    double operator[](size_t idx) const { return data_.at(idx); }

    // поэлементное сложение
    Matrix& operator+=(const Matrix& other) {
        if (rows_ != other.rows_ || cols_ != other.cols_)
            throw MatrixException("Размеры матриц должны совпадать для сложения");
        for (size_t i = 0; i < data_.size(); ++i)
            data_[i] += other.data_[i];
        return *this;
    }

    // Умножение всех элементов матрицы на число с присваиванием
    Matrix& operator*=(double k) {
        for (auto& v : data_) v *= k;
        return *this;
    }

    // Оператор сложения 
    Matrix operator+(const Matrix& other) const {
        Matrix res = *this;
        res += other;
        return res;
    }

    // Оператор вычитания 
    Matrix operator-(const Matrix& other) const {
        if (rows_ != other.rows_ || cols_ != other.cols_)
            throw MatrixException("Размеры матриц должны совпадать для вычитания");
        Matrix res(rows_, cols_);
        for (size_t i = 0; i < data_.size(); ++i)
            res.data_[i] = data_[i] - other.data_[i];
        return res;
    }

    // Оператор умножения матриц
    Matrix operator*(const Matrix& other) const {
        if (cols_ != other.rows_)
            throw MatrixException("Размеры матриц не подходят для умножения");
        Matrix res(rows_, other.cols_);
        for (size_t i = 0; i < rows_; ++i)
            for (size_t j = 0; j < other.cols_; ++j)
                for (size_t k = 0; k < cols_; ++k)
                    res.at(i, j) += at(i, k) * other.at(k, j);
        return res;
    }

    // Возведение матрицы в целочисленную степень 
    Matrix pow(unsigned int p) const {
        if (rows_ != cols_)
            throw MatrixException("Возведение в степень определено только для квадратных матриц");
        Matrix res(*this);
        if (p == 0) {
            res.set_identity();
            return res;
        }
        for (unsigned int i = 1; i < p; ++i)
            res = res * (*this);
        return res;
    }

    // Печать матрицы в поток вывода
    void print(std::ostream& os = std::cout) const {
        for (size_t i = 0; i < rows_; ++i) {
            for (size_t j = 0; j < cols_; ++j)
                os << std::setw(8) << at(i, j) << ' ';
            os << '\n';
        }
        os << '\n';
    }

    // названием операции
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
        // Создание и заполнение матриц
        Matrix A(3, 3);
        Matrix B(3, 3);
        B.fill_with_indices(); // Заполнение B индексами
        A = B;                 // Копирование B в A
        A.print_with_label("Матрица A (копия B)");

        A *= 2;                // Умножение A на 2
        A.print_with_label("Матрица A после умножения на 2");

        double res = A[2];     // Получение элемента по плоскому индексу
        std::cout << "Элемент A[2]: " << res << "\n\n";

        A.set_value(0, 2.3);   // Установка значения 2.3 в первый элемент
        A.print_with_label("Матрица A после установки значения 2.3 в индекс 0");

        std::cout << "Определитель матрицы A: " << A.determinant() << "\n\n";

        Matrix C = B - A;      // Вычитание матриц
        C.print_with_label("Матрица C = B - A");

        Matrix D(2, 3), E(3, 4);
        D.fill_with_indices();
        E.fill_with_indices();
        D.print_with_label("Матрица D (2x3) с индексами");
        E.print_with_label("Матрица E (3x4) с индексами");

        Matrix F = D * E;      // Умножение матриц
        F.print_with_label("Матрица F = D * E");

        Matrix G = F.transpose(); // Транспонирование матрицы
        G.print_with_label("Матрица G = транспонированная F");

        Matrix J = B.pow(2);   // Возведение матрицы B в квадрат
        J.print_with_label("Матрица J = B в квадрате");

        Matrix P(2, 2), Q(2, 2), T(2, 2);
        P.fill_with_indices();
        Q.fill_with_indices();
        T.fill_with_indices();
        Matrix U = T.transpose();
        Matrix R = P + Q * U;  // Сложение и умножение матриц
        R.print_with_label("Матрица R = P + Q * U (U = транспонированная T)");
    }
    catch (const MatrixException& ex) {
        std::cerr << "Ошибка матрицы: " << ex.what() << std::endl;
    }
}
