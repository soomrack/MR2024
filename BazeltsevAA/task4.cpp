#include <iostream>
#include <cstring>
#include <cmath>
#include <stdexcept>

enum MatrixType {ZERO, UNIT};

class MatrixException : public std::domain_error {
public:
    explicit MatrixException(const std::string& message) : std::domain_error(message) {} 
};

const MatrixException MEMORY_ERROR("Ошибка выделения памяти");
const MatrixException SIZE_ERROR("Матрицы разного размера");
const MatrixException MULTIPLY_ERROR("Количество столбцов A и строк B не совпадает");
const MatrixException EMP_ERROR("Матрица пустая");
const MatrixException SQR_ERROR("Определитель можно вычислить только для квадратных матриц");

class Matrix {
private:
    size_t rows;
    size_t cols;
    double* data;

    void check_non_empty() const {
        if (rows == 0 || cols == 0 || data == nullptr) {
            throw EMP_ERROR;
        }
    }

public:
    // Конструкторы
    Matrix();
    Matrix(const Matrix& M);
    Matrix(Matrix&& M);
    Matrix(const size_t n);
    Matrix(const size_t row, const size_t col);
    ~Matrix();

    // Операторы  
    Matrix& operator= (Matrix&& M);
    Matrix& operator= (const Matrix& M);
    Matrix& operator+= (const Matrix& M);
    Matrix& operator-= (const Matrix& M);
    Matrix& operator*= (const Matrix& M);
    Matrix& operator*= (const double k);
    
    Matrix operator+(const Matrix& M) const {
        Matrix result = *this;
        result += M;
        return result;
    }

    Matrix operator-(const Matrix& M) const {
        Matrix result = *this;
        result -= M;
        return result;
    }

    Matrix operator*(const Matrix& M) const {
        Matrix result = *this;
        result *= M;
        return result;
    }

    Matrix operator*(const double k) const {
        Matrix result = *this;
        result *= k;
        return result;
    }

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


Matrix operator*(const double k, const Matrix& M) {
    return M * k;
}


// Конструкторы
Matrix::Matrix(): rows(0), cols(0), data(nullptr) {}


Matrix::Matrix(const Matrix& M) : rows(M.rows), cols(M.cols)
{
    if (rows > 0 && cols > 0) {
        data = new (std::nothrow) double[rows * cols];
        if (!data) throw MEMORY_ERROR;
        memcpy(data, M.data, rows * cols * sizeof(double));
    } else {
        data = nullptr;
    }
}


Matrix::Matrix(Matrix&& M): rows(M.rows), cols(M.cols), data(M.data)
{  
    M.rows = 0;
    M.cols = 0;
    M.data = nullptr;
}


Matrix::Matrix(const size_t n): rows(n), cols(n)
{
    if (n > 0) {
        data = new (std::nothrow) double[n * n];
        if (!data) throw MEMORY_ERROR;
    } else {
        data = nullptr;
    }
}


Matrix::Matrix(const size_t row, const size_t col): rows(row), cols(col)
{
    if (row > 0 && col > 0) {
        data = new (std::nothrow) double[row * col];
        if (!data) throw MEMORY_ERROR;
    } else {
        data = nullptr;
    }
}


Matrix::~Matrix()
{
    delete[] data;
}


// Операторы
Matrix& Matrix::operator=(const Matrix& M) {
    if (this == &M) return *this;
    
    delete[] data;
    rows = M.rows;
    cols = M.cols;

    if (rows > 0 && cols > 0) {
        data = new (std::nothrow) double[rows * cols];
        if (!data) throw MEMORY_ERROR;
        memcpy(data, M.data, cols * rows * sizeof(double));
    } else {
        data = nullptr;
    }

    return *this;
}


Matrix& Matrix::operator=(Matrix&& M) { 
    if (this == &M) return *this;

    delete[] data;

    rows = M.rows;
    cols = M.cols;
    data = M.data;

    M.cols = 0;
    M.rows = 0;
    M.data = nullptr;

    return *this;
}


Matrix& Matrix::operator+=(const Matrix& M) {
    check_non_empty();
    M.check_non_empty();
    
    if ((rows != M.rows) || (cols != M.cols)) {
        throw SIZE_ERROR;
    }

    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] += M.data[idx];
    }
    return *this;
}


Matrix& Matrix::operator-=(const Matrix& M) {
    check_non_empty();
    M.check_non_empty();
    
    if ((rows != M.rows) || (cols != M.cols)) {
        throw SIZE_ERROR;
    }

    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] -= M.data[idx];
    }
    return *this;
}


Matrix& Matrix::operator*=(const double k) {
    check_non_empty();

    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] *= k;
    }
    return *this;
}


Matrix& Matrix::operator*=(const Matrix& M) {
    check_non_empty();
    M.check_non_empty();
    
    if (cols != M.rows) {
        throw MULTIPLY_ERROR;
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


// Функции
void Matrix::print() const {
    check_non_empty();
    
    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++) {
            printf("%.2f ", data[row * cols + col]);
        }
        printf("\n");
    }
}


void Matrix::enter() {
    check_non_empty();
    
    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++) {
            std::cin >> data[row * cols + col];
        }
    }
}


void Matrix::set(enum MatrixType type) {
    check_non_empty();
    
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
    check_non_empty();
    
    Matrix result(cols, rows);
    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++) {
            result.data[col * rows + row] = data[row * cols + col];
        }
    }
    *this = std::move(result);
}


void Matrix::exponent(const Matrix& A, const size_t order) {
    A.check_non_empty();
    if (A.rows != A.cols) throw SQR_ERROR;
    
    Matrix result(A.getRows(), A.getCols());
    result.set(UNIT);

    Matrix term = result;
    for (size_t i = 1; i <= order; i++) {
        term *= A * (1.0 / tgamma(i + 1));
        result += term;
    }

    *this = std::move(result);
}


double Matrix::determinant() const {
    check_non_empty();
    
    if (rows != cols) {
        throw SQR_ERROR;
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

    throw MatrixException("Determinant calculation not implemented for matrices larger than 3x3");
}


void matrix_operation(size_t number, const Matrix A, const Matrix B)
{
    try {
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
        
        if (number == 5 || number == 1) {
            printf("Определитель матрицы A: %.2f\n", A.determinant());
        }
    }
    catch (const MatrixException& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Неизвестная ошибка: " << e.what() << std::endl;
    }
}


int main() {
    try {
        printf("Укажите номер операции, которую вы хотите выполнить:\n");
        printf("1. Выполнить все доступные операции\n");
        printf("2. Сложение матриц A и B\n");
        printf("3. Разность матриц A и B\n");
        printf("4. Умножение матриц A и B\n");
        printf("5. Вычисление определителя матрицы A\n");
        printf("6. Транспонирование матрицы A\n");
        printf("7. Возведение матрицы A в степень p\n");
        printf("8. Умножение матрицы A на число r\n");
        printf("9. Экспонента матрицы A порядка o\n");

        size_t number;
        scanf("%zu", &number);

        if (number < 1 || number > 9) {
            throw MatrixException("Такой операции нет");
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
        if (number < 5) {
            printf("Ввод матрицы B\n");
            printf("Введите количество строк: ");
            scanf("%zu", &rows);
            printf("Введите количество столбцов: ");
            scanf("%zu", &cols);
            B = Matrix(rows, cols);
            B.enter();
        }

        matrix_operation(number, A, B);
    }
    catch (const MatrixException& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (const std::exception& e) {
        std::cerr << "Неизвестная ошибка: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}
