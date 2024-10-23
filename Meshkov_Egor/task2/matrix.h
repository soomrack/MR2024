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


MatrixExceptions_t malloc_matrix(size_t rows, size_t cols, Matrix *m);

MatrixExceptions_t create_zero_matrix(size_t rows, size_t cols, Matrix *m);

MatrixExceptions_t create_unit_matrix(size_t rows, size_t cols, Matrix *m);

MatrixExceptions_t create_random_matrix(Matrix *m, size_t rows, size_t cols, double min, double max, unsigned int accuracy);

void free_matrix(Matrix *m);

MatrixExceptions_t print_matrix(const Matrix m, const int accuracy);

MatrixExceptions_t add_matrix(const Matrix A, const Matrix B, const Matrix C);

MatrixExceptions_t sub_matrix(const Matrix A, const Matrix B, const Matrix C);

MatrixExceptions_t multi_matrix(const Matrix A, const Matrix B, const Matrix C);

MatrixExceptions_t transpose_matrix(const Matrix src_matrix, const Matrix res_matrix);

MatrixExceptions_t multiply_matrix_by_scalar(const Matrix src_matrix, double scalar, Matrix res_matrix);

MatrixExceptions_t matrix_determinant_gauss_method(const Matrix matrix, double *det);

MatrixExceptions_t reverse_matrix(const Matrix matrix, const Matrix reverse_matrix);

MatrixExceptions_t solve_matrix_equation(const Matrix A, const Matrix B, const Matrix X);

MatrixExceptions_t powerMatrix(const Matrix matrix, const unsigned int degree);

MatrixExceptions_t expMatrix(const Matrix matrix, Matrix *EXPmatrix, const int accuracy);
