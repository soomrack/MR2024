#include<iostream>

class MatrixException : public std::exception
{
    public: 
        MatrixException(std::string message): message{message}{}
        std::string getMessage() const {return message;}
    private:
        std::string message;
};


class Matrix {

    private:
    size_t cols;
    size_t rows;
    double *data;

    public:

        Matrix(size_t col, size_t row);
        Matrix(const Matrix &X);
        Matrix(Matrix &&X);
        ~Matrix();
        
        void print();
        void zeroes();
        void random();
        void identity();
        double gauss_det();

        Matrix &operator=(Matrix &X);
        Matrix &operator=(Matrix &&X);
        Matrix operator+(const Matrix &X);
        Matrix operator*(const double x);
        Matrix operator*(const Matrix &B);
        Matrix operator/(const double x);
        Matrix operator-(const Matrix &X);

        Matrix m_pow(const int level);
        Matrix matrix_exp(const unsigned int level);
        // конструктор из массива
        //дополнить функции, транспонирование

};


Matrix::Matrix(size_t col, size_t row){
    if(((double)SIZE_MAX / col / row / sizeof(double)) < 1.)throw MatrixException{"Matrix overflow"};
    if(col == 0 && row == 0)throw MatrixException{"Matrix size can not be zero"};
    cols = col;
    rows = row;
    data = new double[cols * rows];  //data = (double*)malloc(cols * rows * sizeof(double));
    zeroes();
}


Matrix::Matrix(const Matrix &X){
    cols = X.cols;
    rows = X.rows;
    data = new double[cols * rows];
    memcpy(data, X.data, sizeof(double) * X.cols * X.rows);
}


Matrix::Matrix(Matrix &&X){
    if((cols != X.cols) || (rows != X.rows))throw MatrixException{"Size error"};
    data = X.data;
    rows  = X.rows;
    cols = X.cols;
    X.data = nullptr;
    X.rows = 0;
    X.cols = 0;
}


void Matrix::zeroes(){
    for (size_t index = 0; index < cols * rows; index++){
        data[index] = 0;//mem set
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
    if((cols != X.cols) || (rows != X.rows))throw MatrixException{"Size error"};

    memcpy(data, X.data, sizeof(double) * X.cols * X.rows);
    return *this;
}


Matrix &Matrix::operator=(Matrix &&X)
{
    if(&X == this) return *this;
    if((cols != X.cols) || (rows != X.rows))throw MatrixException{"Size error"};

    delete data;
    data = X.data;
    rows  = X.rows;
    cols = X.cols;
    X.data = nullptr;
    X.rows = 0;
    X.cols = 0;
    return *this;
}


Matrix Matrix::operator+(const Matrix &X) 
{
    if((cols != X.cols) || (rows != X.rows))throw MatrixException{"Size error"};

    Matrix Y(cols, rows);
    for (size_t index = 0; index < cols * rows; index++){
        Y.data[index] = data[index] + X.data[index];
    }
    return Y;
}


Matrix Matrix::operator*(const double x){
    Matrix MUL(*this);
    for (size_t index = 0; index < cols * rows; index++){
        MUL.data[index] *= x;
    }
    return MUL;
}


Matrix Matrix::operator/(const double x){
    Matrix DIV(*this);
    for (size_t index = 0; index < cols * rows; index++){
        DIV.data[index] /= x;
    }
    return DIV;
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
    if((cols != X.cols) || (rows != X.rows))throw MatrixException{"Size error"};

    Matrix Y(X);
    Y = *this + (Y * -1);
    return Y;
}

double Matrix::gauss_det() {
    if(!(cols == rows))throw MatrixException{"Size error"};

    double det = 1;
    Matrix A(*this);
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


void Matrix::identity(){
    if(!(cols == rows))throw MatrixException{"Size error"};

    for (size_t col = 0; col < cols; col++){
        for(size_t row = 0; row < rows; row++){
            if(col == row) data[row + (col * cols)] = 1;
            else data[row + (col * cols)] = 0;
        }   
    }
}


Matrix Matrix::m_pow(const int level){
    if(!((cols == rows)))throw MatrixException{"Size error"};

    Matrix INV_RES(*this);
    if(level == 0){
        INV_RES.identity();
        return INV_RES;
    }
    if(level == 1) return INV_RES;
    for (unsigned int idx = 1; idx < level; idx++) {
        INV_RES = INV_RES * *this;
    }
    return INV_RES;
}


static double fact(int num) {
    double f = 1;
    if(num == 0) return f;
    for(double i = 1; i <= num; i++)f *= i;
    return f;
}


Matrix  Matrix::matrix_exp(const unsigned int level) {
    Matrix EXP_RES(*this);
    EXP_RES.zeroes();
    for (int idx = 0; idx <= level; idx++) {
        EXP_RES = (EXP_RES + (m_pow(idx)/fact(idx)));
    }
    return EXP_RES;
}


void Matrix::print(){
    for (size_t col = 0; col < cols; col++){
        for(size_t row = 0; row < rows; row++){
            std::printf("%.2f ", data[row + (col * cols)]);
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

    C = A + B;
    std::printf("A+B\n");
    C.print();

    C = A - B;
    std::printf("A-B\n");
    C.print();

    C = A * B;
    std::printf("A*B\n");
    C.print();

    std::printf("A exp 3\n");
    A.matrix_exp(3).print();

    A = B;
    std::printf("A = B\n");
    A.print();

    Matrix D(5,5);
    D.random();
    std::printf("D\n");
    D.print();
    std::printf("Det |D|:%.2f\n\n",D.gauss_det());

    A.zeroes();
    std::printf("A.zeroes\n");
    A.print();

    A.identity();
    std::printf("A.identity\n");
    A.print();
    return 0;
}