#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stddef.h>

typedef double Element;
struct Mat {
    size_t cols;
    size_t rows;
    Element* data;
};

const struct Mat NULL_MAT = { 0, 0, NULL };

enum MatException { ERR, WARN, INFO, DEBUG };
typedef enum MatException MatException;

void mat_error(const enum MatException level, const char* msg) {
    if (level == ERR) {
        fprintf(stderr, "ERROR: %s\n", msg);
    }
    if (level == WARN) {
        fprintf(stderr, "WARNING: %s\n", msg);
    }
    if (level == INFO) {
        fprintf(stderr, "INFO: %s\n", msg);
    }
    if (level == DEBUG) {
        fprintf(stderr, "DEBUG: %s\n", msg);
    }
}

struct Mat mat_create(const size_t cols, const size_t rows) {
    if (cols == 0 || rows == 0) {
        struct Mat M = { cols, rows, NULL };
        return M;
    }

    if (rows >= SIZE_MAX / sizeof(Element) / cols) return NULL_MAT;
    struct Mat M = { cols, rows, NULL };
    size_t total = sizeof(Element) * M.cols * M.rows;
    M.data = (Element*)malloc(total);

    if (M.data == NULL) return NULL_MAT;
    return M;
}

void mat_zero(struct Mat M) {
    if (M.data == NULL) return;
    memset(M.data, 0, sizeof(Element) * M.cols * M.rows);
}

void mat_identity(struct Mat M) {
    mat_zero(M);
    for (size_t idx = 0; idx < M.cols * M.rows; idx += M.cols + 1)
        M.data[idx] = 1;
}

void mat_delete(struct Mat* M) {
    if (M == NULL) return;
    free(M->data);
    M->rows = 0;
    M->cols = 0;
    M->data = NULL;
}

void mat_fill(struct Mat* M, const Element values[]) {
    if (M == NULL || values == NULL) {
        return;
    }

    size_t num_elements = M->rows * M->cols;

    if (num_elements == 0 || M->data == NULL) {
        return;
    }

    memcpy(M->data, values, num_elements * sizeof(Element));
}

struct Mat mat_multiply(const struct Mat A, const struct Mat B) {
    if (A.cols != B.rows) {
        mat_error(ERR, "Невозможно умножить матрицы заданных размеров.");
        return NULL_MAT;
    }

    struct Mat result = mat_create(B.cols, A.rows);
    Element sum;

    for (size_t row = 0; row < result.rows; row++) {
        for (size_t col = 0; col < result.cols; col++) {
            sum = 0;
            for (size_t idx = 0; idx < A.cols; idx++) {
                sum += A.data[row * A.cols + idx] * B.data[idx * B.cols + col];
            }
            result.data[row * result.cols + col] = sum;
        }
    }

    return result;
}

struct Mat mat_subtract(const struct Mat A, const struct Mat B) {
    if (A.cols != B.cols || A.rows != B.rows) {
        mat_error(ERR, "Невозможно вычитать матрицы разных размеров.");
        return NULL_MAT;
    }

    struct Mat result = mat_create(A.cols, A.rows);
    for (size_t idx = 0; idx < A.cols * A.rows; ++idx) {
        result.data[idx] = A.data[idx] - B.data[idx];
    }
    return result;
}

struct Mat mat_add(const struct Mat A, const struct Mat B) {
    if (A.cols != B.cols || A.rows != B.rows) {
        mat_error(ERR, "Невозможно добавить матрицы разных размеров.");
        return NULL_MAT;
    }

    struct Mat result = mat_create(A.cols, A.rows);
    for (size_t idx = 0; idx < A.cols * A.rows; ++idx) {
        result.data[idx] = A.data[idx] + B.data[idx];
    }
    return result;
}

void mat_transpose(struct Mat A, struct Mat T) {
    if (T.rows != A.cols || T.cols != A.rows) {
        mat_error(ERR, "Размеры матрицы T не соответствуют размерам транспонированной матрицы A.");
        return;
    }
    for (size_t row = 0; row < A.rows; row++) {
        for (size_t col = 0; col < A.cols; col++) {
            T.data[col * A.rows + row] = A.data[row * A.cols + col];
        }
    }
}

double mat_gauss_determinant(struct Mat A) {
    if (A.cols != A.rows || A.rows == 0) {
        return NAN;
    }

    struct Mat temp = mat_create(A.cols, A.rows);
    if (temp.data == NULL) {
        return NAN;
    }
    memcpy(temp.data, A.data, sizeof(Element) * A.cols * A.rows);

    double det = 1;
    int sign = 1;

    for (size_t i = 0; i < A.rows; i++) {
        if (temp.data[i * A.cols + i] == 0) {
            size_t j = i + 1;
            while (j < A.rows && temp.data[j * A.cols + i] == 0) {
                j++;
            }
            if (j == A.rows) {
                mat_delete(&temp);
                return 0;
            }
            for (size_t k = 0; k < A.cols; k++) {
                Element t = temp.data[i * A.cols + k];
                temp.data[i * A.cols + k] = temp.data[j * A.cols + k];
                temp.data[j * A.cols + k] = t;
            }
            sign = -sign;
        }
        for (size_t j = i + 1; j < A.rows; j++) {
            double factor = temp.data[j * A.cols + i] / temp.data[i * A.cols + i];
            for (size_t k = i; k < A.cols; k++) {
                temp.data[j * A.cols + k] -= factor * temp.data[i * A.cols + k];
            }
        }
    }

    for (size_t i = 0; i < A.rows; i++) {
        det *= temp.data[i * A.cols + i];
    }

    mat_delete(&temp);
    return sign * det;
}

void mat_print_determinant(struct Mat A, const char* text) {
    printf("%s\n", text);
    printf("%f\n", mat_gauss_determinant(A));
    printf("\n");
}

struct Mat mat_multiply_by_scalar(const struct Mat A, const double scalar) {
    if (A.data == NULL)  return NULL_MAT;

    struct Mat result = mat_create(A.cols, A.rows);

    if (result.data == NULL) return NULL_MAT;

    for (size_t idx = 0; idx < result.cols * result.rows; ++idx) {
        result.data[idx] = A.data[idx] * scalar;
    }

    return result;
}

struct Mat mat_inverse(const struct Mat B) {
    if (B.cols != B.rows) {
        mat_error(ERR, "Матрица не является квадратной.");
        return NULL_MAT;
    }

    double det = mat_gauss_determinant(B);
    if (det == 0) {
        printf("Error: Матрица не имеет обратной матрицы, определитель равен 0.\n");
        return NULL_MAT;
    }

    struct Mat Inv = mat_create(B.rows, B.cols);
    if (Inv.data == NULL) return NULL_MAT;

    struct Mat temp = mat_create(B.rows, B.cols * 2);
    for (size_t i = 0; i < B.rows; i++) {
        for (size_t j = 0; j < B.cols; j++) {
            temp.data[i * temp.cols + j] = B.data[i * B.cols + j];
        }
        temp.data[i * temp.cols + B.cols + i] = 1;
    }

    for (size_t i = 0; i < B.rows; i++) {
        double factor = temp.data[i * temp.cols + i];
        for (size_t j = 0; j < temp.cols; j++) {
            temp.data[i * temp.cols + j] /= factor;
        }
        for (size_t j = 0; j < B.rows; j++) {
            if (i != j) {
                factor = temp.data[j * temp.cols + i];
                for (size_t k = 0; k < temp.cols; k++) {
                    temp.data[j * temp.cols + k] -= factor * temp.data[i * temp.cols + k];
                }
            }
        }
    }

    for (size_t i = 0; i < B.rows; i++) {
        for (size_t j = 0; j < B.cols; j++) {
            Inv.data[i * B.cols + j] = temp.data[i * temp.cols + B.cols + j];
        }
    }

    mat_delete(&temp);
    return Inv;
}

struct Mat mat_exponent(const struct Mat A, const unsigned long long int n) {
    if (A.cols != A.rows) {
        mat_error(ERR, "Матрица не является квадратной.");
        return NULL_MAT;
    }

    struct Mat term = mat_create(A.rows, A.cols);
    mat_identity(term);

    struct Mat result = mat_create(A.rows, A.cols);
    mat_zero(result);

    if (n == 0) {
        mat_delete(&result);
        return term;
    }

    double factorial = 1.0;

    for (unsigned long long int idx = 1; idx <= n; idx++) {
        struct Mat temp = mat_multiply(term, A);
        mat_delete(&term);

        term = temp;
        factorial *= idx;

        struct Mat scaled = mat_multiply_by_scalar(term, 1.0 / factorial);
        mat_delete(&term);
        term = scaled;

        struct Mat added = mat_add(result, term);
        mat_delete(&result);
        result = added;
    }

    mat_delete(&term);
    return result;
}

void mat_print(const struct Mat A, const char* text) {
    printf("%s\n", text);
    for (size_t row = 0; row < A.rows; ++row) {
        printf("[");
        for (size_t col = 0; col < A.cols; ++col) {
            printf("%4.2f ", A.data[row * A.cols + col]);
        }
        printf("]\n");
    }
    printf("\n");
}

void perform_calculations() {
    Element values_A[] = { 1., 2., 3., 4. };
    Element values_B[] = { 1., 1., 2., 2. };

    struct Mat matA = mat_create(2, 2);
    struct Mat matB = mat_create(2, 2);
    struct Mat matE = mat_create(2, 2);
    mat_identity(matE);

    struct Mat matC = mat_create(2, 2);
    struct Mat matD = mat_create(2, 2);
    struct Mat matP = mat_create(2, 2);
    struct Mat matT = mat_create(2, 2);
    struct Mat matK = mat_create(2, 2);
    struct Mat matInvA = mat_inverse(matA);
    struct Mat matExp = mat_exponent(matA, 3);

    mat_fill(&matA, values_A);
    mat_fill(&matB, values_B);
    mat_fill(&matK, values_A);

    matP = mat_multiply(matA, matB);
    matD = mat_subtract(matA, matB);
    matC = mat_add(matA, matB);
    mat_transpose(matA, matT);
    matK = mat_multiply_by_scalar(matA, 2.0);

    mat_print(matA, "Matrix A");
    mat_print(matB, "Matrix B");
    mat_print(matC, "C = A + B");
    mat_print(matD, "D = A - B");
    mat_print(matP, "P = A * B");
    mat_print(matT, "T = A^T");
    mat_print(matK, "K = 2 * A");
    mat_print(matE, "Matrix E");
    mat_print_determinant(matA, "det A = ");
    mat_print(matExp, "Matrix Exp^A=");
    mat_print(matInvA, "Inverse of A");

    mat_delete(&matA);
    mat_delete(&matB);
    mat_delete(&matC);
    mat_delete(&matD);
    mat_delete(&matP);
    mat_delete(&matT);
    mat_delete(&matE);
    mat_delete(&matK);
    mat_delete(&matInvA);
    mat_delete(&matExp);
}

int main() {
    perform_calculations();
    return 0;
}
