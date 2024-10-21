#include <math.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <stdio.h>
#include <string.h>


typedef struct {
  double *ptr;
  size_t rows;
  size_t cols;
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
} MatrixExceptions_t;


//===== allocate memory for matrix ================================================================
MatrixExceptions_t malloc_matrix(const size_t rows, const size_t cols, Matrix *m) {
  if(m == NULL) {
    return MAT_EMPTY_ERR;
  }

  size_t matrix_mem_size = rows * cols * sizeof(double);

  // if(matrix_mem_size > sizeof(size_t)) {
  //   return MAT_ALLOC_ERR;
  // }
  
  m->rows = rows;
  m->cols = cols;
  m->ptr = (double *)malloc(matrix_mem_size);
  
  if(m->ptr == NULL) {
    return MAT_ALLOC_ERR;
  }
  
  return MAT_OK;
}


static int calculate_max_width(const Matrix m, const int accuracy) {
  int max_width = 0;

  for(size_t idx = 0; idx < m.rows * m.cols; idx++) {
    int width = snprintf(NULL, 0, "%.*f", accuracy, m.ptr[idx]);
    if (width > max_width) {
      max_width = width;
    }
  }

  return max_width;
}


//==== function for print matrix with choose accuracy of numbers in matrix ========================
MatrixExceptions_t print_matrix(const Matrix m, const int accuracy) {
  if(m.rows == 0 || m.cols == 0 || m.ptr == NULL) {
    return MAT_EMPTY_ERR;
  }

  int width = calculate_max_width(m, accuracy);
  double number;

  for (size_t idx = 0; idx < m.rows * m.cols; ++idx) {
    if (idx % (int)m.cols == 0) {
      if (idx != 0) {
        printf("|\n");
      }
      printf("| ");
    }
    number = round(m.ptr[idx] * pow(10, accuracy)) / pow(10, accuracy);
    printf("%*.*f ", width, accuracy, number);
  }
  printf("|\n\n"); // Final new line for better formatting

  return MAT_OK;
}


static double random_number(const double min, const double max, const unsigned int accuracy) {
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


//==== Create zero matrix==========================================================================
MatrixExceptions_t create_zero_matrix(const size_t rows, const size_t cols, Matrix *m) {
  if(m == NULL) {
    return MAT_EMPTY_ERR;
  }

  if(malloc_matrix(rows, cols, m) != MAT_OK) {
    return MAT_ALLOC_ERR;
  }

  memset(m->ptr, 0, m->rows * m->cols);
  
  return MAT_OK;
}


//==== Create unit matrix==========================================================================
MatrixExceptions_t create_unit_matrix(const size_t rows, const size_t cols, Matrix *m) {
  if(m == NULL) {
    return MAT_EMPTY_ERR;
  }

  if(malloc_matrix(rows, cols, m) != MAT_OK) {
    return MAT_ALLOC_ERR;
  }

  memset(m->ptr, 0, m->rows * m->cols);

  for(size_t row = 0; row < m->rows; ++row) {
    m->ptr[row * m->rows + row] = 1;
  }
  
  return MAT_OK;
}


//==== Create matrix with random numbers===========================================================
MatrixExceptions_t create_random_matrix(Matrix *m, const size_t rows, const size_t cols, const double min, const double max, const unsigned int accuracy) {
  if(m == NULL) {
    return MAT_EMPTY_ERR;
  }

  if(malloc_matrix(rows, cols, m) != MAT_OK) {
    return MAT_ALLOC_ERR;
  }

  if(max < min) {
    return MAT_RANGE_ERR;
  }

  for(size_t idx = 0; idx < m->rows * m->cols; ++idx) {
    m->ptr[idx] = random_number(min, max, accuracy);
  }
  
  return MAT_OK;
}


//==== free memory of matrix=======================================================================
void free_matrix(Matrix *m) {
  if(m != NULL) {
    if(m->ptr != NULL) {
      free(m->ptr);
      m->ptr = NULL; 
      m->cols = 0;
      m->rows = 0;
    }
    free(m);
    m = NULL;
  }
}


static void copy_matrix(const Matrix src, const Matrix dest)
{
  memcpy(dest.ptr, src.ptr, src.rows * src.cols * sizeof(double));
}


//==== A + B = C ==================================================================================
MatrixExceptions_t add_matrix(const Matrix A, const Matrix B, const Matrix C) {
  if(A.ptr == NULL || B.ptr == NULL || C.ptr == NULL) {
    return MAT_EMPTY_ERR;
  }

  if(((A.rows == B.rows) && (A.cols == B.cols)) == 0) {
    return MAT_SIZE_ERR;
  }

  for(size_t idx = 0; idx < A.rows * A.cols; ++idx) {
    C.ptr[idx] = A.ptr[idx] + B.ptr[idx];
  }

  return MAT_OK;
}


//==== A - B = C ==================================================================================
MatrixExceptions_t sub_matrix(const Matrix A, const Matrix B, const Matrix C) {
  if(A.ptr == NULL || B.ptr == NULL || C.ptr == NULL) {
    return MAT_EMPTY_ERR;
  }

  if(((A.rows == B.rows) && (A.cols == B.cols)) == 0) {
    return MAT_SIZE_ERR;
  }

  for(size_t idx = 0; idx < A.rows * A.cols; ++idx) {
    C.ptr[idx] = A.ptr[idx] - B.ptr[idx];
  }

  return MAT_OK;
}


//==== A * B = C ==================================================================================
MatrixExceptions_t multi_matrix(const Matrix A, const Matrix B, const Matrix C) {
  if((A.cols != B.rows) || (C.rows != A.rows) || (C.cols != B.cols)) {
    return MAT_SIZE_ERR;
  } 

  Matrix *tmp = malloc(sizeof(Matrix));
  MatrixExceptions_t status = malloc_matrix(C.rows, C.cols, tmp);
  if(status != MAT_OK) {
    return status;
  }
  
  for (size_t rowA = 0; rowA < A.rows; ++rowA) {
    for (size_t colB = 0; colB < B.cols; ++colB) {
      for (size_t innerDim = 0; innerDim < A.cols; ++innerDim) {
        tmp->ptr[rowA * tmp->cols + colB] += A.ptr[rowA * A.cols + innerDim] * B.ptr[innerDim * B.cols + colB];
      }
    }
  }
  
  copy_matrix(*tmp, C);
  free(tmp);
  
  return MAT_OK;
}


//==== A * b = C, "b" is a scalar number ==========================================================
MatrixExceptions_t multiply_matrix_by_scalar(const Matrix src_matrix, const double scalar, const Matrix res_matrix) {
  for (size_t idx = 0; idx < src_matrix.rows * src_matrix.cols; ++idx) {
    res_matrix.ptr[idx] = src_matrix.ptr[idx] * scalar;
  }

  return MAT_OK;
}


//==== res_matrix = src_matrix^(T) ================================================================
MatrixExceptions_t transpose_matrix(const Matrix src_matrix, const Matrix res_matrix) {
  if(src_matrix.ptr == NULL) {
    return MAT_EMPTY_ERR;
  }

  if(res_matrix.rows != src_matrix.cols || res_matrix.cols != src_matrix.rows) {
    return MAT_SIZE_ERR;
  }

  for (size_t row = 0; row < src_matrix.rows; ++row) {
    for (size_t col = 0; col < src_matrix.cols; ++col) {
      res_matrix.ptr[col * res_matrix.cols + row] = src_matrix.ptr[row * src_matrix.cols + col];
    }
  }
  
  return MAT_OK;
}


static double find_row_max_element(const Matrix M, const size_t currentRow) {
  size_t rowMaxElement = currentRow;

  for (size_t row = currentRow; row < M.rows; row++) {
    if (fabs(M.ptr[row * M.cols + currentRow]) > fabs(M.ptr[rowMaxElement * M.cols + currentRow])) {
      rowMaxElement = row;
    }
  }

  return rowMaxElement; 
}


static void swap_rows(const Matrix M, const size_t currentRow, const size_t rowMaxElement) {
  if(currentRow == rowMaxElement) {
    return;
  }

  for (size_t col = currentRow; col < M.cols; col++) {
    double temp = M.ptr[rowMaxElement * M.cols + col];
    M.ptr[rowMaxElement * M.cols + col] = M.ptr[currentRow * M.cols + col];
    M.ptr[currentRow * M.cols + col] = temp;
  }
}


static void zeroing_elements_below_diagonal(const Matrix matrix, const size_t currentRow) {
  for (size_t row = currentRow + 1; row < matrix.rows; row++) {
    // The coefficient by which the strings are multiplied
    double factor = matrix.ptr[row * matrix.cols + currentRow] / matrix.ptr[currentRow * matrix.cols + currentRow];

    for (size_t col = currentRow; col < matrix.cols; col++) {
      if (currentRow == col) {
        matrix.ptr[row * matrix.cols + col] = 0;
      } else {
        matrix.ptr[row * matrix.cols + col] -= factor * matrix.ptr[currentRow * matrix.cols + col];
      }
    }
  }
}


static double det_triangular_matrix(const Matrix M) {
  double det = 1;

  for (size_t row = 0; row < M.rows; row++) {
    det *= M.ptr[row * M.cols + row];
  }

  return det;
}


// return sign of determinant or zero if determinant = 0
static int make_zero_down_triangular_matrix(const Matrix matrix) {
  size_t rowMaxElement = 0;
  int count_of_swap = 0;

  for (size_t currentRow = 0; currentRow < matrix.rows; currentRow++) {
    // Finding the max element in the column for stability
    rowMaxElement = find_row_max_element(matrix, currentRow);
   
    if(matrix.ptr[rowMaxElement * matrix.cols + currentRow] == 0) {
      return 0; // Returns zero if the determinant is known to be zero
    }

    swap_rows(matrix, currentRow, rowMaxElement);

    if(currentRow != rowMaxElement) {
      count_of_swap++;
    }

    zeroing_elements_below_diagonal(matrix, currentRow);
  }

  return (count_of_swap % 2 != 0) ? -1 : 1;
}


//==== calculate matrix determinant by gauss method ===============================================
MatrixExceptions_t matrix_determinant_gauss_method(const Matrix matrix, double *det) {
  if(matrix.rows != matrix.cols) {
    return MAT_SIZE_ERR;
  }

  Matrix *matrixC = malloc(sizeof(Matrix));
  MatrixExceptions_t status;
  
  status = malloc_matrix(matrix.rows, matrix.cols, matrixC);
  if(status != MAT_OK) {
    return status;
  }

  copy_matrix(matrix, *matrixC);  // Copying the matrix so as not o spoil the original one
  
  int sign_of_det = make_zero_down_triangular_matrix(*matrixC);
  if(sign_of_det == 0) {
    *det = 0;  // One of the diagonal elements is zero
    return MAT_OK;
  }

  *det = sign_of_det * det_triangular_matrix(*matrixC);
 
  free_matrix(matrixC);

  return MAT_OK;
}


static void create_extend_matrix(const Matrix matrix, const Matrix Ematrix) {
  for(size_t row = 0; row < Ematrix.rows; ++row) { 
    for(size_t col = 0; col < Ematrix.cols; ++col) {
      if(col >= matrix.cols) {
        Ematrix.ptr[row * Ematrix.cols + col] = (row == col - matrix.cols) ? 1 : 0;
      } else {
        Ematrix.ptr[row * Ematrix.cols + col] = matrix.ptr[row * matrix.cols + col];
      }
    }
  }
}


static void extract_inverse_matrix(const Matrix reverse_matrix, const Matrix extend_matrix) {
  for(size_t row = 0; row < reverse_matrix.rows; row++) {
    for (size_t col = 0; col < reverse_matrix.cols; col++) {
      reverse_matrix.ptr[row * reverse_matrix.cols + col] = extend_matrix.ptr[row * extend_matrix.cols + reverse_matrix.cols + col];
    }
  }
}


static void zeroing_elements_above_diagonal(const Matrix matrix, const size_t current_row) {
  for (size_t row = 0; row < current_row; row++) {
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


static void normalize_diagonal(const Matrix extend_matrix,const size_t currentRow) {
  double diagElement = extend_matrix.ptr[currentRow * extend_matrix.cols + currentRow];
  for (size_t col = 0; col < extend_matrix.cols; col++) {
    extend_matrix.ptr[currentRow * extend_matrix.cols + col] /= diagElement;
  }
}


static int transform_extend_matrix(const Matrix extend_matrix) {
  size_t rowMaxElement = 0;

  for (size_t currentRow = 0; currentRow < extend_matrix.rows; currentRow++) {
    // Finding the max element in the column for stability
    rowMaxElement = find_row_max_element(extend_matrix, currentRow);

    if(extend_matrix.ptr[rowMaxElement * extend_matrix.cols + currentRow] == 0) {
    print_matrix(extend_matrix, 3);
      return 0;
    }

    swap_rows(extend_matrix, currentRow, rowMaxElement);

    zeroing_elements_below_diagonal(extend_matrix, currentRow);

    normalize_diagonal(extend_matrix, currentRow);

    zeroing_elements_above_diagonal(extend_matrix, currentRow);
  }

  return 1;
}
                                
                                
//==== function to calculate reverse matrix =======================================================
MatrixExceptions_t reverse_matrix(const Matrix matrix, const Matrix reverse_matrix) {
  if(matrix.rows != matrix.cols) {
    return MAT_SIZE_ERR;
  }

  if((reverse_matrix.rows != matrix.rows) && (reverse_matrix.cols != matrix.cols)) {
    return MAT_SIZE_ERR;
  }

  Matrix *extend_matrix = malloc(sizeof(Matrix));
  MatrixExceptions_t status = malloc_matrix(matrix.rows, matrix.cols * 2, extend_matrix);
  
  if(status != MAT_OK) {
    return status;
  }

  create_extend_matrix(matrix, *extend_matrix);

  if(transform_extend_matrix(*extend_matrix) == 0) {
    return MAT_DET_ERR;
  }

  extract_inverse_matrix(reverse_matrix, *extend_matrix);

  free_matrix(extend_matrix);

  return MAT_OK;
} 

//==== Function to solve matrix equation A * X = B ================================================
MatrixExceptions_t solve_matrix_equation(const Matrix A, const Matrix B, const Matrix X) {
  if(((A.rows == B.rows) && (A.rows == X.rows) && (B.cols == 1) && (X.cols == 1)) == 0) {
    return MAT_SIZE_ERR;
  }

  if(A.cols > X.rows) {
    return MAT_SOLVE_ERR;
  }

  Matrix *reverse_matrixA = malloc(sizeof(Matrix));
  MatrixExceptions_t status = malloc_matrix(A.rows, A.cols, reverse_matrixA);
  
  if(status != MAT_OK) {
    return status;
  }

  status = reverse_matrix(A, *reverse_matrixA);
  if(status != MAT_OK) {
    return status;
  }
  
  print_matrix(*reverse_matrixA, 3);
  status = multi_matrix(*reverse_matrixA, B, X);
  if(status != MAT_OK)
  {
    return status;
  }

  return MAT_OK;
}


static double max_element_matrix(const Matrix matrix) {
  double max_element = matrix.ptr[0];

  for(size_t idx = 1; idx < matrix.rows * matrix.cols; idx++) {
    max_element = (matrix.ptr[idx] > max_element) ? matrix.ptr[idx] : max_element;
  }
  
  return max_element;
}


static long long int factorial(const int k) {
  long long int fact = 1;

  for(int n = 2; n <= k; n++) {
    fact *= n;
  }
  
  return fact;
}


MatrixExceptions_t powerMatrix(const Matrix matrix, const unsigned int degree) {
  if(degree == 0) {
    for (size_t idx = 0; idx < matrix.rows * matrix.cols; idx++) {
      matrix.ptr[idx] = (idx % (matrix.cols + 1) == 0) ? 1 : 0;
    }
    return MAT_OK;
  }

  if(degree == 1) {
    return MAT_OK;
  }

  Matrix *TMPmatrix = malloc(sizeof(Matrix));
  MatrixExceptions_t status = malloc_matrix(matrix.rows, matrix.cols, TMPmatrix);

  if(status != MAT_OK) {
    return status;
  }

  copy_matrix(matrix, *TMPmatrix);

  // Perform matrix multiplication degree times
  for (unsigned int k = 1; k < degree; k++) {
    Matrix *result = NULL;
    status = create_unit_matrix(matrix.rows, matrix.cols, result);
    if (status != MAT_OK) {
      free_matrix(TMPmatrix);
      return status;
    }

    for (size_t row = 0; row < matrix.rows; ++row) {
      for (size_t col = 0; col < matrix.cols; ++col) {
        result->ptr[row * matrix.cols + col] = 0;
        for (size_t inner = 0; inner < matrix.cols; ++inner) {
          result->ptr[row * matrix.cols + col] +=
            matrix.ptr[row * matrix.cols + inner] * TMPmatrix->ptr[inner * matrix.cols + col];
        }
      }
    }

    copy_matrix(*result, *TMPmatrix);
    free_matrix(result);
  }

  copy_matrix(*TMPmatrix, matrix);
  free_matrix(TMPmatrix);

  return MAT_OK;
}


//==== calculate exponent of matrix ===============================================================
MatrixExceptions_t expMatrix(const Matrix matrix, Matrix *EXPmatrix, const int accuracy) {
  if(matrix.rows != matrix.cols) {
    return MAT_SIZE_ERR;
  }

  if(matrix.ptr == NULL) {
    return MAT_EMPTY_ERR;
  }

  MatrixExceptions_t status = create_unit_matrix(matrix.rows, matrix.cols, EXPmatrix);
  if(status != MAT_OK) {
    return status;
  }

  Matrix *TMPmatrix = malloc(sizeof(Matrix));
  status = malloc_matrix(matrix.rows, matrix.cols, TMPmatrix);
  if(status != MAT_OK) {
    free_matrix(EXPmatrix);
    return status;
  }

  copy_matrix(matrix, *TMPmatrix);
  add_matrix(matrix, *EXPmatrix, *EXPmatrix);
  
  double number = 1.0;
  double error = pow(10, -accuracy);

  for(unsigned int k = 2; ; ++k) {
    if(fabs(max_element_matrix(*TMPmatrix)) < error) {
      break;
    }
   
    multi_matrix(*TMPmatrix, matrix, *TMPmatrix);
    number *= 1.0 / k;
    multiply_matrix_by_scalar(*TMPmatrix, number, *TMPmatrix);
    add_matrix(*EXPmatrix, *TMPmatrix, *EXPmatrix);
    multiply_matrix_by_scalar(*TMPmatrix, 1.0 / number, *TMPmatrix);
  }

  free_matrix(TMPmatrix);

  return MAT_OK;
}
