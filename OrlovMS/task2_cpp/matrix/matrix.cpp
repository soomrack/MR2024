#include "matrix.h"
#include <cstdarg>
#include <cstdio>


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


Matrix::Matrix() : rows(0), cols(0), data(NULL)
{
    print_log(LOG_INFO, "create empty matrix\n");
}


Matrix::Matrix(const size_t rows, const size_t cols) : rows(rows), cols(cols), data(NULL)
{
    print_log(LOG_INFO, "create matrix with size\n");

    alloc(rows, cols);
}


Matrix::Matrix(const Matrix &mat) : rows(mat.rows), cols(mat.cols), data(NULL)
{
    print_log(LOG_INFO, "create matrix copy\n");

    alloc(mat.rows, mat.cols);

    print_log(LOG_INFO, "copy data\n");

    memcpy(data, mat.data, rows * cols * sizeof(double));
}


Matrix::~Matrix()
{
    print_log(LOG_INFO, "matrix destructor\n");

    free();
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
    return data == NULL;
}


bool Matrix::is_square() const noexcept
{
    return rows == cols;
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
