#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>

enum MatrixExceptionLevel {ERROR, WARNING, INFO, DEBUG};

void matrix_exception(const enum MatrixExceptionLevel level, const char *msg)
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
    Matrix() : rows(0), cols(0), data(nullptr) {}

    Matrix(size_t rows, size_t cols) : rows(rows), cols(cols) {
        data = new double[rows * cols]();
    }

    Matrix(size_t rows, size_t cols, const double* data) : rows(rows), cols(cols) {
        
        if (rows == 0  || cols == 0) { 
            matrix_exception(WARNING, "Матрица содержит 0 элементов!\n");
            rows = 0;
            cols = 0;
        }
    
        if (SIZE_MAX / cols < rows) { 
            matrix_exception(ERROR, "Размеры матрицы превышают допустимое значение\n");
            rows = 0;
            cols = 0;
            data = nullptr; 
        }
    
        if (SIZE_MAX / (cols * sizeof(double)) < rows) { 
            matrix_exception(ERROR, "Не хватит места для выделения памяти под строки и столбцы\n");
            rows = 0;
            cols = 0;
            data = nullptr;
        }
        
        this->data = new double[rows * cols];
        memcpy(this->data, data, rows * cols * sizeof(double));
    
        if (data == nullptr) 
            matrix_exception(WARNING, "Сбой выделения памяти!");
    }

    ~Matrix() {
        delete[] data;
    }

    void matrix_print() const {
        for (size_t row = 0; row < rows; row++) {
            for (size_t col = 0; col < cols; col++) 
                printf("%.2f ", data[row * cols + col]);
            
            printf("\n");
        }
        printf("\n");
    }

    Matrix operator=(const Matrix& B) {
        delete[] data;
        rows = B.rows;
        cols = B.cols;
        data = new double[rows * cols];
        for (size_t i = 0; i < rows * cols; i++) 
            data[i] = B.data[i];
    };

    Matrix operator+(const Matrix& B) const {
        if (rows != B.rows || cols != B.cols) 
            matrix_exception(WARNING, "Размеры матриц не подходят для вычитания");
        

        Matrix result(rows, cols);
        for (size_t i = 0; i < rows * cols; i++) 
            result.data[i] = data[i] + B.data[i];
        
        return result;
    }
};

int main() {
    double A[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    double B[9] = {9, 8, 7, 6, 5, 4, 3, 2, 1};

    Matrix mA(3, 3, A);
    Matrix mB(3, 3, B);
    Matrix C  = mA + mB;
    mA.matrix_print();
    mB.matrix_print();
    C.matrix_print();

    return 0;
}
