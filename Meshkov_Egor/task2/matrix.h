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
} Status_t;

Status_t mallocMatrix(size_t rows, size_t cols, Matrix **matrix);

Status_t createMatrixO(size_t rows, size_t cols, Matrix **matrix);

Status_t createMatrixE(size_t rows, size_t cols, Matrix **matrix);

Status_t createMatrixR(Matrix **matrix, size_t rows, size_t cols, double min, double max, unsigned int accuracy);

void freeMatrix(Matrix **matrix);

void printMatrix(Matrix *matrix, int accuracy);

Status_t addMatrix(const Matrix *matrixA, const Matrix *matrixB, Matrix *matrixC);

Status_t subMatrix(const Matrix *matrixA, const Matrix *matrixB, Matrix *matrixC);

Status_t multiMatrix(Matrix *matrixA,const Matrix *matrixB, Matrix *matrixC);

Status_t transposeMatrix(const Matrix *src_matrix, Matrix *res_matrix);

Status_t multiplyMatrixByScalar(const Matrix *src_matrix, double scalar, Matrix *res_matrix);

Status_t gaussMethod(Matrix *matrix, double *det);

Status_t reverseMatrix(Matrix *matrix, Matrix *reverseMatrix);

Status_t solveMatrixEquation(Matrix *matrixA, Matrix *matrixB, Matrix *matrixX);

Status_t powerMatrix(Matrix *matrix, unsigned int degree);

Status_t expMatrix(Matrix *matrix, Matrix **EXPmatrix, const int accuracy);
