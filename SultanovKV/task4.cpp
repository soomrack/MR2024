#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <iomanip>

class Matrix {
private:
    size_t cols;
    size_t rows;
    std::vector<std::vector<double>> data;
    
    void initializeZero(){
        data.resize(rows, std::vector<double>(cols,0.0));
    };
    
    static unsigned long long factorial (unsigned int n){
        unsigned long long result = 1;
        for (unsigned int i = 2; i <= n; ++i){
            
            result*=i;
        }
        return result;
    }
public:
enum class exceptionLevel {Error, Warning, Info};
Matrix(size_t rows =0, size_t cols = 0): rows(rows),cols(cols){
    if( rows == 0 || cols == 0){
        logMessage(exceptionLevel :: Info,"Matrix has 0 rows or cols");
        rows = cols = 0;
        return;
    }
    initializeZero();
}
Matrix(const std::vector<std::vector<double>>& values){
    if(values.empty() || values[0].empty()){
        rows = cols = 0;
        logMessage(exceptionLevel::Info,"Matrix initialized with empty data");
    return;
    }
    rows = values.size();
    cols = values[0].size();
    data = values;
    }

static void logMessage(exceptionLevel levele, const std::string& msg){
    switch (levele){
        
        case exceptionLevel::Error:std::cerr<<"Error:"<<msg<<std::endl;break;
        case exceptionLevel::Warning:std::cout<<"Warning:"<<msg<<std::endl;break;
        case exceptionLevel::Info:std::cout<<"Info:"<<msg<<std::endl;break;
    }
}

size_t getRows()const {return rows;}
size_t getCols()const {return cols;}
bool isSquare()const {return rows == cols;}

double& operator() (size_t row, size_t col){
    if(row>= rows || col>= cols){
        throw std::out_of_range("Matrix indeces out of range"); /////////////////////поч оф ренж именно
    }
return data[row][col];
    }

const double& operator()(size_t row, size_t col) const{
    if(row>=rows || col>= cols){
    throw std::out_of_range("Matrix indeces out of range");///////////////////поч тоже оф ренж именно
    }
    return data[row][col];
}


static Matrix identity(size_t size){
    Matrix result (size,size);
    for (size_t i=0;i<size;++i){
        
        result(i,i) =1.;
    }
    return result;
}


void print() const{
    for (size_t i = 0; i < rows; ++i){         //////////////////////////////
         for (size_t j = 0; j < cols; ++j) {
                std::cout << std::fixed << std::setprecision(2) << data[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }

    // Операции с матрицами
    Matrix operator+(const Matrix& other) const {
        if (rows != other.rows || cols != other.cols) {
            logMessage(exceptionLevel::Warning, "Matrix dimensions don't match for addition");
            return Matrix(0, 0);
        }

        Matrix result(rows, cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result(i, j) = data[i][j] + other.data[i][j];
            }
        }
        return result;
    }

    Matrix operator-(const Matrix& other) const {
        if (rows != other.rows || cols != other.cols) {
            logMessage(exceptionLevel::Warning, "Matrix dimensions don't match for subtraction");
            return Matrix(0, 0);
        }

        Matrix result(rows, cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result(i, j) = data[i][j] - other.data[i][j];
            }
        }
        return result;
    }

    Matrix operator*(const Matrix& other) const {
        if (cols != other.rows) {
            logMessage(exceptionLevel::Warning, 
                      "Number of columns in first matrix doesn't match number of rows in second matrix");
            return Matrix(0, 0);
        }

        Matrix result(rows, other.cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < other.cols; ++j) {
                for (size_t k = 0; k < cols; ++k) {
                    result(i, j) += data[i][k] * other.data[k][j];
                }
            }
        }
        return result;
    }

    Matrix operator*(double scalar) const {
        Matrix result(rows, cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result(i, j) = data[i][j] * scalar;
            }
        }
        return result;
    }

    // Транспонирование
    Matrix transpose() const {
        Matrix result(cols, rows);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result(j, i) = data[i][j];
            }
        }
        return result;
    }

    // Возведение в степень
    Matrix power(unsigned int power) const {
        if (!isSquare()) {
            logMessage(exceptionLevel::Warning, "Matrix must be square for exponentiation");
            return Matrix(0, 0);
        }

        if (power == 0) {
            return identity(rows);
        }

        Matrix result = *this;
        for (unsigned int i = 1; i < power; ++i) {
            result = result * (*this);
        }
        return result;
    }

    // Определитель
    double determinant() const {
        if (!isSquare()) {
            logMessage(exceptionLevel::Warning, "Matrix must be square to calculate determinant");
            return NAN;
        }

        if (rows == 1) return data[0][0];
        if (rows == 2) return data[0][0] * data[1][1] - data[0][1] * data[1][0];
        if (rows == 3) {
            return data[0][0] * (data[1][1] * data[2][2] - data[1][2] * data[2][1]) -
                   data[0][1] * (data[1][0] * data[2][2] - data[1][2] * data[2][0]) +
                   data[0][2] * (data[1][0] * data[2][1] - data[1][1] * data[2][0]);
        }
        
        logMessage(exceptionLevel::Warning, "Determinant calculation not implemented for matrices larger than 3x3");
        return NAN;
    }

    // Матричная экспонента
    Matrix exponential(unsigned int terms = 10) const {
        if (!isSquare()) {
            logMessage(exceptionLevel::Warning, "Matrix must be square for exponential calculation");
            return Matrix(0, 0);
        }

        Matrix result = identity(rows);
        Matrix term = identity(rows);
        
        for (unsigned int n = 1; n <= terms; ++n) {
            term = term * (*this) * (1.0 / factorial(n));
            result = result + term;
        }
        
        return result;
    }
};

int main() {
    try {
        // Создаем матрицы
        Matrix A({
            {7, 3, 6},
            {3, 6, 7},
            {2, 1, 1}
        });

        Matrix B({
            {5, 7, 7},
            {5, 3, 2},
            {1, 2, 4}
        });

        // Печать исходных матриц
        std::cout << "Matrix A:" << std::endl;
        A.print();
        
        std::cout << "Matrix B:" << std::endl;
        B.print();

        // Операции с матрицами
        std::cout << "Sum of A and B:" << std::endl;
        (A + B).print();

        std::cout << "Subtraction of A and B:" << std::endl;
        (A - B).print();

        std::cout << "Multiplication of A and B:" << std::endl;
        (A * B).print();

        std::cout << "Transpose of A:" << std::endl;
        A.transpose().print();

        std::cout << "A to the power of 3:" << std::endl;
        A.power(3).print();

        std::cout << "A multiplied by 5:" << std::endl;
        (A * 5.0).print();

        std::cout << "Determinant of A: " << A.determinant() << std::endl;

        std::cout << "Exponential of A (10 terms):" << std::endl;
        A.exponential(10).print();

    } catch (const std::exception& e) {
        Matrix::logMessage(Matrix::exceptionLevel::Error, e.what());
        return 1;
    }

    return 0;
}
        
   








