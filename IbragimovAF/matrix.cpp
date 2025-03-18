#include <iostream>
// TODO: sample


class MatrixException
{
    public: 
        MatrixException(std::string message): message{message}{}
        std::string getMessage() const {return message;}
    private:
        std::string message;
};


class Matrix {
    public:

        Matrix(size_t col, size_t row);
        Matrix(const Matrix &X);
        ~Matrix();
        
        void print();
        void zeroes();
        void random();
        double gauss_det();

        Matrix &operator=(Matrix &X);
        Matrix &operator=(Matrix &&X);
        Matrix operator+(const Matrix &X);  //Matrix& operator+()noexcept;
        Matrix operator*(const double x);
        Matrix operator*(const Matrix &B);
        Matrix operator-(const Matrix &X);

    private:
        size_t cols;
        size_t rows;
        double *data;
};


Matrix::Matrix(size_t col, size_t row){
    cols = col;
    rows = row;
    data = new double[cols * rows];  //data = (double*)malloc(cols * rows * sizeof(double));
    this->zeroes();
}


Matrix::Matrix(const Matrix &X){
    cols = X.cols;
    rows = X.rows;
    data = new double[cols * rows];
    for (size_t index = 0; index < cols * rows; index++){
        data[index] = X.data[index];
    }
}


void Matrix::zeroes(){
    for (size_t index = 0; index < cols * rows; index++){
        data[index] = 0;
    }
}


void Matrix::random(){
    for (size_t index = 0; index < cols * rows; index++){
        data[index] = (double)(rand() % 5);  // data[k] == *(data + sizeof(double) * k)  == k[data]
    }
}


Matrix &Matrix::operator=(Matrix &X) 
{
    if(&X == this) return *this;
    if((cols != X.cols) || (rows != X.rows)){
        throw MatrixException{"Size error"};
    }
    else{
        memcpy(data, X.data, sizeof(double) * X.cols * X.rows);
    }
    return *this;
}


Matrix &Matrix::operator=(Matrix &&X)
{
    if(&X == this) return *this;
    if((cols != X.cols) || (rows != X.rows)){
        throw MatrixException{"Size error"};
    }
    else{
        delete data;
        data = X.data;
        rows  = X.rows;
        cols = X.cols;
        X.data = nullptr;
        X.rows = 0;
        X.cols = 0;
    }
    return *this;
}


Matrix Matrix::operator+(const Matrix &X) 
{
    if((cols != X.cols) || (rows != X.rows)){
        throw MatrixException{"Size error"};
    }
    else{
        Matrix Y(cols, rows);
        for (size_t index = 0; index < cols * rows; index++){
            Y.data[index] = data[index] + X.data[index];
        }
        return Y;
    }
}


Matrix Matrix::operator*(const double x){
    for (size_t index = 0; index < cols * rows; index++){
        data[index] *= x;
    }
    return *this;
}


Matrix Matrix::operator*(const Matrix &B) {
    if(!(cols == B.rows))throw MatrixException{"Size error"};
    Matrix mul_res(rows, B.cols);
    for (size_t B_col = 0; B_col< B.cols; B_col++) {
        for(size_t A_row = 0; A_row < rows; A_row++) {
            for(size_t B_row = 0; B_row< B.rows; B_row++) {
                mul_res.data[A_row * mul_res.cols + B_col] += data[A_row * cols + B_row] * B.data[B_row * B.cols + B_col];
            }
        }
    }
    return mul_res;
}



Matrix Matrix::operator-(const Matrix &X) 
{
    Matrix Y(X);
    Y = *this + (Y * -1);
    return Y;
}

double Matrix::gauss_det() {
    double det = 1;
    Matrix A(*this);
    if(!((A.cols == A.rows))) {
        throw MatrixException{"Size error"};
    }
    for(size_t col = 0; col < A.cols; col++) {
        double max = abs(A.data[col * A.cols + col]);
        size_t index = col;
        for(size_t row = col + 1; row < A.rows; row++) {
            if(max < abs(A.data[col * A.cols + row])) {
                max = abs(A.data[col * A.cols + row]);
                index = row;
            }
        }
        if(index != col) { //  замена столбцов
            for(size_t row = col; row <A.rows; row++) {
                double dat = A.data[row * A.cols + col];
                A.data[row * A.cols + col] = A.data[row * A.cols + index];
                A.data[row * A.cols + index] = dat;
            }
            det *= -1;
        }
        for(size_t row = col + 1; row < A.rows; row++) {
            double t = A.data[row * A.cols + col] / A.data[col * A.cols + col];
            for(size_t idx = col; idx < A.cols; idx++) {
                A.data[row * A.cols + idx] = A.data[row * A.cols + idx] - t * A.data[col * A.cols + idx];
            }
        }
    }
    for(size_t idx = 0; idx <= A.cols - 1; idx++) {
        det *= A.data[idx * A.cols + idx];
    }
    return det;
}


void Matrix::print(){
    for (size_t col = 0; col < cols; col++){
        for(size_t row = 0; row < rows; row++){
            std::printf("%.0f ", data[row + (col * cols)]);
        }   
        std::printf("\n");
    }
    std::printf("\n");
}


Matrix::~Matrix(){
    delete[] data;
    // std::printf("clear\n");
}


int main(){
    Matrix A(3,3);
    Matrix B(3,3);
    Matrix C(B);

    A.random();
    std::printf("A\n");
    A.print();

    B.random();
    std::printf("B\n");
    B.print();

    C.random();
    std::printf("C\n");
    C.print();

    C = A * B;
    std::printf("A*B\n");
    C.print();

    C = A + B;
    std::printf("A+B\n");
    C.print();

    C = A - B;
    std::printf("A-B\n");
    C.print();

    A = B;
    std::printf("A = B\n");
    A.print();

    Matrix D(5,5);
    D.random();
    std::printf("D\n");
    D.print();
    std::printf("Det  D:%.2f\n\n",D.gauss_det());

    A.zeroes();
    std::printf("A.zeroes\n");
    A.print();
    return 0;
}