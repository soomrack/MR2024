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
        Matrix(Matrix &&);
        ~Matrix();

        // void exception(const enum err::MatrixExceptionLevel level, char *msg);
        void random();
        void print();
        double deter();
        void Free();

        Matrix transpose();
        Matrix idenity(size_t size);
        Matrix invert();
        Matrix exp(int accuracy);
        Matrix degree(unsigned int degree);

        Matrix operator+(const Matrix& B);
        Matrix operator-(const Matrix&B);
        void operator=(const Matrix& B);
        void operator=( Matrix && B);
        Matrix operator*(const Matrix& B);
        Matrix operator*(double count);

    private:
        size_t cols_;
        size_t rows_;
        double* data_;
};

class MatrixExeption : public std::domain_error {
    public:
        explicit MatrixExeption( const std::string& message) : std::domain_error(message) {}
};

const Matrix MATRIX_NULL(0, 0);


Matrix::Matrix( size_t cols, size_t rows )
{
    cols_ = cols;
    rows_ = rows;
    data_ = new double[cols_ * rows_];
}


Matrix::Matrix( const Matrix & B )
{
    cols_ = B.cols_;
    rows_ = B.rows_;
    data_ = new double[cols_ * rows_];

    memcpy(data_, B.data_, sizeof(double) * cols_ * rows_);

    // for(size_t i = 0; i < B.rows_ * B.cols_; ++i){
    //     data_[i] = B.data_[i];
    // }
}


Matrix::Matrix( Matrix && B ){
    cols_ = B.cols_;
    rows_ = B.rows_;
    data_ = B.data_;
    B.data_ = nullptr;
    B.cols_ = 0;
    B.rows_ = 0;
}


Matrix::~Matrix()
{
    delete [] data_;
    data_ = nullptr;
}


// void Matrix::exception( const enum err::MatrixExceptionLevel level, char *msg )
// {
//     if (level == err::ERROR) {
//         printf("ERROR: %s", msg);
//     }

//     if (level == err::WARNING) {
//         printf("WARNING: %s", msg);
//     }

//     if (level == err::INFO) {
//         printf("INFO: %s", msg);
//     }
// }


void Matrix::random()
{
    for (size_t i = 0; i < cols_ * rows_; i++){
        data_[i] = i +1;
        // data_[i] = (double) rand();
    }
}


void Matrix::print()
{
    for (size_t i = 0; i < rows_; i++){
        for(size_t j = 0; j < cols_; j++){
            printf("%.2f ", data_[i * cols_ + j]);
        }
        printf("\n");
    }
    printf("\n");
}


double Matrix::deter()
{
    if (cols_ != rows_){
        MatrixExeption SIZE_ERROR("Matrites of different sizes!");
        throw SIZE_ERROR;
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


void Matrix::Free()
{
    delete [] data_;
    data_ = nullptr;
    cols_ = 0;
    rows_ = 0;
}


Matrix Matrix::operator +( const Matrix &B )  /// A+B   A.operator+(B)
{
    Matrix R(*this);

    if ( rows_!=B.rows_ || cols_ != B.cols_ ) {
        MatrixExeption SIZE_ERROR("Matrites of different sizes!");
        throw SIZE_ERROR;
    }

    for ( size_t i = 0; i < cols_ * rows_; i++ ){
        R.data_[i] = data_[i] + B.data_[i];
    }
    return R;
}


void Matrix::operator=( const Matrix & A )
{
    // if (rows_ != A.rows_ || cols_ != A.cols_) {
    //     exception(err::ERROR, (char*)"не соответствующие размеры матрицы.\n");
    // } else {
            
    // }
    memcpy(data_, A.data_, sizeof(double) * rows_ * cols_);
}


void Matrix::operator=(  Matrix && B ){
    delete data_;
    data_ = B.data_;
    cols_ = B.cols_;
    rows_ = B.rows_;
    B.data_ = nullptr;
    B.cols_ = 0;
    B.rows_ = 0;
}


Matrix Matrix::operator-( const Matrix &B )
{
    Matrix R(*this);

    if ( rows_!=B.rows_ || cols_ != B.cols_ ) {
        MatrixExeption SIZE_ERROR("Matrites of different sizes!");
        throw SIZE_ERROR;
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
        MatrixExeption SIZE_ERROR("Matrites of different sizes!");
        throw SIZE_ERROR;
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

    memset(result_matrix.data_, 0, sizeof(double) * size * size);

    for (size_t ind = 0; ind < size; ind++){
        result_matrix.data_[ind * result_matrix.cols_ + ind] = 1;
    }

    return result_matrix;
}


Matrix Matrix::invert()
{
    if (cols_ != rows_) {
        MatrixExeption SIZE_ERROR("Matrites of different sizes!");
        throw SIZE_ERROR;
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
        MatrixExeption SIZE_ERROR("Matrites of different sizes!");
        throw SIZE_ERROR;
    }

    Matrix multiplied(cols_, rows_);

    Matrix result = idenity(cols_);

    double factorial = 1;

    for ( int acc = 1; acc <= accuracy; acc++ ){
        
        factorial /= acc;
        // printf("%f \n", factorial);

        // multiplied.Free();
        multiplied = degree(acc) * factorial;
        // multiplied.print();


        result = result + multiplied;
    }
    multiplied.Free();
    return result;
}

Matrix Matrix::degree(unsigned int degr){
    if (cols_!=rows_) {
        MatrixExeption SIZE_ERROR("Matrites of different sizes!");
        throw SIZE_ERROR;
    }

    size_t n_cols = cols_;
    size_t n_rows = rows_;
    Matrix result_matrix(*this);

    for (int i = 1; i < degr; i++){
        result_matrix = *this * *this;
    }

    return result_matrix;
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

    Matrix degr (A);
    degr = degr.degree(2);
    degr.print();
    degr.Free();

    Matrix exponenta (A);
    exponenta = A.exp(3);
    exponenta.print();
    exponenta.Free();

    return 0;
}