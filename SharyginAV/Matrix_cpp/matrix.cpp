#include <iostream>
#include <vector>

using namespace std;

class Matrix
{
public:
    Matrix(size_t rows, size_t cols) : rows(rows), cols(cols), data(rows * cols, 0.0) {}

    Matrix() : rows(0), cols(0) {}

    Matrix(const Matrix &M) : rows(M.rows), cols(M.cols), data(M.data) {}  // конструктор копирования

    ~Matrix() = default;

    double& operator()(const size_t row_num,const size_t col_num) {
        if(row_num >= rows || col_num >= cols) {
            throw "Out of range";
        }
        return data[row_num * cols + col_num];
    }

    const double& operator()(const size_t row_num, const size_t col_num) const {
        if(row_num >= rows || col_num >= cols) {
            throw "Out of range";
        }
        return data[row_num * cols + col_num];
    }

    Matrix operator+(const Matrix &A) const
    {
        if(!A.equal_size(*this)) {
            throw "Matrices of unequal sizes";
        }

        Matrix result(A.rows, A.cols);
        for(size_t idx = 0; idx < rows * cols; ++idx) {
            result.data[idx] = data[idx] + A.data[idx];
        }

        return result;
    }

    Matrix operator-(const Matrix &A) const
    {
        if(!A.equal_size(*this)) {
            throw "Matrices of unequal sizes";
        }

        Matrix result(A.rows, A.cols);
        for(size_t idx = 0; idx < rows * cols; ++idx) {
            result.data[idx] = data[idx] - A.data[idx];
        }

        return result;
    }

    Matrix& operator+=(const Matrix &A)
    {
        if(!A.equal_size(*this)) {
            throw "Matrices of unequal sizes";
        }

        for(size_t idx = 0; idx < rows * cols; ++idx) {
            data[idx] += A.data[idx];
        }

        return *this;
    }

    Matrix& operator-=(const Matrix &A)
    {
        if(!A.equal_size(*this)) {
            throw "Matrices of unequal sizes";
        }

        for(size_t idx = 0; idx < rows * cols; ++idx) {
            data[idx] -= A.data[idx];
        }

        return *this;
    }

    void print() const noexcept
    {
        for(size_t row_idx = 0; row_idx < rows; ++row_idx) {
            for(size_t col_idx = 0; col_idx < cols; ++col_idx) {
                std::cout << (*this)(row_idx, col_idx);
                col_idx == cols - 1 ? std::cout << '\n' : std::cout << '\t';
            }
        }
    }

    bool equal_size(const Matrix &A) const
    {
        return (rows == A.rows) && (cols == A.cols);
    }

    Matrix transposition()
    {
        Matrix res(cols, rows);

        for(size_t row_idx = 0; row_idx < rows; ++row_idx) {
            for(size_t col_idx = 0; col_idx < cols; ++col_idx) {
                res(col_idx, row_idx) = (*this)(row_idx, col_idx);
            }            
        }

        return res;
    }

    void get_size()
    {
        cout << "Количество строк: " << rows << "\tКоличество столбцов: " << cols << '\n';
    }

    

private:
    size_t rows;
    size_t cols;
    std::vector<double> data;
};


int main()
{
    Matrix m1(2, 3);
    m1(0, 0) = 1;
    m1(0, 1) = 2;
    m1(1, 0) = 3;
    m1(1, 1) = 4;
    
    m1.print();
    m1.get_size();
    m1 = m1.transposition();
    m1.print();
    m1.get_size();
}
