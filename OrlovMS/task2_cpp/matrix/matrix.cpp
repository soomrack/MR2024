#include "matrix.h"
#include <cstdarg>
#include <cstdio>
#include <cmath>


Matrix::LogLevel Matrix::current_level = Matrix::LOG_NONE;


void Matrix::set_log_level(LogLevel level) noexcept
{
    current_level = level;
}


void Matrix::print_log(LogLevel level, const char *format, ...) const noexcept
{
#ifdef MATRIX_LOG_ENABLE
    if(level <= current_level) {
        switch(level)
        {
        case LOG_ERR:
            printf("\033[0;31mERROR: \033[0m");
            break;
        case LOG_WARN:
            printf("\033[1;33mWARNING: \033[0m");
            break;
        case LOG_INFO:
            printf("INFO: ");
        default:
            break;
        }
        va_list list;
        va_start(list, format);
        vprintf(format, list);
        va_end(list);
    }
#endif
}


void Matrix::alloc(const size_t rows, const size_t cols)
{
    print_log(LOG_INFO, "allocate matrix with size: %lux%lu\n", rows, cols);

    if(rows == 0 || cols == 0) {
        print_log(LOG_WARN, "empty matrix allocation\n");
        return;
    }

    if(__SIZE_MAX__ / rows / cols / sizeof(double) == 0) {
        this->rows = 0;
        this->cols = 0;
        print_log(LOG_ERR, "matrix size too big\n");
        throw PARAMS_ERR;
    }

    try {
        this->data = new double[rows * cols];
    }
    catch(...) {
        this->rows = 0;
        this->cols = 0;
        throw ALLOC_ERR;
    }
}


void Matrix::free()
{
    print_log(LOG_INFO, "free matrix\n");

    this->rows = 0;
    this->cols = 0;
    delete[] this->data;
}


Matrix::Matrix() : rows(0), cols(0), data(nullptr)
{
    print_log(LOG_INFO, "create empty matrix\n");
}


Matrix::Matrix(const size_t rows, const size_t cols) : rows(rows), cols(cols), data(nullptr)
{
    print_log(LOG_INFO, "create matrix with size\n");

    alloc(rows, cols);
}


Matrix::Matrix(const Matrix &mat) : rows(mat.rows), cols(mat.cols), data(nullptr)
{
    print_log(LOG_INFO, "create matrix copy\n");

    alloc(mat.rows, mat.cols);

    print_log(LOG_INFO, "copy data\n");

    memcpy(data, mat.data, rows * cols * sizeof(double));
}


Matrix::Matrix(Matrix &&mat) : rows(mat.rows), cols(mat.cols), data(mat.data)
{
    print_log(LOG_INFO, "move matrix\n");

    mat.rows = 0;
    mat.cols = 0;
    mat.data = nullptr;
}


Matrix::~Matrix()
{
    print_log(LOG_INFO, "matrix destructor\n");

    free();
}


void Matrix::resize(const size_t rows, const size_t cols)
{
    print_log(LOG_INFO, "resize matrix\n");

    if(this->rows * this->cols == rows * cols) {
        print_log(LOG_INFO, "data size equals, reallocation not need\n");
        this->rows = rows;
        this->cols = cols;
        return;
    }

    free();
    alloc(rows, cols);
}


void Matrix::print() const noexcept
{
    if(is_empty()) {
        printf("matrix is empty\n");
        return;
    }

    printf("Matrix size: %ux%u\n", rows, cols);
    for(size_t row = 0; row < (rows * cols); row += cols) {
        printf("[ ");
        for(size_t idx = 0; idx < cols - 1; idx++) {
            printf("%8.3f, ", data[row + idx]);
        }
        printf("%8.3f ]\n", data[row +  cols - 1]);
    }
}


size_t Matrix::get_rows() const noexcept
{
    return rows;
}


size_t Matrix::get_cols() const noexcept
{
    return cols;
}


void Matrix::set(const size_t row, const size_t col, const double num)
{
    if(row >= rows || col >= cols) {
        print_log(LOG_ERR, "index out of bound\n");
        throw PARAMS_ERR;
    }

    data[row * cols + col] = num;
}


double Matrix::get(const size_t row, const size_t col) const
{
    if(row >= rows || col >= cols) {
        print_log(LOG_ERR, "index out of bound\n");
        throw PARAMS_ERR;
    }

    return data[row * cols + col];
}


bool Matrix::is_empty() const noexcept
{
    return data == nullptr;
}


bool Matrix::is_square() const noexcept
{
    return rows == cols;
}


bool Matrix::equal_size(const Matrix &second) const noexcept
{
    return (rows == second.rows) && (cols == second.cols);
}


void Matrix::set_zeros() noexcept
{
    print_log(LOG_INFO, "make zero matrix\n");

    if(is_empty()) {
        print_log(LOG_WARN, "matrix is empty\n");
        return;
    }

    memset(data, 0, rows * cols * sizeof(double));
}


void Matrix::set_identity()
{
    print_log(LOG_INFO, "make identity matrix\n");

    if(!is_square()) {
        print_log(LOG_ERR, "matrix not square\n");
        throw SIZE_ERR;
    }

    set_zeros();

    for(size_t idx = 0; idx < rows * cols; idx += cols + 1) {
        data[idx] = 1.0;
    }
}


Matrix& Matrix::operator=(const Matrix &mat)
{
    print_log(LOG_INFO, "matrix assignment\n");

    resize(mat.rows, mat.cols);

    memcpy(data, mat.data, rows * cols * sizeof(double));

    return *this;
}


Matrix& Matrix::operator=(Matrix &&mat)
{
    print_log(LOG_INFO, "matrix move assignment\n");

    free();

    rows = mat.rows;
    cols = mat.cols;
    data = mat.data;

    mat.rows = 0;
    mat.cols = 0;
    mat.data = nullptr;

    return *this;
}


Matrix Matrix::operator-() const
{
    print_log(LOG_INFO, "matrix opposite\n");

    Matrix res(rows, cols);

    for(size_t idx = 0; idx < rows * cols; idx++) {
        res.data[idx] = -data[idx];
    }

    return res;
}


Matrix Matrix::operator+(const Matrix &second) const
{
    print_log(LOG_INFO, "matrix sum\n");

    if(!equal_size(second)) {
        print_log(LOG_ERR, "matrix size not equals\n");
        throw SIZE_ERR;
    }

    Matrix res(rows, cols);

    for(size_t idx = 0; idx < rows * cols; idx++) {
        res.data[idx] = data[idx] + second.data[idx];
    }

    return res;
}


Matrix& Matrix::operator+=(const Matrix &mat)
{
    print_log(LOG_INFO, "matrix sum assignment\n");

    if(!equal_size(mat)) {
        print_log(LOG_ERR, "matrix size not equals\n");
        throw SIZE_ERR;
    }

    for(size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] += mat.data[idx];
    }

    return *this;
}


Matrix Matrix::operator-(const Matrix &second) const
{
    print_log(LOG_INFO, "matrix subtraction\n");

    if(!equal_size(second)) {
        print_log(LOG_ERR, "matrix size not equals\n");
        throw SIZE_ERR;
    }

    Matrix res(rows, cols);

    for(size_t idx = 0; idx < rows * cols; idx++) {
        res.data[idx] = data[idx] - second.data[idx];
    }

    return res;
}


Matrix& Matrix::operator-=(const Matrix &mat)
{
    print_log(LOG_INFO, "matrix subtraction assignment\n");

    if(!equal_size(mat)) {
        print_log(LOG_ERR, "matrix size not equals\n");
        throw SIZE_ERR;
    }

    for(size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] -= mat.data[idx];
    }

    return *this;
}


Matrix Matrix::operator*(const double num) const noexcept
{
    print_log(LOG_INFO, "matrix multiplication by number\n");

    Matrix res(rows, cols);

    for(size_t idx = 0; idx < rows * cols; idx++) {
        res.data[idx] = data[idx] * num;
    }

    return res;
}


Matrix& Matrix::operator*=(const double num) noexcept
{
    print_log(LOG_INFO, "matrix multiplication by number assignment\n");

    for(size_t idx = 0; idx < rows * cols; idx++) {
        data[idx] *= num;
    }

    return *this;
}


Matrix Matrix::operator*(const Matrix &second) const
{
    print_log(LOG_INFO, "matrix multiplication\n");

    if(cols != second.rows) {
        print_log(LOG_ERR, "first.cols and second.rows not equals\n");
        throw SIZE_ERR;
    }

    Matrix res(rows, second.cols);

    for(size_t row = 0; row < rows; row++) {
        for(size_t col = 0; col < second.cols; col++) {
            res.data[row * res.cols + col] = 0.0;
            for(size_t idx = 0; idx < cols; idx++) {
                res.data[row * res.cols + col] += data[row * cols + idx] * second.data[idx * second.cols + col];
            }
        }
    }

    return res;
}


Matrix& Matrix::operator*=(const Matrix &second)
{
    print_log(LOG_INFO, "matrix multiplication assignment\n");

    if(cols != second.rows) {
        print_log(LOG_ERR, "first.cols and second.rows not equals\n");
        throw SIZE_ERR;
    }

    *this = (*this) * second;

    return *this;
}


Matrix Matrix::pow(const unsigned int pow) const
{
    print_log(LOG_INFO, "matrix power\n");

    if(!is_square()) {
        print_log(LOG_ERR, "matrix not square\n");
        throw SIZE_ERR;
    }

    Matrix res(rows, cols);
    res.set_identity();

    for(unsigned int cnt = 0; cnt < pow; cnt++) {
        res *= (*this);
    }

    return res;
}


Matrix Matrix::exp() const
{
    print_log(LOG_INFO, "matrix exponent\n");

    if(!is_square()) {
        print_log(LOG_ERR, "matrix not square\n");
        throw SIZE_ERR;
    }

    Matrix res(rows, cols);
    res.set_identity();

    Matrix term(rows, cols);
    term.set_identity();

    for(unsigned int num = 1; num < 10; num++) {
        term *= (*this);
        term *= (1 / (double)num);
        res += term;
    }

    return res;
}


Matrix Matrix::transp() const
{
    print_log(LOG_INFO, "matrix transposition\n");

    Matrix res(cols, rows);

    for(size_t row = 0; row < rows; row++) {
        for(size_t col = 0; col < cols; col++) {
            res.data[col * rows + row] = data[row * cols + col];
        }
    }

    return res;
}


size_t Matrix::find_non_zero_in_col(const size_t idx_start) const noexcept
{
    for(size_t row = idx_start + 1; row < rows; row++) {
        if(fabs(data[row * cols + idx_start]) >= 0.00001) {
            return row;
        }
    }

    return 0;
}


void Matrix::swap_rows(const size_t first, const size_t second) noexcept
{
    for(size_t idx = 0; idx < cols; idx++) {
        double tmp = data[first * cols + idx];
        data[first * cols + idx] = data[second * cols + idx];
        data[second * cols + idx] = tmp;
    }
}


void Matrix::sub_row(const size_t row, const size_t row_base, const double ratio) noexcept
{
    for(size_t idx = 0; idx < cols; idx++) {
        data[row * cols + idx] -= ratio * data[row_base * cols + idx];
    }
}


double Matrix::det() const
{
    print_log(LOG_INFO, "matrix determinant\n");

    if(!is_square()) {
        print_log(LOG_ERR, "matrix not square\n");
        throw SIZE_ERR;
    }

    Matrix tmp(*this);

    double det = 1.0;
    for(size_t row_base = 0; row_base < rows - 1; row_base++) {
        if(fabs(tmp.data[row_base * (cols + 1)]) < 0.00001) {
            size_t idx_non_zero = tmp.find_non_zero_in_col(row_base);
            if(idx_non_zero == 0) {
                return 0.0;
            }

            tmp.swap_rows(row_base, idx_non_zero);
            det *= -1.0;
        }

        for(size_t row = row_base + 1; row < rows; row++) {
            double mult = tmp.data[row * cols + row_base] / tmp.data[row_base * (cols + 1)];
            tmp.sub_row(row, row_base, mult);
        }
    }

    for(size_t idx = 0; idx < rows; idx++) {
        det *= tmp.data[idx * cols + idx];
    }

    return det;
}
