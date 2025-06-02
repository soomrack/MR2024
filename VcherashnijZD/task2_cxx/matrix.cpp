#include "matrix.h"
#include <cstdarg>
#include <cstdio>
#include <cmath>

Matrix::LogLevel Matrix::current_log_level = Matrix::LOG_NONE;

void Matrix::set_log_level(LogLevel level) noexcept {
    current_log_level = level;
}

void Matrix::print_log(LogLevel level, const char *format, ...) const noexcept {
#ifdef MATRIX_LOG_ENABLE
    if(level <= current_log_level) {
        switch(level) {
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
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
#endif
}

void Matrix::alloc(const size_t row_num, const size_t col_num) {
    print_log(LOG_INFO, "allocate matrix with size: %lux%lu\n", row_num, col_num);

    if(row_num == 0 || col_num == 0) {
        print_log(LOG_WARN, "empty matrix allocation\n");
        return;
    }

    if(__SIZE_MAX__ / row_num / col_num / sizeof(double) == 0) {
        row_count = 0;
        col_count = 0;
        print_log(LOG_ERR, "matrix size too big\n");
        throw MatrixException(MatrixException::PARAMS_ERR);
    }

    data_array = new double[row_num * col_num];
    row_count = row_num;
    col_count = col_num;
}

void Matrix::free() {
    print_log(LOG_INFO, "free matrix\n");

    row_count = 0;
    col_count = 0;
    delete[] data_array;
}

Matrix::Matrix() : row_count(0), col_count(0), data_array(nullptr) {
    print_log(LOG_INFO, "create empty matrix\n");
}

Matrix::Matrix(const size_t row_num, const size_t col_num) : row_count(row_num), col_count(col_num), data_array(nullptr) {
    print_log(LOG_INFO, "create matrix with size\n");

    alloc(row_num, col_num);
}

Matrix::Matrix(const Matrix &source_matrix) : row_count(source_matrix.row_count), col_count(source_matrix.col_count), data_array(nullptr) {
    print_log(LOG_INFO, "create matrix copy\n");

    alloc(source_matrix.row_count, source_matrix.col_count);

    print_log(LOG_INFO, "copy data\n");

    memcpy(data_array, source_matrix.data_array, row_count * col_count * sizeof(double));
}

Matrix::Matrix(Matrix &&source_matrix) : row_count(source_matrix.row_count), col_count(source_matrix.col_count), data_array(source_matrix.data_array) {
    print_log(LOG_INFO, "move matrix\n");

    source_matrix.row_count = 0;
    source_matrix.col_count = 0;
    source_matrix.data_array = nullptr;
}

Matrix::~Matrix() {
    print_log(LOG_INFO, "matrix destructor\n");

    free();
}

void Matrix::resize(const size_t row_num, const size_t col_num) {
    print_log(LOG_INFO, "resize matrix\n");

    if(row_count * col_count == row_num * col_num) {
        print_log(LOG_INFO, "data size equals, reallocation not need\n");
        row_count = row_num;
        col_count = col_num;
        return;
    }

    free();
    alloc(row_num, col_num);
}

void Matrix::print() const noexcept {
    if(is_empty()) {
        printf("matrix is empty\n");
        return;
    }

    printf("Matrix size: %lu%lu\n", row_count, col_count);
    for(size_t row = 0; row < (row_count * col_count); row += col_count) {
        printf("[ ");
        for(size_t idx = 0; idx < col_count - 1; idx++) {
            printf("%8.3f, ", data_array[row + idx]);
        }
        printf("%8.3f ]\n", data_array[row + col_count - 1]);
    }
}

size_t Matrix::get_rows() const noexcept {
    return row_count;
}

size_t Matrix::get_cols() const noexcept {
    return col_count;
}

void Matrix::set(const size_t row_index, const size_t col_index, const double number) {
    if(row_index >= row_count || col_index >= col_count) {
        print_log(LOG_ERR, "index out of bound\n");
        throw MatrixException(MatrixException::PARAMS_ERR);
    }

    data_array[row_index * col_count + col_index] = number;
}

double Matrix::get(const size_t row_index, const size_t col_index) const {
    if(row_index >= row_count || col_index >= col_count) {
        print_log(LOG_ERR, "index out of bound\n");
        throw MatrixException(MatrixException::PARAMS_ERR);
    }

    return data_array[row_index * col_count + col_index];
}

bool Matrix::is_empty() const noexcept {
    return data_array == nullptr;
}

bool Matrix::is_square() const noexcept {
    return row_count == col_count;
}

bool Matrix::equal_size(const Matrix &second_matrix) const noexcept {
    return (row_count == second_matrix.row_count) && (col_count == second_matrix.col_count);
}

void Matrix::set_zeros() noexcept {
    print_log(LOG_INFO, "make zero matrix\n");

    if(is_empty()) {
        print_log(LOG_WARN, "matrix is empty\n");
        return;
    }

    memset(data_array, 0, row_count * col_count * sizeof(double));
}

void Matrix::set_identity() {
    print_log(LOG_INFO, "make identity matrix\n");

    if(!is_square()) {
        print_log(LOG_ERR, "matrix not square\n");
        throw MatrixException(MatrixException::SIZE_ERR);
    }

    set_zeros();

    for(size_t idx = 0; idx < row_count * col_count; idx += col_count + 1) {
        data_array[idx] = 1.0;
    }
}

Matrix& Matrix::operator=(const Matrix &source_matrix) {
    print_log(LOG_INFO, "matrix assignment\n");

    if(data_array == source_matrix.data_array) {
        print_log(LOG_WARN, "matrix self assignment\n");
        return *this;
    }

    resize(source_matrix.row_count, source_matrix.col_count);

    memcpy(data_array, source_matrix.data_array, row_count * col_count * sizeof(double));

    return *this;
}

Matrix& Matrix::operator=(Matrix &&source_matrix) {
    print_log(LOG_INFO, "matrix move assignment\n");

    if(data_array == source_matrix.data_array) {
        print_log(LOG_WARN, "matrix self assignment\n");
        return *this;
    }

    free();

    row_count = source_matrix.row_count;
    col_count = source_matrix.col_count;
    data_array = source_matrix.data_array;

    source_matrix.row_count = 0;
    source_matrix.col_count = 0;
    source_matrix.data_array = nullptr;

    return *this;
}

Matrix Matrix::operator-() const {
    print_log(LOG_INFO, "matrix opposite\n");

    Matrix result(row_count, col_count);

    for(size_t idx = 0; idx < row_count * col_count; idx++) {
        result.data_array[idx] = -data_array[idx];
    }

    return result;
}

Matrix Matrix::operator+(const Matrix &second_matrix) const {
    print_log(LOG_INFO, "matrix sum\n");

    if(!equal_size(second_matrix)) {
        print_log(LOG_ERR, "matrix size not equals\n");
        throw MatrixException(MatrixException::SIZE_ERR);
    }

    Matrix result(row_count, col_count);

    for(size_t idx = 0; idx < row_count * col_count; idx++) {
        result.data_array[idx] = data_array[idx] + second_matrix.data_array[idx];
    }

    return result;
}

Matrix& Matrix::operator+=(const Matrix &source_matrix) {
    print_log(LOG_INFO, "matrix sum assignment\n");

    if(!equal_size(source_matrix)) {
        print_log(LOG_ERR, "matrix size not equals\n");
        throw MatrixException(MatrixException::SIZE_ERR);
    }

    for(size_t idx = 0; idx < row_count * col_count; idx++) {
        data_array[idx] += source_matrix.data_array[idx];
    }

    return *this;
}

Matrix Matrix::operator-(const Matrix &second_matrix) const {
    print_log(LOG_INFO, "matrix subtraction\n");

    if(!equal_size(second_matrix)) {
        print_log(LOG_ERR, "matrix size not equals\n");
        throw MatrixException(MatrixException::SIZE_ERR);
    }

    Matrix result(row_count, col_count);

    for(size_t idx = 0; idx < row_count * col_count; idx++) {
        result.data_array[idx] = data_array[idx] - second_matrix.data_array[idx];
    }

    return result;
}

Matrix& Matrix::operator-=(const Matrix &source_matrix) {
    print_log(LOG_INFO, "matrix subtraction assignment\n");

    if(!equal_size(source_matrix)) {
        print_log(LOG_ERR, "matrix size not equals\n");
        throw MatrixException(MatrixException::SIZE_ERR);
    }

    for(size_t idx = 0; idx < row_count * col_count; idx++) {
        data_array[idx] -= source_matrix.data_array[idx];
    }

    return *this;
}

Matrix Matrix::operator*(const double number) const noexcept {
    print_log(LOG_INFO, "matrix multiplication by number\n");

    Matrix result(row_count, col_count);

    for(size_t idx = 0; idx < row_count * col_count; idx++) {
        result.data_array[idx] = data_array[idx] * number;
    }

    return result;
}

Matrix& Matrix::operator*=(const double number) noexcept {
    print_log(LOG_INFO, "matrix multiplication by number assignment\n");

    for(size_t idx = 0; idx < row_count * col_count; idx++) {
        data_array[idx] *= number;
    }

    return *this;
}

Matrix Matrix::operator*(const Matrix &second_matrix) const {
    print_log(LOG_INFO, "matrix multiplication\n");

    if(col_count != second_matrix.row_count) {
        print_log(LOG_ERR, "first.cols and second.rows not equals\n");
        throw MatrixException(MatrixException::SIZE_ERR);
    }

    Matrix result(row_count, second_matrix.col_count);

    for(size_t row = 0; row < row_count; row++) {
        for(size_t col = 0; col < second_matrix.col_count; col++) {
            result.data_array[row * result.col_count + col] = 0.0;
            for(size_t idx = 0; idx < col_count; idx++) {
                result.data_array[row * result.col_count + col] += data_array[row * col_count + idx] * second_matrix.data_array[idx * second_matrix.col_count + col];
            }
        }
    }

    return result;
}

Matrix& Matrix::operator*=(const Matrix &second_matrix) {
    print_log(LOG_INFO, "matrix multiplication assignment\n");

    if(col_count != second_matrix.row_count) {
        print_log(LOG_ERR, "first.cols and second.rows not equals\n");
        throw MatrixException(MatrixException::SIZE_ERR);
    }

    *this = (*this) * second_matrix;

    return *this;
}

Matrix Matrix::pow(const unsigned int power) const {
    print_log(LOG_INFO, "matrix power\n");

    if(!is_square()) {
        print_log(LOG_ERR, "matrix not square\n");
        throw MatrixException(MatrixException::SIZE_ERR);
    }

    Matrix result(row_count, col_count);
    result.set_identity();

    for(unsigned int cnt = 0; cnt < power; cnt++) {
        result *= (*this);
    }

    return result;
}

Matrix Matrix::exp(const unsigned int iterations) const {
    print_log(LOG_INFO, "matrix exponent\n");

    if(!is_square()) {
        print_log(LOG_ERR, "matrix not square\n");
        throw MatrixException(MatrixException::SIZE_ERR);
    }

    Matrix result(row_count, col_count);
    result.set_identity();

    Matrix term(row_count, col_count);
    term.set_identity();

    for(unsigned int num = 1; num < iterations; num++) {
        term *= (*this);
        term *= (1 / (double)num);
        result += term;
    }

    return result;
}

Matrix Matrix::transp() const {
    print_log(LOG_INFO, "matrix transposition\n");

    Matrix result(col_count, row_count);

    for(size_t row = 0; row < row_count; row++) {
        for(size_t col = 0; col < col_count; col++) {
            result.data_array[col * row_count + row] = data_array[row * col_count + col];
        }
    }

    return result;
}

size_t Matrix::find_non_zero_in_col(const size_t start_index) const noexcept {
    for(size_t row = start_index + 1; row < row_count; row++) {
        if(fabs(data_array[row * col_count + start_index]) >= 0.00001) {
            return row;
        }
    }

    return 0;
}

void Matrix::swap_rows(const size_t first_row, const size_t second_row) noexcept {
    for(size_t idx = 0; idx < col_count; idx++) {
        double tmp = data_array[first_row * col_count + idx];
        data_array[first_row * col_count + idx] = data_array[second_row * col_count + idx];
        data_array[second_row * col_count + idx] = tmp;
    }
}

void Matrix::sub_row(const size_t row_index, const size_t base_row_index, const double ratio) noexcept {
    for(size_t idx = 0; idx < col_count; idx++) {
        data_array[row_index * col_count + idx] -= ratio * data_array[base_row_index * col_count + idx];
    }
}

double Matrix::det() const {
    print_log(LOG_INFO, "matrix determinant\n");

    if(!is_square()) {
        print_log(LOG_ERR, "matrix not square\n");
        throw MatrixException(MatrixException::SIZE_ERR);
    }

    Matrix tmp(*this);

    double det = 1.0;
    for(size_t base_row = 0; base_row < row_count - 1; base_row++) {
        if(fabs(tmp.data_array[base_row * (col_count + 1)]) < 0.00001) {
            size_t non_zero_index = tmp.find_non_zero_in_col(base_row);
            if(non_zero_index == 0) {
                return 0.0;
            }

            tmp.swap_rows(base_row, non_zero_index);
            det *= -1.0;
        }

        for(size_t row = base_row + 1; row < row_count; row++) {
            double mult = tmp.data_array[row * col_count + base_row] / tmp.data_array[base_row * (col_count + 1)];
            tmp.sub_row(row, base_row, mult);
        }
    }

    for(size_t idx = 0; idx < row_count; idx++) {
        det *= tmp.data_array[idx * col_count + idx];
    }

    return det;
}

