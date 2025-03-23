#include <iostream>
#include <cstring>
#include <cmath>


enum MatrixType {ZERO, UNIT};
enum Matrix_message_level {ERROR, WARNING, INFO, DEBUG};


void print_message(const enum Matrix_message_level level, const char *msg)
{
    if(level == ERROR) {
        printf("ERROR: %s", msg);
    }
    if(level == WARNING) {
        printf("WARNING: %s", msg);
    }
    if(level == INFO) {
        printf("INFO: %s", msg);
    }
    if(level == DEBUG) {
        printf("DEBUG: %s", msg);
    }
}


class Matrix {
private:
    size_t rows;
    size_t cols;
    double* data;

public:
    // Конструкторы
    Matrix();
    Matrix(const Matrix& M);
    Matrix(Matrix&& M);
    Matrix(const size_t n);
    Matrix(const size_t row, const size_t col);
    ~Matrix();

    // Операторы  
    Matrix& operator= (Matrix&& M); // Присваивание перемещением
    Matrix& operator= (const Matrix& M); // Присваивание копированием
    Matrix& operator+= (const Matrix& M); // Сложение матриц
    Matrix& operator-= (const Matrix& M); // Вычитание матриц
    Matrix& operator*= (const Matrix& M); // Умножение матриц
    Matrix& operator*= (const double k); // Умножение матрицы на число

    // Функции
    void print() const;
    void enter();
    void set(enum MatrixType);
    void transp();
    void multiplication_transp(const Matrix& A, const Matrix& B);
    void exponent(const Matrix& A, const size_t order);
    double determinant() const;

    size_t getRows() const { return rows; }
    size_t getCols() const { return cols; }
    double* getData() const { return data; }
};


// Конструкторы
Matrix::Matrix(): rows(0), cols(0), data(nullptr) {} // Инициализация пустой матрицы


Matrix::Matrix(const Matrix& M) : rows(M.rows), cols(M.cols) // Копирование матрицы
{
    data = new double[rows * cols];
    memcpy(data, M.data, rows * cols * sizeof(double));
}


Matrix::Matrix(Matrix&& M) // Перемещение матрицы
{  
    rows = M.rows;
    cols = M.cols;
    data = M.data;

    M.rows = 0;
    M.cols = 0;
    M.data = nullptr;
}


Matrix::Matrix(const size_t n): rows(n), cols(n) // Создание квадратной матрицы
{
    data = new double[n * n];
}


Matrix::Matrix(const size_t row, const size_t col) : rows(row), cols(col) // Создание матрицы row * col
{
    data = new double[row * col];
}


Matrix::~Matrix() // Деструктор освобождения памяти
{
    delete[] data;
}


// Операторы
Matrix& Matrix::operator= (const Matrix& M) {
    if (this == &M) return *this;
    delete[] data;

    rows = M.rows;
    cols = M.cols;

    data = new double[rows * cols];
    memcpy(data, M.data, cols * rows * sizeof(double));

    return *this;
}


Matrix& Matrix::operator= (Matrix&& M) { 
    if (this == &M)
        return *this;
    delete[] data;

    rows = M.rows;
    cols = M.cols;
    data = M.data;

    M.cols = 0;
    M.rows = 0;
    M.data = nullptr;

    return *this;
}


Matrix& Matrix::operator+= (const Matrix& M) {
    if ((rows != M.rows) || (cols != M.cols)) {
        print_message(ERROR, "Несовпадение размеров матриц\n");
        return *this;
    }

    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] += M.data[idx];
    }
    return *this;
}


Matrix& Matrix::operator-= (const Matrix& M) {
    if ((rows != M.rows) || (cols != M.cols)) {
        print_message(ERROR, "Несовпадение размеров матриц\n");
        return *this;
    }

    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] -= M.data[idx];
    }
    return *this;
}


Matrix& Matrix::operator*= (const double k) {
    if (data == nullptr) {
        print_message(ERROR, "Матрица не инициализирована\n");
        return *this;
    }

    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] *= k;
    }
    return *this;
}


Matrix& Matrix::operator*= (const Matrix& M) {
    if (cols != M.rows) {
        print_message(ERROR, "Несовпадение размеров матриц\n");
        return *this;
    }

    Matrix R(rows, M.cols);
    R.set(ZERO);
    for (size_t row = 0; row < R.rows; row++)
        for (size_t col = 0; col < R.cols; col++)
            for (size_t idx = 0; idx < M.rows; idx++)
                R.data[row * R.cols + col] += data[row * cols + idx] * M.data[idx * M.cols + col];

    *this = std::move(R);
    return *this;
}


Matrix operator+(const Matrix& M, const Matrix& K) {
    Matrix rez = M;
    rez += K;
    return rez;
}


Matrix operator-(const Matrix& M, const Matrix& K) {
    Matrix rez = M;
    rez -= K;
    return rez;
}


Matrix operator*(const Matrix& M, const Matrix& K) {
    Matrix rez = M;
    rez *= K;
    return rez;
}


Matrix operator*(const double k, const Matrix& M) {
    Matrix rez = M;
    rez *= k;
    return rez;
}


Matrix operator*(const Matrix& M, const double k) {
    return k * M;
}


// Функции
void Matrix::print() const {
    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++) {
            printf("%.2f ", data[row * cols + col]);
        }
        printf("\n");
    }
}


void Matrix::enter() {
    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++) {
            std::cin >> data[row * cols + col];
        }
    }
}


void Matrix::set(enum MatrixType type) {
    if (type == ZERO) {
        memset(data, 0, rows * cols * sizeof(double));
    } else if (type == UNIT) {
        memset(data, 0, rows * cols * sizeof(double));
        for (size_t i = 0; i < rows; i++) {
            data[i * cols + i] = 1.0;
        }
    }
}


void Matrix::transp() {
    Matrix result(cols, rows);
    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++) {
            result.data[col * rows + row] = data[row * cols + col];
        }
    }
    *this = std::move(result);
}


void Matrix::multiplication_transp(const Matrix& A, const Matrix& B) {
    Matrix B_transposed = B;
    B_transposed.transp();
    *this = A * B_transposed;
}


void Matrix::exponent(const Matrix& A, const size_t order) {
    Matrix result(A.getRows(), A.getCols());
    result.set(UNIT);

    Matrix term = result;
    for (size_t i = 1; i <= order; i++) {
        term *= A;
        term *= (1.0 / tgamma(i + 1));
        result += term;
    }

    *this = std::move(result);
}


double Matrix::determinant() const {
    if (rows != cols) {
        print_message(ERROR, "Матрица не квадратная\n");
        return NAN;
    }

    if (rows == 1) {
        return data[0];
    }

    if (rows == 2) {
        return data[0] * data[3] - data[1] * data[2];
    }

    if (rows == 3) {
        return data[0] * data[4] * data[8] 
             + data[1] * data[5] * data[6] 
             + data[2] * data[3] * data[7] 
             - data[2] * data[4] * data[6] 
             - data[1] * data[3] * data[8] 
             - data[0] * data[5] * data[7];
    }

    print_message(ERROR, "Определитель для матриц больше 3x3 не реализован\n");
    return NAN;
}


void matrix_operation(size_t number, const Matrix A, const Matrix B)
{
    if (number == 2 || number == 1) {
        Matrix C = A + B;
        printf("Результат сложения матриц A и B:\n");
        C.print();
    } 
    
    if (number == 3 || number == 1) {
        Matrix D = A - B;
        printf("Результат разности матриц A и B:\n");
        D.print();
    } 
    
    if (number == 4 || number == 1) {
        Matrix E = A * B;
        printf("Результат умножения матриц A и B:\n");
        E.print();
    } 
    
    if (number == 5 || number == 1) {
        Matrix F = A;
        F.multiplication_transp(A, B);
        printf("Результат умножения матрицы A на B транспонированную:\n");
        F.print();
    } 
    
    if (number == 6 || number == 1) {
        Matrix G = A;
        G.transp();
        printf("Транспонированная матрица A:\n");
        G.print();
    } 
    
    if (number == 7 || number == 1) {
        size_t power;
        printf("Введите целое неотрицательное число p: ");
        scanf("%zu", &power);
        Matrix H = A;
        for (size_t i = 0; i < power; i++) {
            H *= A;
        }
        printf("Результат возведения матрицы A в степень %zu:\n", power);
        H.print();
    }
    
    if (number == 8 || number == 1) {
        double ratio;
        printf("Введите число r: ");
        scanf("%lf", &ratio);
        Matrix I = A * ratio;
        printf("Результат умножения матрицы A на число %.2f:\n", ratio);
        I.print();
    } 
    
    if (number == 9 || number == 1) {
        size_t order;
        printf("Введите целое неотрицательное число o: ");
        scanf("%zu", &order);
        Matrix J = A;
        J.exponent(A, order);
        printf("Экспонента матрицы A порядка %zu:\n", order);
        J.print();
    } 
    
    if (number == 10 || number == 1) {
        printf("Определитель матрицы A: %.2f\n", A.determinant());
    }  
}


int main() {
    
    printf("Укажите номер операции, которую вы хотите выполнить:\n");
    printf("1. Выполнить все доступные операции\n");
    printf("2. Сложение матриц A и B\n");
    printf("3. Разность матриц A и B\n");
    printf("4. Умножение матриц A и B\n");
    printf("5. Умножение матрицы A на B транспонированную\n");
    printf("6. Транспонирование матрицы A\n");
    printf("7. Возведение матрицы A в степень p\n");
    printf("8. Умножение матрицы A на число r\n");
    printf("9. Экспонента матрицы A порядка o\n");
    printf("10. Вычисление определителя матрицы A\n");

    size_t number;
    scanf("%zu", &number);

    if (number < 1 || number > 10) {
        print_message(ERROR, "Такой операции нет\n");
        return EXIT_FAILURE;
    }

    printf("Ввод матрицы A\n");
    size_t rows, cols;
    printf("Введите количество строк: ");
    scanf("%zu", &rows);
    printf("Введите количество столбцов: ");
    scanf("%zu", &cols);
    Matrix A(rows, cols);
    A.enter();

    Matrix B;
    if (number < 6) {
        printf("Ввод матрицы B\n");
        printf("Введите количество строк: ");
        scanf("%zu", &rows);
        printf("Введите количество столбцов: ");
        scanf("%zu", &cols);
        B = Matrix(rows, cols);
        B.enter();
    }

    matrix_operation(number, A, B);

    return 0;
}
