#pragma once
#include <cstddef>
#include <vector>
#include <thread>
#include <random>
#include "matrixexception.hpp"

/**
 * @enum LogLevel
 * @brief Перечисление уровней логирования
 */
enum class LogLevel {
    None, /**< Логирование отключено */
    Error,  /**< Логировать только ошибки */
    Warning, /**< Логировать предупреждения и ошибки */
    Info  /**< Логировать информацию, предупреждения и ошибки */
};

/**
 * @enum MultiplyMode
 * @brief Режим умножения матриц
 */
enum class MultiplyMode {
    Auto,       /**< Автоматический выбор (оптимизированное для n < 100, блочное для n >= 100) */
    Standard,   /**< Стандартное умножение */
    Block,      /**< Блочное умножение */
    Transpose,  /**< Умножение с транспонированием второй матрицы */
    BlockTranspose, /**< Блочное умножение с транспонированием второй матрицы */
    Optimized,  /**< Оптимизированное умножение с прямым доступом, разворачиванием и ikj */
    Recursive,  /**< Рекурсивное умножение */
    ParallelOptimized, /**< Параллельное оптимизированное умножение */
    OptimizedTranspose /**< Оптимизированное умножение с транспонированием */
};

/**
 * @struct MatrixView
 * @brief Представление подматрицы без копирования данных
 */
struct MatrixView {
    const double* data; /**< Указатель на данные матрицы */
    std::size_t rows;  /**< Количество строк подматрицы */
    std::size_t cols;  /**< Количество столбцов подматрицы */
    std::size_t row_offset; /**< Смещение по строкам */
    std::size_t col_offset; /**< Смещение по столбцам */
    std::size_t stride;     /**< Шаг для строк (ширина исходной матрицы) */

    MatrixView(const double* d, std::size_t r, std::size_t c, std::size_t ro, std::size_t co, std::size_t s)
        : data(d), rows(r), cols(c), row_offset(ro), col_offset(co), stride(s) {}
};

/**
 * @class Matrix
 * @brief Класс для работы с матрицами вещественных чисел
 *
 * Реализует операции над матрицами, включая сложение, вычитание, умножение,
 * транспонирование, возведение в степень, вычисление определителя и другие.
 */
class Matrix {
public:
    /**
     * @brief Конструктор по умолчанию
     *
     * Создает пустую матрицу с нулевыми размерами.
     */
    Matrix();

    /**
     * @brief Конструктор с заданными размерами
     * @param rows Количество строк
     * @param cols Количество столбцов
     * @throws MatrixException Если размеры вызывают переполнение или превышают максимальный размер
     */
    Matrix(std::size_t rows, std::size_t cols);

    /**
     * @brief Конструктор копирования
     * @param other Матрица для копирования
     */
    Matrix(const Matrix& other);

    /**
     * @brief Конструктор перемещения
     * @param other Матрица для перемещения
     */
    Matrix(Matrix&& other) noexcept;

    /**
     * @brief Деструктор
     *
     * Автоматически освобождает ресурсы, управляемые std::vector.
     */
    ~Matrix() = default;

    /**
     * @brief Оператор присваивания копированием
     * @param other Матрица для копирования
     * @return Ссылка на текущую матрицу
     */
    Matrix& operator=(const Matrix& other);

    /**
     * @brief Оператор присваивания перемещением
     * @param other Матрица для перемещения
     * @return Ссылка на текущую матрицу
     */
    Matrix& operator=(Matrix&& other) noexcept;

    /**
     * @brief Оператор сложения матриц
     * @param other Матрица для сложения
     * @return Новая матрица - результат сложения
     * @throws MatrixException Если размеры матриц не совпадают или содержат невалидные элементы
     */
    [[nodiscard]] Matrix operator+(const Matrix& other) const;

    /**
     * @brief Оператор вычитания матриц
     * @param other Матрица для вычитания
     * @return Новая матрица - результат вычитания
     * @throws MatrixException Если размеры матриц не совпадают или содержат невалидные элементы
     */
    [[nodiscard]] Matrix operator-(const Matrix& other) const;

    /**
     * @brief Оператор умножения матриц
     * @param other Матрица для умножения
     * @return Новая матрица - результат умножения
     * @throws MatrixException Если размеры не подходят для умножения
     */
    [[nodiscard]] Matrix operator*(const Matrix& other) const;

    /**
     * @brief Оператор умножения матрицы на число
     * @param number Множитель
     * @return Новая матрица - результат умножения
     * @throws MatrixException Если множитель или элементы матрицы невалидны
     */
    [[nodiscard]] Matrix operator*(double number) const;

    /**
     * @brief Оператор доступа к элементу матрицы
     * @param row Индекс строки
     * @param col Индекс столбца
     * @return Ссылка на элемент матрицы
     * @throws MatrixException Если индексы вне допустимого диапазона
     */
    double& operator()(std::size_t row, std::size_t col);

    /**
     * @brief Оператор доступа к элементу матрицы (константный)
     * @param row Индекс строки
     * @param col Индекс столбца
     * @return Константная ссылка на элемент матрицы
     * @throws MatrixException Если индексы вне допустимого диапазона
     */
    const double& operator()(std::size_t row, std::size_t col) const;

    /**
     * @brief Оператор сравнения матриц
     * @param other Матрица для сравнения
     * @return true, если матрицы равны, иначе false
     */
    [[nodiscard]] bool operator==(const Matrix& other) const;

    /**
     * @brief Умножение матриц с указанным режимом
     * @param other Матрица для умножения
     * @param mode Режим умножения
     * @return Новая матрица - результат умножения
     * @throws MatrixException Если размеры не подходят для умножения
     */
    [[nodiscard]] Matrix multiply(const Matrix& other, MultiplyMode mode) const;

    /**
     * @brief Получить количество строк
     * @return Количество строк матрицы
     */
    [[nodiscard]] std::size_t rows() const noexcept { return m_rows; }

    /**
     * @brief Получить количество столбцов
     * @return Количество столбцов матрицы
     */
    [[nodiscard]] std::size_t cols() const noexcept { return m_cols; }

    /**
     * @brief Проверка, является ли матрица пустой
     * @return true, если матрица пуста, иначе false
     */
    [[nodiscard]] bool isEmpty() const noexcept { return m_data.empty(); }

    /**
     * @brief Проверка, является ли матрица квадратной
     * @return true, если матрица квадратная, иначе false
     */
    [[nodiscard]] bool isSquare() const noexcept { return m_rows == m_cols; }

    /**
     * @brief Установить уровень логирования
     * @param level Уровень логирования
     */
    void setLogLevel(LogLevel level);

    /**
     * @brief Заполнить матрицу нулями
     * @throws MatrixException Если матрица пуста
     */
    void setZeros();

    /**
     * @brief Заполнить матрицу случайными вещественными значениями
     * @param min Минимальное значение диапазона
     * @param max Максимальное значение диапазона
     * @throws MatrixException Если матрица пуста или min > max
     */
    void fillRandom(double min = 0.0, double max = 1.0);

    /**
     * @brief Сделать матрицу единичной
     * @throws MatrixException Если матрица не квадратная
     */
    void setIdentity();

    /**
     * @brief Транспонировать матрицу
     * @throws MatrixException Если элементы матрицы невалидны
     */
    void transpose();

    /**
     * @brief Возвести матрицу в степень
     * @param power Степень
     * @throws MatrixException Если матрица не квадратная или содержит невалидные элементы
     */
    void pow(unsigned int power);

    /**
     * @brief Вычислить экспоненту матрицы
     * @throws MatrixException Если матрица не квадратная или содержит невалидные элементы
     */
    void exp();

    /**
     * @brief Вычислить определитель матрицы
     * @return Значение определителя
     * @throws MatrixException Если матрица не квадратная или содержит невалидные элементы
     */
    [[nodiscard]] double det() const;

    /**
     * @brief Вывести матрицу в консоль
     */
    void print() const;

private:
    std::size_t m_rows;         /**< Количество строк */
    std::size_t m_cols;         /**< Количество столбцов */
    std::vector<double> m_data; /**< Данные матрицы */

    /**
     * @brief Проверка равенства двух чисел с учетом погрешности
     * @param a Первое число
     * @param b Второе число
     * @return true, если числа равны, иначе false
     */
    static bool isEqual(double a, double b) noexcept {
        return std::abs(a - b) < 0.00001;
    }

    /**
     * @brief Стандартное умножение матриц
     * @param other Матрица для умножения
     * @return Новая матрица - результат умножения
     */
    [[nodiscard]] Matrix standardMultiply(const Matrix& other) const;

    /**
     * @brief Умножение матриц с транспонированием второй матрицы
     * @param other Матрица для умножения
     * @return Новая матрица - результат умножения
     */
    [[nodiscard]] Matrix transposeMultiply(const Matrix& other) const;

    /**
     * @brief Блочное умножение матриц
     * @param other Матрица для умножения
     * @return Новая матрица - результат умножения
     */
    [[nodiscard]] Matrix blockMultiply(const Matrix& other) const;

    /**
     * @brief Блочное умножение с транспонированием второй матрицы
     * @param other Матрица для умножения
     * @return Новая матрица - результат умножения
     */
    [[nodiscard]] Matrix blockTransposeMultiply(const Matrix& other) const;

    /**
     * @brief Оптимизированное умножение с прямым доступом, разворачиванием и ikj
     * @param other Матрица для умножения
     * @return Новая матрица - результат умножения
     */
    [[nodiscard]] Matrix optimizedMultiply(const Matrix& other) const;

    /**
     * @brief Оптимизированное умножение с транспонированием второй матрицы
     * @param other Матрица для умножения
     * @return Новая матрица - результат умножения
     */
    [[nodiscard]] Matrix optimizedTransposeMultiply(const Matrix& other) const;

    /**
     * @brief Параллельное оптимизированное умножение
     * @param other Матрица для умножения
     * @return Новая матрица - результат умножения
     */
    [[nodiscard]] Matrix parallelOptimizedMultiply(const Matrix& other) const;

    /**
     * @brief Рекурсивное умножение матриц
     * @param other Матрица для умножения
     * @return Новая матрица - результат умножения
     */
    [[nodiscard]] Matrix recursiveMultiply(const Matrix& other) const;

    /**
     * @brief Умножение блока матриц
     * @param a Первая матрица
     * @param b Вторая матрица
     * @param result Результирующая матрица
     * @param iStart Начало диапазона строк
     * @param iEnd Конец диапазона строк
     * @param jStart Начало диапазона столбцов
     * @param jEnd Конец диапазона столбцов
     */
    static void multiplyBlock(const Matrix& a, const Matrix& b, Matrix& result,
                             std::size_t iStart, std::size_t iEnd, std::size_t jStart, std::size_t jEnd);

    /**
     * @brief Вспомогательная функция для рекурсивного умножения через MatrixView
     * @param a Первая подматрица
     * @param b Вторая подматрица
     * @param result Результирующая подматрица
     */
    static void recursiveMultiplyView(const MatrixView& a, const MatrixView& b, Matrix& result);
};

/**
 * @brief Оператор умножения числа на матрицу
 * @param number Множитель
 * @param matrix Матрица
 * @return Новая матрица - результат умножения
 * @throws MatrixException Если множитель или элементы матрицы невалидны
 */
[[nodiscard]] Matrix operator*(double number, const Matrix& matrix);
