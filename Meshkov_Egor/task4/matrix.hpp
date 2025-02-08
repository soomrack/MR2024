#pragma once
#include <cstddef>
#include <memory>


namespace MTL { // Matrix Lib


void set_log_level(unsigned int new_level);


class Matrix {
public:
    Matrix();
    Matrix(size_t rows, size_t cols);
    Matrix(double *new_data, size_t rows, size_t cols);
    Matrix(double number);

    Matrix(const Matrix&);
    Matrix(Matrix&&);

    ~Matrix();
    
    void resize(size_t new_rows, size_t new_cols);
    size_t get_rows() const noexcept;
    size_t get_cols() const noexcept;

    bool is_empty() const noexcept;
    bool is_unit() const noexcept;
    bool is_zeros() const noexcept;

    Matrix& operator+() noexcept;
    Matrix& operator++() noexcept;
    Matrix& operator-() noexcept;
    Matrix& operator--() noexcept;
    
    double& operator()(size_t row, size_t col);
    double& operator()(size_t idx);

    Matrix& operator=(const Matrix&);
    Matrix& operator=(Matrix&&);
    
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
    
    double determinant() const;
    void reverse();
    void transpoze();
    void exp(const unsigned short accuracy);
    
    friend double determinant(const Matrix&);
    friend Matrix reverse(const Matrix&);
    friend Matrix transpoze(const Matrix&);
    friend Matrix exp(const Matrix&, const unsigned short accuracy);

    void print(unsigned char accuracy = 3);

protected:
    size_t rows, cols;
    std::unique_ptr<double []> data = nullptr;

    enum class Operations {
        ADD,
        SUB,
        MULTIPLY_BY_NUMBER,
        MULTIPLY,
        DETERMINANT,
        REVERSE,
        TRANSPOSE,
        EXP
    };

    void check_condition(Operations operation, const Matrix& A = Matrix(1.0)) const;

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


class Matrix_unit : public Matrix {
public:
    Matrix_unit(size_t rows, size_t cols);
    
    void resize(size_t new_rows, size_t new_cols) = delete;

    bool is_empty() const noexcept { return false; }
    bool is_unit() const noexcept { return true; }
    bool is_zeros() const noexcept { return false; }

    Matrix_unit& operator++() noexcept = delete;
    Matrix_unit& operator--() noexcept = delete;

    Matrix_unit& operator=(const Matrix_unit&) = delete;
    Matrix_unit& operator=(Matrix_unit&&) = delete;
    
    friend Matrix& operator+=(Matrix_unit&, const Matrix&) = delete;
    friend Matrix& operator-=(Matrix_unit&, const Matrix&) = delete;
    friend Matrix& operator*=(Matrix_unit& A, const double number) = delete;
    friend Matrix& operator*=(const double number, Matrix_unit&A) = delete;
    friend Matrix& operator*=(Matrix_unit&, const Matrix&) = delete;
    
    double determinant() const { return 1.0; }
    void reverse() {}
    void transpoze() {}
    
    friend double determinant(const Matrix_unit&) { return 1.0; }
    friend Matrix_unit reverse(const Matrix_unit& A) { return Matrix_unit(A.rows, A.cols); }
    friend Matrix_unit transpoze(const Matrix_unit& A) { return Matrix_unit(A.rows, A.cols); }

private:
    double find_max_element() { return 1.0;} ;
};


}
