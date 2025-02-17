#include <iostream>
#include <vector>
using namespace std;


class Matrix {
private:
    size_t rows;
    size_t cols;
    vector<double> data; // Одномерный вектор для хранения элементов

public:
    // Конструктор по умолчанию
    Matrix() : rows(0), cols(0) {}

    // Конструктор с размерами
    Matrix(size_t rows, size_t cols) : rows(rows), cols(cols), data(rows * cols, 0.0) {}

    // Конструктор из массива
    Matrix(double* new_data, size_t rows, size_t cols) 
        : rows(rows), cols(cols), data(new_data, new_data + rows * cols) {}

    // Конструктор из одного числа
    Matrix(double number) : rows(1), cols(1), data(1, number) {}

    // Конструктор копирования
    Matrix(const Matrix& other) 
        : rows(other.rows), cols(other.cols), data(other.data) {
        printf("Вызван конструктор копирования \n");
    }

    // Конструктор перемещения
    Matrix(Matrix&& other) : rows(other.rows), cols(other.cols), data(move(other.data)) {
        printf("Вызван конструктор перемещения \n");
        other.rows = 0;
        other.cols = 0;
    }

    // Деструктор
    ~Matrix() {
        printf("Вызван деструктор \n");
    }

    //для вывода матрицы
    void print() const {
        for (size_t idx = 0; idx < rows; ++idx) {
            for (size_t jdx = 0; jdx < cols; ++jdx) {
                cout << data[idx * cols + jdx] << " ";
            }
            cout << endl;
        }
    }

    //для заполнения матрицы случайными числами
    void fillRandom() {
        srand(time(0)); // Инициализация генератора случайных чисел
        for (size_t idx = 0; idx < rows * cols; ++idx) {
            int sight = (rand() % 2) * 2 - 1;
            data[idx] = (rand() % 10) * sight;
        }
    }
};


int main() {
    // 1. Конструктор по умолчанию
    Matrix m1;
    printf("m1 (по умолчанию): \n");
    m1.print();

    // 2. Конструктор с размерами
    Matrix m2(2, 3);
    printf("m2 (2x3): \n");
    m2.print();

    // 3. Конструктор из массива
    double arr[] = {1, 2, 3, 4, 5, 6};
    Matrix m3(arr, 2, 3);
    printf("m3 (2x3 из массива): \n");
    m3.print();

    // 4. Конструктор из числа
    Matrix m4(7.0);
    printf("m4 (1x1 из числа): \n");
    m4.print();

    // 5. Конструктор копирования
    Matrix m5 = m3;
    printf("m5 (копия m3): \n");
    m5.print();

    // 6. Конструктор перемещения
    Matrix m6 = std::move(m3);
    printf("m6 (перемещение временного объекта): \n");
    m6.print();

    // 7. Заполнение матрицы случайными значениями
    printf("m2 (после случайного заполнения): \n");
    m2.fillRandom();  // Заполняем случайными значениями
    m2.print();  // Выводим матрицу

    return 0;
}
