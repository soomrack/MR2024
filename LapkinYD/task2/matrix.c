#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*Необходимые операции:
Сложение
Вычитание
Умножение
Транспонирование
Возведение в степень
Умножение на число
Определитель
Матричная экспонента
+Реализовать "обычные" функции*/


struct Matrix MATRIX_NULL = {0, 0, NULL};

const size_t exp_steps = 5;
// exp(A) = E + A + (A ^ 2) / 4 + .... <- there will be exp_accuracy of summarizing matrices


// Print exception and function name as the location where the exception was got
void exception_handler(MATRIX_EXCEPTION exc, const char* function_name)
{
    switch (exc)
    {
    case EXC_NULL:
        printf("Received exception: EXC_NULL; location: '%s'\n", function_name);
        break;
    case EXC_MEMORY:
        printf("Received exception: EXC_MEMORY; location: '%s'\n", function_name);
        break;
    case EXC_SIZE:
        printf("Received exception: EXC_SIZE; location: '%s'\n", function_name);
        break;
    case EXC_NOT_SQUARE:
        printf("Received exception: EXC_NOT_SQUARE; location: '%s'\n", function_name);
        break;
    case EXC_DIMENSION:
        printf("Received exception: EXC_DIMENSION; location: '%s'\n", function_name);
        break;
    case EXC_MULTIPLY:
        printf("Received exception: EXC_MULTIPLY; location: '%s'\n", function_name);
        break;
    case EXC_GETTING_MINOR:
        printf("Received exception: EXC_GETTING_MINOR; location: '%s'\n", function_name);
        break;
    default:
        //printf("The program %s has been successfully completed", function_name);
        break;
    }
}

// Matrix must be initialized
MATRIX_TYPE get_matrix_type(const Matrix *matrix)
{
    if (matrix->cols == matrix->rows){
        return SQUARE_MATRIX;
    }
    if (matrix->cols == 1 || matrix->rows == 1){
        return VECTOR;
    } else {
        return ORDINARY_MATRIX;
    }
}


static void swap_variables(double *var1, double *var2)
{
    double v = *var1;
    *var1 = *var2;
    *var2 = v;
}


static size_t get_factorial(size_t number)
{
    size_t result = 1;
    for(int i = 2; i <= number; i++)
    {
        result *= i;
    }
    return result;
}


static void set_null_matrix(Matrix *mtx)
{
    mtx->cols = MATRIX_NULL.cols;
    mtx->data = MATRIX_NULL.data;
    mtx->rows = MATRIX_NULL.rows;
}


// Allocate memory and set rows and cols.
// If cannot do it to return zero matrix.
MATRIX_EXCEPTION malloc_matrix(Matrix *matrix, size_t rows, size_t cols)
{
    if (matrix == NULL){
        return EXC_NULL;
    }
    
    matrix->rows = rows;
    matrix->cols = cols;

    matrix->data = (double*)malloc(rows * cols * sizeof(double));
    if(matrix->data == NULL){
        set_null_matrix(matrix);
        return EXC_MEMORY;
    }

    return OK;
}


// Matrix must be allocated. Copy elements from data
MATRIX_EXCEPTION set_data(Matrix *mtx, const double *data, size_t data_bite_size)
{
    if(data == NULL || mtx == NULL){
        return EXC_NULL;
    }

    size_t arr_elements = data_bite_size / sizeof(data[0]);

    if (arr_elements != mtx->cols * mtx->rows) {
        return EXC_SIZE;
    }

    memcpy(mtx->data, data, data_bite_size);
    return OK;
}


// Matrix must be initialized
MATRIX_EXCEPTION print_matrix(const Matrix *matrix)
{
    if (matrix == NULL || matrix->data == NULL){
        return EXC_NULL;
    }
    
    size_t cols = matrix->cols;
    double* data = matrix->data;

    for(size_t i = 0; i < cols * matrix->rows; i += cols)
    {
        // i += cols equals to row += 1; 
        for(size_t j = i; j < cols + i; j++)
        {
            printf("%lf\t", data[j]);
        }
        printf("\n");
    }
    printf("----------------\n");
    return OK;
}


// <retn> must NOT be allocated. Data memory must be freed by yourself
MATRIX_EXCEPTION get_identity_matrix(Matrix *retn, size_t cols, size_t rows)
{
    if(cols != rows){
        return EXC_NOT_SQUARE;
    }

    retn->rows = rows;
    retn->cols = cols;

    retn->data = (double*)calloc(rows * cols, sizeof(double)); // After allocating matrix is filled by zero.
    if(retn->data == NULL){
        set_null_matrix(retn);
        return EXC_MEMORY;
    }

    double *data = retn->data;
    for(int i = 0; i < rows * cols; i += cols + 1)
    {
        data[i] = 1;
        //memset(&data[i + 1], 0, cols);
    }
    return OK;
}


// Array must be initialized
static double get_cubic_determinant(const double *array)
{
    const double *a = array;

    double result = a[0] * a[4] * a[8] + a[2] * a[3] * a[7] + a[1] * a[5] * a[6]
                - a[2] * a[4] * a[6] - a[0] * a[5] * a[7] - a[1] * a[4] * a[8];
    return result;

}


// Array must be initialized
static double get_quadratic_determinant(const double *array)
{
    const double *a = array;

    double result = a[0] * a[3] - a[1] * a[2];
    return result;
}


// <retn_mtx> return from method as parameter as got minor. Free memory by yourself
static MATRIX_EXCEPTION get_minor(Matrix *retn_mtx, const Matrix *mtx, size_t row_i, size_t col_i)
{
    if ( (row_i + 1) > mtx->rows || (col_i + 1) > mtx->cols){
        return EXC_GETTING_MINOR;
    }

    MATRIX_EXCEPTION exc = malloc_matrix(retn_mtx, mtx->rows - 1, mtx->cols - 1);
    if (exc != OK){
        return EXC_MEMORY;
    }

    double *pointer = mtx->data;
    double *old_array = mtx->data;
    size_t cols = mtx->cols;
    double *new_array = retn_mtx->data;
    size_t size_double = sizeof(double);

    int j = col_i;
    int k = 0;
    for(; j < row_i * cols; j += cols)
    {
        memcpy(new_array, pointer, (j - k) * size_double);
        k = j + 1;
        pointer = &old_array[k];
    }

    k = j + 1;
    pointer = &old_array[k];
    if (k < row_i * cols){
        memcpy(new_array, pointer, (row_i * cols - k) * size_double);
    }

    k = (row_i + 1) * cols;
    pointer = &old_array[k];
    for(j = k + col_i; j < mtx->cols * mtx->rows; j += cols)
    {
        memcpy(new_array, pointer, (j - k) * size_double);
        k = j + 1;
        pointer = &old_array[k];
    }
    
    k = mtx->rows * (cols - 1) + col_i + 1;
    pointer = &old_array[k];
    if (k < mtx->cols * mtx->rows){
        memcpy(new_array, pointer, (mtx->cols * mtx->rows - k) * size_double);
    }
    return OK;
}


// Return value as parameter <algebraic_addition>
static MATRIX_EXCEPTION get_algebraic_addition(const Matrix *mtx, size_t row_i, size_t col_i, double *algebraic_addition)
{
    //row_i++; // when calculate algebraic index of row or col always > 0
    //col_i++; // 
    Matrix new_mtx;
    MATRIX_EXCEPTION exc = get_minor(&new_mtx, mtx, row_i, col_i);
    if (exc != OK){
        *algebraic_addition = 0.0;
        return exc;
    }

    int power_of_negative_one = row_i + 1 + col_i + 1;
    signed int negative_one = fmod(power_of_negative_one, 2) == 0 ? 1 : -1;
    size_t minor_count = mtx->cols * mtx->rows - mtx->cols - mtx->rows + 1;

    if (minor_count > 9){
        MATRIX_EXCEPTION exc = get_determinant(&new_mtx, algebraic_addition);
        if (exc != OK){
            return exc;
        }
    }
    if (minor_count == 9){
        *algebraic_addition = get_cubic_determinant(new_mtx.data);
    }
    if (minor_count == 4){
        *algebraic_addition = get_quadratic_determinant(new_mtx.data);
    }
    if (minor_count == 1){
        *algebraic_addition = new_mtx.data[0];
    }
    free(new_mtx.data);
    *algebraic_addition *= negative_one;
    return OK;
}


// det |A|
// Return value with <determinant> and success or exception of execution as return value
MATRIX_EXCEPTION get_determinant(const Matrix *mtx, double *det)
{
    if(mtx == NULL){
        return EXC_NULL;
    }
    if (get_matrix_type(mtx) != SQUARE_MATRIX){
        *det = 0.0;
        return EXC_NOT_SQUARE;
    }
    
    if(mtx->rows == 1 && mtx->cols == 1){
        *det = mtx->data[0];
        return OK;
    }
    if (mtx->cols == 2 && mtx->rows == 2){
        *det = get_quadratic_determinant(mtx->data);
        return OK;
    }
    if (mtx->cols == 3 && mtx->rows == 3){
        *det = get_cubic_determinant(mtx->data);
        return OK;
    }

    // We will always decompose only first row (having zero index)
    size_t row = 0;
    
    double determinant = 0.0;
    double *arr = mtx->data;
    for (int col = 0; col < mtx->cols; col++)
    {
        double addition;
        MATRIX_EXCEPTION exc = get_algebraic_addition(mtx, row, col, &addition);
        if (exc != OK){
            return exc;
        }
        determinant += arr[col] * addition;
    }
    *det = determinant;
    return OK; 
}


// A -> A ^ (-1) 
// <inverse> must NOT be allocated. Data memory must be freed by yourself
MATRIX_EXCEPTION invert_matrix(Matrix *inverse, const Matrix *basic)
{
    if (basic == NULL || inverse == NULL){
        return EXC_NULL;
    }

    MATRIX_EXCEPTION exc = malloc_matrix(inverse, basic->rows, basic ->cols);
    if (exc != OK){
        return exc;
    }

    double determinant;
    exc = get_determinant(basic, &determinant);
    if (exc != OK){
        return exc;
    }
    size_t cols = inverse->cols;

    double *data = inverse->data;
    double addition;
    for(size_t row = 0, index = 0; row < inverse->rows; row++)
    {
        for(size_t col = 0; col < cols; col++, index++)
        {
            MATRIX_EXCEPTION exc = get_algebraic_addition(basic, row, col, &addition);
            if (exc != OK){
                return exc;
            }
            data[index] = addition * (1.0 / determinant);
        }
    }
    return OK;
}


// A = B
// <retn> must NOT be allocated. Data memory must be freed by yourself
MATRIX_EXCEPTION copy_matrix(Matrix *retn, const Matrix *src)
{
    if (retn == NULL || src == NULL){
        return EXC_NULL;
    }

    MATRIX_EXCEPTION exc = malloc_matrix(retn, src->rows, src->cols);
    if (exc != OK){
        return exc;
    }

    memcpy(retn->data, src->data, sizeof(double) * src->cols * src->rows);
    retn->cols = src->cols;
    retn->rows = src->rows;
    return OK;
}


// A + B = C 
// <resl_mtx> must NOT be allocated. Data memory must be freed by yourself
MATRIX_EXCEPTION summarize(Matrix *resl_mtx, const Matrix *mtx1, const Matrix *mtx2)
{
    if (mtx1 == NULL && mtx2 == NULL && resl_mtx == NULL){
        return EXC_NULL;
    }
    if (mtx1->cols != mtx2->cols && mtx1->cols != mtx2->rows){
        return EXC_DIMENSION;
    }

    MATRIX_EXCEPTION exc = malloc_matrix(resl_mtx, mtx1->rows, mtx1->rows);
    if (exc == EXC_MEMORY){
        return EXC_MEMORY;
    }

    size_t cols = mtx1->cols;
    double *data1 = mtx1->data;
    double *data2 = mtx2->data;
    double *data = resl_mtx->data;

    for(int i = 0; i < mtx1->cols * mtx1->rows; i += cols)
    {
        // i += cols equals to row += 1
        for(int j = i; j < cols + i; j++)
        {
            data[j] = data1[j] + data2[j];
        }
    }
    resl_mtx->cols = mtx1->cols;
    resl_mtx->rows = mtx1->rows;

    return OK;
}


// A += B
// New matrix is not creating. Without any checking
void add(const Matrix *basic, const Matrix *adding)
{
    size_t cols = basic->cols;
    double *data1 = basic->data;
    double *data2 = adding->data;

    for(int i = 0; i < basic->cols * basic->rows; i += cols)
    {
        // i += cols equals to row += 1
        for(int j = i; j < cols + i; j++)
        {
            data1[j] += data2[j];
        }
    }
}


// A - B = C 
// <sub_mtx> must NOT be allocated. Data memory must be freed by yourself
MATRIX_EXCEPTION subtract(Matrix *resl_mtx, const Matrix *mtx, const Matrix *sub_mtx)
{
    if (mtx == sub_mtx &&  resl_mtx == NULL){
        return EXC_NULL;
    }
    if (mtx->cols != sub_mtx->cols && mtx->cols != sub_mtx->rows){
        return EXC_DIMENSION;
    }

    MATRIX_EXCEPTION exc = malloc_matrix(resl_mtx, mtx->rows, mtx->rows);
    if (exc == EXC_MEMORY){
        return EXC_MEMORY;
    }

    size_t cols = mtx->cols;
    double *data1 = mtx->data;
    double *data2 = sub_mtx->data;
    double *data = resl_mtx->data;

    for(int i = 0; i < mtx->cols * mtx->rows; i += cols)
    {
        // i += cols equals to row += 1
        for(int j = i; j < cols + i; j++)
        {
            data[j] = data1[j] - data2[j];
        }
    }
    resl_mtx->cols = mtx->cols;
    resl_mtx->rows = mtx->rows;
    
    return OK;
}


// A * n
// Change the same matrix
void multiply_by_number(Matrix *matrix, double number)
{
    size_t cols = matrix->cols;
    double* data = matrix->data;

    for(int i = 0; i < matrix->cols * matrix->rows; i += cols)
    {
        // i += cols equals to row += 1
        for(int j = i; j < cols + i; j++)
        {
            data[j] *= number;
        }
    }
}


// resl_mtx = base * multiplier
// <resl_mtx> can be either allocated or not. Old memory it delete. New memory must be fread by yourself
MATRIX_EXCEPTION multiply_matrices(Matrix *resl_mtx, const Matrix *base, const Matrix *multiplier)
{
    if (base == NULL || multiplier == NULL){
        return EXC_NULL;
    }
    if (base->cols != multiplier->rows){
        return EXC_MULTIPLY;
    }

    double *data;
    if (resl_mtx == base){
        data = (double*)malloc(base->rows * multiplier->cols * sizeof(double)); 
        if (data == NULL){                                                     
            return EXC_MEMORY;                                                 
        }
        memset(data, 0, base->rows * multiplier->cols * sizeof(double));
    } else {
        MATRIX_EXCEPTION exc = malloc_matrix(resl_mtx, base->rows, multiplier->cols);
        if (exc != OK){
            return exc;
        }
        data = resl_mtx->data;
        memset(resl_mtx->data, 0, base->rows * multiplier->cols * sizeof(double));
    }

    size_t cols2 = multiplier->cols;
    size_t count2 = cols2 * multiplier->rows;
    double *data1 = base->data;
    double *data2 = multiplier->data;

    for (size_t row1 = 0, ind_new_matrix = 0; row1 < base->cols * base->rows; row1 += base->cols)
    {
        for (size_t col2 = 0; col2 < cols2; col2++, ind_new_matrix++)
        {
            for (size_t j = row1, h = col2; h < count2; j++, h += cols2)
            {
                data[ind_new_matrix] += data1[j] * data2[h];
            }
        }
    }

    if (resl_mtx == base){
        resl_mtx->cols = multiplier->cols;
        free(resl_mtx->data);
        resl_mtx->data = data;
    }
    return OK;
}


// A ^ n 
// <resl_mtx> must be allocated. Free memory by yourself
MATRIX_EXCEPTION rase_to_power(Matrix *resl_mtx, const Matrix *mtx, signed int power)
{
    if (power == 1){
        MATRIX_EXCEPTION exc = copy_matrix(resl_mtx, mtx);
        if (exc != OK){
            return exc;
        }
    }

    if (get_matrix_type(mtx) != SQUARE_MATRIX){
        return EXC_NOT_SQUARE;
    }
    if (power == 0) {
        MATRIX_EXCEPTION exc = get_identity_matrix(resl_mtx, mtx->cols, mtx->rows);
        if (exc != OK){
            return exc;
        }
    }

    if (power > 1){
        MATRIX_EXCEPTION exc = copy_matrix(resl_mtx, mtx);
        if (exc != OK) {
            return exc;
        }
        for (int i = 0; i < power; i++)
        {
            exc = multiply_matrices(resl_mtx, resl_mtx, mtx);
            if (exc != OK){
                return exc;
            }
        }
    }

    if (power < 1){
        Matrix inverse_mtx;
        MATRIX_EXCEPTION exc = invert_matrix(&inverse_mtx, mtx);
        if (exc != OK){
            return exc;
        }
        power *= -1;
        exc = copy_matrix(resl_mtx, &inverse_mtx);
        if (exc != OK){
            return exc;
        }

        for(int i = 0; i < power; i++)
        {
            exc = multiply_matrices(resl_mtx, resl_mtx, &inverse_mtx);
            if (exc != OK){
                return exc;
            }
        }
        free(inverse_mtx.data);
    }
    return OK;
}


// exp(A) 
// resl_mtx must not be initialized
MATRIX_EXCEPTION get_exp(Matrix *resl_mtx, const Matrix *mtx)
{
    if (mtx == NULL){
        return EXC_NULL;
    }

    MATRIX_EXCEPTION exc = malloc_matrix(resl_mtx, mtx->rows, mtx->cols);
    if (exc != OK){
        return exc;
    }

    // Adding identity matrix // 1th element
    double *data = resl_mtx->data;
    for(int i = 0; i < resl_mtx->rows * resl_mtx->cols; i += resl_mtx->cols + 1)
    {
        data[i] += 1;
    }

    Matrix mtx2;
    exc = copy_matrix(&mtx2, mtx);
    if (exc != OK){
        return exc;
    }

    add(resl_mtx, &mtx2);

    for(size_t i = 2; i < exp_steps; i++)
    {
        double multiplier = 1.0 / (double)i; 
        exc = multiply_matrices(&mtx2, &mtx2, mtx);
        if (exc != OK){
            return exc;
        }
        multiply_by_number(&mtx2, multiplier);
        add(resl_mtx, &mtx2);
    }

    free(mtx2.data);
    return OK;
}


// Swap variables or create new matrix
static MATRIX_EXCEPTION transpose_vector(Matrix *new_matrix, const Matrix *matrix)
{
    if (new_matrix == matrix){
        double var1 = (double)new_matrix->cols;
        double var2 = (double)new_matrix->rows;
        swap_variables(&var1, &var2);
        return OK;
    }
    
    MATRIX_EXCEPTION exc = malloc_matrix(new_matrix, matrix->cols, matrix->rows);
    if (exc != OK){
        return exc;
    }
    return OK;
}


// You can return the same transposed matrix or get new matrix and free memory for by yourself
static MATRIX_EXCEPTION transpose_square(Matrix *new_matrix, const Matrix *matrix)
{
    double *data = matrix->data;
    size_t matrix_cols = matrix->cols;
    size_t count = matrix->cols * matrix->rows;

    if (new_matrix == matrix){
        for (size_t j = 0, i = 0; j < matrix_cols; j++)
        {
            for (size_t k = j; k < count; i++, k += matrix_cols )
            {
                swap_variables(&data[i], &data[k]);
            }
        }
        return OK;
    }

    MATRIX_EXCEPTION exc = malloc_matrix(new_matrix, matrix->cols, matrix->rows);
    if (exc != OK){
        return exc;
    }
    double *new_data = new_matrix->data;

    for (size_t j = 0, i = 0; j < matrix_cols; j++)
    {
        for (size_t k = j; k < count; i++, k += matrix_cols )
        {
            new_data[i] = data[k];
        }
    }
    return OK;
}


// You can return the same transposed matrix or get new matrix and free memory for by yourself
MATRIX_EXCEPTION transpose(Matrix *result_matrix, const Matrix *matrix)
{   
    if (matrix == NULL){
        return EXC_NULL;
    }

    MATRIX_TYPE type = get_matrix_type(matrix);
    MATRIX_EXCEPTION exc;
    switch (type)
    {
    case VECTOR:
        exc = transpose_vector(result_matrix, matrix);
        if (exc != OK){
            return exc;
        }
        break;
    case SQUARE_MATRIX:
        exc = transpose_square(result_matrix, matrix);
        if (exc != OK){
            return exc;
        }
        break;
    case ORDINARY_MATRIX:
        return EXC_NOT_SQUARE;
    }
    return OK;
}