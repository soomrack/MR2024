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

bool pointer_on_matrix_is_null(const Matrix* M);
bool matrix_is_empty(const Matrix M);
bool matrix_is_square(const Matrix M);
MatrixStatus matrix_zero(Matrix* M);
bool matrix_equal_size(const Matrix A, const Matrix B);
void matrix_set(Matrix* M, size_t row, size_t col, double number);
Matrix matrix_allocate(const size_t rows, const size_t cols);
void matrix_free(Matrix* M);
void matrix_log(MatrixLogLevel level, const char* location, const char* msg);
void matrix_print(const Matrix M);
MatrixStatus matrix_copy(Matrix* dest, const Matrix src);
MatrixStatus matrix_sum(Matrix* result, const Matrix A, const Matrix B);
MatrixStatus matrix_subtract(Matrix* result, const Matrix A, const Matrix B);
MatrixStatus matrix_multiply(Matrix* result, const Matrix A, const Matrix B);
MatrixStatus matrix_scalar_multiply(Matrix* result, Matrix M, double scalar);
MatrixStatus matrix_transpose(Matrix* result, const Matrix M);
double matrix_determinant(const Matrix M);
MatrixStatus matrix_power(Matrix* result, const Matrix M, int power);
MatrixStatus matrix_exponent(Matrix* result, const Matrix A, unsigned int num);
double factorial(unsigned int n);