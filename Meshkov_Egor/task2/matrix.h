typedef struct
{
  double *ptr;
  int rows;
  int cols;
  int len;
} Matrix;

Matrix mallocMatrix(size_t rows, size_t cols);

Matrix createMatrixO(size_t rows, size_t cols);

Matrix createMatrixE(size_t rows, size_t cols);

Matrix createMatrixR(size_t rows, size_t cols, int min, int max, int accuracy);

void freeMatrix(Matrix matrix);

void printMatrix(Matrix matrix, int accuracy);

Matrix addMatrix(Matrix matrixA, Matrix matrixB);

Matrix subMatrix(Matrix matrixA, Matrix matrixB);

Matrix multiMatrix(Matrix matrixA, Matrix matrixB);

Matrix transposeMatrix(Matrix matrix);

Matrix multiplyMatrixByScalar(Matrix matrix, double scalar);

int calculate_max_width(Matrix matrix);

double gauss_method(Matrix matrix);

Matrix reverseMatrix(Matrix matrix);

Matrix solveMatrixEquation(Matrix matrixA, Matrix matrixB);
