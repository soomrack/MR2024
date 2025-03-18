#include <iostream>

#include <stdio.h>
#include <string.h>
#include <math.h>

namespace err{
    enum MatrixExceptionLevel {ERROR, WARNING, INFO, DEBUG};
}

class Matrix
{
    public:
        Matrix(size_t cols, size_t rows);
        Matrix(const Matrix &);
        ~Matrix();

        void exception(const enum err::MatrixExceptionLevel level, char *msg);
        void random();
        void print();
        double deter();
        void Free();

        Matrix transpose();
        Matrix idenity(size_t size);
        Matrix invert();
        Matrix exp(int accuracy);

        Matrix operator + (const Matrix& B);
        Matrix operator - (const Matrix&B);
        void operator = (const Matrix& B);
        Matrix operator * (const Matrix& B);
        Matrix operator * (double count);

    private:
        size_t cols_;
        size_t rows_;
        double* data_;
};

const Matrix MATRIX_NULL(0, 0);

Matrix :: Matrix( size_t cols, size_t rows )
{
    cols_ = cols;
    rows_ = rows;
    data_ = new double[cols_ * rows_];
}

Matrix :: Matrix( const Matrix & B )
{
    cols_ = B.cols_;
    rows_ = B.rows_;
    data_ = new double[cols_ * rows_];

    for(size_t i = 0; i < B.rows_ * B.cols_; ++i){
        data_[i] = B.data_[i];
    }
}

Matrix :: ~Matrix()
{
    delete [] data_;
    data_ = nullptr;
}

void Matrix :: exception( const enum err::MatrixExceptionLevel level, char *msg )
{
    if (level == err::ERROR) {
        printf("ERROR: %s", msg);
    }

    if (level == err::WARNING) {
        printf("WARNING: %s", msg);
    }

    if (level == err::INFO) {
        printf("INFO: %s", msg);
    }
}

void Matrix :: random()
{
    for (size_t i = 0; i < cols_ * rows_; i++){
        data_[i] = i +1;
        // data_[i] = (double) rand();
    }
}

void Matrix :: print()
{
    for (size_t i = 0; i < rows_; i++){
        for(size_t j = 0; j < cols_; j++){
            printf("%.2f ", data_[i * cols_ + j]);
        }
        printf("\n");
    }
    printf("\n");
}

double Matrix :: deter()
{
    if (cols_ != rows_){
        exception(err::ERROR, (char*)"не соответствующие размеры матрицы.\n");
        return 0;
    }

    double result = 0;
    size_t n = cols_;

    if (cols_ == 1){
        result = data_[0];
        return result;
    }

    for ( size_t row = 0; row < cols_; ++row ){

        size_t col = 0;
        Matrix submatrix(cols_ - 1, rows_ - 1);
        size_t row_offset = 0;
        size_t col_offset = 0;

        for ( size_t sub_row = 0; sub_row < rows_ - 1; ++sub_row ){
            for ( size_t sub_col = 0; sub_col < cols_ - 1; ++ sub_col ){
                if (sub_row == row) { row_offset = 1; }

                if (sub_col == col) { col_offset = 1; }

                submatrix.data_[sub_row * (n - 1) + sub_col] =
                    data_[ (sub_row + row_offset) * n + (sub_col + col_offset) ];
            } 
        }
        result += pow(-1, row + col) * data_[row * n + col] * submatrix.deter();
    }
    return result;
}

void Matrix :: Free()
{
    delete [] data_;
    data_ = nullptr;
    cols_ = 0;
    rows_ = 0;
}

Matrix Matrix :: operator +( const Matrix &B )  /// A+B   A.operator+(B)
{
    Matrix R(*this);

    if ( rows_!=B.rows_ || cols_ != B.cols_ ) {
        exception(err::ERROR, (char*)"не соответствующие размеры матрицы.");
        return MATRIX_NULL;
    }

    for ( size_t i = 0; i < cols_ * rows_; i++ ){
        R.data_[i] = data_[i] + B.data_[i];
    }
    return R;
}

void Matrix:: operator = ( const Matrix & A )
{
    // if (rows_!=A.rows_ || cols_ != A.cols_) {
    //     exception(err::ERROR, (char*)"не соответствующие размеры матрицы.\n");
    //     return MATRIX_NULL;
    // }

    Free();
    cols_ = A.cols_;
    rows_ = A.rows_;
    data_ = new double[cols_ * rows_];

    for (size_t i = 0; i < A.cols_ * A.rows_; i++){
        data_[i] = A.data_[i];
    }

}

Matrix Matrix :: operator -( const Matrix &B )
{
    Matrix R(*this);

    if ( rows_!=B.rows_ || cols_ != B.cols_ ) {
        exception(err::ERROR, (char*)"не соответствующие размеры матрицы.");
        return MATRIX_NULL;
    }

    for ( size_t i = 0; i < cols_ * rows_; i++ ){
        R.data_[i] = data_[i] - B.data_[i];
    }
    return R;
}

Matrix Matrix::operator*( double count ) 
{
    Matrix result_matrix(*this);
    
    for ( size_t i = 0; i < cols_ * rows_; i++ ){
        result_matrix.data_[i] = data_[i] * count;
    }

    return result_matrix;
}

 Matrix Matrix::operator*( const Matrix &B )
{
    if (cols_!=rows_) {
        exception(err::ERROR, (char*)"не соответствующие размеры матрицы.");
        return MATRIX_NULL;
    }

    size_t n_cols = B.cols_;
    size_t n_rows = rows_;
    Matrix result_matrix(n_cols, n_rows);

    double summa = 0.0;
    for ( size_t row = 0; row < rows_; row++ ){
        for ( size_t col = 0; col < B.cols_; col++ ){
            summa = 0.0;
            for ( size_t k = 0; k < cols_; k++ ){
                summa += data_[row * cols_ + k] * B.data_[k * B.cols_ + col];
            }
            result_matrix.data_[row * B.cols_ + col] = summa;
        }
    }

    return result_matrix;
}

Matrix Matrix::transpose()
{
    Matrix result_matrix( rows_, cols_ );

    for ( size_t i = 0; i < result_matrix.rows_; ++i ){
        for (size_t j = 0; j < result_matrix.cols_; ++j){
            result_matrix.data_[i * result_matrix.cols_ + j] = 
                data_[j * result_matrix.rows_ + i];
        }
    }

    return result_matrix;
}

Matrix Matrix::idenity(size_t size)
{
    Matrix result_matrix(size, size);

    for (size_t i = 0; i < size; ++i){
        for (size_t j = 0; j < size; ++j){
            result_matrix.data_[i * result_matrix.cols_ + j] = 
            (i == j) ? 1. : 0.;
        }
    }

    return result_matrix;
}

Matrix Matrix::invert()
{
    if (cols_ != rows_) {
        exception(err::ERROR, (char*)"не соответствующие размеры матрицы.");
        return MATRIX_NULL;
    }

    Matrix transponent = transpose();

    Matrix result (*this);

    size_t n = transponent.rows_;

    for (size_t i = 0; i < n; ++i){
        for (size_t j = 0; j < n; ++j){

            Matrix submatrix (n - 1, n - 1);

            size_t row_offset = 0;
            size_t col_offset = 0;

            for (size_t sub_row = 0; sub_row < n - 1; ++sub_row){
                if (sub_row == i) {row_offset = 1;}
                
                for (size_t sub_col = 0; sub_col < n - 1; ++sub_col){
                    if (sub_col == j) {col_offset = 1;}

                    submatrix.data_[sub_row * (n - 1) + sub_col] = 
                        transponent.data_[(sub_row + row_offset) * n + (sub_col + col_offset)];
                }
            }

            result.data_[i * cols_ + j] = pow(-1, i + j) * submatrix.deter();
            submatrix.Free();
        }
    }

    transponent.Free();
    Matrix multiplied_result = result * (1 / result.deter());

    result.Free();
    return multiplied_result;
}

Matrix Matrix::exp(int accuracy)
{
    if (cols_ != rows_ || cols_ == 0) {
        exception(err::ERROR, (char*)"не соответствующие размеры матрицы.");
        return MATRIX_NULL;
    }

    Matrix new_result(MATRIX_NULL), new_powered(MATRIX_NULL), 
    multiplied(*this), powered(*this);

    Matrix result (*this);
    result.idenity(cols_);

    int factorial = 1;

    for (int acc = 1; acc <= accuracy; ++acc){
        
        factorial *= acc;

        new_powered.Free();

        multiplied.Free();
        multiplied = powered * (1 / factorial);

        new_result.Free();
        new_result = result + multiplied;

        result.Free();
        result = new_result;

        new_powered = powered * *this;

        powered.Free();
        powered = new_powered;
    }

    powered.Free();
    return result;
}

int main()
{
    Matrix A(2, 2);
    Matrix B(2, 2);

    A.random();
    B.random();
    A.print();

    double determinant = A.deter();
    printf("%.2f\n\n", determinant);

    Matrix sum(2,2);
    sum = A + B;
    sum.print();
    sum.Free();

    Matrix sub(2, 2);
    sub = A - B;
    sub.print(); 
    sub.Free();

    Matrix mult_numb(2, 2);
    mult_numb = A * 2;
    mult_numb.print(); 
    mult_numb.Free();

    Matrix mult (2,2);
    mult = A * B;
    mult.print();
    mult.Free();

    Matrix trans (A);
    trans = A.transpose();
    trans.print();
    trans.Free();

    Matrix idenity (A);
    idenity = idenity.idenity(2);
    idenity.print();
    idenity.Free();

    Matrix invertible (A);
    invertible = invertible.invert();
    invertible.print();
    invertible.Free();

    Matrix exponenta (A);
    exponenta = A.exp(3);
    exponenta.print();
    exponenta.Free();

    return 0;
}