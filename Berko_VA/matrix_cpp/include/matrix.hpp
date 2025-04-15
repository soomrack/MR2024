/// @author Вадим Берко
/// @date 2025-03-09
/// @version 1.0.0

#include <cstddef>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <thread>
#include <random>


/**
 * @enum MatrixStatus
 * @brief Перечисление для статусов операций с матрицей
 */
enum class MatrixStatus {
    OK = 0,       /**< Успешное выполнение */
    ALLOC_ERR,    /**< Ошибка выделения памяти */
    EMPTY_ERR,    /**< Ошибка: матрица пуста */
    SIZE_ERR      /**< Ошибка: неверный размер матрицы */
};

/**
 * @enum LogLevel
 * @brief Перечисление уровней логирования
 */
enum class LogLevel {
    NONE, /**< Логирование отключено */
    ERR,  /**< Логировать только ошибки */
    WARN, /**< Логировать предупреждения и ошибки */
    INFO  /**< Логировать информацию, предупреждения и ошибки */
};

/**
 * @enum MultiplyMode
 * @brief Режим умножения матриц
 */
enum class MultiplyMode {
    AUTO,       /**< Автоматический выбор (стандартное для n < 100, блочное для n >= 100) */
    STANDARD,   /**< Стандартное умножение */
    BLOCK       /**< Блочное умножение */
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
     *
     * Создает матрицу с указанными размерами, заполненную нулями.
     * Если rows или cols равны 0, создается пустая матрица.
     */
    Matrix(std::size_t rows, std::size_t cols);

    /**
     * @brief Конструктор копирования
     * @param other Матрица для копирования
     *
     * Создает копию переданной матрицы.
     */
    Matrix(const Matrix& other);

    /**
     * @brief Конструктор перемещения
     * @param other Матрица для перемещения
     *
     * Перемещает данные из переданной матрицы, оставляя её пустой.
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
     *
     * Копирует данные из переданной матрицы.
     */
    Matrix& operator=(const Matrix& other);

    /**
     * @brief Оператор присваивания перемещением
     * @param other Матрица для перемещения
     * @return Ссылка на текущую матрицу
     *
     * Перемещает данные из переданной матрицы, оставляя её пустой.
     */
    Matrix& operator=(Matrix&& other) noexcept;

    /**
     * @brief Оператор сложения матриц
     * @param other Матрица для сложения
     * @return Новая матрица - результат сложения
     * @throws std::runtime_error Если размеры матриц не совпадают
     */
    Matrix operator+(const Matrix& other) const;

    /**
     * @brief Оператор вычитания матриц
     * @param other Матрица для вычитания
     * @return Новая матрица - результат вычитания
     * @throws std::runtime_error Если размеры матриц не совпадают
     */
    Matrix operator-(const Matrix& other) const;

    /**
     * @brief Оператор умножения матриц
     * @param other Матрица для умножения
     * @return Новая матрица - результат умножения
     * @throws std::runtime_error Если размеры не подходят для умножения
     */
    Matrix operator*(const Matrix& other) const;

    /**
     * @brief Оператор умножения матрицы на число
     * @param number Множитель
     * @return Новая матрица - результат умножения
     */
    Matrix operator*(double number) const;

    /**
     * @brief Оператор доступа к элементу матрицы
     * @param row Индекс строки
     * @param col Индекс столбца
     * @return Ссылка на элемент матрицы
     * @throws std::out_of_range Если индексы вне допустимого диапазона
     */
    double& operator()(std::size_t row, std::size_t col);

    /**
     * @brief Оператор доступа к элементу матрицы (константный)
     * @param row Индекс строки
     * @param col Индекс столбца
     * @return Константная ссылка на элемент матрицы
     * @throws std::out_of_range Если индексы вне допустимого диапазона
     */
    const double& operator()(std::size_t row, std::size_t col) const;

    /**
     * @brief Оператор сравнения матриц
     * @param other Матрица для сравнения
     * @return true, если матрицы равны, иначе false
     */
    bool operator==(const Matrix& other) const;

	Matrix multiply(const Matrix& other, MultiplyMode mode) const;

    /**
     * @brief Получить количество строк
     * @return Количество строк матрицы
     */
    std::size_t rows() const { return m_rows; }

    /**
     * @brief Получить количество столбцов
     * @return Количество столбцов матрицы
     */
    std::size_t cols() const { return m_cols; }

    /**
     * @brief Проверка, является ли матрица пустой
     * @return true, если матрица пуста, иначе false
     */
    bool is_empty() const { return m_data.empty(); }

    /**
     * @brief Проверка, является ли матрица квадратной
     * @return true, если матрица квадратная, иначе false
     */
    bool is_square() const { return m_rows == m_cols; }

    /**
     * @brief Установить уровень логирования
     * @param level Уровень логирования
     */
    void set_log_level(LogLevel level);

    /**
     * @brief Заполнить матрицу нулями
     * @return Статус выполнения операции
     */
    MatrixStatus set_zeros();

	/**
     * @brief Заполнить матрицу случайными вещественными значениями
     * @return Статус выполнения операции
	 * @param min Минимальное значение диапазона 
     * @param max Максимальное значение диапазона
     */
	MatrixStatus fill_random(double min = 0.0, double max = 1.0);

    /**
     * @brief Сделать матрицу единичной
     * @return Статус выполнения операции
     * @throws MatrixStatus::SIZE_ERR Если матрица не квадратная
     */
    MatrixStatus set_identity();

    /**
     * @brief Транспонировать матрицу
     * @return Статус выполнения операции
     * @throws MatrixStatus::SIZE_ERR Если матрица не квадратная
     */
    MatrixStatus transpose();

    /**
     * @brief Возвести матрицу в степень
     * @param power Степень
     * @return Статус выполнения операции
     * @throws MatrixStatus::SIZE_ERR Если матрица не квадратная
     */
    MatrixStatus pow(unsigned int power);

    /**
     * @brief Вычислить экспоненту матрицы
     * @return Статус выполнения операции
     * @throws MatrixStatus::SIZE_ERR Если матрица не квадратная
     */
    MatrixStatus exp();

    /**
     * @brief Вычислить определитель матрицы
     * @param result Переменная для результата
     * @return Статус выполнения операции
     * @throws MatrixStatus::SIZE_ERR Если матрица не квадратная
     */
    MatrixStatus det(double& result) const;

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
    static bool is_equal(double a, double b) {
        return std::abs(a - b) < 0.00001;
    }

	/**
     * @brief Последовательное циклическое умножение матриц
     * @param other Матрица, на которую умножается исходная
	 * @return Матрица, являющаяся результатом умножения
     */
	Matrix standard_multiply(const Matrix& other) const;

	/**
     * @brief Блочное умножение для больших матриц
     * @param other Матрица, на которую умножается исходная
	 * @return Матрица, являющаяся результатом умножения
     */
    Matrix block_multiply(const Matrix& other) const;

	/**
     * @brief Вспомогательная функция для вычисления одного блока
     * @param A Блок исходной матрицы
     * @param B Блок для умножения
     * @param result Блок, являющийся результатом умножения
     * @param i_start Начало диапазона строк
     * @param i_end Конец диапазона строк
     * @param j_start Начало диапазона столбцов
     * @param j_end Конец диапазона столбцов
	 * @return Матрица, являющаяся результатом умножения
     */
    static void multiply_block(const Matrix& A, const Matrix& B, Matrix& result,
                              int i_start, int i_end, int j_start, int j_end);

};

/**
 * @brief Оператор умножения числа на матрицу
 * @param number Множитель
 * @param matrix Матрица
 * @return Новая матрица - результат умножения
 */
Matrix operator*(double number, const Matrix& matrix);
