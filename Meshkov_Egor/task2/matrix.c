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


typedef enum {
  MAT_OK = 0,
  MAT_ALLOC_ERR,
  MAT_EMPTY_ERR,
  MAT_SIZE_ERR,
  MAT_RANGE_ERR,
  MAT_DET_ERR,
  MAT_SOLVE_ERR,
  MAT_EXP_ERR,
} Status_t;


static const Matrix EMPTY_MATRIX = {NULL, 0, 0, 0};


Status_t mallocMatrix(size_t rows, size_t cols, Matrix **m)
{
  if(*m != NULL)
  {
    return MAT_EMPTY_ERR;
  }
  
  *m = (Matrix *)malloc(sizeof(Matrix));
  if(*m == NULL) 
  {
    return MAT_ALLOC_ERR;
  }
  
  (*m)->rows = rows;
  (*m)->cols = cols;
  (*m)->len = (*m)->rows * (*m)->cols;
  (*m)->ptr = (double *)malloc((*m)->len * sizeof(double));
  
  if((*m)->ptr == NULL)
  {
    return MAT_ALLOC_ERR;
  }
  
  return MAT_OK;
}


static int calculate_max_width(Matrix *m, const int accuracy)
{
  int max_width = 0;

  for(size_t pos = 0; pos < m->len; pos++)
  {
    int width = snprintf(NULL, 0, "%.*f", accuracy, m->ptr[pos]);
    if (width > max_width)
    {
      max_width = width;
    }
  }

  return max_width;
}


void printMatrix(Matrix *m, const int accuracy)
{
  int width = calculate_max_width(m, accuracy);
  double number;

  for (size_t pos = 0; pos < m->len; ++pos)
  {
    if (pos % m->cols == 0)
    {
      if (pos != 0)
      {
        printf("|\n");
      }
        printf("| ");
    }
    number = round(m->ptr[pos] * pow(10, accuracy)) / pow(10, accuracy);
    printf("%*.*f ", width, accuracy, number);
  }
  printf("|\n\n"); // Final new line for better formatting
}


static double random_number(const double min, const double max, const unsigned int accuracy)
{
  int sign = (rand() % 2) * 2 - 1;
  double range = max - min;
  double number;

  if(sign == 1)
  {
    number = min + ((double)rand() / (double)RAND_MAX) * range;
  } else
  {
    number = max - ((double)rand() / (double)RAND_MAX) * range;
  }

  return round(number * pow(10, accuracy)) / pow(10, accuracy);
}


static Status_t checkMatrixForCreate(Matrix **m, const size_t rows, const size_t cols)
{
  if((*m) != NULL)
  {
    return MAT_EMPTY_ERR;
  }

  if(mallocMatrix(rows, cols, m) != MAT_OK)
  {
    return MAT_ALLOC_ERR;
  }

  return MAT_OK;
}


Status_t createMatrixO(size_t rows, size_t cols, Matrix **m)
{
  Status_t status = checkMatrixForCreate(m, rows, cols);

  if(status != MAT_OK)
  {
    return status;
  }

  for(size_t pos = 0; pos < (*m)->len; ++pos)
  {
    (*m)->ptr[pos] = 0;
  }
  
  return MAT_OK;
}


Status_t createMatrixE(size_t rows, size_t cols, Matrix **m)
{
  Status_t status = checkMatrixForCreate(m, rows, cols);

  if(status != MAT_OK)
  {
    return status;
  }

  for(size_t pos = 0; pos < (*m)->len; ++pos)
  {
    (*m)->ptr[pos] = (pos % ((*m)->cols + 1) == 0) ? 1 : 0;
  }
  
  return MAT_OK;
}


Status_t createMatrixR(Matrix **m, size_t rows, size_t cols, double min, double max, unsigned int accuracy)
{
  Status_t status = checkMatrixForCreate(m, rows, cols);
  
  if(status != MAT_OK)
  {
    return status;
  }

  if(max < min)
  {
    return MAT_RANGE_ERR;
  }

  for(size_t pos = 0; pos < (*m)->len; ++pos)
  {
    (*m)->ptr[pos] = random_number(min, max, accuracy);
  }
  
  return MAT_OK;
}


void freeMatrix(Matrix **m)
{
  if(m != NULL && *m != NULL)
  {
    if ((*m)->ptr != NULL)
    {
      free((*m)->ptr);
      (*m)->ptr = NULL; 
    }
    free(*m);
    *m = NULL;
  }
}


Status_t addMatrix(const Matrix *A, const Matrix *B, Matrix *C)
{
  if(((A->rows == B->rows) && (A->cols == B->cols)) == 0)
  {
    return MAT_SIZE_ERR;
  }

  for(size_t pos = 0; pos < A->len; ++pos)
  {
    C->ptr[pos] = A->ptr[pos] + B->ptr[pos];
  }

  return MAT_OK;
}


Status_t subMatrix(const Matrix *A, const Matrix *B, Matrix *C)
{
  if(((A->rows == B->rows) && (A->cols == B->cols)) == 0)
  {
    return MAT_SIZE_ERR;
  }

  for(size_t pos = 0; pos < A->len; ++pos)
  {
    C->ptr[pos] = A->ptr[pos] - B->ptr[pos];
  }

  return MAT_OK;
}


Status_t multiMatrix(Matrix *A, const Matrix *B, Matrix *C)
{
  if(A->cols != B->rows)
  {
    return MAT_SIZE_ERR;
  }

  for (size_t row = 0; row < A->rows; ++row)
  {
    for (size_t colB = 0; colB < B->cols; ++colB)
    {
      C->ptr[row * C->cols + colB] = 0;
      for (size_t colA = 0; colA < A->cols; ++colA)
      {
        C->ptr[row * C->cols + colB] += A->ptr[row * A->cols + colA] * B->ptr[colA * B->cols + colB];
      }
    }
  }

  return MAT_OK;
}


Status_t multiplyMatrixByScalar(const Matrix *src_matrix, const double scalar, Matrix *res_matrix)
{
  for (size_t i = 0; i < src_matrix->len; ++i)
  {
    res_matrix->ptr[i] = src_matrix->ptr[i] * scalar;
  }

  return MAT_OK;
}


Status_t transposeMatrix(const Matrix *src_matrix, Matrix *res_matrix)
{
  for (size_t row = 0; row < src_matrix->rows; ++row)
  {
    for (size_t col = 0; col < src_matrix->cols; ++col)
    {
      res_matrix->ptr[col * res_matrix->cols + row] = src_matrix->ptr[row * src_matrix->cols + col];
    }
  }
  
  return MAT_OK;
}


static void copyMatrix(Matrix *src, Matrix *dest)
{
  for (size_t pos = 0; pos < src->len; pos++)
  {
    dest->ptr[pos] = src->ptr[pos];
  }
}


static double findRowMaxElement(const Matrix *m, const size_t *currentRow)
{
  size_t rowMaxElement = *currentRow;

  for (size_t row = *currentRow; row < m->rows; row++)
  {
    if (fabs(m->ptr[row * m->cols + *currentRow]) > fabs(m->ptr[rowMaxElement * m->cols + *currentRow]))
    {
      rowMaxElement = row;
    }
  }

  return rowMaxElement; 
}


static void swapRows(Matrix *m, const size_t *currentRow, const size_t *rowMaxElement)
{
  for (size_t col = *currentRow; col < m->cols; col++)
  {
    double temp = m->ptr[*rowMaxElement * m->cols + col];
    m->ptr[*rowMaxElement * m->cols + col] = m->ptr[*currentRow * m->cols + col];
    m->ptr[*currentRow * m->cols + col] = temp;
  }
}


static void zeroingElementsBelowDiagonal(Matrix *matrix, size_t *currentRow) 
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


static double detTriangularMatrix(Matrix *m)
{
  double det = 1;

  for (size_t row = 0; row < m->rows; row++)
  {
    det *= m->ptr[row * m->cols + row];
  }

  return det;
}


static int makeZeroDownTriangularMatrix(Matrix *matrix)
{
  size_t rowMaxElement = 0;

  for (size_t currentRow = 0; currentRow < matrix->rows; currentRow++)
  {
    // Finding the max element in the column for stability
    rowMaxElement = findRowMaxElement(matrix, &currentRow);
   
    if(matrix->ptr[rowMaxElement * matrix->cols + currentRow] == 0)
    {
      return 0; // Returns zero if the determinant is known to be zero
    }

    swapRows(matrix, &currentRow, &rowMaxElement);

    zeroingElementsBelowDiagonal(matrix, &currentRow);
  }

  return 1;
}


Status_t gaussMethod(Matrix *matrix, double *det)
{
  if(matrix->rows != matrix->cols)
  {
    return MAT_SIZE_ERR;
  }

  Matrix *matrixC = NULL;
  Status_t status;
  
  status = mallocMatrix(matrix->rows, matrix->cols, &matrixC);
  if(status != MAT_OK)
  {
    return status;
  }

  copyMatrix(matrix, matrixC);  // Copying the matrix so as not o spoil the original one
  //
  if(makeZeroDownTriangularMatrix(matrixC) == 0)
  {
    *det = 0;  // One of the diagonal elements is zero
    return MAT_OK;
  }

  *det = detTriangularMatrix(matrixC);
 
  freeMatrix(&matrixC);

  return MAT_OK;
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


static void zeroingElementsAboveDiagonal(Matrix *matrix, size_t *currentRow)
{
  for (size_t row = 0; row < *currentRow; row++)
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


static void normalizeDiagonal(Matrix *extendMatrix, size_t currentRow)
{
  double diagElement = extendMatrix->ptr[currentRow * extendMatrix->cols + currentRow];
  for (size_t col = 0; col < extendMatrix->cols; col++)
  {
    extendMatrix->ptr[currentRow * extendMatrix->cols + col] /= diagElement;
  }
}


static int transformExtendMatrix(Matrix *extendMatrix)
{
  size_t rowMaxElement = 0;

  for (size_t currentRow = 0; currentRow < extendMatrix->rows; currentRow++)
  {
    // Finding the max element in the column for stability
    rowMaxElement = findRowMaxElement(extendMatrix, &currentRow);

    if(extendMatrix->ptr[rowMaxElement * extendMatrix->cols + currentRow] == 0)
    {
    printMatrix(extendMatrix, 3);
      return 0;
    }

    swapRows(extendMatrix, &currentRow, &rowMaxElement);

    zeroingElementsBelowDiagonal(extendMatrix, &currentRow);

    normalizeDiagonal(extendMatrix, currentRow);

    zeroingElementsAboveDiagonal(extendMatrix, &currentRow);
  }

  return 1;
}
                                
                                

Status_t reverseMatrix(Matrix *matrix, Matrix *reverseMatrix)
{
  if(matrix->rows != matrix->cols)
  {
    return MAT_SIZE_ERR;
  }

  if((reverseMatrix->rows != matrix->rows) && (reverseMatrix->cols != matrix->cols))
  {
    return MAT_SIZE_ERR;
  }

  Matrix *extendMatrix = NULL;
  Status_t status;

  status = mallocMatrix(matrix->rows, matrix->cols * 2, &extendMatrix);
  if(status != MAT_OK)
  {
    return status;
  }

  createExtendMatrix(matrix, extendMatrix);

  if(transformExtendMatrix(extendMatrix) == 0)
  {
    return MAT_DET_ERR;
  }

  extractInverseMatrix(reverseMatrix, extendMatrix);

  freeMatrix(&extendMatrix);

  return MAT_OK;
}

// Function to solve matrix equation A * X = B
Status_t solveMatrixEquation(Matrix *A, Matrix *B, Matrix *X)
{
  if(((A->rows == B->rows) && (A->rows == X->rows) && (B->cols == 1) && (X->cols == 1)) == 0)
  {
    return MAT_SIZE_ERR;
  }

  if(A->cols > X->rows)
  {
    return MAT_SOLVE_ERR;
  }

  Matrix *reverseMatrixA = NULL;
  Status_t status;
  
  status = mallocMatrix(A->rows, A->cols, &reverseMatrixA);
  if(status != MAT_OK)
  {
    return status;
  }

  status = reverseMatrix(A, reverseMatrixA);
  if(status != MAT_OK)
  {
    return status;
  }

  status = multiMatrix(reverseMatrixA, B, X);
  if(status != MAT_OK)
  {
    return status;
  }

  return MAT_OK;
}


static double maxElementMatrix(const Matrix *matrix)
{
  double maxElement = matrix->ptr[0];

  for(size_t pos = 1; pos < matrix->len; pos++)
  {
    maxElement = (matrix->ptr[pos] > maxElement) ? matrix->ptr[pos] : maxElement;
  }
  
  return maxElement;
}


static long long int factorial(const int k)
{
  long long int fact = 1;

  for(int n = 2; n <= k; n++)
  {
    fact *= n;
  }
  
  return fact;
}


Status_t powerMatrix(Matrix *matrix, unsigned int degree)
{
  if(degree == 0)
  {
    for (size_t pos = 0; pos < matrix->len; pos++)
    {
      matrix->ptr[pos] = (pos % (matrix->cols + 1) == 0) ? 1 : 0;
    }
    return MAT_OK;
  }

  if(degree == 1)
  {
    return MAT_OK;
  }

  Matrix *TMPmatrix = NULL;
  Status_t status = mallocMatrix(matrix->rows, matrix->cols, &TMPmatrix);
  if(status != MAT_OK)
  {
    return status;
  }

  copyMatrix(matrix, TMPmatrix);

  // Perform matrix multiplication degree times
  for (unsigned int k = 1; k < degree; k++)
  {
    Matrix *result = NULL;
    status = createMatrixE(matrix->rows, matrix->cols, &result);
    if (status != MAT_OK)
    {
      freeMatrix(&TMPmatrix);
      return status;
    }

    for (size_t row = 0; row < matrix->rows; ++row)
    {
      for (size_t col = 0; col < matrix->cols; ++col)
      {
        result->ptr[row * matrix->cols + col] = 0;
        for (size_t inner = 0; inner < matrix->cols; ++inner)
        {
          result->ptr[row * matrix->cols + col] +=
            matrix->ptr[row * matrix->cols + inner] * TMPmatrix->ptr[inner * matrix->cols + col];
        }
      }
    }

    copyMatrix(result, TMPmatrix);
    freeMatrix(&result);
  }

  copyMatrix(TMPmatrix, matrix);
  freeMatrix(&TMPmatrix);

  return MAT_OK;
}


Status_t expMatrix(Matrix *matrix, Matrix **EXPmatrix, const int accuracy)
{
  if(matrix->rows != matrix->cols)
  {
    return MAT_SIZE_ERR;
  }

  if(matrix == NULL)
  {
    return MAT_EMPTY_ERR;
  }

  Status_t status = createMatrixO(matrix->rows, matrix->cols, EXPmatrix);
  if(status != MAT_OK)
  {
    return status;
  }

  Matrix *TMPmatrix = NULL;
  status = mallocMatrix(matrix->rows, matrix->cols, &TMPmatrix);
  if(status != MAT_OK)
  {
    freeMatrix(EXPmatrix);
    return status;
  }

  copyMatrix(matrix, TMPmatrix);

  double number;
  double error = pow(10, -accuracy);

  for(unsigned int k = 0; ; ++k)
  {
    if(fabs(maxElementMatrix(TMPmatrix)) < error)
    {
      break;
    }

    copyMatrix(matrix, TMPmatrix);

    status = powerMatrix(TMPmatrix, k);
    if(status != MAT_OK) {
      freeMatrix(&TMPmatrix);
      freeMatrix(EXPmatrix);
      return status;
    }

    number = 1.0 / factorial(k);
    multiplyMatrixByScalar(TMPmatrix, number, TMPmatrix);
    addMatrix(*EXPmatrix, TMPmatrix, *EXPmatrix);
  }

  freeMatrix(&TMPmatrix);

  return MAT_OK;
}
