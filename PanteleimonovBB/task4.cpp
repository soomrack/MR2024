#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>

// Функция вычисления факториала
std::uint64_t factorial(unsigned int f) {
    std::uint64_t res = 1;
    for (unsigned int i = 1; i <= f; i++) {
        res *= i;
    }
    return res;
}

class Matrix {
public:
    size_t rows, cols;
    std::vector<double> data;

public:
    // Конструктор
    Matrix(size_t r, size_t c) : rows(r), cols(c), data(r * c, 0.0) {
        if (r == 0 || c == 0) throw std::runtime_error("Размер матрицы не может быть 0.");
    }

    // Ввод матрицы
    void input() {
        std::cout << "Введите элементы матрицы (" << rows << "x" << cols << "):\n";
        for (size_t i = 0; i < rows * cols; i++) {
            std::cin >> data[i];
        }
    }

    // Вывод матрицы
    void print() const {
        std::cout << "Матрица:\n";
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                std::cout << data[i * cols + j] << " ";
            }
            std::cout << std::endl;
        }
    }

    // Создание единичной матрицы
    static Matrix identity(size_t n) {
        Matrix I(n, n);
        for (size_t i = 0; i < n; i++) {
            I.data[i * n + i] = 1.0;
        }
        return I;
    }

    // Перегрузка оператора сложения
    Matrix operator+(const Matrix &B) const {
        if (rows != B.rows || cols != B.cols) throw std::runtime_error("Размеры матриц не совпадают.");
        Matrix result(rows, cols);
        for (size_t i = 0; i < rows * cols; i++) {
            result.data[i] = data[i] + B.data[i];
        }
        return result;
    }

    // Перегрузка оператора вычитания
    Matrix operator-(const Matrix &B) const {
        if (rows != B.rows || cols != B.cols) throw std::runtime_error("Размеры матриц не совпадают.");
        Matrix result(rows, cols);
        for (size_t i = 0; i < rows * cols; i++) {
            result.data[i] = data[i] - B.data[i];
        }
        return result;
    }

    // Перегрузка оператора умножения на скаляр
    Matrix operator*(double scalar) const {
        Matrix result(rows, cols);
        for (size_t i = 0; i < rows * cols; i++) {
            result.data[i] = data[i] * scalar;
        }
        return result;
    }

    // Перегрузка оператора умножения матриц
    Matrix operator*(const Matrix &B) const {
        if (cols != B.rows) throw std::runtime_error("Нельзя умножить: число столбцов первой не равно числу строк второй.");
        Matrix result(rows, B.cols);
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < B.cols; j++) {
                for (size_t k = 0; k < cols; k++) {
                    result.data[i * B.cols + j] += data[i * cols + k] * B.data[k * B.cols + j];
                }
            }
        }
        return result;
    }

    // Перегрузка оператора деления на скаляр
    Matrix operator/(double scalar) const {
        if (scalar == 0) throw std::runtime_error("Деление на ноль невозможно.");
        Matrix result(rows, cols);
        for (size_t i = 0; i < rows * cols; i++) {
            result.data[i] = data[i] / scalar;
        }
        return result;
    }

    // Перегрузка оператора сравнения ==
    bool operator==(const Matrix &B) const {
        if (rows != B.rows || cols != B.cols) return false;
        for (size_t i = 0; i < rows * cols; i++) {
            if (data[i] != B.data[i]) return false;
        }
        return true;
    }

    // Перегрузка оператора сравнения !=
    bool operator!=(const Matrix &B) const {
        return !(*this == B);
    }

    // Возведение матрицы в степень
    Matrix power(size_t exp) const {
        if (rows != cols) throw std::runtime_error("Матрица должна быть квадратной.");
        if (exp == 0) return identity(rows);
        if (exp == 1) return *this;

        Matrix result = *this;
        for (size_t i = 1; i < exp; i++) {
            result = result * (*this);
        }
        return result;
    }

    // Транспонирование
    Matrix transpose() const {
        Matrix T(cols, rows);
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                T.data[j * rows + i] = data[i * cols + j];
            }
        }
        return T;
    }

    // Вычисление экспоненты матрицы
    Matrix exponent(size_t order) const {
        if (rows != cols) throw std::runtime_error("Матрица должна быть квадратной для экспоненты.");

        Matrix result = identity(rows);
        Matrix term = identity(rows);

        for (size_t n = 1; n < order; ++n) {
            term = term * (*this) * (1.0 / factorial(n));
            result = result + term;
        }

        return result;
    }

    // Вычисление определителя
    double determinant() const {
        if (rows != cols) throw std::runtime_error("Матрица должна быть квадратной.");
        if (rows == 1) return data[0];
        if (rows == 2) return data[0] * data[3] - data[1] * data[2];

        if (rows == 3) {
            return data[0] * (data[4] * data[8] - data[5] * data[7]) -
                   data[1] * (data[3] * data[8] - data[5] * data[6]) +
                   data[2] * (data[3] * data[7] - data[4] * data[6]);
        }
        throw std::runtime_error("Определитель поддерживается только для 1x1, 2x2 и 3x3 матриц.");
    }
};

// Перегрузка оператора << для вывода матрицы
std::ostream& operator<<(std::ostream& os, const Matrix& matrix) {
    matrix.print();
    return os;
}

int main() {
    try {
        size_t choice;
        std::cout << "Выберите операцию:\n";
        std::cout << "1. Сложение матриц\n";
        std::cout << "2. Разность матриц\n";
        std::cout << "3. Умножение матриц\n";
        std::cout << "4. Транспонирование\n";
        std::cout << "5. Возведение в степень\n";
        std::cout << "6. Определитель матрицы\n";
        std::cout << "7. Вычисление экспоненты\n";
        std::cout << "8. Выход\n";
        std::cout << "Ваш выбор: ";
        std::cin >> choice;

        if (choice == 8) {
            std::cout << "Выход.\n";
            return 0;
        }

        size_t rows, cols;
        std::cout << "Введите размер матрицы A (строки столбцы): ";
        std::cin >> rows >> cols;
        Matrix A(rows, cols);
        A.input();

        if (choice == 1 || choice == 2 || choice == 3) {
            std::cout << "Введите размер матрицы B (строки столбцы): ";
            std::cin >> rows >> cols;
            Matrix B(rows, cols);
            B.input();

            if (choice == 1) {
                Matrix result = A + B;
                std::cout << "Результат сложения:\n" << result;
            } else if (choice == 2) {
                Matrix result = A - B;
                std::cout << "Результат вычитания:\n" << result;
            } else {
                Matrix result = A * B;
                std::cout << "Результат умножения:\n" << result;
            }
        } else if (choice == 4) {
            Matrix result = A.transpose();
            std::cout << "Транспонированная матрица:\n" << result;
        } else if (choice == 5) {
            size_t exp;
            std::cout << "Введите степень: ";
            std::cin >> exp;
            Matrix result = A.power(exp);
            std::cout << "Матрица в степени " << exp << ":\n" << result;
        } else if (choice == 6) {
            std::cout << "Определитель: " << A.determinant() << std::endl;
        } else if (choice == 7) {
            size_t order;
            std::cout << "Введите порядок разложения экспоненты: ";
            std::cin >> order;
            Matrix result = A.exponent(order);
            std::cout << "Экспонента матрицы:\n" << result;
        }
    } catch (const std::exception &e) {
        std::cout << "Ошибка: " << e.what() << std::endl;
    }

    return 0;
}
