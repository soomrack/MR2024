#include <stdio.h>
#include <stdlib.h>


typedef struct {
    unsigned int rows;
    unsigned int cols;
    double * data;
} Matrix;


void matrix_create(Matrix *matrix)
{
    matrix->data = malloc((matrix->rows * matrix->cols) * sizeof(double));
    for (int row = 0; row < matrix->rows * matrix->cols; row++)
        matrix->data[row] = 0;
    
}


void pass_array(Matrix *matrix, double array_name[])
{
    matrix_create(matrix);

    for (int rows = 0; rows < matrix->rows * matrix->cols; rows++)
    {
        matrix->data[rows] = array_name[rows];
    }
}


void memory_free(Matrix *matrix)
{
    free(matrix->data);
}


void matrix_print(Matrix *matrix)
{
    for (int rows = 0; rows < matrix->rows; rows++)
    {
        for(int cols = 0; cols < matrix->cols; cols++)
        {
            printf("%.2f ", matrix->data[rows * matrix->cols + cols]);
        }
        printf("\n");
    }
    printf("\n");
}


void matrix_add (Matrix A, Matrix B)
{
    printf("Addition:\n");

    if (A.cols == B.cols && A.rows == B.rows) {
        Matrix new_matrix_add = {A.rows, A.cols};
        matrix_create(&new_matrix_add);

        for (int rows = 0; rows < new_matrix_add.rows; rows++)
        {
            for (int cols = 0; cols < new_matrix_add.cols; cols++)
            {
                new_matrix_add.data[rows * new_matrix_add.cols + cols] = A.data[rows * A.cols + cols] + B.data[rows * B.cols + cols];
            }
        }
        matrix_print(&new_matrix_add);
        memory_free(&new_matrix_add);
    } else {
        printf("Matrix sizes are not equal!\n");
    }
}


void matrix_subtract (Matrix A, Matrix B)
{
    printf("Subtraction:\n");
    if (A.cols == B.cols && A.rows == B.rows) {

    Matrix new_matrix_subtract = {2, 2};
    matrix_create(&new_matrix_subtract);

    for (int rows = 0; rows < new_matrix_subtract.rows; rows++)
    {
        for (int cols = 0; cols < new_matrix_subtract.cols; cols++)
        {
            new_matrix_subtract.data[rows * new_matrix_subtract.cols + cols] = A.data[rows * A.cols + cols] - B.data[rows * B.cols + cols];
        }
    }
    matrix_print(&new_matrix_subtract);
    memory_free(&new_matrix_subtract);
    } else {
        printf("Matrix sizes are not equal!\n");
    }
}


void matrix_multiply (Matrix A, Matrix B)
{
    printf("Multiplication:\n");

    if (A.cols == B.rows) {
        Matrix new_matrix_multi = {A.rows, B.cols};
        matrix_create(&new_matrix_multi);

        for (int rows = 0; rows < new_matrix_multi.rows; rows++)
        {
            for (int cols = 0; cols < new_matrix_multi.cols; cols++)
            {
                for (int k = 0; k < A.cols; k++)
                {
                    new_matrix_multi.data[rows * new_matrix_multi.cols + cols] += A.data[rows * A.cols + k] * B.data[k * B.cols + cols];
                }
            }
        }
        matrix_print(&new_matrix_multi);
        memory_free(&new_matrix_multi);
    } else {
        printf("Incorrect matrix sizes for multiplication!");
    }
}


void matrix_transpose (Matrix A, int matrix_name)
{
    printf("Matrix %d Transposed:\n", matrix_name);

    Matrix new_matrix_transpose = {A.cols, A.rows};
    matrix_create(&new_matrix_transpose);

    for (int rows = 0; rows < new_matrix_transpose.rows; rows++)
    {
        for (int cols = 0; cols < new_matrix_transpose.cols; cols++)
        {
            new_matrix_transpose.data[rows * new_matrix_transpose.cols + cols] = A.data[cols * new_matrix_transpose.rows + rows];
        }
    }
    matrix_print(&new_matrix_transpose);
    memory_free(&new_matrix_transpose);
}


void make_calculations()
{
    Matrix A = {2, 3};
    Matrix B = {3, 2};
    matrix_create(&A);
    matrix_create(&B);
    pass_array(&A, (double[]){1, 3, 5, 7, 9, 11});
    pass_array(&B, (double[]){1, 2, 3, 4, 5, 6});
    matrix_add(A, B);
    matrix_subtract(A, B);
    matrix_multiply(A, B);
    matrix_transpose(A, 1);
    matrix_transpose(B, 2);
    // matrix_print(&A);
    // matrix_print(&B);
    memory_free(&A);
    memory_free(&B);

}


int main()
{
    make_calculations();
    return 0;
}
