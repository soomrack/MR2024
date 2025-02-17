#pragma once
#include <cmath>
#include <cstddef>
#include <memory>
#include <string>


namespace MTL { // Matrix Lib


class MatrixException : public std::exception {
protected:
    std::string message;

    enum class TypeException {
        ERROR,
        WARNING,
        INFO,
    };

    const std::string get_prefix_message(TypeException type_exception) const noexcept;
public:
    explicit MatrixException(TypeException type_exception, const std::string msg) 
        : message(get_prefix_message(type_exception) + msg) {}

    virtual const char* what() const noexcept override {
        return message.c_str();
    }
};


class MatrixExceptionIsEmpty : public MatrixException {
public:
    MatrixExceptionIsEmpty() 
        : MatrixException(TypeException::ERROR, "Matrix must not be empty\n") {}
};


class MatrixExceptionSizeMismatch : public MatrixException {
public:
    MatrixExceptionSizeMismatch() 
        : MatrixException(TypeException::ERROR, "Matrix are must be have equal sizes\n") {}
};


class MatrixExceptionMultiSizeMismatch : public MatrixException {
public:
    MatrixExceptionMultiSizeMismatch() 
        : MatrixException(TypeException::ERROR, "Matrix are have incorrect sizes for multiplication\n") {}
};


class MatrixExceptionNotSquare : public MatrixException {
public:
    MatrixExceptionNotSquare() 
        : MatrixException(TypeException::ERROR, "Matrix must be square\n") {}
};


class MatrixExceptionDeterminantZero : public MatrixException {
public:
    MatrixExceptionDeterminantZero() 
        : MatrixException(TypeException::ERROR, "Determinant of reverse matrix must not be zero\n") {}
};


class MatrixExceptionOutOfRange : public MatrixException {
public:
    MatrixExceptionOutOfRange() 
        : MatrixException(TypeException::ERROR, "Element index is outside the matrix bounds\n") {}
};


class MatrixExceptionOverflow : public MatrixException {

public:
    MatrixExceptionOverflow() 
        : MatrixException(TypeException::ERROR, "Overflow, sizes of matrix is big\n") {}
};


class Matrix {
private:
    size_t rows, cols;
    std::unique_ptr<double []> data = nullptr;
public:
    Matrix();
    Matrix(size_t rows, size_t cols);
    Matrix(double *new_data, size_t rows, size_t cols);
    Matrix(const Matrix&);
    Matrix(Matrix&&);
    ~Matrix() {};

    size_t get_rows() const noexcept;
    size_t get_cols() const noexcept;
    bool is_empty() const noexcept;
    bool is_unit(double error = pow(10, -9)) const noexcept;
    bool is_zeros(double error = pow(10, -9)) const noexcept;
    Matrix to_unit();
    Matrix to_zeros() noexcept;
    double& at(size_t row, size_t col);
    double determinant() const;
    Matrix reverse();
    Matrix transpoze();
    Matrix exp(const unsigned int iteration_count);
    void print(unsigned char accuracy = 3);


    Matrix& operator=(const Matrix&);
    Matrix& operator=(Matrix&&);
    Matrix& operator+() noexcept;
    Matrix& operator-() noexcept;
    friend Matrix operator+(const Matrix&, const Matrix&);
    friend Matrix& operator+=(Matrix&, const Matrix&);
    friend Matrix operator-(const Matrix&, const Matrix&);
    friend Matrix& operator-=(Matrix&, const Matrix&);
    friend Matrix operator*(const Matrix&, const double number);
    friend Matrix& operator*=(Matrix& A, const double number);
    friend Matrix operator*(const double number, const Matrix&);
    friend Matrix& operator*=(const double number, Matrix&A);
    friend Matrix operator*(const Matrix&, const Matrix&);
    friend Matrix& operator*=(Matrix&, const Matrix&);
    friend Matrix operator^(const Matrix&, const unsigned int degree);
    friend bool operator==(const Matrix&, const Matrix&);
    
private:
    void swap_rows(const size_t first_row, const size_t second_row);
    
    // Auxilary functions for determinant()
    void gauss_zeroing_elements_in_column_below_diagonal(const size_t _current_col);
    size_t gauss_find_max_element_in_column_below_diagonal(size_t col_current_element);
    double triangular_determinant();
    
    // Auxilary functions for reverse()
    void fill_extend_matrix(Matrix& extend_matrix);
    void extract_inverse_matrix(const Matrix& extend_matrix);
    void gauss_zeroing_elements_in_column_above_diagonal(const size_t current_col);
    void gauss_reduce_diagonal_element_to_one(const size_t current_row);
    int transform_extend_matrix();

    // Auxilary function for exp()
    double find_max_element();
};


}
