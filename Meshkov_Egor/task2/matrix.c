#include "matrix.h"
#include <math.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>


LogLevel CURRENT_LEVEL = LOG_NONE;


Matrix EMPTY = {.rows = 0, .cols = 0, .ptr = NULL};


// Function to set log level
void matrix_set_log_level(LogLevel level) {
    CURRENT_LEVEL = level;
}

 
// Function for print message about exceptions
static void matrix_print_log(LogLevel level, const char *format, ...) {
#ifdef MATRIX_LOG_ENABLE
    if(level <= CURRENT_LEVEL) {
        switch (level) {
            case LOG_ERROR:
                printf("\033[0;31mERROR: \033[0m");
                break;
            case LOG_WARNING:
                printf("\033[1;33mWARNING: \033[0m");
                break;
            case LOG_NOTE: 
                printf("NOTE: ");
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


// Function to allocate memory for matrix
MatrixExceptions matrix_alloc(Matrix *M, const size_t rows, const size_t cols) {
    if(M == NULL) {
        matrix_print_log(LOG_ERROR, "matrix pointer must not be NULL\n");
        return MAT_NULL_POINTER_ERROR;
    }

    if(M->ptr != NULL) {
        matrix_print_log(LOG_ERROR, "an initialized matrix is used\n");
        matrix_print_log(LOG_NOTE, "please, declare matrix like this: \"matrix M = EMPTY\";\n");
        return MAT_NULL_POINTER_ERROR;
    }

    if(rows == 0 || cols == 0) {
        *M = EMPTY;
        matrix_print_log(LOG_WARNING, "empty matrix allocation\n");
        return MAT_OK;
    }

    size_t matrix_mem_size = rows * cols * sizeof(double);
    
    if(__SIZE_MAX__ / rows / cols / sizeof(double) == 0) {
        matrix_print_log(LOG_ERROR, "matrix size too big for allocate\n");
        return MAT_OVERFLOW_ERROR;
    }
    
    M->rows = rows;
    M->cols = cols;
    M->ptr = (double *)malloc(matrix_mem_size);

    if(M->ptr == NULL) {
        M->rows = 0;
        M->cols = 0;
        matrix_print_log(LOG_ERROR, "memory allocation error\n");
        return MAT_ALLOCATION_ERROR;
    }
    
    return MAT_OK;
}


// Auxiliary function for function matrix_print
static int matrix_calculate_max_width(const Matrix M, const int accuracy) {
    int max_width = 0;

    for(size_t idx = 0; idx < M.rows * M.cols; idx++) {
        int width = snprintf(NULL, 0, "%.*f", accuracy, M.ptr[idx]);
        if (width > max_width) {
            max_width = width;
        }
    }

    return max_width;
}


// Function for print matrix with choose accuracy of numbers in matrix
MatrixExceptions matrix_print(const Matrix M, const int accuracy) {
    if(M.ptr == NULL) {
        matrix_print_log(LOG_ERROR, "matrix must not be empty\n");
        return MAT_EMPTY_MATRIX_ERROR;
    }

    int width = matrix_calculate_max_width(M, accuracy);
    double number;

    for (size_t idx = 0; idx < M.rows * M.cols; ++idx) {
        if (idx % (int)M.cols == 0) {
            if (idx != 0) {
                printf("|\n");
            }
            printf("| ");
        }
        number = round(M.ptr[idx] * pow(10, accuracy)) / pow(10, accuracy);
        printf("%*.*f ", width, accuracy, number);
    }
    printf("|\n\n"); // Final new line for better formatting

    return MAT_OK;
}


// Auxiliary function for function matrix_create_random
static double matrix_create_random_number(const double min, const double max, const unsigned int accuracy) {
    int sign = (rand() % 2) * 2 - 1;
    double range = max - min;
    double number;

    if(sign == 1) {
        number = min + ((double)rand() / (double)RAND_MAX) * range;
    } else {
        number = max - ((double)rand() / (double)RAND_MAX) * range;
    }

    return round(number * pow(10, accuracy)) / pow(10, accuracy);
}


// Function to initialize matrix and fills it with zeros
MatrixExceptions matrix_create_zero(Matrix *M, const size_t rows, const size_t cols) {  
    MatrixExceptions status = matrix_alloc(M, rows, cols);
    
    if(status != MAT_OK) {
        return status;
    }

    memset(M->ptr, 0, M->rows * M->cols * sizeof(double));
    
    return MAT_OK;
}


// Function to initialize the matrix and fills its diagonal with ones
MatrixExceptions matrix_create_unit(Matrix *M, const size_t rows, const size_t cols) {
    if(rows != cols) {
        matrix_print_log(LOG_ERROR, "matrix must be square\n");
        return MAT_SIZE_ERROR;
    }

    MatrixExceptions status = matrix_alloc(M, rows, cols);
    
    if(status != MAT_OK) {
        return status;
    }

    memset(M->ptr, 0, M->rows * M->cols * sizeof(double));

    for(size_t row = 0; row < M->rows; ++row) {
        M->ptr[row * M->rows + row] = 1.0;
    }
    
    return MAT_OK;
}


// Function to initialize the matrix and fill its diagonal with random double numbers
MatrixExceptions matrix_create_random(Matrix *M, const size_t rows, const size_t cols, const double min, const double max, const unsigned int accuracy) {
    MatrixExceptions status = matrix_alloc(M, rows, cols);

    if(status != MAT_OK) {
        return status;
    }

    if(max < min) {
        matrix_print_log(LOG_ERROR, "range of random nubmers is incorrect\n");
        return MAT_RANGE_RANDOM_NUMBERS_ERR;
    }

    for(size_t idx = 0; idx < M->rows * M->cols; ++idx) {
        M->ptr[idx] = matrix_create_random_number(min, max, accuracy);
    }
    
    return MAT_OK;
}


// Function to free memory of matrix
void matrix_free(Matrix *M) {
    if(M == NULL) {
        matrix_print_log(LOG_NOTE, "matrix pointer is NULL, using function is pointless\n");
        return;
    }

    free(M->ptr);
    *M = EMPTY;
}


// Function for copy matrix
MatrixExceptions matrix_copy(const Matrix dest, const Matrix src) {
    if(src.ptr == NULL || dest.ptr == NULL) {
        matrix_print_log(LOG_ERROR, "matrix must not be empty\n");
        return MAT_EMPTY_MATRIX_ERROR;
    }

    if(((dest.rows == src.rows) && (dest.cols == src.cols)) == 0) {
        matrix_print_log(LOG_ERROR, "matrix size must be equal\n");
        return MAT_SIZE_ERROR;
    }

    memcpy(dest.ptr, src.ptr, src.rows * src.cols * sizeof(double));

    return MAT_OK;
}


// Function to addition matrix: C = A + B
MatrixExceptions matrix_add(const Matrix C, const Matrix A, const Matrix B) {
    if(A.ptr == NULL || B.ptr == NULL || C.ptr == NULL) {
        matrix_print_log(LOG_ERROR, "matrix must not be empty\n");
        return MAT_EMPTY_MATRIX_ERROR;
    }

    if(((A.rows == B.rows) && (A.cols == B.cols) && (C.rows == A.rows) && (C.cols == A.cols)) == 0) {
        matrix_print_log(LOG_ERROR, "matrix size must be equal\n");
        return MAT_SIZE_ERROR;
    }

    for(size_t idx = 0; idx < A.rows * A.cols; ++idx) {
        C.ptr[idx] = A.ptr[idx] + B.ptr[idx];
    }

    return MAT_OK;
}


// Function to substraction matrix: C = A - B
MatrixExceptions matrix_sub(const Matrix C, const Matrix A, const Matrix B) {
    if(A.ptr == NULL || B.ptr == NULL || C.ptr == NULL) {
        matrix_print_log(LOG_ERROR, "matrix must not be empty\n");
        return MAT_EMPTY_MATRIX_ERROR;
    }

    if(((A.rows == B.rows) && (A.cols == B.cols) && (C.rows == A.rows) && (C.cols == A.cols)) == 0) {
        matrix_print_log(LOG_ERROR, "matrix size must be equal\n");
        return MAT_SIZE_ERROR;
    }

    for(size_t idx = 0; idx < A.rows * A.cols; ++idx) {
        C.ptr[idx] = A.ptr[idx] - B.ptr[idx];
    }

    return MAT_OK;
}


// Function for multiplication matrix: C = A * B
MatrixExceptions matrix_multi(const Matrix C, const Matrix A, const Matrix B) {
    if((A.ptr == NULL) || (B.ptr == NULL) || (C.ptr == NULL)) {
        matrix_print_log(LOG_ERROR, "matrix must not be empty\n");
        return MAT_EMPTY_MATRIX_ERROR;
    }
 
    if((A.cols != B.rows) || (C.rows != A.rows) || (C.cols != B.cols)) {
        matrix_print_log(LOG_ERROR, "matrix size are incorrect for this operation\n");
        return MAT_SIZE_ERROR;
    } 
    
    Matrix tmp = EMPTY; // Auxilary matrix
    MatrixExceptions status = matrix_create_zero(&tmp, A.rows, B.cols);
    if(status != MAT_OK) {
        return status;
    }

    for (size_t rowA = 0; rowA < A.rows; ++rowA) {
        for (size_t colB = 0; colB < B.cols; ++colB) {
            for (size_t innerDim = 0; innerDim < A.cols; ++innerDim) {
                tmp.ptr[rowA * tmp.cols + colB] += A.ptr[rowA * A.cols + innerDim] * B.ptr[innerDim * B.cols + colB];
            }
        }
    }
    
    matrix_copy(C, tmp);
    matrix_free(&tmp);
    
    return MAT_OK;
}


// Function to multiplication matrix by number: res_matrix = src_matrix * b, "b" is a scalar number
MatrixExceptions matrix_multiply_by_scalar(const Matrix res_matrix, const Matrix src_matrix, const double scalar) {
    if(src_matrix.ptr == NULL || res_matrix.ptr == NULL) {
        matrix_print_log(LOG_ERROR, "matrix must not be empty\n");
        return MAT_EMPTY_MATRIX_ERROR;
    }

    if((src_matrix.rows != res_matrix.rows) || (src_matrix.cols != res_matrix.cols)) {
        matrix_print_log(LOG_ERROR, "matrix size are incorrect for this operation\n");
        return MAT_SIZE_ERROR;
    }

    for (size_t idx = 0; idx < src_matrix.rows * src_matrix.cols; ++idx) {
        res_matrix.ptr[idx] = src_matrix.ptr[idx] * scalar;
    }

    return MAT_OK;
}


// Function to transpoze matrix: result_matrix = source_matrix^(T)
MatrixExceptions matrix_transpoze(const Matrix res_matrix, const Matrix src_matrix) {
    if(src_matrix.ptr == NULL) {
        matrix_print_log(LOG_ERROR, "matrix must not be empty\n");
        return MAT_EMPTY_MATRIX_ERROR;
    }

    if(res_matrix.rows != src_matrix.cols || res_matrix.cols != src_matrix.rows) {
        matrix_print_log(LOG_ERROR, "matrix size must be equal\n");
        return MAT_SIZE_ERROR;
    }

    for (size_t row = 0; row < src_matrix.rows; ++row) {
        for (size_t col = 0; col < src_matrix.cols; ++col) {
            res_matrix.ptr[col * res_matrix.cols + row] = src_matrix.ptr[row * src_matrix.cols + col];
        }
    }
    
    return MAT_OK;
}


// Auxilary function for functions: make_zero_down_triangular_matrix, transform_extend_matrix
static size_t matrix_find_max_element_row_in_column_below_diagonal(const Matrix M, const size_t current_row) {
    size_t row_max_element = current_row;

    for (size_t row = current_row; row < M.rows; row++) {
        if (fabs(M.ptr[row * M.cols + current_row]) > fabs(M.ptr[row_max_element * M.cols + current_row])) {
            row_max_element = row;
        }
    }

    return row_max_element; 
}


// Auxilary function to swap rows in matrix
static void matrix_swap_rows(const Matrix M, const size_t current_row, const size_t row_max_element) {
    if(current_row == row_max_element) {
        return;
    }

    for (size_t col = current_row; col < M.cols; col++) {
        double temp = M.ptr[row_max_element * M.cols + col];
        M.ptr[row_max_element * M.cols + col] = M.ptr[current_row * M.cols + col];
        M.ptr[current_row * M.cols + col] = temp;
    }
}


// Auxilary function to functions: make_zero_down_triangular_matrix, transform_extend_matrix
static void matrix_zeroing_elements_in_column_below_diagonal(const Matrix matrix, const size_t current_row) {
    for (size_t row = current_row + 1; row < matrix.rows; row++) {
        // The coefficient by which the strings are multiplied
        double factor = matrix.ptr[row * matrix.cols + current_row] / matrix.ptr[current_row * matrix.cols + current_row];

        for (size_t col = current_row; col < matrix.cols; col++) {
            if (current_row == col) {
                matrix.ptr[row * matrix.cols + col] = 0;
            } else {
                matrix.ptr[row * matrix.cols + col] -= factor * matrix.ptr[current_row * matrix.cols + col];
            }
        }
    }
}


// Auxilary function to calculate triangular matrix determinant excluding rows permutations
static double matrix_triangular_det(const Matrix M) {
    double det = 1;

    for (size_t row = 0; row < M.rows; row++) {
        det *= M.ptr[row * M.cols + row];
    }

    return det;
}


// Auxilary function, return sign of determinant or zero if determinant = 0
static int matrix_make_zero_down_triangular_matrix(const Matrix matrix) {
    size_t row_max_element = 0;
    int count_of_swap = 0;

    for (size_t current_row = 0; current_row < matrix.rows; current_row++) {
        row_max_element = matrix_find_max_element_row_in_column_below_diagonal(matrix, current_row);
     
        if(matrix.ptr[row_max_element * matrix.cols + current_row] == 0) {
            return 0;
        }

        matrix_swap_rows(matrix, current_row, row_max_element);

        if(current_row != row_max_element) {
            count_of_swap++;
        }

        matrix_zeroing_elements_in_column_below_diagonal(matrix, current_row);
    }

    return (count_of_swap % 2 != 0) ? -1 : 1;
}


// Function to calculate matrix determinant by gauss method
MatrixExceptions matrix_determinant_gauss_method(double *det, const Matrix matrix) {
    if(matrix.ptr == NULL) {
        matrix_print_log(LOG_ERROR, "matrix must not be empty\n");
        return MAT_EMPTY_MATRIX_ERROR;
    }

    if(matrix.rows != matrix.cols) {
        matrix_print_log(LOG_ERROR, "matrix must be square\n");
        return MAT_SIZE_ERROR;
    }

    Matrix TMPmatrix = EMPTY; // Auxilary matri
    MatrixExceptions status = matrix_alloc(&TMPmatrix, matrix.rows, matrix.cols);
    if(status != MAT_OK) {
        return status;
    }

    matrix_copy(TMPmatrix, matrix);  // Copying the matrix so as not o spoil the original one
    
    int sign_of_det = matrix_make_zero_down_triangular_matrix(TMPmatrix);
    if(sign_of_det == 0) {
        *det = 0;  // One of the diagonal elements is zero
        return MAT_OK;
    }

    *det = sign_of_det * matrix_triangular_det(TMPmatrix);

    return MAT_OK;
}


// Auxilary function to function reverse_matrix
static void matrix_create_extend(const Matrix matrix, const Matrix extend_matrix) {
    for(size_t row = 0; row < extend_matrix.rows; ++row) { 
        for(size_t col = 0; col < extend_matrix.cols; ++col) {
            if(col >= matrix.cols) {
                extend_matrix.ptr[row * extend_matrix.cols + col] = (row == col - matrix.cols) ? 1 : 0;
            } else {
                extend_matrix.ptr[row * extend_matrix.cols + col] = matrix.ptr[row * matrix.cols + col];
            }
        }
    }
}


// Auxilary function to function reverse_matrix
static void matrix_extract_inverse_matrix(const Matrix reverse_matrix, const Matrix extend_matrix) {
    for(size_t row = 0; row < reverse_matrix.rows; row++) {
        for (size_t col = 0; col < reverse_matrix.cols; col++) {
            reverse_matrix.ptr[row * reverse_matrix.cols + col] = extend_matrix.ptr[row * extend_matrix.cols + reverse_matrix.cols + col];
        }
    }
}


// Auxilary function to function reverse_matrix
static void matrix_zeroing_elements_in_column_above_diagonal(const Matrix matrix, const size_t current_row) {
    double factor = 0;
    for(size_t row = 0; row < current_row; row++) {
        factor = matrix.ptr[row * matrix.cols + current_row] / matrix.ptr[current_row * matrix.cols + current_row];
        for(size_t col = current_row; col < matrix.cols; col++) {
            matrix.ptr[row * matrix.cols + col] -= factor * matrix.ptr[current_row * matrix.cols + col];
        }
    }
}


// Auxilary function to function reverse_matrix
static void matrix_reduce_diagonal_elements_to_one(const Matrix extend_matrix, const size_t current_row) {
    double diagonal_element = extend_matrix.ptr[current_row * extend_matrix.cols + current_row];
    
    for (size_t col = 0; col < extend_matrix.cols; col++) {
        extend_matrix.ptr[current_row * extend_matrix.cols + col] /= diagonal_element;
    }
}


// Auxilary function to function reverse_matrix
static int matrix_transform_extend_matrix(const Matrix extend_matrix) {
    size_t row_max_element = 0;
    double error = pow(10, -9);

    for (size_t current_row = 0; current_row < extend_matrix.rows; current_row++) {
        row_max_element = matrix_find_max_element_row_in_column_below_diagonal(extend_matrix, current_row);

        if(fabs(extend_matrix.ptr[row_max_element * extend_matrix.cols + current_row]) < error) {
            return 0;
        }

        matrix_swap_rows(extend_matrix, current_row, row_max_element);
        
        matrix_reduce_diagonal_elements_to_one(extend_matrix, current_row);

        matrix_zeroing_elements_in_column_below_diagonal(extend_matrix, current_row);
        
        matrix_zeroing_elements_in_column_above_diagonal(extend_matrix, current_row);
    }

    return 1;
}
                                                                
                                                                
// Function to calculate reverse matrix
MatrixExceptions matrix_calculate_reverse(const Matrix reverse_matrix, const Matrix matrix) {
    if(reverse_matrix.ptr == NULL || matrix.ptr == NULL) {
        matrix_print_log(LOG_ERROR, "matrix must not be empty\n");
        return MAT_EMPTY_MATRIX_ERROR;
    }

    if(matrix.rows != matrix.cols) {
        matrix_print_log(LOG_ERROR, "matrix must be square\n");
        return MAT_SIZE_ERROR;
    }

    if((reverse_matrix.rows != matrix.rows) || (reverse_matrix.cols != matrix.cols)) {
        matrix_print_log(LOG_ERROR, "matrix size must be equal\n");
        return MAT_SIZE_ERROR;
    }

    Matrix extend_matrix = EMPTY;
    MatrixExceptions status = matrix_create_zero(&extend_matrix, matrix.rows, matrix.cols * 2);
    
    if(status != MAT_OK) {
        return status;
    }

    matrix_create_extend(matrix, extend_matrix);

    if(matrix_transform_extend_matrix(extend_matrix) == 0) {
        matrix_print_log(LOG_ERROR, "determinant of reverse matrix must not be zero\n");
        return MAT_ZERO_DETERMINANT_REVERSE_MATRIX_ERROR;
    }

    matrix_extract_inverse_matrix(reverse_matrix, extend_matrix);

    matrix_free(&extend_matrix);

    return MAT_OK;
} 

// Function to solve matrix equation A * X = B
MatrixExceptions matrix_solve_equation(const Matrix X, const Matrix A, const Matrix B) {
    if(A.ptr == NULL || B.ptr == NULL || X.ptr == NULL) {
        matrix_print_log(LOG_ERROR, "matrix must not be empty\n");
        return MAT_EMPTY_MATRIX_ERROR;
    }

    if(((A.cols == X.rows) && (A.rows == B.rows) && (B.cols == 1) && (X.cols == 1)) == 0) {
        matrix_print_log(LOG_ERROR, "matrix size are incorrect\n");
        return MAT_SIZE_ERROR;
    }

    if(X.rows < A.rows) {
        matrix_print_log(LOG_ERROR, "system should containt only independent equations\n");
        return MAT_SIZE_ERROR;
    } 

    if(X.rows > A.rows) {
        matrix_print_log(LOG_ERROR, "there are too many unknowns in system\n");
        return MAT_SIZE_ERROR;
    }

    Matrix reverse_matrixA = EMPTY;
    MatrixExceptions status = matrix_alloc(&reverse_matrixA, A.rows, A.cols);
     
    if(status != MAT_OK) {
        return status;
    }

    status = matrix_calculate_reverse(reverse_matrixA, A);
    if(status != MAT_OK) {
        return status;
    }
    
    status = matrix_multi(X, reverse_matrixA, B);
    if(status != MAT_OK) {
        return status;
    }

    return MAT_OK;
}


// Auxilary function to function matrix_exponent
static double matrix_find_max_element(const Matrix matrix) {
    double max_element = matrix.ptr[0];

    for(size_t idx = 1; idx < matrix.rows * matrix.cols; idx++) {
        max_element = (matrix.ptr[idx] > max_element) ? matrix.ptr[idx] : max_element;
    }
    
    return max_element;
}


// Auxilary function to function matrix_power
static int matrix_is_zero(const Matrix matrix) {
    for(size_t idx = 0; idx < matrix.rows * matrix.cols; idx++) {
        if(matrix.ptr[idx] != 0) return 0;
    }

    return 1;
}


// Auxilary function to function matrix_power
static int matrix_is_unit(const Matrix matrix) {
    for(size_t idx = 0; idx < matrix.rows * matrix.cols; idx++) {
        if(matrix.ptr[idx] != 1) return 0;
    }

    return 1;
}


// Function to power matrix by choose degree: result_matrix = source_matrix ^ degree
MatrixExceptions matrix_power(const Matrix matrix, const unsigned int degree) {
    if(matrix.ptr == NULL) {
        matrix_print_log(LOG_ERROR, "matrix must not be empty\n");
        return MAT_EMPTY_MATRIX_ERROR;
    }

    if(matrix_is_unit(matrix)) return MAT_OK;

    if(matrix_is_zero(matrix) && degree != 0) {
        return MAT_OK;
    }

    if(degree == 0) {
        for (size_t idx = 0; idx < matrix.rows * matrix.cols; idx++) {
            matrix.ptr[idx] = (idx % (matrix.cols + 1) == 0) ? 1 : 0;
        }
        return MAT_OK;
    }

    if(degree == 1) {
        return MAT_OK;
    }

    for(unsigned int k = 1; k < degree; k++) {
        matrix_multi(matrix, matrix, matrix);
    }

    return MAT_OK;
}


// Function to calculate exponent of matrix
MatrixExceptions matrix_exponent(Matrix *EXPmatrix, const Matrix matrix, const int accuracy) {
    if(EXPmatrix == NULL) {
        matrix_print_log(LOG_ERROR, "matrix pointer must not be NULL\n");
        return MAT_NULL_POINTER_ERROR;
    }

    if(matrix.ptr == NULL) {
        matrix_print_log(LOG_ERROR, "matrix must not be empty\n");
        return MAT_EMPTY_MATRIX_ERROR;
    }

    if(matrix.rows != matrix.cols) {
        matrix_print_log(LOG_ERROR, "matrix must be square\n");
        return MAT_SIZE_ERROR;
    }
    
    if(EXPmatrix->ptr != NULL) {
        matrix_print_log(LOG_WARNING, "matrix used initialized\n");
        matrix_print_log(LOG_NOTE, "please, declare matrix like this: \"Matrix M = EMPTY;\"\n");
        matrix_free(EXPmatrix);
    }
    
    MatrixExceptions status = matrix_create_unit(EXPmatrix, matrix.rows, matrix.cols);
    if(status != MAT_OK) {
        return status;
    }

    Matrix TMPmatrix = EMPTY; // Auxilary matrix
    status = matrix_alloc(&TMPmatrix, matrix.rows, matrix.cols);
    if(status != MAT_OK) {
        matrix_free(EXPmatrix);
        return status;
    }

    matrix_copy(TMPmatrix, matrix);
    matrix_add(*EXPmatrix, matrix, *EXPmatrix);
    
    double number = 1.0;
    double error = pow(10, -accuracy);

    for(unsigned int k = 2; ; ++k) {
        number *= 1.0 / k;
        matrix_multi(TMPmatrix, matrix, TMPmatrix);
        matrix_multiply_by_scalar(TMPmatrix, TMPmatrix, number);
        if(fabs(matrix_find_max_element(TMPmatrix)) < error) break;
        matrix_add(*EXPmatrix, TMPmatrix, *EXPmatrix);
        matrix_multiply_by_scalar(TMPmatrix, TMPmatrix, 1.0 / number);
    }

    matrix_free(&TMPmatrix);

    return MAT_OK;
}
