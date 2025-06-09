#include "matrix-cpp.h"
#include <cstring>
#include <vector>
#include <iostream>
#include <string.h>
#include <cstdint>
#include <cmath>

Matrix::Matrix() : rowCount(0), columnCount(0), data(nullptr) {}

Matrix::Matrix(size_t rowCount, size_t columnCount)
{
    alloc(rowCount, columnCount);
}

Matrix::Matrix(size_t rowCount, size_t columnCount, const std::vector<double> &inData)
{
    if(inData.size() != rowCount * columnCount){
        throw MatrixException(MatrixException::InputDataException);
    }
    alloc(rowCount, columnCount);
    memcpy(data, inData.data(), rowCount * columnCount * sizeof(double));
}

Matrix::Matrix(const Matrix &sourceMatrix)
{
    if(sourceMatrix.rowCount == 0 || sourceMatrix.columnCount == 0){
        throw MatrixException(MatrixException::ZeroDimensionException);
    }
    alloc(sourceMatrix.rowCount, sourceMatrix.columnCount);
    memcpy(data, sourceMatrix.data, sourceMatrix.rowCount * sourceMatrix.columnCount * sizeof(double));
}

Matrix::Matrix(Matrix &&sourceMatrix) : 
                                    rowCount(sourceMatrix.rowCount), 
                                    columnCount(sourceMatrix.columnCount), 
                                    data(sourceMatrix.data)
{
    sourceMatrix.setNull();
}

Matrix::~Matrix()
{
    free();
}

void Matrix::setNull()
{
    rowCount = 0;
    columnCount = 0;
    data = nullptr;
}

bool Matrix::isEmpty() const
{
    if(rowCount == 0 && columnCount == 0){
        return true;
    }
    return false;
}

void Matrix::alloc(size_t rowCount, size_t columnCount)
{
    if (rowCount == 0 || columnCount == 0) {
        throw MatrixException(MatrixException::ZeroDimensionException);
    }
    if (rowCount > SIZE_MAX / columnCount / sizeof(double)){
        setNull();
        throw MatrixException(MatrixException::BigSizeAllocationException);
    }

    data = new double[rowCount * columnCount]{}; // set all the way zero
    this->rowCount= rowCount;
    this->columnCount = columnCount;
}

void Matrix::free()
{
    rowCount = 0;
    columnCount = 0;
    delete[] data;
}

size_t Matrix::getColumnCount() const noexcept
{
    return columnCount;
}

size_t Matrix::getRowCount() const noexcept
{
    return rowCount;
}

void Matrix::print() const
{
    if(columnCount == 0 || rowCount == 0){
        throw MatrixException(MatrixException::ZeroDimensionException);
    }

    std::cout << std::endl;
    for(size_t idx = 0; idx < rowCount * columnCount; idx += columnCount){
        for(size_t col = 0; col < columnCount; col++){
            std::cout << data[idx + col] << std::string(" ");
        }
        std::cout << std::endl;
    } 
}

void Matrix::getIdentity()
{
    if(rowCount == 0 || columnCount == 0){
        throw MatrixException(MatrixException::ZeroDimensionException);
    }
    if(rowCount != columnCount){
        throw MatrixException(MatrixException::NotQuadraticException);
    }

    memset(data, 0, rowCount * columnCount * sizeof(double));
    for (size_t i = 0; i < rowCount * columnCount; i += columnCount + 1) {
        data[i] = 1.0;
    }
}

Matrix& Matrix::operator=(const Matrix &sourceMatrix)
{
    if(rowCount * columnCount == sourceMatrix.columnCount * sourceMatrix.rowCount){
        columnCount = sourceMatrix.columnCount;
        rowCount = sourceMatrix.rowCount;
        memcpy(data, sourceMatrix.data, rowCount * columnCount * sizeof(double));
    }
    else{
        free();
        alloc(sourceMatrix.rowCount, sourceMatrix.columnCount);
    }
    return *this;
}

Matrix& Matrix::operator=(Matrix &&sourceMatrix)
{
    if(data == sourceMatrix.data){
        return *this;
    }

    free();

    rowCount = sourceMatrix.rowCount;
    columnCount = sourceMatrix.columnCount;
    data = sourceMatrix.data;

    sourceMatrix.rowCount = 0;
    sourceMatrix.columnCount = 0;
    sourceMatrix.data = nullptr;
}

Matrix Matrix::operator-(const Matrix &sourceMatrix) const
{
    if(rowCount != sourceMatrix.rowCount || columnCount != sourceMatrix.columnCount){
        throw MatrixException(MatrixException::DimensionException);
    }

    Matrix result(rowCount, columnCount);

    for(size_t idx = 0; idx < rowCount * columnCount; idx++){
        result.data[idx] = data[idx] - sourceMatrix.data[idx];
    }

    return result;
}

Matrix& Matrix::operator-=(const Matrix &sourceMatrix)
{
    if(rowCount != sourceMatrix.rowCount || columnCount != sourceMatrix.columnCount){
        throw MatrixException(MatrixException::DimensionException);
    }

    for(size_t idx = 0; idx < rowCount * columnCount; idx++){
        data[idx] -= sourceMatrix.data[idx];
    }
    return *this;
}

Matrix Matrix::operator-() const
{
    Matrix result(rowCount, columnCount);

    for(size_t idx = 0; idx < rowCount * columnCount; idx++){
        result.data[idx] = -data[idx];
    }
    return result;
}

Matrix Matrix::operator+(const Matrix &sourceMatrix) const
{
    if(rowCount != sourceMatrix.rowCount || columnCount != sourceMatrix.columnCount){
        throw MatrixException(MatrixException::DimensionException);
    }

    Matrix result(rowCount, columnCount);

    for(size_t idx = 0; idx < rowCount * columnCount; idx++){
        result.data[idx] = data[idx] + sourceMatrix.data[idx];
    }

    return result;
}

Matrix& Matrix::operator+=(const Matrix &sourceMatrix)
{
    if(rowCount != sourceMatrix.rowCount || columnCount != sourceMatrix.columnCount){
        throw MatrixException(MatrixException::DimensionException);
    }

    for(size_t idx = 0; idx < rowCount * columnCount; idx++){
        data[idx] += sourceMatrix.data[idx];
    }
    return *this;
}

Matrix Matrix::operator*(const Matrix &sourceMatrix) const
{
    if(columnCount != sourceMatrix.rowCount){
        throw MatrixException(MatrixException::DimensionException);
    }

    size_t sourceColumnCount = sourceMatrix.columnCount;
    Matrix result(rowCount, sourceColumnCount);

    for(size_t row = 0; row < rowCount; row++) {
        for(size_t col = 0; col < sourceColumnCount; col++) {
            for(size_t idx = 0; idx < columnCount; idx++) {
                result.data[row * sourceColumnCount + col] += data[row * columnCount + idx] * sourceMatrix.data[idx * sourceColumnCount + col];
            }
        }
    }
    return result;
}

Matrix& Matrix::operator*=(const double number) noexcept {
    for(size_t idx = 0; idx < rowCount * columnCount; idx++) {
        data[idx] *= number;
    }

    return *this;
}

Matrix& Matrix::operator*=(const Matrix &sourceMatrix)
{
    if(columnCount != sourceMatrix.rowCount){
        throw MatrixException(MatrixException::DimensionException);
    }

    size_t sourceColumnCount = sourceMatrix.columnCount;
    Matrix result(rowCount, sourceColumnCount);

    *this = (*this) * sourceMatrix;

    return *this;
}

// Be mindful of a index start with 0
double Matrix::operator()(const size_t rowIndex, const size_t columnIndex) const
{
    if(rowIndex >= rowCount || columnIndex >= columnCount){
        throw MatrixException(MatrixException::OutIndexException);
    }
    return data[rowIndex * columnCount + columnIndex];
}

Matrix Matrix::transpose() const {
    Matrix result(columnCount, rowCount);

    for(size_t row = 0; row < rowCount; row++) {
        for(size_t col = 0; col < columnCount; col++) {
            result.data[col * rowCount + row] = data[row * columnCount + col];
        }
    }

    return result;
}

Matrix Matrix::pow(const size_t power) const {
    if(rowCount != columnCount) {
        throw MatrixException(MatrixException::NotQuadraticException);
    }

    Matrix result(rowCount, columnCount);
    result.getIdentity();

    for(size_t idx = 0; idx < power; idx++) {
        result *= (*this);
    }

    return result;
}

Matrix Matrix::exp(const size_t iterations) const {
    if(rowCount != columnCount) {
        throw MatrixException(MatrixException::NotQuadraticException);
    }

    Matrix result(rowCount, columnCount);
    result.getIdentity();

    Matrix term(rowCount, columnCount);
    term.getIdentity();

    for(size_t num = 1; num < iterations; num++) {
        term *= (*this);
        term *= (1 / (double)num);
        result += term;
    }

    return result;
}

double Matrix::getAlgebraicAddition(const size_t rowIndex, const size_t columnIndex) const
{
    if (rowIndex >= rowCount || columnIndex >= columnCount) {
        throw MatrixException(MatrixException::OutIndexException);
    }

    Matrix minorMatrix(rowCount - 1, columnCount - 1);
    
    size_t minorRow = 0;
    for (size_t i = 0; i < rowCount; ++i) {
        if (i == rowIndex) continue;
        
        size_t minorCol = 0;
        for (size_t j = 0; j < columnCount; ++j) {
            if (j == columnIndex) continue;
            
            minorMatrix.data[minorRow * minorMatrix.columnCount + minorCol] = 
                data[i * columnCount + j];
            minorCol++;
        }
        minorRow++;
    }

    int sign = ((rowIndex + columnIndex) % 2 == 0) ? 1 : -1;
    return sign * minorMatrix.getDeterminant();
}

double Matrix::getDeterminant() const
{
    if (columnCount != rowCount) {
        throw MatrixException(MatrixException::NotQuadraticException);
    }
    
    if (rowCount == 1) {
        return data[0];
    }
    if (rowCount == 2) {
        return data[0] * data[3] - data[1] * data[2];
    }
    if (rowCount == 3) {
        return data[0] * data[4] * data[8] + 
               data[1] * data[5] * data[6] + 
               data[2] * data[3] * data[7] -
               data[2] * data[4] * data[6] - 
               data[0] * data[5] * data[7] - 
               data[1] * data[3] * data[8];
    }

    double det = 0.0;
    for (size_t col = 0; col < columnCount; col++) {
        det += data[col] * getAlgebraicAddition(0, col);
    }
    return det;
}