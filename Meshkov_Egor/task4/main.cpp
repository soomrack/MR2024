#include "matrix.hpp"
#include <cmath>
#include <gtest/gtest.h>
#include <random>


// Глобальные константы для размеров матриц
const size_t ROWS = 25;
const size_t COLS = 25;


// Генератор случайных чисел
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(-10.0, 10.0);


// Функция для создания случайной матрицы
MTL::Matrix create_random_matrix(size_t rows, size_t cols) {
    MTL::Matrix mat(rows, cols);
    for(size_t idx = 0; idx < rows * cols; idx++) {
        mat[idx] = dis(gen);
    }
    
    return mat;
}


// Тесты для класса Matrix
TEST(MatrixTest, Constructors) {
    using namespace MTL;
    
    Matrix A;
    EXPECT_EQ(A.is_empty(), true);
    EXPECT_EQ(A.get_rows(), 0);
    EXPECT_EQ(A.get_cols(), 0);
    
    Matrix B(ROWS, COLS);
    EXPECT_EQ(B.is_empty(), false);
    EXPECT_EQ(B.is_zeros(), true);
    EXPECT_EQ(B.get_rows(), ROWS);
    EXPECT_EQ(B.get_cols(), COLS);

    double buf[ROWS * COLS];
    for(size_t idx = 0; idx < ROWS * COLS; idx++) {
        buf[idx] = dis(gen);
    
    }
    Matrix C(buf, ROWS, COLS);
    EXPECT_EQ(C.is_empty(), false);
    EXPECT_EQ(C.get_rows(), ROWS);
    EXPECT_EQ(C.get_cols(), COLS);
    for(size_t idx = 0; idx < ROWS * COLS; idx++) {
        EXPECT_EQ(buf[idx], C[idx]);
    }
    
    double number = dis(gen);
    Matrix D(number);
    EXPECT_EQ(D.is_empty(), false);
    EXPECT_EQ(D.get_rows(), 1);
    EXPECT_EQ(D.get_cols(), 1);
    EXPECT_EQ(D[0], number);
}


TEST(MatrixTest, GetterSetterOperations) {
    MTL::Matrix mat = create_random_matrix(ROWS, COLS);

    EXPECT_EQ(mat.get_rows(), ROWS);
    EXPECT_EQ(mat.get_cols(), COLS);

    // Проверка оператора ()
    for(size_t idx = 0; idx < ROWS * COLS; idx++) {
        EXPECT_NO_THROW(mat[idx]);
    }
}


TEST(MatrixTest, ArithmeticOperations) {
    using namespace MTL;
    Matrix mat1 = create_random_matrix(ROWS, COLS);
    Matrix mat2 = create_random_matrix(ROWS, COLS);

    // Сложение
    Matrix sum = mat1 + mat2;
    for (size_t idx = 0; idx < ROWS * COLS; idx++) {
        EXPECT_DOUBLE_EQ(sum[idx], mat1[idx] + mat2[idx]);
    }

    // Вычитание
    Matrix sub = mat1 - mat2;
    for (size_t idx = 0; idx < ROWS * COLS; idx++) {
        EXPECT_DOUBLE_EQ(sub[idx], mat1[idx] - mat2[idx]);
    }

    // Умножение на число
    double scalar = dis(gen);
    Matrix scaled = mat1 * scalar;
    for (size_t idx = 0; idx < ROWS * COLS; idx++) {
        EXPECT_DOUBLE_EQ(scaled[idx], mat1[idx] * scalar);
    }

    // Умножение матриц
    Matrix mat3 = create_random_matrix(COLS, ROWS);
    Matrix prod = mat1 * mat3;
    EXPECT_EQ(prod.get_rows(), ROWS);
    EXPECT_EQ(prod.get_cols(), ROWS);
}


TEST(MatrixTest, DeterminantAndInverse) {
    using namespace MTL;
    Matrix mat = create_random_matrix(ROWS, ROWS);  // Квадратная матрица для определителя и обратной

    // Определитель
    double det = mat.determinant();
    EXPECT_TRUE(std::isfinite(det));

    // Обратная матрица
    Matrix tmp(mat);
    Matrix inv = tmp.reverse();
    Matrix identity = mat * inv;
    for (size_t row = 0; row < ROWS; ++row) {
        for (size_t col = 0; col < ROWS; ++col) {
            if (row == col) {
                EXPECT_NEAR(identity[row * ROWS + col], 1.0, 1e-6);
            } else {
                EXPECT_NEAR(identity[row * ROWS + col], 0.0, 1e-6);
            }
        }
    }
}


TEST(MatrixTest, Transpose) {
    using namespace MTL;
    Matrix mat = create_random_matrix(ROWS, COLS);

    // Транспонирование
    Matrix tmp = mat;
    Matrix transposed = tmp.transpoze();
    EXPECT_EQ(transposed.get_rows(), COLS);
    EXPECT_EQ(transposed.get_cols(), ROWS);
    for (size_t row = 0; row < ROWS; ++row) {
        for (size_t col = 0; col < COLS; ++col) {
            EXPECT_DOUBLE_EQ(transposed[col * ROWS + row], mat[row * COLS + col]);
        }
    }
} 


TEST(MatrixTest, Exponential) {
    using namespace MTL;
    Matrix mat = create_random_matrix(ROWS, COLS);

    Matrix exp_mat = mat.exp(6);
    EXPECT_EQ(exp_mat.get_rows(), ROWS);
    EXPECT_EQ(exp_mat.get_cols(), COLS);
} 


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

