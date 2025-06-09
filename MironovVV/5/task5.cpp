#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <cmath>
#include <cstring>   // Для std::memcpy
#include <sstream>

 // Класс исключения для матриц, наследник std::exception.
class MatrixException : public std::exception {
private:
    std::string message;
public:
    explicit MatrixException(const std::string& msg);
    virtual const char* what() const noexcept override;
};

MatrixException::MatrixException(const std::string& msg) : message(msg) {}

const char* MatrixException::what() const noexcept {
    return message.c_str();
}

class Matrix {
private:
    size_t rows;
    size_t cols;
    double* data;

    // Вспомогательная функция для выделения памяти (без инициализации элементов).
    void allocateMemory();

public:
    // Конструкторы:
    Matrix();                                  // Пустой конструктор.
    Matrix(size_t rows, size_t cols);          // Параметрический конструктор.
    Matrix(const Matrix& other);               // Копирующий конструктор.
    Matrix(Matrix&& other) noexcept;           // Перемещающий конструктор.

    // Деструктор.
    ~Matrix();

    // Перегрузка оператора присваивания.
    // Копирующее присваивание: если размеры не совпадают, выбрасывается исключение.
    Matrix& operator=(const Matrix& other);
    // Перемещающее присваивание: быстро передаёт ресурсы (указатель data) из временного объекта.
    Matrix& operator=(Matrix&& other) noexcept;

    // Функции, соответствующие исходным функциям на Си:
    void matrix_set(const double* values);
    void matrix_identity();
    double matrix_determinant() const;  // const – метод не изменяет объект.

    // Перегрузка оператора сложения (аналог matrix_add).
    Matrix operator+(const Matrix& other) const;

    // Перегрузка оператора умножения.
    // 1) Для матричного умножения (если аргумент – Matrix).
    Matrix operator*(const Matrix& other) const;
    // 2) Для умножения матрицы на скаляр (если аргумент – double).
    Matrix operator*(double scalar) const;

    Matrix matrix_transpose() const;
    Matrix matrix_exponent() const;
    void matrix_print(const std::string& title, double scalar_result = NAN) const;

    // Геттеры для размеров матрицы.
    size_t getRows() const;
    size_t getCols() const;
};

// ---------------- Реализация методов класса Matrix ----------------

void Matrix::allocateMemory() {
    if (rows == 0 || cols == 0)
        throw MatrixException("Matrix dimensions cannot be zero");
    if (rows > SIZE_MAX / cols)
        throw MatrixException("Matrix dimensions are too large");
    data = new double[rows * cols];
    // Здесь не инициализируем память нулями согласно требованию.
}

Matrix::Matrix() : rows(0), cols(0), data(nullptr) {}

Matrix::Matrix(size_t r, size_t c) : rows(r), cols(c), data(nullptr) {
    allocateMemory();
}

Matrix::Matrix(const Matrix& other) : rows(other.rows), cols(other.cols), data(nullptr) {
    if (other.data == nullptr || rows * cols == 0)
        return; // Если данные отсутствуют, выходим.
    allocateMemory();
    std::memcpy(data, other.data, rows * cols * sizeof(double));
}

Matrix::Matrix(Matrix&& other) noexcept
    : rows(other.rows), cols(other.cols), data(other.data) {
    // Перемещающий конструктор: просто "переносим" указатель, не копируя данные.
    other.rows = 0;
    other.cols = 0;
    other.data = nullptr;
}

Matrix::~Matrix() {
    delete[] data;
}

// Копирующий оператор присваивания.
// Если текущий объект и источник совпадают – просто возвращаем *this.
// Если размеры не совпадают, выбрасывается исключение.
// Если память еще не выделена (data == nullptr), вызываем allocateMemory().
Matrix& Matrix::operator=(const Matrix& other) {
    if (this == &other)
        return *this;
    if (rows != other.rows || cols != other.cols)
        throw MatrixException("Assignment failed: matrix sizes do not match");
    if (data == nullptr) {
        allocateMemory();
    }
    // Оптимизированное копирование данных с помощью std::memcpy.
    std::memcpy(data, other.data, rows * cols * sizeof(double));
    return *this;
}

// Перемещающее присваивание.
// Освобождаем текущую память, затем передаём размеры и указатель из other,
// после чего зануляем other.
Matrix& Matrix::operator=(Matrix&& other) noexcept {
    delete[] data;
    rows = other.rows;
    cols = other.cols;
    data = other.data;
    other.rows = 0;
    other.cols = 0;
    other.data = nullptr;
    return *this;
}

// Функция matrix_set копирует данные из переданного массива в матрицу.
void Matrix::matrix_set(const double* values) {
    if (values == nullptr || rows * cols == 0)
        return;
    std::memcpy(data, values, rows * cols * sizeof(double));
}

// Функция matrix_identity превращает матрицу в единичную (работает только для квадратных матриц).
void Matrix::matrix_identity() {
    if (rows != cols)
        throw MatrixException("Matrix must be square for identity operation");
    // Обнуляем все элементы.
    for (size_t i = 0; i < rows * cols; ++i)
        data[i] = 0.0;
    // Устанавливаем 1 на диагонали.
    for (size_t i = 0; i < rows; ++i)
        data[i * cols + i] = 1.0;
}

// Функция matrix_determinant вычисляет определитель для матриц 1x1, 2x2 и 3x3.
double Matrix::matrix_determinant() const {
    if (rows != cols)
        throw MatrixException("Matrix must be square to calculate determinant");
    if (rows == 1)
        return data[0];
    if (rows == 2)
        return data[0] * data[3] - data[1] * data[2];
    if (rows == 3)
        return data[0] * (data[4] * data[8] - data[5] * data[7])
        - data[1] * (data[3] * data[8] - data[5] * data[6])
        + data[2] * (data[3] * data[7] - data[4] * data[6]);
    throw MatrixException("Determinant is not implemented for matrices larger than 3x3");
}

// Перегруженный оператор сложения матриц (аналог matrix_add).
Matrix Matrix::operator+(const Matrix& other) const {
    if (rows != other.rows || cols != other.cols)
        throw MatrixException("Matrix dimensions must match for addition");
    Matrix result(rows, cols);
    for (size_t i = 0; i < rows * cols; ++i)
        result.data[i] = data[i] + other.data[i];
    return result;
}

// Перегруженный оператор умножения для матричного умножения (аналог matrix_multiply).
Matrix Matrix::operator*(const Matrix& other) const {
    if (cols != other.rows)
        throw MatrixException("Matrix dimensions are invalid for multiplication");
    Matrix result(rows, other.cols);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < other.cols; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < cols; ++k)
                sum += data[i * cols + k] * other.data[k * other.cols + j];
            result.data[i * other.cols + j] = sum;
        }
    }
    return result;
}

// Перегруженный оператор умножения для умножения матрицы на скаляр (аналог matrix_scalar_multiply).
Matrix Matrix::operator*(double scalar) const {
    Matrix result(rows, cols);
    for (size_t i = 0; i < rows * cols; ++i)
        result.data[i] = data[i] * scalar;
    return result;
}

// Функция matrix_transpose возвращает транспонированную матрицу.
Matrix Matrix::matrix_transpose() const {
    Matrix result(cols, rows);
    for (size_t i = 0; i < rows; ++i)
        for (size_t j = 0; j < cols; ++j)
            result.data[j * rows + i] = data[i * cols + j];
    return result;
}

// Функция matrix_exponent вычисляет экспоненту матрицы через разложение в ряд Тейлора (до 10-го члена).
Matrix Matrix::matrix_exponent() const {
    if (rows != cols)
        throw MatrixException("Matrix must be square for exponentiation");

    /*
     * Что происходит в этой функции:
     * 1. Создаётся матрица result, равная единичной матрице того же размера, что и исходная.
     *    Это будет начальное значение для суммы ряда Тейлора.
     * 2. Создаётся матрица term как копия result. Это текущий член ряда.
     * 3. В цикле от k = 1 до 10:
     *    a) Вычисляется временная матрица temp = term * (*this), то есть, умножение текущего члена ряда на исходную матрицу.
     *    b) Затем происходит присваивание: term = std::move(temp);
     *       Здесь std::move(temp) преобразует temp в rvalue (временный объект), позволяя вызвать перемещающее присваивание.
     *       Это означает, что вместо копирования данных из temp в term просто передаётся владение указателем на данные.
     *       Таким образом, данные из temp «перемещаются» в term, а temp становится пустым.
     *    c) Далее term умножается на 1/k, что соответствует делению на k! (точнее, на k, так как предыдущие члены уже учитывают предыдущие деления).
     *    d) Затем создаётся новая матрица newResult как сумма предыдущего result и обновлённого term.
     *    e) Результат newResult присваивается в result (через копирующее присваивание, так как размеры совпадают).
     * 4. По окончании цикла возвращается result, который является приближённым значением экспоненты матрицы.
     *
     * Особое внимание уделяем std::move:
     * std::move не копирует данные, а лишь приводит объект к типу rvalue, что позволяет вызвать перемещающий оператор присваивания.
     * Это ускоряет работу, поскольку не происходит дорогостоящего копирования больших массивов данных.
     */

    Matrix result(rows, cols);    // Параметрический конструктор.
    result.matrix_identity();       // Приводим result к единичной матрице.

    Matrix term(result);            // Копирующий конструктор: term изначально равна единичной матрице.

    for (size_t k = 1; k <= 10; ++k) {
        // Вычисляем временную матрицу temp = term * (*this)
        Matrix temp = term * (*this);  // Здесь вызывается оператор* для матричного умножения.
        // Перемещающее присваивание: перемещаем данные из temp в term.
        term = std::move(temp);

        // Умножаем term на скаляр 1/k (оператор* для умножения на double).
        Matrix scaled = term * (1.0 / static_cast<double>(k));
        // Перемещающее присваивание: перемещаем данные из scaled в term.
        term = std::move(scaled);

        // Вычисляем newResult как сумму result и term.
        Matrix newResult = result + term;  // Здесь вызывается оператор+ (сложение матриц).
        // Присваиваем результат в result (копирующее присваивание, так как newResult – именованный объект).
        result = newResult;
    }
    return result;
}

// Функция matrix_print выводит матрицу или, если данных нет, скалярный результат.
void Matrix::matrix_print(const std::string& title, double scalar_result) const {
    std::cout << title << ":\n";
    if (data != nullptr) {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j)
                std::cout << std::setw(6) << data[i * cols + j] << " ";
            std::cout << "\n";
        }
    }
    else {
        std::cout << scalar_result << "\n";
    }
    std::cout << "\n";
}

size_t Matrix::getRows() const { return rows; }
size_t Matrix::getCols() const { return cols; }

//
// Функция main демонстрирует работу с матрицами и операторами.
//
int main() {
    try {
        // Создаем матрицу A 2x2 (вызывается параметрический конструктор).
        Matrix A(2, 2);
        double valuesA[] = { 1.0, 2.0, 3.0, 4.0 };
        A.matrix_set(valuesA);

        // Создаем матрицу B 2x2 (также параметрический конструктор).
        Matrix B(2, 2);
        double valuesB[] = { 5.0, 6.0, 7.0, 8.0 };
        B.matrix_set(valuesB);

        // Создаем копию матрицы A для масштабирования (копирующий конструктор).
        Matrix A_scaled(A);
        // Умножаем A_scaled на скаляр 2.0.
        // Оператор * для скаляра возвращает временный объект, который присваивается A_scaled
        // с использованием перемещающего оператора присваивания.
        A_scaled = A_scaled * 2.0;
        A_scaled.matrix_print("Matrix A multiplied by scalar 2");

        double detA = A_scaled.matrix_determinant();
        std::cout << "Determinant of A_scaled: " << detA << "\n\n";

        // Сложение матриц через перегруженный оператор+.
        Matrix sumAB = A_scaled + B;
        sumAB.matrix_print("Sum of A_scaled and B");

        // Матричное умножение через оператор* (матрица на матрицу).
        Matrix productAB = A_scaled * B;
        productAB.matrix_print("Product of A_scaled and B");

        // Вычисляем экспоненту матрицы через функцию matrix_exponent.
        Matrix expA = A_scaled.matrix_exponent();
        expA.matrix_print("Exponent of A_scaled (e^A)");

        // Транспонирование матрицы.
        Matrix transposeA = A_scaled.matrix_transpose();
        transposeA.matrix_print("Transpose of A_scaled");
    }
    catch (const MatrixException& e) {
        std::cerr << "MatrixException: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    catch (const std::exception& e) {
        std::cerr << "Standard exception: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
