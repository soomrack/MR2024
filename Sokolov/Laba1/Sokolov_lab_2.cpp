#include <iostream>
#include <stdexcept>

using namespace std;

enum MatrixExceptionLevel {
    ERROR,   // завершение выполнения программы
    WARNING, // предупреждение, об ошибках в расчетах
    INFO,    // информационное сообщение о процессе
    DEBUG    // отладочная информация
};

// Структура для представления матрицы
struct Matrix {
    size_t rows;   // Количество строк
    size_t cols;   // Количество столбцов
    double* data;  // Данные матрицы, хранящиеся в одномерном массиве
};
typedef struct Matrix Matrix;

// Обработчик ошибок
void handle_exception(const MatrixExceptionLevel level, const string& message) {
    switch (level) {
    case ERROR:
        cerr << "ERROR: " << message << endl;
        exit(EXIT_FAILURE);
    case WARNING:
        cerr << "WARNING: " << message << endl;
        break;
    case INFO:
        cout << "INFO: " << message << endl;
        break;
    case DEBUG:
        cout << "DEBUG: " << message << endl;
        break;
    }
}

// Функция для инициализации матрицы
Matrix create_matrix(size_t rows, size_t cols) {
    Matrix mat;
    mat.rows = rows;
    mat.cols = cols;
    mat.data = new double[rows * cols](); // Инициализация элементов значением 0
    return mat;
}

// Освобождение памяти матрицы
void delete_matrix(Matrix& mat) {
    delete[] mat.data;
    mat.data = nullptr;
}

// Получение элемента матрицы
double get_element(const Matrix& mat, size_t row, size_t col) {
    if (row >= mat.rows || col >= mat.cols) {
        handle_exception(ERROR, "Index out of bounds in get_element");
    }
    return mat.data[row * mat.cols + col];
}

// Установка элемента матрицы
void set_element(Matrix& mat, size_t row, size_t col, const double value) {
    if (row >= mat.rows || col >= mat.cols) {
        handle_exception(ERROR, "Index out of bounds in set_element");
    }
    mat.data[row * mat.cols + col] = value;
}

// Инициализация значений для матриц
void initialize_matrices(Matrix& mat1, Matrix& mat2) {
    set_element(mat1, 0, 0, 1.0);
    set_element(mat1, 0, 1, 2.0);
    set_element(mat1, 1, 0, 3.0);
    set_element(mat1, 1, 1, 4.0);

    set_element(mat2, 0, 0, 5.0);
    set_element(mat2, 0, 1, 6.0);
    set_element(mat2, 1, 0, 7.0);
    set_element(mat2, 1, 1, 8.0);
}

// Сложение двух матриц
Matrix add_matrices(const Matrix& mat1, const Matrix& mat2) {
    if (mat1.rows != mat2.rows || mat1.cols != mat2.cols) {
        handle_exception(ERROR, "Matrix dimensions do not match for addition");
    }

    Matrix result = create_matrix(mat1.rows, mat1.cols);
    for (size_t i = 0; i < mat1.rows; ++i) {
        for (size_t j = 0; j < mat1.cols; ++j) {
            double sum = get_element(mat1, i, j) + get_element(mat2, i, j);
            set_element(result, i, j, sum);
        }
    }
    return result;
}

// Умножение двух матриц
Matrix multiply_matrices(const Matrix& mat1, const Matrix& mat2) {
    if (mat1.cols != mat2.rows) {
        handle_exception(ERROR, "Matrix dimensions do not match for multiplication");
    }

    Matrix result = create_matrix(mat1.rows, mat2.cols);
    for (size_t i = 0; i < mat1.rows; ++i) {
        for (size_t j = 0; j < mat2.cols; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < mat1.cols; ++k) {
                sum += get_element(mat1, i, k) * get_element(mat2, k, j);
            }
            set_element(result, i, j, sum);
        }
    }
    return result;
}

// Транспонирование матрицы
Matrix transpose_matrix(const Matrix& mat) {
    Matrix result = create_matrix(mat.cols, mat.rows);
    for (size_t i = 0; i < mat.rows; ++i) {
        for (size_t j = 0; j < mat.cols; ++j) {
            set_element(result, j, i, get_element(mat, i, j));
        }
    }
    return result;
}

// Печать матрицы
void print_matrix(const Matrix& mat) {
    for (size_t i = 0; i < mat.rows; ++i) {
        for (size_t j = 0; j < mat.cols; ++j) {
            cout << get_element(mat, i, j) << " ";
        }
        cout << endl;
    }
}

// Функция для вывода всех результатов
void display_results(const Matrix& sum, const Matrix& product, const Matrix& transpose) {
    cout << "Sum of matrices:" << endl;
    print_matrix(sum);

    cout << "Product of matrices:" << endl;
    print_matrix(product);

    cout << "Transpose of first matrix:" << endl;
    print_matrix(transpose);
}

// Основная функция
int main() {
    // Создание и инициализация матриц
    Matrix mat1 = create_matrix(2, 2);
    Matrix mat2 = create_matrix(2, 2);
    initialize_matrices(mat1, mat2);

    // Выполнение операций
    Matrix sum = add_matrices(mat1, mat2);
    Matrix product = multiply_matrices(mat1, mat2);
    Matrix transpose = transpose_matrix(mat1);

    // Вывод результатов
    display_results(sum, product, transpose);

    // Освобождение выделенной памяти
    delete_matrix(mat1);
    delete_matrix(mat2);
    delete_matrix(sum);
    delete_matrix(product);
    delete_matrix(transpose);

    return 0;
}
