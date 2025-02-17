#include "matrix.hpp"
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <ctime>
#include <memory>
#include <cmath>
#include <iostream>
#include <utility>


namespace MTL {


const std::string MatrixException::get_prefix_message(TypeException type_exception) const noexcept {
    switch (type_exception) {
        case TypeException::ERROR:
            return "\033[0;31mERROR:\033[0m ";
            break;
        case TypeException::WARNING:
            return "\033[0;33mWARNING:\033[0m ";
            break;
        default:  
            return "INFO: ";
            break;
    }
}


Matrix::Matrix() : rows(0), cols(0), data(nullptr) {
    std::cout << "constructor default\n" << std::endl;
}


Matrix::Matrix(size_t rows, size_t cols)
    : rows(rows), cols(cols) {
    std::cout << "constructor with rows, cols\n" << std::endl;
    
    if(__SIZE_MAX__ / rows / cols / sizeof(double) == 0) {
        throw MatrixExceptionOverflow();
    }

    if(rows != 0 && cols != 0) {
        data = std::make_unique<double[]>(rows * cols);
        std::fill(data.get(), data.get() + rows * cols, 0.0);
    }
}


Matrix::Matrix(double *new_data, size_t rows, size_t cols)
    : rows(rows), cols(cols) {
    std::cout << "constructor from double*\n" << std::endl;
    
    if(__SIZE_MAX__ / rows / cols / sizeof(double) == 0) {
        throw MatrixExceptionOverflow();
    }
    
    if(rows != 0 && cols != 0) {
        data = std::make_unique<double[]>(rows * cols);
        std::memcpy(data.get(), new_data, rows * cols * sizeof(double)) ;
    }
}


Matrix::Matrix(const Matrix& other) {
    std::cout << "constructor copy\n" << std::endl;
    
    rows = other.rows;
    cols = other.cols;

    if(rows != 0 && cols != 0) {
        data = std::make_unique<double[]>(rows * cols);
        std::copy(other.data.get(), other.data.get() + rows * cols, data.get());
    }
}


Matrix::Matrix(Matrix&& other)
    : rows(other.rows), cols(other.cols), data(std::move(other.data)) {
    std::cout << "constructor move\n" << std::endl;
    
    other.rows = 0;
    other.cols = 0;
    other.data = nullptr;
}


Matrix::~Matrix() {
    std::cout << "~Matrix\n" << std::endl;
}


size_t Matrix::get_rows() const noexcept {
    return rows;
}


size_t Matrix::get_cols() const noexcept {
    return cols;
}


bool Matrix::is_empty() const noexcept {
    return data.get() == nullptr;
}


bool Matrix::is_unit(double error) const noexcept {
    if(data.get() == nullptr) return false;
    if(rows != cols) return false;

    size_t diag_dimention = 0;

    for(size_t idx = 0; idx < rows * cols; idx++) {
        if(idx == diag_dimention * rows + diag_dimention) {
            diag_dimention++;
            if(std::fabs(data[idx] - 1.0) > error) return false; 
        } else {
            if(std::fabs(data[idx]) > error) return false;
        }
    }

    return true;
}


bool Matrix::is_zeros(double error) const noexcept {
    if(data.get() == nullptr) return false;
    
    for(size_t idx = 0; idx < rows * cols; idx++) {
        if(std::fabs(data[idx]) > error) return false;
    }
    
    return true;
}


Matrix Matrix::to_unit() {
    if(rows != cols) { 
        throw MatrixExceptionNotSquare();
    }

    std::fill(data.get(), data.get() + rows * cols, 0.0);
    
    for(size_t idx = 0; idx < rows * cols; idx+=(rows + 1)) {
        data[idx] = 1.0;
    }

    return *this;
}


Matrix Matrix::to_zeros() noexcept {
    std::fill(data.get(), data.get() + rows * cols, 0.0);

    return *this;
}


Matrix& Matrix::operator+() noexcept {
    return *this;
}


Matrix& Matrix::operator-() noexcept {
    for(size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] = -data[idx];
    }

    return *this;
}


double& Matrix::at(size_t row, size_t col) {
    if(row * cols + col >= rows * cols) {
        throw MatrixExceptionOutOfRange(); 
    }
    return data[row * cols + col];  
}


Matrix& Matrix::operator=(const Matrix& other) {
    std::cout << "operator copy\n" << std::endl;
    
    if(*this == other) return *this;
    
    if((rows == other.rows && cols == other.cols) || (rows * cols == other.rows * other.cols)) {
        std::copy(other.data.get(), other.data.get() + rows * cols, data.get());
    } else {
        rows = other.rows;
        cols = other.cols;
        if(data.get() != nullptr) data.release();
        data = std::make_unique<double[]>(rows * cols);
        std::copy(other.data.get(), other.data.get() + rows * cols, data.get());
    }
    
    return *this;
}


Matrix& Matrix::operator=(Matrix&& other) { 
    std::cout << "operator move\n" << std::endl;
    
    if(*this == other) return *this;
    
    rows = other.rows;
    cols = other.cols;
    data = std::move(other.data);
    
    other.rows = 0;
    other.cols = 0;
    other.data = nullptr;
    
    return *this;
}


Matrix operator+(const Matrix& A, const Matrix& B) {
    if(A.is_empty() || B.is_empty()) {
        throw MatrixExceptionIsEmpty();
    }
    
    if(A.rows != B.rows || A.cols != B.cols) {
        throw MatrixExceptionSizeMismatch();
    }
    
    Matrix result(A.rows, A.cols);

    for(size_t idx = 0; idx < result.rows * result.cols; idx++) {
        result.data[idx] = A.data[idx] + B.data[idx]; 
    }

    return result;
}


Matrix& operator+=(Matrix& A, const Matrix& B) {
    A = A + B;

    return A;
}


Matrix operator-(const Matrix& A, const Matrix& B) {
    if(A.is_empty() || B.is_empty()) {
        throw MatrixExceptionIsEmpty();
    }
    
    if(A.rows != B.rows || A.cols != B.cols) {
        throw MatrixExceptionSizeMismatch();
    }
    
    Matrix result(A.rows, A.cols);

    for(size_t idx = 0; idx < result.rows * result.cols; idx++) {
        result.data[idx] = A.data[idx] - B.data[idx]; 
    }

    return result;
}


Matrix& operator-=(Matrix& A, const Matrix& B) {
    A = A - B;

    return A;
}


Matrix operator*(const Matrix& A, const double number) {
    if(A.is_empty()) {
        throw MatrixExceptionIsEmpty();
    }
    
    Matrix result(A.rows, A.cols);

    for(size_t idx = 0; idx < A.rows * A.cols; idx++) {
        result.data[idx] = A.data[idx] * number; 
    }

    return result;
}


Matrix operator*(const double number, const Matrix&A) {
    return A * number;
}


Matrix& operator*=(Matrix& A, const double number) {
    A = A * number;

    return A;
}


Matrix& operator*=(const double number, Matrix&A) {
    A = A * number;

    return A;
}


Matrix operator*(const Matrix& A, const Matrix& B) {
    if(A.is_empty() || B.is_empty()) {
        throw MatrixExceptionIsEmpty();
    }
    
    if(A.rows != B.rows || A.cols != B.cols) {
        throw MatrixExceptionMultiSizeMismatch();
    }
    
    Matrix tmp(A.rows, B.cols);

    double sum = 0.0; // For cache
    for(size_t rowA = 0; rowA < A.rows; ++rowA) {
        for(size_t colB = 0; colB < B.cols; ++colB) {
            sum = 0.0;
            for(size_t innerDim = 0; innerDim < A.cols; ++innerDim) {
                sum += A.data[rowA * A.cols + innerDim] * B.data[innerDim * B.cols + colB];
            }
            tmp.data[rowA * tmp.cols + colB] = sum; 
        }
    }
    
    return tmp;
}


Matrix& operator*=(Matrix& A, const Matrix& B) {
    A = A * B;
    
    return A;
}


Matrix operator^(const Matrix& A, const unsigned int degree) {
    if(A.is_empty()) {
        throw MatrixExceptionIsEmpty();
    }

    if(A.rows != A.cols) {
        throw MatrixExceptionNotSquare();
    }

    if(degree == 0) return Matrix(A.rows, A.cols).to_unit();

    Matrix result(A);

    if(degree == 1) return result;
    if(A.is_unit()) return result;
    if(A.is_zeros()) return result;

    for(unsigned int k = 1; k < degree; k++) result *= A;

    return result;
}


bool operator==(const Matrix& A, const Matrix& B) {
    if (A.rows != B.rows || A.cols != B.cols) return false;

    double epsilon = 1e-9; // Absolute error
    size_t size = A.rows * A.cols;

    for (size_t i = 0; i < size; ++i) {
        double elemA = A.data[i];
        double elemB = B.data[i];

        // Adaptive error
        double error = epsilon * std::max(std::fabs(elemA), std::fabs(elemB));

        if (std::fabs(elemA - elemB) > error) {
            return false;
        }
    }
    return true;
}


Matrix Matrix::transpoze() {
    if(this->is_empty()) {
        throw MatrixExceptionIsEmpty();
    }

    Matrix result(cols, rows);

    for(size_t row = 0; row < result.rows; ++row) {
        for(size_t col = 0; col < result.cols; ++col) {
            result.data[row * result.cols + col] = data[col * cols + row];
        }
    }
    
    *this = result;

    return result;
}


void Matrix::swap_rows(const size_t first_row, const size_t second_row) {
    if(first_row == second_row) return;

    std::unique_ptr<double[]> buf_row = std::make_unique<double[]>(cols);
    size_t size = sizeof(double) * cols;

    memcpy(buf_row.get(), data.get() + first_row * cols, size);
    memcpy(data.get() + first_row * cols, data.get() + second_row * cols, size);
    memcpy(data.get() + second_row * cols, buf_row.get(), size);
}


void Matrix::gauss_zeroing_elements_in_column_below_diagonal(const size_t current_col) {
    // Current_col == current_row, because matrix is square
    for (size_t row = current_col + 1; row < rows; row++) {
        // The coefficient by which the strings are multiplied
        double factor = data[row * cols + current_col] / data[current_col * cols + current_col];

        for (size_t col = current_col; col < cols; col++) {
            data[row * cols + col] -= factor * data[current_col * cols + col];
        }
    }
}


size_t Matrix::gauss_find_max_element_in_column_below_diagonal(size_t col_current_element) {
    size_t row_max_element = col_current_element;

    // Current_col == current_row, because matrix is square
    for(size_t row = col_current_element; row < rows; row++) {
        if (fabs(data[row * cols + col_current_element]) > fabs(data[row_max_element * rows + col_current_element])) {
            row_max_element = row;
        }
    }

    return row_max_element;
}


double Matrix::triangular_determinant() {
    double det = 1.0;

    for (size_t row = 0; row < rows; row++) {
        det *= data[row * cols + row];
    }

    return det;
}


double Matrix::determinant() const {
    if(this->is_empty()) {
        throw MatrixExceptionIsEmpty();
    }

    if(rows != cols) {
        throw MatrixExceptionNotSquare();
    }

    Matrix tmp(*this);

    size_t row_max_element = 0;
    int count_of_swap = 0;
    double error = pow(10, -9);

    for (size_t current_col = 0; current_col < cols; current_col++) {
        row_max_element = tmp.gauss_find_max_element_in_column_below_diagonal(current_col);
        
        if(fabs(tmp.data[row_max_element * tmp.cols + current_col]) < error) return 0.;

        // Current_col == current_row, because matrix is square
        tmp.swap_rows(current_col, row_max_element);

        if(current_col != row_max_element) count_of_swap++;

        tmp.gauss_zeroing_elements_in_column_below_diagonal(current_col);
    }

    int sign_of_det = (count_of_swap % 2 != 0) ? -1 : 1;

    return sign_of_det * tmp.triangular_determinant();
}


void Matrix::fill_extend_matrix(Matrix& extend_matrix) {
    for(size_t row = 0; row < extend_matrix.rows; ++row) { 
        for(size_t col = 0; col < cols; ++col) {
            extend_matrix.data[row * extend_matrix.cols + col] = data[row * cols + col];
        }

        for(size_t col = cols; col < extend_matrix.cols; ++col) {
            extend_matrix.data[row * extend_matrix.cols + col] = (row == col - cols) ? 1.0 : 0.0;
        }
    }
}


void Matrix::extract_inverse_matrix(const Matrix& extend_matrix) {
    for(size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++) {
            data[row * cols + col] = extend_matrix.data[row * extend_matrix.cols + cols + col];
        }
    }
}


void Matrix::gauss_zeroing_elements_in_column_above_diagonal(const size_t current_col) {
    double factor = 0.0;

    // Current_col == current_row, because Matrix is square
    for(size_t row = 0; row < current_col; row++) {
        factor = data[row * cols + current_col] / data[current_col * cols + current_col];
        
        for(size_t col = current_col; col < cols; col++) {
            data[row * cols + col] -= factor * data[current_col * cols + col];
        }
    }
}


void Matrix::gauss_reduce_diagonal_element_to_one(const size_t current_row) {
   double diagonal_element = data[current_row * cols + current_row];

   for (size_t col = 0; col < cols; col++) {
        data[current_row * cols + col] /= diagonal_element;
   }
}


int Matrix::transform_extend_matrix() {
    size_t row_max_element = 0;
    double error = pow(10, -9);

    for (size_t current_row = 0; current_row < rows; current_row++) {
        row_max_element = this->gauss_find_max_element_in_column_below_diagonal(current_row);

        if(fabs(this->data[row_max_element * cols + current_row]) < error) return 0;

        this->swap_rows(current_row, row_max_element);
        
        this->gauss_reduce_diagonal_element_to_one(current_row);

        this->gauss_zeroing_elements_in_column_below_diagonal(current_row);
        
        this->gauss_zeroing_elements_in_column_above_diagonal(current_row);
    }

    return 1;
}
                                                                
                                                                
Matrix Matrix::reverse() {
    if(this->is_empty()) {
        throw MatrixExceptionIsEmpty();
    }

    if(rows != cols) {
        throw MatrixExceptionNotSquare();
    }

    Matrix extend_matrix(rows, cols * 2);

    this->fill_extend_matrix(extend_matrix);
    
    if(extend_matrix.transform_extend_matrix() == 0) {
        throw MatrixExceptionDeterminantZero();
    }

    this->extract_inverse_matrix(extend_matrix);

    return *this;
}


double Matrix::find_max_element() {
    double max_element = data[0];

    for(size_t idx = 1; idx < rows * cols; idx++) {
        max_element = (data[idx] > max_element) ? data[idx] : max_element;
    }
    
    return max_element;
}


Matrix Matrix::exp(const unsigned int iteration_count) {
    if(this->is_empty()) {
        throw MatrixExceptionIsEmpty();
    } 

    if(rows != cols) {
        throw MatrixExceptionNotSquare();
    }

    Matrix exp_matrix = *this;
    exp_matrix += Matrix(rows, cols).to_unit();

    Matrix tmp = *this;

    double number = 1.0;

    for(unsigned int k = 2; k <= iteration_count; ++k) {
        number *= 1.0 / k;
        tmp *= (*this);
        tmp *= number;
        exp_matrix += tmp;
        tmp *= 1.0 / number;
    }

    return exp_matrix;
}


void Matrix::print(unsigned char accuracy) {
    for(size_t row = 0; row < rows; row++) {
        for(size_t col = 0; col < cols; col++) {
            double element = data[row * cols + col];
            double rounded_element = std::round(element * pow(10, accuracy)) / pow(10, accuracy);
            std::cout << rounded_element << " ";
        }
        std::cout << "\n";
    }

    std::cout << std::endl;
}


};
