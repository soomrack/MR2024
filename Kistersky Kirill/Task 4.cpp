#include <iostream>
#include <string>
#include <cstring>

typedef double MatrixItem;
enum MatrixType { ZERO, ONES, RANDOM, I };

class Matrix {
private:
    size_t rows;
    size_t cols;
    MatrixItem* data;
public:
    Matrix();
    Matrix(const size_t n);
    Matrix(const size_t row, const size_t col);
    Matrix(const Matrix& M);
    Matrix(Matrix&& M);
    ~Matrix();
public:
    Matrix& operator=(const Matrix& M); 
    Matrix& operator=(Matrix&& M);
    Matrix& operator+=(const Matrix& M);
    Matrix& operator-=(const Matrix& M);
    Matrix& operator*=(const double k);
    Matrix& operator*=(const Matrix& M);

public:
    void print();
    void set(enum MatrixType mat_type);
    void transposition(const Matrix& M);
    double determinant(void);
    void exp(const unsigned int level);
};


Matrix operator+(const Matrix& M, const Matrix& K);
Matrix operator-(const Matrix& M, const Matrix& K);
Matrix operator*(const Matrix& M, const Matrix& K);
Matrix operator*(const double k, const Matrix& M);
Matrix operator*(const Matrix& M, const double k);


// Обработка исключений
class Matrix_Exception : public std::exception
{
private:
    std::string message;
public:
    Matrix_Exception(std::string message) : message{ message } {}
    std::string get_message() const { return message; }
};

Matrix_Exception INCORRECT_INPUT_MATRIX("Incorrect input matrix\n");
Matrix_Exception INCORRECT_DIMENTION_OF_MATRIX("The matrix has an incorrect dimension\n");
Matrix_Exception NULL_MATRIX("Your matrix is empty\n");
Matrix_Exception OTHER_ERROR("An unfamiliar error\n");


// Конструкторы проверка на переполнние во всех
Matrix::Matrix() : rows(0), cols(0), data(nullptr) {
}


Matrix::Matrix(const size_t n) : rows(n), cols(n) {
    if (n == 0) {
        data = nullptr;
        return;
    }

    if (n > std::numeric_limits<size_t>::max() / n) {     
        throw std::overflow_error("Размер матрицы слишком велик для выделения памяти.");
    }
    data = new MatrixItem[n * n];
}


Matrix::Matrix(const size_t row, const size_t col) : rows(row), cols(col) {
    if (row == 0 || col == 0) {
        data = nullptr;
        return;
    }

    if (row > std::numeric_limits<size_t>::max() / col) {     
        throw std::overflow_error("Размер матрицы слишком велик для выделения памяти.");
    }
    data = new MatrixItem[row * col];
}


Matrix::Matrix(const Matrix& M) : rows(M.rows), cols(M.cols) {
    data = new MatrixItem[rows * cols];

    memcpy(data, M.data, rows * cols * sizeof(MatrixItem));
}


Matrix::Matrix(Matrix&& M) : rows(M.rows),cols(M.cols),data(M.data) {

    M.rows = 0;
    M.cols = 0;
    M.data = nullptr;
}


Matrix::~Matrix() {
    rows = 0;
    cols = 0;
    delete[] data;
}


// Методы
void Matrix::print()
{
    if (data == nullptr) throw NULL_MATRIX;

    std::cout << "\n";
    for (size_t r = 0; r < rows; r++) {
        std::cout << "[";
        for (size_t c = 0; c < cols; c++) {
            std::cout << data[r * cols + c];
            if (cols != cols - 1)
                std::cout << "\t";
        }
        std::cout << "]\n";
    }
    std::cout << "\n";
}


void Matrix::set(enum MatrixType mat_type)
{
    switch (mat_type)
    {
    case (ZERO):
        memset(data, 0, cols * rows * sizeof(MatrixItem));
        break;

    case (ONES):
        for (size_t idx = 0; idx < cols * rows; idx++)
            data[idx] = 1.0;
        break;

    case (RANDOM):
        for (size_t idx = 0; idx < cols * rows; idx++)
            data[idx] = (MatrixItem)(rand() % 10);
        break;

    case (I):
        this->set(ZERO);
        for (size_t row_col = 0; row_col < rows; row_col++)
            data[row_col * cols + row_col] = 1;
        break;
    }
}


void Matrix::transposition(const Matrix& M) 
{
    if (M.data == nullptr) {
        delete[] data;
        data = nullptr;
        rows = M.cols;
        cols = M.rows;
        return;
    }

    Matrix R(M.rows, M.cols);

    for (size_t row = 0; row < M.rows; row++)
        for (size_t col = 0; col < M.cols; col++)
            data[row * M.rows + col] = M.data[col * M.cols + row];
    *this = R;
}


double Matrix::determinant(void)
{
    if (cols == 1) {
        return data[0];
    }

    if (cols == 2) {
        return data[0] * data[3] - data[1] * data[2];
    }

    if (cols == 3) {
        return (data[0] * data[4] * data[8]) + \
            (data[1] * data[5] * data[6]) + \
            (data[2] * data[3] * data[7]) - \
            (data[2] * data[4] * data[6]) - \
            (data[0] * data[5] * data[7]) - \
            (data[1] * data[3] * data[8]);
    }

    else throw INCORRECT_INPUT_MATRIX;
}


void Matrix::exp(const unsigned int level = 10) {
    if (data == nullptr) throw NULL_MATRIX;

    Matrix R(rows, cols);
    R.set(I);

    double n = 1.0;
    Matrix temp = R;
    for (unsigned int idx = 1; idx < level; idx++) {
        temp *= *this;
        n /= (idx);
        R += (temp * n);
    }
    *this = R;
}


// Перегрузка операторов
Matrix& Matrix::operator=(const Matrix& M) {     
    if (this == &M) return *this;
    if (M.data == nullptr) {
        delete[] data;
        data = nullptr;
        rows = 0;
        cols = 0;
        return *this;
    }

    if (rows == M.rows && cols == M.cols) {
        memcpy(data, M.data, rows * cols * sizeof(MatrixItem));
        return *this;
    }


    if (data != nullptr) delete[] data;

    rows = M.rows;
    cols = M.cols;

    this->data = new MatrixItem[rows * cols];
    memcpy(data, M.data, cols * rows * sizeof(MatrixItem));

    return *this;
}


Matrix& Matrix::operator= (Matrix&& M) {
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
    if ((rows != M.rows) or (cols != M.cols))
        throw INCORRECT_DIMENTION_OF_MATRIX;

    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] += M.data[idx];
    }
    return *this;
}


Matrix& Matrix::operator-= (const Matrix& M) {
    if ((rows != M.rows) or (cols != M.cols))
        throw INCORRECT_DIMENTION_OF_MATRIX;

    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] -= M.data[idx];
    }
    return *this;
}


Matrix& Matrix::operator*= (const double k) {
    if (data == nullptr) throw NULL_MATRIX;

    for (size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] *= k;
    }
    return *this;
}


Matrix& Matrix::operator*= (const Matrix& M) {
    if (cols != M.rows)
        throw INCORRECT_DIMENTION_OF_MATRIX;

    Matrix R(rows, M.cols);
    R.set(ZERO);
    for (size_t row = 0; row < R.rows; row++)
        for (size_t col = 0; col < R.cols; col++)
            for (size_t idx = 0; idx < M.rows; idx++)
                R.data[row * R.cols + col] += data[row * cols + idx] * M.data[idx * M.cols + col];
    
    delete[] data;
    data = R.data;
    rows = R.rows;
    cols = R.cols;

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



int main(void)
{
    try {
        unsigned int select, run = 1;
        Matrix A(3), B(3, 3), C(4);

        while (run)
        {
            std::cout << "\n";
            std::cout << "\\\\\\\\\\\\\\\\\\\\\\\n"
                "0 - exit\n"
                "1 - set\n"
                "2 - print A, B, C\n"
                "3 - C = A\n"
                "4 - C += A\n"
                "5 - C -= A\n"
                "6 - C *= 2.0\n"
                "7 - C *= B\n"
                "8 - C = A + B\n"
                "9 - C = A - B\n"
                "10 - C = A * 2\n"
                "11 - C = A * B\n"
                "12 - A.print()\n"
                "13 - B.print()\n"
                "14 - C.transposition(A)\n"
                "15 - C.determinant()\n"
                "16 - C.exp(20)\n";
            std::cout << "\\\\\\\\\\\\\\\\\\\\\\\n"
                << std::endl;

            std::cout << "Select the required operation number: " << std::endl;
            std::cin >> select;

            switch (select)
            {
            case 0:
                run = 0;
                break;
            case 1:
                A.set(RANDOM);
                B.set(RANDOM);
                C.set(I);
                break;
            case 2:
                A.print();
                B.print();
                C.print();
                break;
            case 3:
                C = A;
                C.print();
                break;
            case 4:
                C += A;
                C.print();
                break;
            case 5:
                C -= A;
                C.print();
                break;
            case 6:
                C *= 2.0;
                C.print();
                break;
            case 7:
                C *= B;
                C.print();
                break;
            case 8:
                C = A + B;
                C.print();
                break;
            case 9:
                C = A - B;
                C.print();
                break;
            case 10:
                C = A * 2;
                C.print();
                break;
            case 11:
                C = A * B;
                C.print();
                break;
            case 12:
                A.print();
                break;
            case 13:
                B.print();
                break;
            case 14:
                C.transposition(A);
                C.print();
                break;
            case 15:
                std::cout << C.determinant() << std::endl;
                break;
            case 16:
                C.exp(20);
                C.print();
                break;
            case 17:
                break;
            default:
                printf("wrong choice\n");
                break;
            }
        }
        return 0;
    }
    catch (const Matrix_Exception& exception)
    {
        std::cout << exception.get_message() << std::endl;
    }
}
