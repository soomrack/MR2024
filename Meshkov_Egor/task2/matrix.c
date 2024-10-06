#include <math.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <stdio.h>


typedef struct
{
  double *ptr;
  int rows;
  int cols;
  int len;
} Matrix;


static void error_malloc()
{
  fprintf(stderr, "Memory allocation failed\n");
  exit(1);
}


// Function for function for allocating memory for matrices
Matrix mallocMatrix(size_t rows, size_t cols)
{
  Matrix matrix;

  matrix.rows = rows;
  matrix.cols = cols;
  matrix.len = matrix.rows * matrix.cols;
  matrix.ptr = (double *)malloc(matrix.len * sizeof(double));
  
  if (matrix.ptr == NULL)
  {
    error_malloc();
  }
  
  return matrix;
}


// Function for finding the largest width of numbers in a matrix
static int calculate_max_width(Matrix matrix, int accuracy)
{
  int max_width = 0;

  for(size_t pos = 0; pos < matrix.len; pos++)
  {
    int width = snprintf(NULL, 0, "%.*f", accuracy, matrix.ptr[pos]);
    if (width > max_width)
    {
      max_width = width;
    }
  }

  return max_width;
}

// Function to print matrix
void printMatrix(Matrix matrix, int accuracy)
{
  int width = calculate_max_width(matrix, accuracy);
  double number;

  for(size_t pos = 0; pos < matrix.len; ++pos)
  {
    if(pos % matrix.cols == 0 && pos != 0)
    {
      printf("\n"); // New line after each row
    }
    number = round(matrix.ptr[pos] * pow(10, accuracy)) / pow(10, accuracy);
    printf("%*.*f ", width, accuracy, number);
  }
  printf("\n\n"); // Final new line for better formatting
}


static double random_number(int min, int max, int accuracy)
{
  int sign = (rand() % 2) * 2 - 1;
  int range = max - min;
  double number;

  if(sign == 1)
  {
    number = min + ((double)rand() / (double)RAND_MAX) * range;
  } else
  {
    number = max - ((double)rand() / (double)RAND_MAX) * range;
  }

  return round(number * pow(10, accuracy)) / pow(10, accuracy);
  //return min + sign * (((double)rand() + (double)rand() / (double)RAND_MAX) % range);
}


// Function to create O-matrix
Matrix createMatrixO(size_t rows, size_t cols)
{
  Matrix matrix = mallocMatrix(rows, cols);

  for(size_t pos = 0; pos < matrix.len; ++pos)
  {
    matrix.ptr[pos] = 0;
  }
  
  return matrix;
}


// Function to create E-matrix
Matrix createMatrixE(size_t rows, size_t cols)
{
  Matrix matrix = mallocMatrix(rows, cols);

  for(size_t pos = 0; pos < matrix.len; ++pos)
  {
    matrix.ptr[pos] = (pos % (matrix.cols + 1) == 0) ? 1 : 0;
  }
  
  return matrix;
}


static int check_range(int min, int max)
{
  return (max >= min);
}


static void error_range()
{
  printf("Error to create random matrix, incorrect range of random numbers\n");
  exit(1);
}


// Function to create random matrix in choose range and with choose accuracy
Matrix createMatrixR(size_t rows, size_t cols, int min, int max, int accuracy)
{
  if(check_range(min, max) == 0)
    error_range();

  Matrix matrix = mallocMatrix(rows, cols);

  for(size_t pos = 0; pos < matrix.len; ++pos)
  {
    matrix.ptr[pos] = random_number(min, max, accuracy);
  }
  
  return matrix;
}


void freeMatrix(Matrix matrix)
{
  free(matrix.ptr);
}


static void error_to_sum()
{
  printf("Error to matrix addition/subtraction, columns and rows should be equal respectively\n");
  exit(1);
}


static int check_size_matrix_for_sum(Matrix matrixA, Matrix matrixB)
{
  return ((matrixA.rows == matrixB.rows) && (matrixA.cols == matrixB.cols));
}


// function for matrix addition
Matrix addMatrix(Matrix matrixA, Matrix matrixB)
{
  if(check_size_matrix_for_sum(matrixA, matrixB) == 0)
    error_to_sum();

  Matrix matrixC = mallocMatrix(matrixA.rows, matrixA.cols);

  for(size_t pos = 0; pos < matrixA.len; ++pos)
  {
    matrixC.ptr[pos] = matrixA.ptr[pos] + matrixB.ptr[pos];
  }

  return matrixC;
}


// function for matrix subtraction
Matrix subMatrix(Matrix matrixA, Matrix matrixB)
{
  if(check_size_matrix_for_sum(matrixA, matrixB))
    error_to_sum();

  Matrix matrixC = mallocMatrix(matrixA.rows, matrixA.cols);

  for(size_t pos = 0; pos < matrixA.len; ++pos)
  {
    matrixC.ptr[pos] = matrixA.ptr[pos] - matrixB.ptr[pos];
  }

  return matrixC;
}


static int check_size_matrixes_for_multi(Matrix matrixA, Matrix matrixB)
{
  return (matrixA.cols == matrixB.rows);
}


// Function to handle multiplication error
void error_to_multi()
{
    fprintf(stderr, "Matrix dimensions are not suitable for multiplication\n");
    exit(1);
}


// Function for matrix-matrix multiplication
Matrix multiMatrix(Matrix matrixA, Matrix matrixB)
{
  if (!check_size_matrixes_for_multi(matrixA, matrixB))
  {
    error_to_multi();
  }

  Matrix matrixC = mallocMatrix(matrixA.rows, matrixB.cols);

  for (size_t row = 0; row < matrixA.rows; ++row)
  {
    for (size_t colB = 0; colB < matrixB.cols; ++colB)
    {
      matrixC.ptr[row * matrixC.cols + colB] = 0;
      for (size_t colA = 0; colA < matrixA.cols; ++colA)
      {
        matrixC.ptr[row * matrixC.cols + colB] += matrixA.ptr[row * matrixA.cols + colA] * matrixB.ptr[colA * matrixB.cols + colB];
      }
    }
  }

  return matrixC;
}


// Function to multiply a matrix by a number
Matrix multiplyMatrixByScalar(Matrix matrix, double scalar)
{
  Matrix result = mallocMatrix(matrix.rows, matrix.cols);

  for (size_t i = 0; i < matrix.len; ++i)
  {
    result.ptr[i] = matrix.ptr[i] * scalar;
  }

  return result;
}


// Function to transpose a matrix
Matrix transposeMatrix(Matrix matrix)
{
  Matrix result = mallocMatrix(matrix.cols, matrix.rows);

  for (size_t row = 0; row < matrix.rows; ++row)
  {
    for (size_t col = 0; col < matrix.cols; ++col)
    {
      result.ptr[col * result.cols + row] = matrix.ptr[row * matrix.cols + col];
    }
  }
  
  return result;
}


// Function to check if a matrix is square
static int is_square_Matrix(Matrix matrix)
{
  return (matrix.rows == matrix.cols);
}


// Function to handle determinant calculation error
static void error_to_calculate_determinant()
{
    fprintf(stderr, "Error to calculate determinant, rows and columns must be equal\n");
    exit(1);
}


// Function to check for zeros in the first column
static int check_zeros_in_first_col(Matrix matrix)
{
  for (size_t row = 0; row < matrix.rows; row++)
  {
    if (matrix.ptr[row * matrix.cols] != 0)
    {
      return 1;
    }
  }
  
  return 0;
}


// Function to copy a matrix
static void copyMatrix(Matrix *src, Matrix *dest)
{
  for (size_t pos = 0; pos < src->len; pos++)
  {
    dest->ptr[pos] = src->ptr[pos];
  }
}

// Function to find the row with the maximum element in the current column
static void findMaxRow(const Matrix *matrix, const size_t *currentRow, size_t *maxRow)
{
  *maxRow = *currentRow;

  for (size_t row = *currentRow + 1; row < matrix->rows; row++)
  {
    if (fabs(matrix->ptr[row * matrix->cols + *currentRow]) > fabs(matrix->ptr[*maxRow * matrix->cols + *currentRow]))
    {
      *maxRow = row;
    }
  }
}


// Function to swap rows in a matrix
static void swapRows(Matrix *matrix, const size_t *currentRow, const size_t *maxRow)
{
  for (size_t col = *currentRow; col < matrix->cols; col++)
  {
    double temp = matrix->ptr[*maxRow * matrix->cols + col];
    matrix->ptr[*maxRow * matrix->cols + col] = matrix->ptr[*currentRow * matrix->cols + col];
    matrix->ptr[*currentRow * matrix->cols + col] = temp;
  }
}


// Function to make a matrix triangular
static void makeTriangularMatrix(Matrix *matrix, size_t *currentRow) 
{
  for (size_t row = *currentRow + 1; row < matrix->rows; row++)
  {
    // The coefficient by which the strings are multiplied
    double factor = matrix->ptr[row * matrix->cols + *currentRow] / matrix->ptr[*currentRow * matrix->cols + *currentRow];
    for (size_t col = *currentRow; col < matrix->cols; col++)
    {
      if (*currentRow == col)
      {
        matrix->ptr[row * matrix->cols + col] = 0;
      } else
      {
        matrix->ptr[row * matrix->cols + col] -= factor * matrix->ptr[*currentRow * matrix->cols + col];
      }
    }
  }
}


// Function for calculating the matrix determinant
double gauss_method(Matrix matrix)
{
  if (!is_square_Matrix(matrix))
  {
    error_to_calculate_determinant();
  }

  if (!check_zeros_in_first_col(matrix))
  {
    return 0;
  }

  size_t size = matrix.rows; // rows and columns must be equal
  Matrix matrixC = mallocMatrix(size, size);
  copyMatrix(&matrix, &matrixC); // Copying the matrix so as not to spoil the original one

  size_t maxRow;
  for (size_t currentRow = 0; currentRow < size; currentRow++)
  {
    // Finding the max element in the column for stability
    findMaxRow(&matrixC, &currentRow, &maxRow);

    // Swap lines
    swapRows(&matrixC, &currentRow, &maxRow);

    // Reduce to triangular form
    makeTriangularMatrix(&matrixC, &currentRow);
  }

  double det = 1;

  // Calculating the determinant of a triangular matrix
  for (size_t pos = 0; pos < size; pos++)
  {
    det *= matrixC.ptr[pos * matrixC.cols + pos];
  }

  freeMatrix(matrixC);

  return det;
}


// Function to check if the determinant is zero
static int check_determinant_to_calculate_reverse_Matrix(const double *determinant)
{
  return (*determinant == 0) ? 0 : 1;
}


static void createExtendMatrix(Matrix *matrix, Matrix *Ematrix)
{
  for(size_t row = 0; row < Ematrix->rows; ++row)
  { 
    for(size_t col = 0; col < Ematrix->cols; ++col)
    {
      if(col >= matrix->cols)
      {
        Ematrix->ptr[row * Ematrix->cols + col] = (row == col - matrix->cols) ? 1 : 0;
      } else
      {
        Ematrix->ptr[row * Ematrix->cols + col] = matrix->ptr[row * matrix->cols + col];
      }
    }
  }
}


// Function to handle reverse Matrix calculation error
static void error_to_calculate_reverse_Matrix()
{
  fprintf(stderr, "Error to calculate reverse Matrix, rows and columns must be equal\n");
  exit(1);
}


// Function to handle determinant for reverse Matrix calculation error
static void error_to_calculate_determinant_for_reverse_Matrix()
{
  fprintf(stderr, "Error to calculate reverse Matrix, determinant source matrix equal zero\n");
  exit(1);
}


static void extractInverseMatrix(Matrix *reverse_matrix, Matrix *Ematrix)
{
  for(size_t row = 0; row < reverse_matrix->rows; row++)
  {
    for (size_t col = 0; col < reverse_matrix->cols; col++)
    {
      reverse_matrix->ptr[row * reverse_matrix->cols + col] = Ematrix->ptr[row * Ematrix->cols + reverse_matrix->cols + col];
    }
  }
}


static void zeroUpperTriangle(Matrix *Ematrix)
{
  for (size_t currentRow = 1; currentRow < Ematrix->rows; currentRow++)
  {
    for (size_t row = 0; row < currentRow; row++)
    {
      double factor = Ematrix->ptr[row * Ematrix->cols + currentRow];
      for (size_t col = currentRow; col < Ematrix->cols; col++)
      {
        Ematrix->ptr[row * Ematrix->cols + col] -= factor * Ematrix->ptr[currentRow * Ematrix->cols + col];
      }
    }
  }
}


static void normalizeDiagonal(Matrix *Ematrix, size_t currentRow)
{
      double diagElement = Ematrix->ptr[currentRow * Ematrix->cols + currentRow];
    for (size_t col = 0; col < Ematrix->cols; col++)
    {
      Ematrix->ptr[currentRow * Ematrix->cols + col] /= diagElement;
    }
}


void reverseMatrix(Matrix *matrix, Matrix *reverse_matrix)
{
  if(!is_square_Matrix(*matrix))
  {
    error_to_calculate_reverse_Matrix();
  }

  double det = gauss_method(*matrix);
  if(check_determinant_to_calculate_reverse_Matrix(&det) == 0)
  {
    error_to_calculate_determinant_for_reverse_Matrix();
  }

  Matrix Ematrix = mallocMatrix(matrix->rows, matrix->cols * 2);
  createExtendMatrix(matrix, &Ematrix);

  size_t maxRow;
  for (size_t currentRow = 0; currentRow < Ematrix.rows; currentRow++)
  {
    // Finding the max element in the column for stability
    findMaxRow(&Ematrix, &currentRow, &maxRow);

    // Swap lines
    swapRows(&Ematrix, &currentRow, &maxRow);

    // Reduce to triangular form
    makeTriangularMatrix(&Ematrix, &currentRow);

    // Делаем диагональный элемент равным 1
    normalizeDiagonal(&Ematrix, currentRow);
  }
  zeroUpperTriangle(&Ematrix);

  extractInverseMatrix(reverse_matrix, &Ematrix);

  freeMatrix(Ematrix);
}


// Function to solve matrix equation A * X = B
Matrix solveMatrixEquation(Matrix matrixA, Matrix matrixB)
{
  Matrix reverse_MatrixA = mallocMatrix(matrixA.rows, matrixA.cols);
  reverseMatrix(&matrixA, &reverse_MatrixA);
  
  return multiMatrix(reverse_MatrixA, matrixB);
}
