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

double ADD_VAL[9] = { 11.0 , 13.0 , 15.0,
                     17.0 , 19.0 , 21.0,
                     23.0 , 25.0 , 27.0 };
Matrix MAT_ADD = { 3, 3, ADD_VAL };

double SUB_VAL[9] = { -9.0, -9.0 , -9.0,
                     -9.0 , -9.0 , -9.0,
                     -9.0 , -9.0 , -9.0 };
Matrix MAT_SUB = { 3, 3, SUB_VAL };

double MUL_VAL[9] = { 84.0 , 90.0 , 96.0,
                     201.0 , 216.0 , 231.0,
                     318.0 , 342.0 , 366.0 };
Matrix MAT_MUL = { 3, 3, MUL_VAL };

double MAT_VAL[9] = { 1.0 , 12.0, 31.0,
                     2.0 , 45.0 , 122.0,
                     4.0 , 4.0 , 33.0 };
Matrix MAT_DET = { 3, 3, MAT_VAL };

double DET_VAL = 729.0;

double EXP_VAL[9] = { 1118906.699 , 1374815.063 , 1630724.426 ,
                     2533881.042 , 3113415.031 , 3692947.021,
                     3948856.384 , 4852013.000 , 5755170.615 };
Matrix MAT_EXP = { 3,3,EXP_VAL };

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

void check_add() {
    Matrix A = create_zero_matrix(3, 3);
    double A_VAL[9] = { 1.0 , 2.0 , 3.0 ,
                         4.0 , 5.0 , 6.0 ,
                         7.0 , 8.0 , 9.0 };
    fill_matrix_val(&A, A_VAL);
    Matrix B = create_zero_matrix(3, 3);
    double B_VAL[9] = { 10.0 , 11.0 , 12.0 ,
                       13.0 , 14.0 , 15.0 ,
                       16.0 , 17.0 , 18.0 };
    fill_matrix_val(&B, B_VAL);
    Matrix res;
    res = matrix_add(A, B);
    if (!is_mat_equal_tst(res, MAT_ADD)) {
        printf("ERROR: ADDITION INCORRECT\n");
        free_mat(&A);
        free_mat(&B);
        free_mat(&res);
        return;
    }
    free_mat(&A);
    free_mat(&B);
    free_mat(&res);
    printf("ADDITION CORRECT\n");
}


void check_sub() {
    Matrix A = create_zero_matrix(3, 3);
    double A_VAL[9] = { 1.0 , 2.0 , 3.0 ,
                         4.0 , 5.0 , 6.0 ,
                         7.0 , 8.0 , 9.0 };
    fill_matrix_val(&A, A_VAL);
    Matrix B = create_zero_matrix(3, 3);
    double B_VAL[9] = { 10.0 , 11.0 , 12.0 ,
                       13.0 , 14.0 , 15.0 ,
                       16.0 , 17.0 , 18.0 };
    fill_matrix_val(&B, B_VAL);
    Matrix res;
    res = matrix_subt(A, B);
    if (!is_mat_equal_tst(res, MAT_SUB)) {
        printf("ERROR: SUBTRACTION INCORRECT\n");
        free_mat(&A);
        free_mat(&B);
        free_mat(&res);
        return;
    }
    free_mat(&A);
    free_mat(&B);
    free_mat(&res);
    printf("SUBTRACTION CORRECT\n");
}

void check_mult() {
    Matrix A = create_zero_matrix(3, 3);
    double A_VAL[9] = { 1.0 , 2.0 , 3.0 ,
                         4.0 , 5.0 , 6.0 ,
                         7.0 , 8.0 , 9.0 };
    fill_matrix_val(&A, A_VAL);
    Matrix B = create_zero_matrix(3, 3);
    double B_VAL[9] = { 10.0 , 11.0 , 12.0 ,
                       13.0 , 14.0 , 15.0 ,
                       16.0 , 17.0 , 18.0 };
    fill_matrix_val(&B, B_VAL);
    Matrix res;
    res = matrix_mult(A, B);
    if (!is_mat_equal_tst(res, MAT_MUL)) {
        printf("ERROR: MULTIPLICATION INCORRECT\n");
        free_mat(&A);
        free_mat(&B);
        free_mat(&res);
        return;
    }
    free_mat(&A);
    free_mat(&B);
    free_mat(&res);
    printf("MULTIPLICATION CORRECT\n");
}

void check_det() {
    Matrix A = copy_mat(MAT_DET);
    double det_a = matrix_det(A);
    if (abs(det_a - DET_VAL) > EQUAL_TEST_ACCURACY) {
        printf("ERROR: DETERMINANT INCORRECT\n");
        //printf("%0.2f",det_a);
        free_mat(&A);
        return;
    }
    free_mat(&A);
    printf("DETERMINANT CORRECT\n");
}

void check_exp() {
    Matrix A = create_zero_matrix(3, 3);
    double A_VAL[9] = { 1.0 , 2.0 , 3.0 ,
                         4.0 , 5.0 , 6.0 ,
                         7.0 , 8.0 , 9.0 };
    fill_matrix_val(&A, A_VAL);
    Matrix res = matrix_exp(A);
    if (!is_mat_equal_tst(res, MAT_EXP)) {
        printf("ERROR: EXPONENT INCORRECT\n");
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
    srand(time(NULL)); // Initialize random seed
    check_add();
    check_sub();
    check_mult();
    check_det();
    check_exp();
    return 0;
}
