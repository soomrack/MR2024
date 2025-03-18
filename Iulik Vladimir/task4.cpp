#include <iostream>
#include <vector>
using namespace std;

class Matrix
{
private:
    size_t rows;
    size_t cols;
    vector<double> data; // Одномерный вектор для хранения элементов

public:
    // Конструктор по умолчанию
    Matrix() : rows(0), cols(0) {}

    // Конструктор с размерами
    Matrix(size_t rows, size_t cols) : rows(rows), cols(cols), data(rows * cols, 0.0) {} // rows * cols - колич. элементов, 0.0 - значение каждого элемента

    // Конструктор из массива
    Matrix(double *new_data, size_t rows, size_t cols)
        : rows(rows), cols(cols), data(new_data, new_data + rows * cols) {}

    // Конструктор из одного числа
    Matrix(double number) : rows(1), cols(1), data(1, number) {}

    // Конструктор копирования
    Matrix(const Matrix &other)
        : rows(other.rows), cols(other.cols), data(other.data)
    {
        // printf("Вызван конструктор копирования \n");
    }

    // Конструктор перемещения
    Matrix(Matrix &&other) : rows(other.rows), cols(other.cols), data(move(other.data))
    {
        // printf("Вызван конструктор перемещения \n");
        other.rows = 0;
        other.cols = 0;
    }

    // Деструктор
    ~Matrix()
    {
        // printf("Вызван деструктор \n");
    }

    // для вывода матрицы
    void print() const
    {
        for (size_t idx = 0; idx < rows; ++idx)
        {
            for (size_t jdx = 0; jdx < cols; ++jdx)
            {
                cout << data[idx * cols + jdx] << " ";
            }
            cout << endl;
        }
    }

    // Конструктор единичной матрицы
    Matrix identity(size_t size) const
    {
        Matrix result(size, size);
        for (size_t i = 0; i < size; ++i)
        {
            result.data[i * size + i] = 1.0;
        }
        return result;
    }

    // для заполнения матрицы случайными числами
    void fillRandom()
    {
        srand(time(0));
        for (size_t idx = 0; idx < rows * cols; ++idx)
        {
            int sight = (rand() % 2) * 2 - 1;
            data[idx] = (rand() % 10) * sight;
        }
    }

    // Оператор присваивания копированием
    Matrix &operator=(const Matrix &other)
    {
        if (this != &other)
        { // Проверка на самоприсваивание
            rows = other.rows;
            cols = other.cols;
            data = other.data;
        }
        return *this;
    }

    // Конструктор сложения двух матриц
    Matrix operator+(const Matrix &other) const
    {
        if (rows != other.rows || cols != other.cols)
        {
            throw invalid_argument("Матрицы должны иметь одинаковые размеры для сложения");
        }
        Matrix result(rows, cols);
        for (size_t idx = 0; idx < rows * cols; ++idx)
        {
            result.data[idx] = data[idx] + other.data[idx];
        }
        return result;
    }

    // Конструктор вычитания двух матриц
    Matrix operator-(const Matrix &other) const
    {
        if (rows != other.rows || cols != other.cols)
        {
            throw invalid_argument("Матрицы должны иметь одинаковые размеры для вычитания");
        }
        Matrix result(rows, cols);
        for (size_t idx = 0; idx < rows * cols; ++idx)
        {
            result.data[idx] = data[idx] - other.data[idx];
        }
        return result;
    }

    // Конструктор умножения двух матриц
    Matrix operator*(const Matrix &other) const
    {
        if (cols != other.rows)
        {
            throw invalid_argument("Количество столбцов первой матрицы должно быть равно количеству строк второй матрицы");
        }
        Matrix result(rows, other.cols);
        for (size_t idx = 0; idx < rows; ++idx)
        {
            for (size_t jdx = 0; jdx < other.cols; ++jdx)
            {
                result.data[idx * other.cols + jdx] = 0;
                for (size_t k = 0; k < cols; ++k)
                {
                    result.data[idx * other.cols + jdx] += data[idx * cols + k] * other.data[k * other.cols + jdx];
                }
            }
        }
        return result;
    }

    // Конструктор умножения матрицы на число
    Matrix operator*(double scalar) const
    {
        Matrix result(rows, cols);
        for (size_t idx = 0; idx < rows; ++idx)
        {
            for (size_t jdx = 0; jdx < cols; ++jdx)
            {
                result.data[idx * cols + jdx] = data[idx * cols + jdx] * scalar;
            }
        }
        return result;
    }

    // Возведение матрицы в степень
    Matrix pow(size_t n) const
    {
        if (rows != cols)
        {
            throw invalid_argument("Для возведения в степень матрица должна быть квадратной");
        }

        Matrix result(*this); // Начинаем с исходной матрицы
        Matrix base(*this);   // Копия исходной матрицы для умножений

        for (size_t idx = 1; idx < n; ++idx)
        {
            result = result * base; // Умножаем результат на матрицу
        }

        return result;
    }

    // Транспонирование матрицы
    Matrix transpose() const
    {
        Matrix result(cols, rows); // Новая матрица будет иметь перевёрнутые размеры

        for (size_t idx = 0; idx < rows; ++idx)
        {
            for (size_t jdx = 0; jdx < cols; ++jdx)
            {
                result.data[jdx * rows + idx] = data[idx * cols + jdx]; // Меняем строки и столбцы местами
            }
        }

        return result;
    }

    // Вычисление детерминанта матрицы
    double determinant() const
    {
        if (rows != cols)
        {
            throw invalid_argument("Детерминант можно вычислить только для квадратных матриц");
        }

        // Базовый случай: если матрица 1x1
        if (rows == 1)
        {
            return data[0]; // Детеминант матрицы 1x1 — это её единственный элемент
        }

        // Для матрицы 2x2 детерминант можно вычислить напрямую
        if (rows == 2)
        {
            return data[0] * data[3] - data[1] * data[2]; // det(A) = a11*a22 - a12*a21
        }

        // Рекурсивное вычисление для матрицы больше 2x2
        double det = 0;
        for (size_t jdx = 0; jdx < cols; ++jdx)
        {
            // Создание матрицы без первой строки и текущего столбца
            Matrix minorMatrix = getMinorMatrix(0, jdx);
            det += (jdx % 2 == 0 ? 1 : -1) * data[jdx] * minorMatrix.determinant();
        }

        return det;
    }

    // Вспомогательная функция для получения минорной матрицы (матрицы без указанной строки и столбца)
    Matrix getMinorMatrix(size_t row, size_t col) const
    {
        Matrix minor(rows - 1, cols - 1);

        size_t minorIdx = 0;
        for (size_t idx = 0; idx < rows; ++idx)
        {
            if (idx == row)
                continue;
            size_t minorJdx = 0;
            for (size_t jdx = 0; jdx < cols; ++jdx)
            {
                if (jdx == col)
                    continue;
                minor.data[minorIdx * (cols - 1) + minorJdx] = data[idx * cols + jdx];
                minorJdx++;
            }
            minorIdx++;
        }

        return minor;
    }

    // Вычисление экспоненты матрицы
    Matrix exp(size_t iterations = 20) const
    {
        if (rows != cols)
        {
            throw invalid_argument("Экспонента может быть вычислена только для квадратных матриц");
        }

        Matrix result = identity(rows);
        Matrix term = identity(rows);

        for (size_t idx = 1; idx <= iterations; ++idx)
        {
            term = term * (*this) * (1.0 / idx);
            result = result + term;
        }

        return result;
    }
};

int main()
{
    /*
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
    m3.print();

    // 7. Заполнение матрицы случайными значениями
    printf("m7,8 (после случайного заполнения): \n");
    Matrix m7(2, 3);
    Matrix m8(2, 3);
    m7.fillRandom(); // Заполняем случайными значениями
    m7.print();      // Выводим матрицу
    m8.fillRandom(); // Заполняем случайными значениями
    m8.print();      // Выводим матрицу
    */
    double arr1[] = {1, 2, 3, 4, 5, 6, 73, 8, 9};
    Matrix m1(arr1, 2, 2);
    double det = m1.determinant();
    m1.print();
    printf("Определитель матрицы: %lf\n", det);

    // double arr2[] = {7, 8, 9, 0, 1, 2};
    // Matrix m2(arr2, 2, 3);
    m1.print();
    Matrix expon = m1.exp();
    expon.print();

    return 0;
}
