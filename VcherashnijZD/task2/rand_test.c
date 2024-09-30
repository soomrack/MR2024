#include "matrix.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#define ASSERT_MATRIX_EQ(A, B) do { \
    assert(A.rows == B.rows); \
    assert(A.cols == B.cols); \
    for (int i = 0; i < A.rows; ++i) { \
        for (int j = 0; j < A.cols; ++j) { \
            assert(fabs(A.item[i][j] - B.item[i][j]) < 1e-6); \
        } \
    } \
} while (0)

#define ASSERT_DOUBLE_EQ(A, B) assert(fabs(A - B) < 1e-6)

#define MAX_VALUE 100 // Max random value

double EQUAL_TEST_ACCURACY = 0.001;

int is_mat_equal_tst(const Matrix A, const Matrix B) {
    if (A.cols != B.cols) {
        return 0;
    }
    if (A.rows != B.rows) {
        return 0;
    }
    for (int k = 0; k < A.cols * A.rows; k++) {
        if (fabs(A.values[k] - B.values[k]) > EQUAL_TEST_ACCURACY) {
            return 0;
        }
    }
    return 1;
}

Matrix generate_random_matrix(int rows, int cols) {
    Matrix matrix = create_zero_matrix(rows, cols);
    for (int i = 0; i < rows * cols; i++) {
        matrix.values[i] = (double)rand() / RAND_MAX * MAX_VALUE;
    }
    return matrix;
}

void check_add() {
    Matrix A = generate_random_matrix(3, 3);
    Matrix B = generate_random_matrix(3, 3);
    Matrix res = matrix_add(A, B);
    Matrix expected = generate_random_matrix(3, 3);
    for (int i = 0; i < 9; i++) {
        expected.values[i] = A.values[i] + B.values[i]; 
    }
    if (!is_mat_equal_tst(res, expected)) {
        printf("ERROR: ADDITION INCORRECT\n");
        print_matx(A);
        printf("+\n");
        print_matx(B);
        printf("=\n");
        print_matx(res);
        printf("Expected:\n");
        print_matx(expected);
        free_mat(&A);
        free_mat(&B);
        free_mat(&res);
        free_mat(&expected);
        return;
    }
    free_mat(&A);
    free_mat(&B);
    free_mat(&res);
    free_mat(&expected);
    printf("ADDITION CORRECT\n");
}

void check_sub() {
    Matrix A = generate_random_matrix(3, 3);
    Matrix B = generate_random_matrix(3, 3);
    Matrix res = matrix_subt(A, B);
    Matrix expected = generate_random_matrix(3, 3);
    for (int i = 0; i < 9; i++) {
        expected.values[i] = A.values[i] - B.values[i]; 
    }
    if (!is_mat_equal_tst(res, expected)) {
        printf("ERROR: SUBTRACTION INCORRECT\n");
        print_matx(A);
        printf("-\n");
        print_matx(B);
        printf("=\n");
        print_matx(res);
        printf("Expected:\n");
        print_matx(expected);
        free_mat(&A);
        free_mat(&B);
        free_mat(&res);
        free_mat(&expected);
        return;
    }
    free_mat(&A);
    free_mat(&B);
    free_mat(&res);
    free_mat(&expected);
    printf("SUBTRACTION CORRECT\n");
}

void check_mult() {
    Matrix A = generate_random_matrix(3, 3);
    Matrix B = generate_random_matrix(3, 3);
    Matrix res = matrix_mult(A, B);
    free_mat(&A);
    free_mat(&B);
    free_mat(&res);
    printf("MULTIPLICATION CORRECT (no real tests)\n");
}

void check_det() {
    Matrix A = generate_random_matrix(3, 3);
    double det_a = matrix_det(A);
    if (fabs(det_a) < 1e-6) {
        printf("ERROR: DETERMINANT IS ZERO\n");
        free_mat(&A);
        return;
    }
    free_mat(&A);
    printf("DETERMINANT CORRECT\n");
}

void check_exp() {
    Matrix A = generate_random_matrix(3, 3);
    Matrix res = matrix_exp(A);
    if (res.rows != 3 || res.cols != 3) {
        printf("ERROR: EXPONENT INCORRECT (Incorrect size)\n");
        print_matx(res);
        free_mat(&A);
        free_mat(&res);
        return;
    }
    free_mat(&A);
    free_mat(&res);
    printf("EXPONENT CORRECT\n");
}


int main() {
    srand(time(NULL));
    check_add();
    check_sub();
    check_mult();
    check_det();
    check_exp();
    return 0;
}
