#include <iostream>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <cstring>
#include <ctime>

class Matrix
{
private:
    size_t rows;
    size_t cols;
    double* data;

public:
    // Конструктор по умолчанию
    Matrix() : rows(0), cols(0), data(nullptr) {};
    Matrix(size_t rows, size_t cols);
    Matrix(const Matrix& other);  // 
    Matrix(Matrix&& other) noexcept;
    Matrix& operator=(const Matrix& other);
    Matrix& operator=(Matrix&& other) noexcept;

    // Деструктор
    ~Matrix();

    // Заполняем матрицу рандомом
    void matrixRandom();

    //Создание единичной матрицы
    void matrixIdentity();

    //Создание нулевой матрицы
    void zero_matrix();

    //Вывод матрицы в консоль
    void matrix_print() const;

    //Операции над матрицами
    Matrix operator+(Matrix& other);
    Matrix operator-(Matrix& other);
    Matrix operator*(Matrix& other);
    Matrix operator*(double scalar);
    Matrix transponate();
    double determinant();
    Matrix matrix_exponent();
    Matrix matrix_involution(unsigned int level);


};  

// Конструктор с параметрами
Matrix::Matrix(size_t rows, size_t cols) : rows(rows), cols(cols) 
{
    data = new double[rows*cols];
    std::memset(data, 0, rows * cols * sizeof(double));
}

// Конструктор копирования
Matrix::Matrix(const Matrix& other) : rows(other.rows), cols(other.cols) 
{
    data = new double[rows * cols];
    std::memcpy(data, other.data, rows * cols * sizeof(double));
}

// Конструктор перемещений
Matrix::Matrix(Matrix&& other) noexcept : rows(other.rows), cols(other.cols), data(other.data) 
{  
    other.rows = 0;
    other.cols = 0;
    other.data = nullptr;
}

// Оператор присваивания копирования
Matrix& Matrix::operator=(const Matrix& other) 
{
    if (this == &other) {
        return *this;
    }

    delete[] data;
    
    rows = other.rows;
    cols = other.cols;
    data = new double[rows*cols];
    memcpy(data, other.data, rows * cols * sizeof(double));
    
    return *this;
}

// Оператор присваивания перемещения
Matrix& Matrix::operator=(Matrix&& other) noexcept 
{
    if (this == &other) return *this;
    
    delete[] data;

    rows = other.rows;
    cols = other.cols;
    data = other.data;
    
    other.rows = 0;
    other.cols = 0;
    other.data = nullptr;
    
    return *this;
}

Matrix::~Matrix() {
    delete[] data;
}


//Заполнение матрицы рандомом
void Matrix::matrixRandom() 
{ 
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            int sign = (std::rand() % 2) * 2 - 1;
            data[row*cols + col] = (std::rand() % 100) * sign;
        }
    }
}


//Единичная матрица
void Matrix::matrixIdentity()
{
    for (size_t row = 0; row < rows; ++row ) {
        for (size_t col = 0; col < cols; ++col) {
            if (row == col) {
                data[row * cols + col] = 1;
            }

            else {
                data[row * cols + col] = 0;
            }
        } 
    }
}


//Вывод матрицы в консоль
void Matrix::matrix_print() const 
{
    for (size_t row = 0; row< rows; ++row) {
        for (size_t col = 0; col< cols; ++col) {
            std:: cout << data[row*cols+col]<< "\t";
        }
        std:: cout << "\n";
    }
    std:: cout << "\n";
}


//Сложение матриц
Matrix Matrix::operator+( Matrix& other)  
{
    if (other.data == nullptr) {
        throw std::invalid_argument("Пустая матрица");
    }

    if ((rows != other.rows) || (cols != other.cols)) {
        throw std::invalid_argument("Неправильные размеры матриц");
    }

    Matrix sum_result(rows, cols);

    if (sum_result.rows == 0 || sum_result.cols == 0) {
        throw std::invalid_argument("Число строк или столбцов не может быть равно 0");
    }

    for (size_t idx = 0; idx < rows*cols; ++idx ) {
        sum_result.data[idx] = data[idx] + other.data[idx];
    }

    return sum_result;
}


//Вычитание матриц
Matrix Matrix::operator-( Matrix& other)  
{
    if (other.data == nullptr) {
        throw std::invalid_argument("Пустая матрица");
    }

    if ((rows != other.rows) || (cols != other.cols)) {
        throw std::invalid_argument("Неправильные размеры матриц");
    }

    Matrix sum_result(rows, cols);

    if (sum_result.rows == 0 || sum_result.cols == 0) {
        throw std::invalid_argument("Число строк или столбцов не может быть равно 0");
    }

    for (size_t idx = 0; idx < rows*cols; ++idx ) {
        sum_result.data[idx] = data[idx] - other.data[idx];
    }

    return sum_result;
}


//Умножение матриц
Matrix Matrix::operator*(Matrix& other)
{
    if (other.data == nullptr) {
        throw std::invalid_argument("Пустая матрица");
    }

    if (cols != other.rows) {
        throw std::invalid_argument("Неправильный размер матриц");
    }

    Matrix multiply_result(rows, other.cols);

    if (rows == 0 || other.cols == 0 ) {
        throw std:: invalid_argument("Число строк или столбцов не может быть равно 0");
    }

    for (size_t row = 0; row < multiply_result.rows; ++row) {
        for (size_t col = 0; col < multiply_result.cols; ++col) {

            multiply_result.data[row*multiply_result.cols + col] = 0;

            for (size_t k = 0; k < other.rows; ++k) {
                multiply_result.data[row*multiply_result.cols + col] += data[row*cols + k] * other.data[k*other.cols+col];
            }
        }
    }
    return multiply_result;
}


//Умножение матрицы на число
Matrix Matrix::operator*(double scalar)
{
    if (data == nullptr) {
        throw std::invalid_argument("Пустая матрица");
    }

    Matrix result(rows, cols);

    if (rows == 0 || cols == 0) {
        throw std:: invalid_argument("Число строк или столбцов не может быть равно 0");
    }

    for (size_t idx = 0; idx<rows*cols; ++idx) {
        result.data[idx] = data[idx] * scalar;  
    }
    return result;
}


//Транспонирование матрицы
Matrix Matrix::transponate()
{
    if (data == nullptr) {
        throw std::invalid_argument("Пустая матрица");
    }

    Matrix result(cols, rows);

    if (rows == 0 || cols == 0) {
        throw std::invalid_argument("Пустая строка или столбец");
    }

    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            result.data[col*rows+row] = data[row*cols + col];
        }
    }

    return result;
}


//Считаем дискриминант
double Matrix::determinant()
{
    if (rows != cols) {
        throw std::invalid_argument("Число строк не равно числу столбцов");
        return -1;
    }

    double Det, det_sign;
    det_sign = 0;

    if (rows == 0) {
        return Det = 0;
    }

    if (rows == 1) {
        return Det = data[0];
    }

    if (rows == 2) {
        Det = (data[0] * data[3]) - (data[1] * data[2]);
        return Det;
    }

    for (size_t col = 0; col < cols; ++col) {

        Matrix minor(rows-1, cols-1);

        if (minor.data == nullptr) {
            std:: cout << "Пустая матрица" << "\n";
            return -1;
        }

        size_t minor_row = 0;

        for (size_t row = 1; row < rows; ++row) {

            size_t minor_col = 0;

            for (size_t k_col=0; k_col < cols; ++k_col) {
                        
                if (col != k_col) {
                    minor.data[minor_row * (cols-1) + minor_col] = data[row * cols + k_col];
                    minor_col++;
                    }
                }
                minor_row++;
            }
        double minor_det = minor.determinant();

        det_sign += (col % 2 == 0 ? 1: -1) * data[col] * minor_det;
    } 
    return det_sign;
}


//Вычисляем факториал
static double factorial(int n) 
{
    return (n < 2) ? 1 : n * factorial (n - 1);
}


//Возведение матрицы с степень
Matrix Matrix:: matrix_involution( unsigned int level)
{
    if (data == nullptr) {
        throw std:: invalid_argument("Пустая матрица");
    }

    if (rows != cols) {
        throw std:: invalid_argument("Матрица не квадратная");
    }

    Matrix result(rows, cols); //выделяем память для матрицы результата
    result.matrixIdentity(); //создаем единичную матрицу

    if (level == 0) {  
        return result;
    }

    Matrix temp = *this; //приравниваем к текущей матрице

    if (level == 1) {
        return temp;
    }

    while (level > 0) {
        if (level % 2 == 1) {
            result = result * temp; 
        }

        temp = temp * temp;
        level /= 2;
    }

    return result;
}


//Создание нулевой матрицы
void Matrix:: zero_matrix() 
{
    if (data == nullptr) {
        throw std:: invalid_argument("Пустая матрица");
    }

    for (size_t idx = 0; idx < rows * cols; ++idx) {
        data[idx] = 0;
    }
}

//Матричная экспонента
Matrix Matrix::matrix_exponent()
{
    if (data == nullptr) {
        throw std:: invalid_argument("Пустая матрица");
    }

    if (rows != cols) {
        throw std:: invalid_argument("Матрица не квадратная");
    }

    Matrix E_sum(rows, cols);

    E_sum.zero_matrix();

    for (int k =0 ; k <= 20; ++k) {
        Matrix C = this->matrix_involution(k); //вычисляем А^k
        double factor = 1 / factorial(k); //Вычисляем 1/k!
        Matrix term = C * factor; //
        E_sum = E_sum + term;
    }
    return E_sum;
}


int main() {

    setlocale(LC_ALL, "");
    std::srand(std::time(nullptr)); 
    
    // Пример использования
    Matrix m1(3, 3);
    Matrix m2(3, 3);

    m1.matrixRandom();
    //m2.matrixRandom();
    m2.matrixIdentity();
    
    m1.matrix_print();
    m2.matrix_print();
 
    Matrix m3 = m1.transponate();    
    m3.matrix_print();
    
    
    double det = m1.determinant();
    std:: cout << det << "\n";
     


    
    return 0;
}