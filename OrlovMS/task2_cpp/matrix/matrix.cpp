#include "matrix.h"
#include <cstdarg>
#include <cstdio>
#include <cmath>


Matrix::LogLevel Matrix::s_current_level = Matrix::LOG_NONE;


void Matrix::set_log_level(LogLevel level) noexcept
{
    s_current_level = level;
}


void Matrix::print_log(LogLevel level, const char *format, ...) const noexcept
{
#ifdef MATRIX_LOG_ENABLE
    if(level <= s_current_level) {
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
        m_rows = 0;
        m_cols = 0;
        print_log(LOG_ERR, "matrix size too big\n");
        throw PARAMS_ERR;
    }

    try {
        m_data = new double[rows * cols];
    }
    catch(...) {
        m_rows = 0;
        m_cols = 0;
        throw ALLOC_ERR;
    }

    m_rows = rows;
    m_cols = cols;
}


void Matrix::free()
{
    print_log(LOG_INFO, "free matrix\n");

    m_rows = 0;
    m_cols = 0;
    delete[] m_data;
}


Matrix::Matrix() : m_rows(0), m_cols(0), m_data(nullptr)
{
    print_log(LOG_INFO, "create empty matrix\n");
}


Matrix::Matrix(const size_t rows, const size_t cols) : m_rows(rows), m_cols(cols), m_data(nullptr)
{
    print_log(LOG_INFO, "create matrix with size\n");

    alloc(rows, cols);
}


Matrix::Matrix(const Matrix &mat) : m_rows(mat.m_rows), m_cols(mat.m_cols), m_data(nullptr)
{
    print_log(LOG_INFO, "create matrix copy\n");

    alloc(mat.m_rows, mat.m_cols);

    print_log(LOG_INFO, "copy data\n");

    memcpy(m_data, mat.m_data, m_rows * m_cols * sizeof(double));
}


Matrix::Matrix(Matrix &&mat) : m_rows(mat.m_rows), m_cols(mat.m_cols), m_data(mat.m_data)
{
    print_log(LOG_INFO, "move matrix\n");

    mat.m_rows = 0;
    mat.m_cols = 0;
    mat.m_data = nullptr;
}


Matrix::~Matrix()
{
    print_log(LOG_INFO, "matrix destructor\n");

    free();
}


void Matrix::resize(const size_t rows, const size_t cols)
{
    print_log(LOG_INFO, "resize matrix\n");

    if(m_rows * m_cols == rows * cols) {
        print_log(LOG_INFO, "data size equals, reallocation not need\n");
        m_rows = rows;
        m_cols = cols;
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

    printf("Matrix size: %ux%u\n", m_rows, m_cols);
    for(size_t row = 0; row < (m_rows * m_cols); row += m_cols) {
        printf("[ ");
        for(size_t idx = 0; idx < m_cols - 1; idx++) {
            printf("%8.3f, ", m_data[row + idx]);
        }
        printf("%8.3f ]\n", m_data[row +  m_cols - 1]);
    }
}


size_t Matrix::get_rows() const noexcept
{
    return m_rows;
}


size_t Matrix::get_cols() const noexcept
{
    return m_cols;
}


void Matrix::set(const size_t row, const size_t col, const double num)
{
    if(row >= m_rows || col >= m_cols) {
        print_log(LOG_ERR, "index out of bound\n");
        throw PARAMS_ERR;
    }

    m_data[row * m_cols + col] = num;
}


double Matrix::get(const size_t row, const size_t col) const
{
    if(row >= m_rows || col >= m_cols) {
        print_log(LOG_ERR, "index out of bound\n");
        throw PARAMS_ERR;
    }

    return m_data[row * m_cols + col];
}


bool Matrix::is_empty() const noexcept
{
    return m_data == nullptr;
}


bool Matrix::is_square() const noexcept
{
    return m_rows == m_cols;
}


bool Matrix::equal_size(const Matrix &second) const noexcept
{
    return (m_rows == second.m_rows) && (m_cols == second.m_cols);
}


void Matrix::set_zeros() noexcept
{
    print_log(LOG_INFO, "make zero matrix\n");

    if(is_empty()) {
        print_log(LOG_WARN, "matrix is empty\n");
        return;
    }

    memset(m_data, 0, m_rows * m_cols * sizeof(double));
}


void Matrix::set_identity()
{
    print_log(LOG_INFO, "make identity matrix\n");

    if(!is_square()) {
        print_log(LOG_ERR, "matrix not square\n");
        throw SIZE_ERR;
    }

    set_zeros();

    for(size_t idx = 0; idx < m_rows * m_cols; idx += m_cols + 1) {
        m_data[idx] = 1.0;
    }
}


Matrix& Matrix::operator=(const Matrix &mat)
{
    print_log(LOG_INFO, "matrix assignment\n");

    if(m_data == mat.m_data) {
        print_log(LOG_WARN, "matrix self assignment\n");
        return *this;
    }

    resize(mat.m_rows, mat.m_cols);

    memcpy(m_data, mat.m_data, m_rows * m_cols * sizeof(double));

    return *this;
}


Matrix& Matrix::operator=(Matrix &&mat)
{
    print_log(LOG_INFO, "matrix move assignment\n");

    if(m_data == mat.m_data) {
        print_log(LOG_WARN, "matrix self assignment\n");
        return *this;
    }

    free();

    m_rows = mat.m_rows;
    m_cols = mat.m_cols;
    m_data = mat.m_data;

    mat.m_rows = 0;
    mat.m_cols = 0;
    mat.m_data = nullptr;

    return *this;
}


Matrix Matrix::operator-() const
{
    print_log(LOG_INFO, "matrix opposite\n");

    Matrix res(m_rows, m_cols);

    for(size_t idx = 0; idx < m_rows * m_cols; idx++) {
        res.m_data[idx] = -m_data[idx];
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

    Matrix res(m_rows, m_cols);

    for(size_t idx = 0; idx < m_rows * m_cols; idx++) {
        res.m_data[idx] = m_data[idx] + second.m_data[idx];
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

    for(size_t idx = 0; idx < m_rows * m_cols; idx++) {
        m_data[idx] += mat.m_data[idx];
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

    Matrix res(m_rows, m_cols);

    for(size_t idx = 0; idx < m_rows * m_cols; idx++) {
        res.m_data[idx] = m_data[idx] - second.m_data[idx];
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

    for(size_t idx = 0; idx < m_rows * m_cols; idx++) {
        m_data[idx] -= mat.m_data[idx];
    }

    return *this;
}


Matrix Matrix::operator*(const double num) const noexcept
{
    print_log(LOG_INFO, "matrix multiplication by number\n");

    Matrix res(m_rows, m_cols);

    for(size_t idx = 0; idx < m_rows * m_cols; idx++) {
        res.m_data[idx] = m_data[idx] * num;
    }

    return res;
}


Matrix& Matrix::operator*=(const double num) noexcept
{
    print_log(LOG_INFO, "matrix multiplication by number assignment\n");

    for(size_t idx = 0; idx < m_rows * m_cols; idx++) {
        m_data[idx] *= num;
    }

    return *this;
}


Matrix Matrix::operator*(const Matrix &second) const
{
    print_log(LOG_INFO, "matrix multiplication\n");

    if(m_cols != second.m_rows) {
        print_log(LOG_ERR, "first.cols and second.rows not equals\n");
        throw SIZE_ERR;
    }

    Matrix res(m_rows, second.m_cols);

    for(size_t row = 0; row < m_rows; row++) {
        for(size_t col = 0; col < second.m_cols; col++) {
            res.m_data[row * res.m_cols + col] = 0.0;
            for(size_t idx = 0; idx < m_cols; idx++) {
                res.m_data[row * res.m_cols + col] += m_data[row * m_cols + idx] * second.m_data[idx * second.m_cols + col];
            }
        }
    }

    return res;
}


Matrix& Matrix::operator*=(const Matrix &second)
{
    print_log(LOG_INFO, "matrix multiplication assignment\n");

    if(m_cols != second.m_rows) {
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

    Matrix res(m_rows, m_cols);
    res.set_identity();

    for(unsigned int cnt = 0; cnt < pow; cnt++) {
        res *= (*this);
    }

    return res;
}


Matrix Matrix::exp(const unsigned int iterations) const
{
    print_log(LOG_INFO, "matrix exponent\n");

    if(!is_square()) {
        print_log(LOG_ERR, "matrix not square\n");
        throw SIZE_ERR;
    }

    Matrix res(m_rows, m_cols);
    res.set_identity();

    Matrix term(m_rows, m_cols);
    term.set_identity();

    for(unsigned int num = 1; num < iterations; num++) {
        term *= (*this);
        term *= (1 / (double)num);
        res += term;
    }

    return res;
}


Matrix Matrix::transp() const
{
    print_log(LOG_INFO, "matrix transposition\n");

    Matrix res(m_cols, m_rows);

    for(size_t row = 0; row < m_rows; row++) {
        for(size_t col = 0; col < m_cols; col++) {
            res.m_data[col * m_rows + row] = m_data[row * m_cols + col];
        }
    }

    return res;
}


size_t Matrix::find_non_zero_in_col(const size_t idx_start) const noexcept
{
    for(size_t row = idx_start + 1; row < m_rows; row++) {
        if(fabs(m_data[row * m_cols + idx_start]) >= 0.00001) {
            return row;
        }
    }

    return 0;
}


void Matrix::swap_rows(const size_t first, const size_t second) noexcept
{
    for(size_t idx = 0; idx < m_cols; idx++) {
        double tmp = m_data[first * m_cols + idx];
        m_data[first * m_cols + idx] = m_data[second * m_cols + idx];
        m_data[second * m_cols + idx] = tmp;
    }
}


void Matrix::sub_row(const size_t row, const size_t row_base, const double ratio) noexcept
{
    for(size_t idx = 0; idx < m_cols; idx++) {
        m_data[row * m_cols + idx] -= ratio * m_data[row_base * m_cols + idx];
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
    for(size_t row_base = 0; row_base < m_rows - 1; row_base++) {
        if(fabs(tmp.m_data[row_base * (m_cols + 1)]) < 0.00001) {
            size_t idx_non_zero = tmp.find_non_zero_in_col(row_base);
            if(idx_non_zero == 0) {
                return 0.0;
            }

            tmp.swap_rows(row_base, idx_non_zero);
            det *= -1.0;
        }

        for(size_t row = row_base + 1; row < m_rows; row++) {
            double mult = tmp.m_data[row * m_cols + row_base] / tmp.m_data[row_base * (m_cols + 1)];
            tmp.sub_row(row, row_base, mult);
        }
    }

    for(size_t idx = 0; idx < m_rows; idx++) {
        det *= tmp.m_data[idx * m_cols + idx];
    }

    return det;
}
