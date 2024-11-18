#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>


struct Matrix {
    size_t cols;
    size_t rows;
    double* data;
} Matrix;

struct Matrix matrix_allocreate(const size_t cols, const size_t rows) {
    struct Matrix A = {0, 0, NULL};
    if (cols == 0 || rows == 0)return A;
    if((SIZE_MAX / A.cols) / (A.rows / sizeof(double)) == 0)return (struct Matrix) {
        0,0, NULL
    };
    A.cols = cols;
    A.rows = rows;
    A.data = (double*)malloc(cols * rows * sizeof(double));
    if(A.data == NULL)return (struct Matrix) {
        0,0, NULL
    };
    return A;
}

void matrix_free(struct Matrix *A) {
    if(A == NULL)return;
    free(A->data);
    *A = (struct Matrix) {
        0,0, NULL
    };
}


void matrix_zeros(struct Matrix A) {
    for (size_t col = 0; col < A.cols ; col++) {
        for (size_t row = 0; row < A.rows ; row++) {
            A.data[row * A.cols + col] = 0;
        }
    }
}


void matrix_random(struct Matrix A) {
    for (size_t col = 0; col < A.cols ; col++) {
        for (size_t row = 0; row < A.rows ; row++) {
            A.data[row * A.cols + col] = (double)(rand() % 5);
        }
    }
}


void matrix_print(const struct Matrix A) {
    for (size_t row = 0; row < A.rows ; row++) {
        for (size_t col = 0; col < A.cols ; col++) {
            printf("%lf ", A.data[row * A.cols + col]);
        }
        printf("\n");
    }
    printf("\n");
}

int matrix_add(const struct Matrix A,const struct Matrix B) {
    if(!((A.cols == B.cols) && (A.rows == B.rows)))return 1;

    for(size_t idx = 0; idx < A.cols * A.rows; idx++) {
        A.data[idx] += B.data[idx];
    }
    return 0;
}


int matrix_sub(const struct Matrix A,const struct Matrix B) {
    if(!((A.cols == B.cols) && (A.rows == B.rows)))return 1;

    for(size_t idx = 0; idx < A.cols * A.rows; idx++) {
        A.data[idx] -= B.data[idx];
    }
    return 0;
}


int matrix_mul(const struct Matrix A,const struct Matrix B, struct Matrix Res) {
    if(!((A.cols == B.rows)))return 1;
    for (size_t B_col = 0; B_col< B.cols; B_col++) {
        for(size_t A_row = 0; A_row < A.rows; A_row++) {
            for(size_t B_row = 0; B_row< B.rows; B_row++) {
                Res.data[A_row * Res.cols + B_col] += A.data[A_row * A.cols + B_row] * B.data[B_row * B.cols + B_col];
            }
        }
    }
    return 0;
}


int matrix_gaus_det(const struct Matrix A, double det) {
    double t;
    if(!((A.cols == A.rows)))return 1;
    for(int i = 0; i < A.cols; i++) {
        double max = fabs(A.data[i * A.cols + i]);
        int index = i;
        for(int j = i + 1; j < A.cols; j++) {
            if(max < fabs(A.data[i * A.cols + j])) {
                max = fabs(A.data[i * A.cols + j]);
                index = j;
            }
        }
        if(index != i) { //замена столбцов
            for(int j = i; j <A.cols; j++) {
                double dat = A.data[j * A.cols + i];
                A.data[j * A.cols + i] = A.data[j * A.cols + index];
                A.data[j * A.cols + index] = dat;
            }
            det *= -1;
        }
        for(int j = i + 1; j < A.cols; j++) {
            t = A.data[j * A.cols + i] / A.data[i * A.cols + i];
            A.data[j * A.cols + i] = 0;
            for(int k = i + 1; k < A.cols; k++) {
                A.data[j * A.cols + k] = A.data[j * A.cols + k] - t * A.data[i * A.cols + k];
            }
        }
    }
    matrix_print(A);
    det = 1;
    for(int i = 0; i <= A.cols - 1; i++) {
        det *= A.data[i * A.cols + i];
    }
    return 0;
}


int main () {
    double det;

    struct Matrix A;
    A = matrix_allocreate (4, 4);
    matrix_random(A);

    struct Matrix B;
    B = matrix_allocreate (3, 2);
    matrix_random(B);

    struct Matrix mul_result = matrix_allocreate(B.cols,A.rows);
    matrix_zeros(mul_result);

    matrix_print(A);
    matrix_print(B);

    if(matrix_mul(A,B,mul_result) == 1) {
        printf("The size of the matrices does not correspond to the multiplication operation");
        printf("\n");
    } else {
        matrix_print(mul_result);
    }

    printf("%f", matrix_gaus_det(A,det));




    matrix_free(&A);
    matrix_free(&B);
    matrix_free(&mul_result);
}
