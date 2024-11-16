#include <stdbool.h>
#include <stddef.h>

typedef enum {
    MAT_OK,
    MAT_MEMORY_ERR,
    MAT_UNINITIALIZED_ERR,
    MAT_DIMENSION_ERR,
    MAT_INDEX_OUT_OF_BOUNDS_ERR,
    MAT_INTERNAL_ERR,
} MatrixStatus;

typedef enum {
    WARNING,
    INFO,
    ERROR,
} MatrixLogLevel;

typedef struct {
    size_t rows;
    size_t cols;
    double* data;
} Matrix;

bool matrix_is_empty(const Matrix M);

bool matrix_is_square(const Matrix M);

bool matrix_equal_size(const Matrix A, const Matrix B);

void matrix_set(Matrix* M, size_t row, size_t col, double number);

Matrix matrix_allocate(const size_t rows, const size_t cols);

void matrix_free(Matrix* M);

void matrix_log(MatrixLogLevel level, const char* location, const char* msg);

void matrix_print(const Matrix M);

MatrixStatus matrix_copy(const Matrix src, Matrix* dest);

MatrixStatus matrix_sum(const Matrix A, const Matrix B, Matrix* result);

MatrixStatus matrix_subtract(const Matrix A, const Matrix B, Matrix* result);

MatrixStatus matrix_multiply(const Matrix A, const Matrix B, Matrix* result);

MatrixStatus matrix_scalar_multiply(Matrix M, double scalar, Matrix* result);

MatrixStatus matrix_transpose(const Matrix M, Matrix* result);

double matrix_determinant(const Matrix M);

MatrixStatus matrix_power(const Matrix M, int power, Matrix* result);

MatrixStatus matrix_exponent(const Matrix A, unsigned int num, Matrix* result);

unsigned long long factorial(unsigned int n);