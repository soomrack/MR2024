#include <stdio.h>
#include <stddef.h>
#include "matrix.h"
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

int main() {

  srand(time(NULL) + getpid());

  // size_t rows = 5, cols = 5; 
  // Matrix *matrixA = NULL, *matrixB = NULL, *matrixC = NULL;
  // printf("%d\n", mallocMatrix(rows, 1, &matrixC));
  // printf("%d\n", createMatrixR(&matrixA, rows, cols, -5.0, 5.0, 3));
  // printf("%d\n", createMatrixR(&matrixB, rows, 1, -2, 2, 3));

  // printMatrix(matrixA, 3);
  // printMatrix(matrixB, 3);

  // printf("%d\n", solveMatrixEquation(matrixA, matrixB, matrixC));

  // printMatrix(matrixC, 6);

  // freeMatrix(&matrixA);
  // freeMatrix(&matrixB);
  // freeMatrix(&matrixC);

  Matrix *matrixA = NULL, *matrixB = NULL;
  createMatrixR(&matrixA, 1, 1, -5, 1, 3);
  
  printMatrix(matrixA, 3);
  
  printf("%d", expMatrix(matrixA, &matrixB, 5));

  printMatrix(matrixB, 5);

  freeMatrix(&matrixA);
  freeMatrix(&matrixB);

  return 0;
}
