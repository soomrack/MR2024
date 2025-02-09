#include "matrix.hpp"
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <ctime>
#include <memory>
#include <stdexcept>
#include <cmath>
#include <iostream>
#include <string>
#include <utility>


//=========================   NAMESPACE MTL   ==============================   
namespace MTL {


//=========================   LOG EXCEPTIONS   =============================   
enum class Log_levels {
    ERRORS,
    WARNING,
    INFO
};


static Log_levels current_log_level = Log_levels::ERRORS;


void set_log_level(unsigned int new_level) {
    switch (new_level) {
        case 0:
            current_log_level = Log_levels::ERRORS;
            break;
        case 1:
            current_log_level = Log_levels::WARNING;
            break;
        default:
            current_log_level = Log_levels::INFO;
            break;
    }
}


//=========================   CONSTRUCTORS   ===============================   
Matrix::Matrix() : rows(0), cols(0), data(nullptr) {}


Matrix::Matrix(size_t rows_, size_t cols_)
    : rows(rows_), cols(cols_), data(std::make_unique<double[]>(rows_ * cols_)) {}


Matrix::Matrix(double *new_data, size_t rows_, size_t cols_)
    : rows(rows_), cols(cols_) {
    
    data = std::make_unique<double[]>(rows_ * cols_);
    
    for(size_t idx = 0; idx < rows_ * cols_; idx++ ) {
        data[idx] = new_data[idx];
    }
}


Matrix::Matrix(double number)
    : rows(1), cols(1), data(std::make_unique<double[]>(1)) { data[0] = number; } 


//=========================   COPY CONSTRUCTOR   ===========================
Matrix::Matrix(const Matrix& other) {
    this->rows = other.rows;
    this->cols = other.cols;
     
    data = std::make_unique<double[]>(rows * cols);
    
    std::copy(other.data.get(), other.data.get() + rows * cols, data.get());
}


//=========================   MOVE CONSTRUCTOR   ===========================   
Matrix::Matrix(Matrix&& other)
    : rows(other.rows), cols(other.cols), data(std::move(other.data)) {

    other.rows = 0;
    other.cols = 0;
    other.data = nullptr;
}


//=========================   DESTRUCTOR   =================================   
Matrix::~Matrix() {
    //std::cout << "~Matrix\n" << std::endl;
}


//=========================   METHODS   ====================================   
size_t Matrix::get_rows() const noexcept {
    return rows;
}


size_t Matrix::get_cols() const noexcept {
    return cols;
}


bool Matrix::is_empty() const noexcept {
    return data.get() == nullptr;
}


bool Matrix::is_unit() const noexcept {
    if(data.get() == nullptr) return false;
    if(rows != cols) return false;

    size_t diag_dimention = 0;
    double epsilon = pow(10, -9);

    for(size_t idx = 0; idx < rows * cols; idx++) {
        if(idx == diag_dimention * rows + diag_dimention) {
            diag_dimention++;
            if(std::fabs(data[idx]) - 1.0 > epsilon) return false; 
        } else {
            if(std::fabs(data[idx]) > epsilon) return false;
        }
    }

    return true;
}


bool Matrix::is_zeros() const noexcept {
    if(data.get() == nullptr) return false;
    
    double error = pow(10, -9);

    for(size_t idx = 0; idx < rows * cols; idx++) {
        if(std::fabs(data[idx]) > error) return false;
    }
    
    return true;
}


Matrix Matrix::to_unit() {
    if(rows != cols) { 
        throw std::runtime_error("\033[0;31mERROR:\033[0m Matrix are must be have equal sizes\n");
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


//=========================   UNARY OPERATORS   ============================   
Matrix& Matrix::operator+() noexcept {
    return *this;
}


Matrix& Matrix::operator++() noexcept {
    for(size_t idx = 0; idx < rows * cols; idx++) {
        data[idx]++;
    }

    return *this;
}


Matrix& Matrix::operator-() noexcept {
    for(size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] = -data[idx];
    }

    return *this;
}


Matrix& Matrix::operator--() noexcept {
    for(size_t idx = 0; idx < rows * cols; idx++) {
        data[idx]--;
    }

    return *this;
}


double& Matrix::operator()(size_t row, size_t col) {
    if(row > rows || col > cols) throw std::runtime_error("\033[0;31merror: \033[0mno access to this element\n");
    
    return data[row * rows + col];
}


double& Matrix::operator()(size_t idx) {
    if(idx > (rows * cols)) throw std::runtime_error("\033[0;31merror: \033[0mno access to this element\n");
    
    return data[idx];
}

//=========================   MOVE & COPY OPERATORS   ======================   
Matrix& Matrix::operator=(const Matrix& other) {
    this->rows = other.rows;
    this->cols = other.cols;
    
    if(data.get() != nullptr) data.release();
    
    data = std::make_unique<double[]>(rows * cols);
    
    std::copy(other.data.get(), other.data.get() + rows * cols, data.get());
    
    return *this;
}


Matrix& Matrix::operator=(Matrix&& other) { 
    this->rows = other.rows;
    this->cols = other.cols;
    data = std::move(other.data);

    other.rows = 0;
    other.cols = 0;
    other.data = nullptr;

    return *this;
}


//=========================   BINARY OPERATORS   ===========================   
Matrix operator+(const Matrix& A, const Matrix& B) {
    A.check_condition(Matrix::Operations::ADD, B);
    
    Matrix result(A.rows, A.cols);

    for(size_t idx = 0; idx < A.rows * A.cols; idx++) {
        result.data[idx] = A.data[idx] + B.data[idx]; 
    }

    return result;
}


Matrix& operator+=(Matrix& A, const Matrix& B) {
    A.check_condition(Matrix::Operations::ADD, B);
    
    for(size_t idx = 0; idx < A.rows * A.cols; idx++) {
        A.data[idx] = A.data[idx] + B.data[idx]; 
    }

    return A;
}


Matrix operator-(const Matrix& A, const Matrix& B) {
    A.check_condition(Matrix::Operations::SUB, B);
    
    Matrix result(A.rows, A.cols);

    for(size_t idx = 0; idx < A.rows * A.cols; idx++) {
        result.data[idx] = A.data[idx] - B.data[idx]; 
    }

    return result;
}


Matrix& operator-=(Matrix& A, const Matrix& B) {
    A.check_condition(Matrix::Operations::SUB, B);
    
    for(size_t idx = 0; idx < A.rows * A.cols; idx++) {
        A.data[idx] = A.data[idx] - B.data[idx]; 
    }

    return A;
}


Matrix operator*(const Matrix& A, const double number) {
    A.check_condition(Matrix::Operations::MULTIPLY_BY_NUMBER);
    
    Matrix result(A.rows, A.cols);

    for(size_t idx = 0; idx < A.rows * A.cols; idx++) {
        result.data[idx] = A.data[idx] * number; 
    }

    return result;
}


Matrix operator*(const double number, const Matrix&A) {
    A.check_condition(Matrix::Operations::MULTIPLY_BY_NUMBER);
    
    Matrix result(A.rows, A.cols);

    for(size_t idx = 0; idx < A.rows * A.cols; idx++) {
        result.data[idx] = A.data[idx] * number; 
    }

    return result;
}


Matrix& operator*=(Matrix& A, const double number) {
    A.check_condition(Matrix::Operations::MULTIPLY_BY_NUMBER);
    
    for(size_t idx = 0; idx < A.rows * A.cols; idx++) {
        A.data[idx] = A.data[idx] * number; 
    }

    return A;
}


Matrix& operator*=(const double number, Matrix&A) {
    A.check_condition(Matrix::Operations::MULTIPLY_BY_NUMBER);
    
    for(size_t idx = 0; idx < A.rows * A.cols; idx++) {
        A.data[idx] = A.data[idx] * number; 
    }

    return A;
}


Matrix operator*(const Matrix& A, const Matrix& B) {
    A.check_condition(Matrix::Operations::MULTIPLY, B);
    
    Matrix tmp(A.rows, A.cols);

    for(size_t rowA = 0; rowA < A.rows; ++rowA) {
        for(size_t colB = 0; colB < B.cols; ++colB) {
            for(size_t innerDim = 0; innerDim < A.cols; ++innerDim) {
                tmp.data[rowA * tmp.cols + colB] += A.data[rowA * A.cols + innerDim] * B.data[innerDim * B.cols + colB];
            }
        }
    }
    
    return tmp;
}


Matrix& operator*=(Matrix& A, const Matrix& B) {
    A.check_condition(Matrix::Operations::MULTIPLY, B);

    Matrix tmp(A.rows, A.cols);
    
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

    A = tmp;
    
    return A;
}


Matrix operator^(const Matrix& A, const unsigned int degree) {
    if(degree == 0) return Matrix(A.rows, A.cols).to_unit();

    Matrix result(A);

    if(degree == 1) return result;
    if(A.is_unit()) return result;
    if(A.is_zeros()) return result;

    for(unsigned int k = 1; k < degree; k++) result *= A;

    return result;
}


bool operator==(const Matrix& A, const Matrix& B) {
    if(A.rows != B.rows || A.cols != A.rows) return false;
    
    for(size_t idx = 0; idx < A.rows * A.cols; idx++) {
        if(A.data[idx] != B.data[idx]) return false;
    }

    return true;
}


//=========================   MATRIX TRANSPOZE   ===========================   
Matrix Matrix::transpoze() {
    this->check_condition(Matrix::Operations::TRANSPOSE);
    
    Matrix result(*this);

    for(size_t row = 0; row < this->rows; ++row) {
        for(size_t col = 0; col < this->cols; ++col) {
            result.data[col * result.cols + row] = this->data[row * this->cols + col];
        }
    }
    
    *this = result;

    return result;
}


//=========================   MATRIX DETERMINANT   =========================   
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
        if (fabs(data[row * rows + col_current_element]) > fabs(data[row_max_element * rows + col_current_element])) {
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
    this->check_condition(Matrix::Operations::DETERMINANT);

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


//=========================   MATRIX REVERSE   =============================   
void Matrix::fill_extend_matrix(Matrix& extend_matrix) {
    for(size_t row = 0; row < extend_matrix.rows; ++row) { 
        for(size_t col = 0; col < this->cols; ++col) {
            extend_matrix.data[row * extend_matrix.cols + col] = this->data[row * this->cols + col];
        }

        for(size_t col = this->cols; col < extend_matrix.cols; ++col) {
            extend_matrix.data[row * extend_matrix.cols + col] = (row == col - this->cols) ? 1.0 : 0.0;
        }
    }
}


void Matrix::extract_inverse_matrix(const Matrix& extend_matrix) {
    for(size_t row = 0; row < this->rows; row++) {
        for (size_t col = 0; col < this->cols; col++) {
            this->data[row * this->cols + col] = extend_matrix.data[row * extend_matrix.cols + this->cols + col];
        }
    }
}


void Matrix::gauss_zeroing_elements_in_column_above_diagonal(const size_t current_col) {
    double factor = 0.0;

    // Current_col == current_row, because Matrix is square
    for(size_t row = 0; row < current_col; row++) {
        factor = this->data[row * this->cols + current_col] / this->data[current_col * this->cols + current_col];
        
        for(size_t col = current_col; col < this->cols; col++) {
            this->data[row * this->cols + col] -= factor * this->data[current_col * this->cols + col];
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
    this->check_condition(Matrix::Operations::REVERSE);

    Matrix extend_matrix(this->rows, this->cols * 2);

    this->fill_extend_matrix(extend_matrix);
    
    if(extend_matrix.transform_extend_matrix() == 0) {
        throw std::runtime_error("\033[0;31mERROR:\033[0;31m Determinant of reverse matrix must not be zero\n");
    }

    this->extract_inverse_matrix(extend_matrix);

    return *this;
}


//=========================   MATRIX EXPONENT   ============================   
double Matrix::find_max_element() {
    double max_element = 0;

    for(size_t idx = 1; idx < this->rows * this->cols; idx++) {
        max_element = (this->data[idx] > max_element) ? this->data[idx] : max_element;
    }
    
    return max_element;
}


Matrix Matrix::exp(const unsigned short accuracy) {
    this->check_condition(Matrix::Operations::EXP);
    
    Matrix exp_matrix = *this;
    exp_matrix += Matrix(this->rows, this->cols).to_unit();

    Matrix tmp = *this;

    double number = 1.0;
    double error = pow(10, -accuracy);

    for(unsigned int k = 2; ; ++k) {
        number *= 1.0 / k;
        tmp *= (*this);
        tmp *= number;
        if(fabs(tmp.find_max_element()) < error) break;
        exp_matrix += tmp;
        tmp *= 1.0 / number;
    }

    *this = exp_matrix;

    return exp_matrix;
}


//=========================   MATRIX PRINT   ===============================   
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


//=========================   CHECK CONDITION   ============================   
void Matrix::check_condition(Operations operation, const Matrix& A) const {
    std::string ERROR = "\033[0;31mERROR:\033[0m ";
    std::string WARNING = "\033[1;33mWARNING:\033[0m ";
    
    if(this->is_empty() || A.is_empty()) {
        throw std::runtime_error(ERROR + "Matrix must not be empty\n");
    }
    
    using Operations = Matrix::Operations;
    switch (operation) {
        case Operations::ADD:
            if(this->rows != A.rows || this->cols != A.cols) {
                throw std::runtime_error(ERROR + "Matrix are must be have equal sizes\n");
            }
            return;
        
        case Operations::SUB:
            if(this->rows != A.rows || this->cols != A.cols) {
                throw std::runtime_error(ERROR + "Matrix are must be have equal sizes\n");
            }
            return;
        
        case Operations::MULTIPLY_BY_NUMBER:
            return;
        
        case Operations::MULTIPLY:
            if(this->cols != A.rows) {
                throw std::runtime_error(ERROR + "Matrix are have incorrect sizes for multiplycation\n");
            }
            return;
        
        case Operations::TRANSPOSE:
            return;
        
        case Operations::DETERMINANT:
            if(this->rows != this->cols) {
                throw std::runtime_error(ERROR + "Matrix must be square\n");
            }
            return;
        
        case Operations::REVERSE:
            if(this->rows != this->cols) {
                throw std::runtime_error(ERROR + "Matrix must be square\n");
            }
            return;
        
        case Operations::EXP:
            if(this->rows != this->cols) {
                throw std::runtime_error(ERROR + "Matrix must be square\n");
            }
            return;
        
        default:
            return;
    }
}


};
