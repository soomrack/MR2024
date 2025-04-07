#include <iostream>

class Matrix {
private:
    size_t rows;
    size_t cols;
    double* data;

public:
    // Конструктор
    Matrix(size_t r, size_t c) : rows(r), cols(c) {
        data = new double[rows * cols](); // Инициализация нулями
    }

    // Деструктор
    ~Matrix() {
        delete[] data;
    }

    // Метод для вывода матрицы
    void print() {
        for (size_t row = 0; row < rows; ++row) {
            for (size_t col = 0; col < cols; ++col) {
                std::cout << data[row * cols + col] << " ";
            }
            std::cout << std::endl;
        }
    }

    // Метод для заполнения матрицы случайными числами
    void fillRandom() {
        for (size_t i = 0; i < rows * cols; ++i) {
            data[i] = rand() % 10; // Случайное число от 0 до 9
        }
    }
};

int main() {
    Matrix A(2, 2);
    A.fillRandom();
    std::cout << "Матрица A:" << std::endl;
    A.print();

    Matrix B(2, 2);
    B.fillRandom();
    std::cout << "Матрица B:" << std::endl;
    B.print();

    return 0;
}