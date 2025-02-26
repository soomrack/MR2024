#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <vector>

using namespace std;

class Matrix
{
public:
    Matrix(size_t rows, size_t cols);

    Matrix();

    Matrix(const Matrix &M);  // конструктор копирования

    ~Matrix();

    double& operator()(const size_t row_num,const size_t col_num);

    const double& operator()(const size_t row_num, const size_t col_num) const;

    Matrix operator+(const Matrix &A) const;

    Matrix operator-(const Matrix &A) const;

    Matrix& operator+=(const Matrix &A);

    Matrix& operator-=(const Matrix &A);

    Matrix& operator=(const Matrix &A);

    Matrix operator*(const double num);
    
    Matrix operator*(const Matrix &A);

    Matrix& operator*=(const double num);

    Matrix& operator*=(const Matrix &A);

    void print() const noexcept;

    bool equal_size(const Matrix &A) const;

    bool is_square() const;

    void set_identity();

    Matrix transpos();

    Matrix pow(const unsigned int pow) const;

    Matrix exp(const unsigned int iretations) const;

    void get_size();

private:
    size_t rows;
    size_t cols;
    std::vector<double> data;
};

#endif