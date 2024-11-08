#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include "matrix.h"
#include <stdlib.h>
#include <unistd.h>

// Вспомогательная функция для инициализации матрицы
void init_matrix(Matrix *M, size_t rows, size_t cols, double *data) {
    M->rows = rows;
    M->cols = cols;
    M->ptr = data;
}


void test_matrix_alloc() {
    Matrix M = EMPTY;
    CU_ASSERT_EQUAL(matrix_alloc(&M, 3, 3), MAT_OK);
    CU_ASSERT_PTR_NOT_NULL(M.ptr);
    CU_ASSERT_EQUAL(M.rows, 3);
    CU_ASSERT_EQUAL(M.cols, 3);
    matrix_free(&M);
}


void test_matrix_create_zero() {
    Matrix M = EMPTY;
    CU_ASSERT_EQUAL(matrix_create_zero(&M, 3, 3), MAT_OK);
    CU_ASSERT_PTR_NOT_NULL(M.ptr);
    for (size_t i = 0; i < M.rows * M.cols; ++i) {
        CU_ASSERT_DOUBLE_EQUAL(M.ptr[i], 0.0, 0.0001);
    }
    matrix_free(&M);
}


void test_matrix_create_unit() {
    Matrix M = EMPTY;
    CU_ASSERT_EQUAL(matrix_create_unit(&M, 3, 3), MAT_OK);
    CU_ASSERT_PTR_NOT_NULL(M.ptr);
    for (size_t i = 0; i < M.rows; ++i) {
        for (size_t j = 0; j < M.cols; ++j) {
            if (i == j) {
                CU_ASSERT_DOUBLE_EQUAL(M.ptr[i * M.cols + j], 1.0, 0.0001);
            } else {
                CU_ASSERT_DOUBLE_EQUAL(M.ptr[i * M.cols + j], 0.0, 0.0001);
            }
        }
    }
    matrix_free(&M);
}


void test_matrix_create_random() {
    Matrix M = EMPTY;
    CU_ASSERT_EQUAL(matrix_create_random(&M, 3, 3, 0.0, 1.0, 2), MAT_OK);
    CU_ASSERT_PTR_NOT_NULL(M.ptr);
    matrix_free(&M);
}


void test_matrix_free() {
    Matrix M = EMPTY;
    CU_ASSERT_EQUAL(matrix_alloc(&M, 3, 3), MAT_OK);
    matrix_free(&M);
    CU_ASSERT_PTR_NULL(M.ptr);
    CU_ASSERT_EQUAL(M.rows, 0);
    CU_ASSERT_EQUAL(M.cols, 0);
}


void test_matrix_copy() {
    double dataA[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    double dataB[9];
    Matrix A, B;
    init_matrix(&A, 3, 3, dataA);
    init_matrix(&B, 3, 3, dataB);
    CU_ASSERT_EQUAL(matrix_copy(B, A), MAT_OK);
    for (size_t i = 0; i < B.rows * B.cols; ++i) {
        CU_ASSERT_DOUBLE_EQUAL(B.ptr[i], dataA[i], 0.0001);
    }
}


void test_matrix_print() {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    Matrix M;
    init_matrix(&M, 3, 3, data);
    printf("\n");
    CU_ASSERT_EQUAL(matrix_print(M, 2), MAT_OK);
    printf("  ");
}


void test_matrix_add() {
    double dataA[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    double dataB[] = {9.0, 8.0, 7.0, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0};
    double dataC[] = {10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0};
    Matrix A, B, C;
    init_matrix(&A, 3, 3, dataA);
    init_matrix(&B, 3, 3, dataB);
    init_matrix(&C, 3, 3, dataC);
    CU_ASSERT_EQUAL(matrix_add(C, A, B), MAT_OK);
    for (size_t i = 0; i < C.rows * C.cols; ++i) {
        CU_ASSERT_DOUBLE_EQUAL(C.ptr[i], 10.0, 0.0001);
    }
}


void test_matrix_sub() {
    double dataA[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    double dataB[] = {9.0, 8.0, 7.0, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0};
    double dataC[] = {-8.0, -6.0, -4.0, -2.0, 0.0, 2.0, 4.0, 6.0, 8.0};
    Matrix A, B, C;
    init_matrix(&A, 3, 3, dataA);
    init_matrix(&B, 3, 3, dataB);
    init_matrix(&C, 3, 3, dataC);
    CU_ASSERT_EQUAL(matrix_sub(C, A, B), MAT_OK);
    for (size_t i = 0; i < C.rows * C.cols; ++i) {
        CU_ASSERT_DOUBLE_EQUAL(C.ptr[i], dataC[i], 0.0001);
    }
}


void test_matrix_multi() {
    double dataA[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    double dataB[] = {9.0, 8.0, 7.0, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0};
    double dataC[] = {30.0, 24.0, 18.0, 84.0, 69.0, 54.0, 138.0, 114.0, 90.0};
    Matrix A, B, C;
    init_matrix(&A, 3, 3, dataA);
    init_matrix(&B, 3, 3, dataB);
    init_matrix(&C, 3, 3, dataC);
    CU_ASSERT_EQUAL(matrix_multi(C, A, B), MAT_OK);
    for (size_t i = 0; i < C.rows * C.cols; ++i) {
        CU_ASSERT_DOUBLE_EQUAL(C.ptr[i], dataC[i], 0.0001);
    }
}


void test_matrix_transpoze() {
    double dataA[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    double dataB[] = {1.0, 4.0, 7.0, 2.0, 5.0, 8.0, 3.0, 6.0, 9.0};
    Matrix A, B;
    init_matrix(&A, 3, 3, dataA);
    init_matrix(&B, 3, 3, dataB);
    CU_ASSERT_EQUAL(matrix_transpoze(B, A), MAT_OK);
    for (size_t i = 0; i < B.rows * B.cols; ++i) {
        CU_ASSERT_DOUBLE_EQUAL(B.ptr[i], dataB[i], 0.0001);
    }
}


void test_matrix_multiply_by_scalar() {
    double dataA[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    double dataB[] = {2.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0, 18.0};
    Matrix A, B;
    init_matrix(&A, 3, 3, dataA);
    init_matrix(&B, 3, 3, dataB);
    CU_ASSERT_EQUAL(matrix_multiply_by_scalar(B, A, 2.0), MAT_OK);
    for (size_t i = 0; i < B.rows * B.cols; ++i) {
        CU_ASSERT_DOUBLE_EQUAL(B.ptr[i], dataB[i], 0.0001);
    }
}


void test_matrix_determinant_gauss_method() {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    Matrix M;
    init_matrix(&M, 3, 3, data);
    double det;
    CU_ASSERT_EQUAL(matrix_determinant_gauss_method(&det, M), MAT_OK);
    CU_ASSERT_DOUBLE_EQUAL(det, 0.0, 0.0001);
}


void test_matrix_calculate_reverse() {
    double data[] = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
    Matrix M;
    init_matrix(&M, 3, 3, data);
    Matrix R = EMPTY;
    CU_ASSERT_EQUAL(matrix_alloc(&R, 3, 3), MAT_OK);
    CU_ASSERT_EQUAL(matrix_calculate_reverse(R, M), MAT_OK);
    for (size_t i = 0; i < R.rows; ++i) {
        for (size_t j = 0; j < R.cols; ++j) {
            if (i == j) {
                CU_ASSERT_DOUBLE_EQUAL(R.ptr[i * R.cols + j], 1.0, 0.0001);
            } else {
                CU_ASSERT_DOUBLE_EQUAL(R.ptr[i * R.cols + j], 0.0, 0.0001);
            }
        }
    }
    matrix_free(&R);
}


void test_matrix_solve_equation() {
    double dataA[] = {1.0, 2.0, 3.0, 4.0, 1.0, 6.0, 7.0, 8.0, 9.0};
    double dataB[] = {1.0, 2.0, 3.0};
    double dataX[] = {0.0, 0.0, 0.333333};
    Matrix A, B, X;
    init_matrix(&A, 3, 3, dataA);
    init_matrix(&B, 3, 1, dataB);
    init_matrix(&X, 3, 1, dataX);
    CU_ASSERT_EQUAL(matrix_solve_equation(X, A, B), MAT_OK);
    for (size_t i = 0; i < X.rows * X.cols; ++i) {
        CU_ASSERT_DOUBLE_EQUAL(X.ptr[i], dataX[i], 0.0001);
    }
}


void test_matrix_power() {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    Matrix M;
    init_matrix(&M, 3, 3, data);
    CU_ASSERT_EQUAL(matrix_power(M, 2), MAT_OK);
    double expected[] = {30.0, 36.0, 42.0, 66.0, 81.0, 96.0, 102.0, 126.0, 150.0};
    for (size_t i = 0; i < M.rows * M.cols; ++i) {
        CU_ASSERT_DOUBLE_EQUAL(M.ptr[i], expected[i], 0.0001);
    }
}


void test_matrix_exponent() {
    double data[] = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
    Matrix M;
    init_matrix(&M, 3, 3, data);
    Matrix EXP = EMPTY;
    CU_ASSERT_EQUAL(matrix_exponent(&EXP, M, 4), MAT_OK);
    for (size_t i = 0; i < EXP.rows; ++i) {
        for (size_t j = 0; j < EXP.cols; ++j) {
            if (i == j) {
                CU_ASSERT_DOUBLE_EQUAL(EXP.ptr[i * EXP.cols + j], exp(1.0), 0.0001);
            } else {
                CU_ASSERT_DOUBLE_EQUAL(EXP.ptr[i * EXP.cols + j], 0.0, 0.0001);
            }
        }
    }
    matrix_free(&EXP);
}

int main() {
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("MatrixTestSuite", 0, 0);

    CU_add_test(suite, "test_matrix_alloc", test_matrix_alloc);
    CU_add_test(suite, "test_matrix_create_zero", test_matrix_create_zero);
    CU_add_test(suite, "test_matrix_create_unit", test_matrix_create_unit);
    CU_add_test(suite, "test_matrix_create_random", test_matrix_create_random);
    CU_add_test(suite, "test_matrix_free", test_matrix_free);
    CU_add_test(suite, "test_matrix_copy", test_matrix_copy);
    CU_add_test(suite, "test_matrix_print", test_matrix_print);
    CU_add_test(suite, "test_matrix_add", test_matrix_add);
    CU_add_test(suite, "test_matrix_sub", test_matrix_sub);
    CU_add_test(suite, "test_matrix_multi", test_matrix_multi);
    CU_add_test(suite, "test_matrix_transpoze", test_matrix_transpoze);
    CU_add_test(suite, "test_matrix_multiply_by_scalar", test_matrix_multiply_by_scalar);
    CU_add_test(suite, "test_matrix_determinant_gauss_method", test_matrix_determinant_gauss_method);
    CU_add_test(suite, "test_matrix_calculate_reverse", test_matrix_calculate_reverse);
    CU_add_test(suite, "test_matrix_solve_equation", test_matrix_solve_equation);
    CU_add_test(suite, "test_matrix_power", test_matrix_power);
    CU_add_test(suite, "test_matrix_exponent", test_matrix_exponent);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return 0;
}
