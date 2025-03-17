#include <cstddef>
#include <iostream>
#include <stdexcept>

enum class MatrixLogLevel {
    WARNING,
    INFO,
    ERROR,
};


enum class MatrixErrorType {
    DIMENSION_MISMATCH_ERROR,  
    DIMENSION_ERROR,  
    NOT_SQUARE_ERROR,
    OUT_OF_BOUND_ERROR,
    INTERNAL_ERROR
};


class MatrixException {
public:
    MatrixErrorType error_type;
    std::string message;

    MatrixException(MatrixErrorType type, const std::string& msg)
        : error_type(type), message(msg) {}

    std::string get_message() const {
        return message;
    }

    MatrixErrorType get_error_type() const {
        return error_type;
    }
};


class Matrix {
private:
    size_t rows;
    size_t cols;
    double* data;
public:
    Matrix();                            
    Matrix(size_t rows, size_t cols);    
    Matrix(const Matrix& other);          
    Matrix(Matrix&& other);
    ~Matrix();
    Matrix& operator=(const Matrix& other);
    Matrix& operator=(const Matrix&& other);
    size_t get_rows() const { return rows; }
    size_t get_cols() const { return cols; }
    bool is_empty() const { return data == nullptr; }
    bool is_square() const { return rows == cols; }
    void set(size_t row, size_t col, double value);
    double get(size_t row, size_t col) const;
    void print() const;
    static void log(MatrixLogLevel level, const char* location, const char* msg);
    Matrix operator+(const Matrix& other) const; 
    Matrix operator-(const Matrix& other) const; 
    Matrix operator*(const Matrix& other) const; 
    Matrix operator*(double scalar) const;       
    Matrix transpose() const;
    Matrix power(int exponent) const;  
    Matrix exponent(unsigned int num_terms) const; 
    double determinant() const;        
    static Matrix identity(size_t size);
private:
    size_t index(size_t row, size_t col) const { return row * cols + col; };
};
