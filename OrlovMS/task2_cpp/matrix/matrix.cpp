#include "matrix.h"
#include <cstdarg>
#include <cstdio>


Matrix::LogLevel Matrix::current_level = Matrix::LOG_NONE;


void Matrix::set_log_level(LogLevel level)
{
    current_level = level;
}


void Matrix::print_log(LogLevel level, const char *format, ...)
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
        return;
    }

    this->data = new double[rows * cols];
    if(this->data == NULL) {
        this->rows = 0;
        this->cols = 0;
        print_log(LOG_ERR, "matrix allocation error\n");
        return;
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
    alloc(rows, cols);
}


Matrix::Matrix(const Matrix &mat) : rows(mat.rows), cols(mat.cols), data(NULL)
{
    alloc(mat.rows, mat.cols);

    print_log(LOG_INFO, "copy matrix data\n");

    memcpy(this->data, mat.data, this->rows * this->cols * sizeof(double));
}


Matrix::~Matrix()
{
    free();
}


size_t Matrix::get_rows()
{
    return this->rows;
}


size_t Matrix::get_cols()
{
    return this->cols;
}


void Matrix::set(const size_t row, const size_t col, const double num)
{
    if(row >= this->rows || col >= this->cols) {
        print_log(LOG_ERR, "index out of bound\n");
        return;
    }

    this->data[row * this->cols + col] = num;
}


double Matrix::get(const size_t row, const size_t col)
{
    if(row >= this->rows || col >= this->cols) {
        print_log(LOG_ERR, "index out of bound\n");
        return 0.0;
    }

    return this->data[row * this->cols + col];
}


bool Matrix::is_empty()
{
    return this->data == NULL;
}


bool Matrix::is_square()
{
    return this->rows == this->cols;
}


void Matrix::set_zeros()
{
    print_log(LOG_INFO, "make zero matrix\n");

    if(is_empty()) {
        print_log(LOG_WARN, "matrix is empty\n");
        return;
    }

    memset(this->data, 0, this->rows * this->cols * sizeof(double));
}


void Matrix::set_identity()
{
    print_log(LOG_INFO, "make identity matrix\n");

    if(!is_square()) {
        print_log(LOG_ERR, "matrix not square\n");
        return;
    }

    set_zeros();

    for(size_t idx = 0; idx < this->rows * this->cols; idx += this->cols + 1) {
        this->data[idx] = 1.0;
    }
}


std::ostream& operator<<(std::ostream& stream, Matrix &mat)
{
    if(mat.is_empty()) {
        stream << "matrix is empty" << std::endl;
        return stream;
    }

    size_t rows = mat.get_rows();
    size_t cols = mat.get_cols();

    stream << "matrix size: " << rows << "x" << cols << std::endl;
    for(size_t row = 0; row < rows; row++) {
        stream << "[ ";
        for(size_t col = 0; col < cols - 1; col++) {
            stream << mat.get(row, col) << ", ";
        }
        stream << mat.get(row, cols - 1) << " ]" << std::endl;
    }

    return stream;
}
