#include <iostream>
#include <vector>


class Matrix
{
public:
    Matrix(size_t rows, size_t cols) : rows(rows), cols(cols), data(rows * cols, 0.0) {}

    Matrix() : rows(0), cols(0) {}

    Matrix(const Matrix &M) : rows(M.rows), cols(M.cols), data(M.data) {}  // конструктор копирования

    ~Matrix() = default;

    double& operator()(size_t row_num, size_t col_num) {
        if(row_num >= rows || col_num >= cols) {
            throw "Out of range";
        }
        return data[row_num * cols + col_num];
    }

    const double& operator()(size_t row_num, size_t col_num) const {
        if(row_num >= rows || col_num >= cols) {
            throw "Out of range";
        }
        return data[row_num * cols + col_num];
    }

    void print() const
    {
        for(size_t row_idx = 0; row_idx < rows; ++row_idx) {
            for(size_t col_idx = 0; col_idx < cols; ++col_idx) {
                std::cout << (*this)(row_idx, col_idx);
                col_idx == cols - 1 ? std::cout << '\n' : std::cout << '\t';
            }
        }
    }


private:
    size_t rows;
    size_t cols;
    std::vector<double> data;
};


int main()
{
    Matrix m1(3, 3);
    m1(0, 0) = 6;
    m1.print();
    Matrix m2(m1);
    m2.print();
}