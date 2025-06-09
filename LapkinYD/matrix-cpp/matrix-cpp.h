#ifndef MATRIX_CPP_H
#define MATRIX_CPP_H

#include <stdlib.h>
#include <exception>
#include <vector>
#include <iostream>

class Matrix
{
    private:
        size_t rowCount;
        size_t columnCount;
        double *data;
    public:
        Matrix();
        Matrix(size_t rowCount, size_t columnCount);
        Matrix(const Matrix &sourceMatrix);
        Matrix(Matrix &&sourceMatrix) noexcept;
        Matrix(size_t rowCount, size_t columnCount, const std::vector<double> &inData);
        ~Matrix();

        void print() const;
        bool isEmpty() const;
        void setNull();
        void getIdentity();
        
        size_t getColumnCount() const noexcept;
        size_t getRowCount() const noexcept;
        
        Matrix transpose() const;
        Matrix exp(const size_t iterations) const;
        Matrix pow(const size_t power) const;
        double getDeterminant() const;
        
        Matrix& operator=(const Matrix &sourceMatrix);
        Matrix& operator=(Matrix &&sourceMatrix);
        Matrix operator-(const Matrix &sourceMatrix) const;
        Matrix operator-() const;
        Matrix operator+(const Matrix &sourceMatrix) const;
        Matrix operator*(const Matrix &sourceMatrix) const;
        Matrix& operator-=(const Matrix &sourceMatrix);
        Matrix& operator+=(const Matrix &sourceMatrix);
        Matrix& operator*=(const Matrix &sourceMatrix);
        Matrix& operator*=(const double number) noexcept;
        double operator()(const size_t rowIndex, const size_t columnIndex) const;
private:
        void alloc(size_t rowCount, size_t columnCount);
        void free();
        double getAlgebraicAddition(const size_t rowIndex, const size_t columnIndex) const;
};

class MatrixException : public std::exception
{
public:

    enum Exception
    {
        BigSizeAllocationException,
        InputDataException,
        ZeroDimensionException,
        DimensionException,
        NotQuadraticException,
        OutIndexException
    };

    MatrixException(const enum Exception exception) : exception_code(exception) {}

    const char* what () const noexcept override
    {
        switch (exception_code)
        {
        case BigSizeAllocationException:
            std::cout << "GET EXCEPTION: BigSizeAllocationException. Size of the matrix is too big!" << std::endl;
            break;
        case DimensionException:
            std::cout << "GET EXCEPTION: DimensionException. Two matrices have different dimensions" << std::endl;
            break;
        case ZeroDimensionException:
            std::cout << "GET EXCEPTION: ZeroDimensionException. Matrix has zero a dimension" << std::endl;
            break;
        case InputDataException:
            std::cout << "GET EXCEPTION: InputDataException. Input data has different element count!" << std::endl;
            break;
        case NotQuadraticException:
            std::cout << "GET EXCEPTION: NotQuadraticException. Matrix must be quadratic!" << std::endl;
            break;
        case OutIndexException:
            std::cout << "GET EXCEPTION: OutIndexException. Index out of matrix" << std::endl;
            break;
        default:
            return nullptr;
        }
    }
private:
    enum Exception exception_code;
};

#endif // MATRIC_CPP_H