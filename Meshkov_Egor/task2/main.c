#include <stdio.h>
#include <stddef.h>
#include "matrix.h"
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  int rows = atoi(argv[1]), cols = atoi(argv[2]);

  srand(time(NULL) + getpid());
  
  Matrix matrixA = createMatrixR(rows, cols, atoi(argv[3]), atoi(argv[4]), 3);
  Matrix matrixB = createMatrixR(rows, 1, atoi(argv[3]), atoi(argv[4]), 3);

  printMatrix(matrixA, 3);
  printMatrix(matrixB, 3);

  Matrix matrixC = solveMatrixEquation(matrixA, matrixB);

  printMatrix(matrixC, 6);

  freeMatrix(matrixA);
  freeMatrix(matrixB);
  freeMatrix(matrixC);

  return 0;
}
