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

struct Mat create_mat(const size_t cols, const size_t rows) {
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

void zero_mat(struct Mat M) {
    if (M.data == NULL) return;
    memset(M.data, 0, sizeof(Element) * M.cols * M.rows);
}

void identity_mat(struct Mat M) {
    zero_mat(M);
    for (size_t idx = 0; idx < M.cols * M.rows; idx += M.cols + 1)
        M.data[idx] = 1;
}

enum MatException { ERR, WARN, INFO, DEBUG };
typedef enum MatException MatException;

void mat_error(const enum MatException level, const char* msg) {
    if (level == ERR) {
        fprintf(stderr, "ERROR: %s\n", msg);
    }
    if (level == WARN) {
        fprintf(stderr, "WARNING: %s\n", msg);
    }
}

void delete_mat(struct Mat* M) {
    if (M == NULL) return;
    free(M->data);
    M->rows = 0;
    M->cols = 0;
    M->data = NULL;
}

void fill_mat(struct Mat* M, const Element values[]) {
    if (M == NULL || values == NULL) {
        return;
    }

    size_t num_elements = M->rows * M->cols;

    if (num_elements == 0 || M->data == NULL) {
        return;
    }

    memcpy(M->data, values, num_elements * sizeof(Element));
}

struct Mat multiply_mat(const struct Mat A, const struct Mat B) {
    if (A.cols != B.rows) {
        mat_error(ERR, "Unable to multiply matrices of given sizes.");
        return NULL_MAT;
    }

    struct Mat result = create_mat(B.cols, A.rows);
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

struct Mat subtract_mat(const struct Mat A, const struct Mat B) {
    if (A.cols != B.cols || A.rows != B.rows) {
        mat_error(ERR, "Unable to subtract matrices of different sizes.");
        return NULL_MAT;
    }

    struct Mat result = create_mat(A.cols, A.rows);
    for (size_t idx = 0; idx < A.cols * A.rows; ++idx) {
        result.data[idx] = A.data[idx] - B.data[idx];
    }
    return result;
}

struct Mat add_mat(const struct Mat A, const struct Mat B) {
    if (A.cols != B.cols || A.rows != B.rows) {
        mat_error(ERR, "Unable to add matrices of different sizes.");
        return NULL_MAT;
    }

    struct Mat result = create_mat(A.cols, A.rows);
    for (size_t idx = 0; idx < A.cols * A.rows; ++idx) {
        result.data[idx] = A.data[idx] + B.data[idx];
    }
    return result;
}

void transpose_mat(struct Mat A, struct Mat T) {
    for (size_t row = 0; row < A.rows; row++) {
        for (size_t col = 0; col < A.cols; col++) {
            T.data[col * A.rows + row] = A.data[row * A.cols + col];
        }
    }
}

double gauss_determinant(struct Mat A) {
    if (A.cols != A.rows || A.rows == 0) {
        return NAN;
    }

    struct Mat temp = create_mat(A.cols, A.rows);
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
                delete_mat(&temp);
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

    delete_mat(&temp);
    return sign * det;
}

void print_determinant(struct Mat A, const char* text) {
    printf("%s\n", text);
    printf("%f\n", gauss_determinant(A));
    printf("\n");
}

struct Mat multiply_by_scalar(const struct Mat A, const double scalar) {
    if (A.data == NULL)  return NULL_MAT;

    struct Mat result = create_mat(A.cols, A.rows);

    if (result.data == NULL) return NULL_MAT;

    for (size_t idx = 0; idx < result.cols * result.rows; ++idx) {
        result.data[idx] = A.data[idx] * scalar;
    }

    return result;
}

struct Mat inverse_mat(const struct Mat B) {
    if (B.cols != B.rows) {
        mat_error(ERR, "The matrix is not square.");
        return NULL_MAT;
    }

    double det = gauss_determinant(B);
    if (det == 0) {
        printf("Error: Matrix is singular and cannot be inverted.\n");
        return NULL_MAT;
    }

    struct Mat Inv = create_mat(B.rows, B.cols);
    if (Inv.data == NULL) return NULL_MAT;

    struct Mat temp = create_mat(B.rows, B.cols * 2);
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

    delete_mat(&temp);
    return Inv;
}

struct Mat exponent_mat(const struct Mat A, const unsigned long long int n) {
    if (A.cols != A.rows) {
        mat_error(ERR, "The matrix is not square.");
        return NULL_MAT;
    }

    struct Mat term = create_mat(A.rows, A.cols);
    identity_mat(term);

    struct Mat result = create_mat(A.rows, A.cols);
    zero_mat(result);

    if (n == 0) {
        delete_mat(&result);
        return term;
    }

    double factorial = 1.0;

    for (unsigned long long int idx = 1; idx <= n; idx++) {
        struct Mat temp = multiply_mat(term, A);
        delete_mat(&term);

        term = temp;
        factorial *= idx;

        term = multiply_by_scalar(term, 1.0 / factorial);

        struct Mat added = add_mat(result, term);

        delete_mat(&result);
        result = added;
    }

    delete_mat(&term);
    return result;
}

void print_mat(const struct Mat A, const char* text) {
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

    struct Mat A = create_mat(2, 2);
    struct Mat B = create_mat(2, 2);
    struct Mat E = create_mat(2, 2);
    identity_mat(E);

    struct Mat C = create_mat(2, 2);
    struct Mat D = create_mat(2, 2);
    struct Mat P = create_mat(2, 2);
    struct Mat T = create_mat(2, 2);
    struct Mat K = create_mat(2, 2);
    struct Mat InvA = inverse_mat(A);
    struct Mat Exp = exponent_mat(A, 3);

    fill_mat(&A, values_A);
    fill_mat(&B, values_B);
    fill_mat(&K, values_A);

    P = multiply_mat(A, B);
    D = subtract_mat(A, B);
    C = add_mat(A, B);
    transpose_mat(A, T);
    K = multiply_by_scalar(A, 2.0);

    print_mat(A, "Matrix A");
    print_mat(B, "Matrix B");
    print_mat(C, "C = A + B");
    print_mat(D, "D = A - B");
    print_mat(P, "P = A * B");
    print_mat(T, "T = A^T");
    print_mat(K, "K = 2 * A");
    print_mat(E, "Matrix E");
    print_determinant(A, "det A = ");
    print_mat(Exp, "Matrix Exp^A=");
    print_mat(InvA, "Inverse of A");

    delete_mat(&A);
    delete_mat(&B);
    delete_mat(&C);
    delete_mat(&D);
    delete_mat(&P);
    delete_mat(&T);
    delete_mat(&E);
    delete_mat(&K);
    delete_mat(&InvA);
    delete_mat(&Exp);
}

int main() {
    perform_calculations();
    return 0;
}
