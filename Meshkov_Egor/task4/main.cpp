#include "matrix.hpp"
#include <cmath>
#include <gtest/gtest.h>
#include <random>


// Глобальные константы для размеров матриц
const size_t ROWS = 100;
const size_t COLS = 100;


// Генератор случайных чисел
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(-10.0, 10.0);


// Функция для создания случайной матрицы
MTL::Matrix create_random_matrix(size_t rows, size_t cols) {
    MTL::Matrix mat(rows, cols);
    for(size_t idx = 0; idx < rows * cols; idx++) {
        mat(idx) = dis(gen);
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
        EXPECT_EQ(buf[idx], C(idx));
    }
    
    double number = dis(gen);
    Matrix D(number);
    EXPECT_EQ(D.is_empty(), false);
    EXPECT_EQ(D.get_rows(), 1);
    EXPECT_EQ(D.get_cols(), 1);
    EXPECT_EQ(D(0), number);
}


TEST(MatrixTest, GetterSetterOperations) {
    MTL::Matrix mat = create_random_matrix(ROWS, COLS);

    EXPECT_EQ(mat.get_rows(), ROWS);
    EXPECT_EQ(mat.get_cols(), COLS);

    // Проверка оператора ()
    for(size_t idx = 0; idx < ROWS * COLS; idx++) {
        EXPECT_NO_THROW(mat(idx));
        EXPECT_NO_THROW(mat(idx / ROWS, idx % COLS));
    }
}


TEST(MatrixTest, ArithmeticOperations) {
    using namespace MTL;
    Matrix mat1 = create_random_matrix(ROWS, COLS);
    Matrix mat2 = create_random_matrix(ROWS, COLS);

    // Сложение
    Matrix sum = mat1 + mat2;
    for (size_t idx = 0; idx < ROWS * COLS; idx++) {
        EXPECT_DOUBLE_EQ(sum(idx), mat1(idx) + mat2(idx));
    }

    // Вычитание
    Matrix sub = mat1 - mat2;
    for (size_t idx = 0; idx < ROWS * COLS; idx++) {
        EXPECT_DOUBLE_EQ(sub(idx), mat1(idx) - mat2(idx));
    }

    // Умножение на число
    double scalar = dis(gen);
    Matrix scaled = mat1 * scalar;
    for (size_t idx = 0; idx < ROWS * COLS; idx++) {
        EXPECT_DOUBLE_EQ(scaled(idx), mat1(idx) * scalar);
    }

    // Умножение матриц
    Matrix mat3 = create_random_matrix(COLS, ROWS);
    Matrix prod = mat1 * mat3;
    EXPECT_EQ(prod.get_rows(), ROWS);
    EXPECT_EQ(prod.get_cols(), ROWS);
    //mat3.print();
}


TEST(MatrixTest, DeterminantAndInverse) {
    using namespace MTL;
    Matrix mat = create_random_matrix(ROWS, ROWS);  // Квадратная матрица для определителя и обратной

    // Определитель
    double det = mat.determinant();
    //std::cout << det << std::endl;
    EXPECT_TRUE(std::isfinite(det));

    // Обратная матрица
    Matrix inv = reverse(mat);
    //inv.print();
    Matrix identity = mat * inv;
    for (size_t i = 0; i < ROWS; ++i) {
        for (size_t j = 0; j < ROWS; ++j) {
            if (i == j) {
                EXPECT_NEAR(identity(i, j), 1.0, 1e-6);
            } else {
                EXPECT_NEAR(identity(i, j), 0.0, 1e-6);
            }
        }
    }
}

TEST(MatrixTest, TransposeAndExponential) {
    using namespace MTL;
    Matrix mat = create_random_matrix(ROWS, COLS);

    // Транспонирование
    Matrix transposed = transpoze(mat);
    EXPECT_EQ(transposed.get_rows(), COLS);
    EXPECT_EQ(transposed.get_cols(), ROWS);
    for (size_t row = 0; row < ROWS; ++row) {
        for (size_t col = 0; col < COLS; ++col) {
            EXPECT_DOUBLE_EQ(transposed(col, row), mat(row, col));
        }
    }

    // Экспонента
    Matrix exp_mat = exp(mat, 6);
    //exp_mat.print();
    EXPECT_EQ(exp_mat.get_rows(), ROWS);
    EXPECT_EQ(exp_mat.get_cols(), COLS);
}


// Тесты для класса Matrix_unit
TEST(MatrixUnitTest, BasicOperations) {
    MTL::Matrix_unit unit_mat(ROWS, COLS);

    // Проверка размеров
    EXPECT_EQ(unit_mat.get_rows(), ROWS);
    EXPECT_EQ(unit_mat.get_cols(), COLS);
    
    size_t diag_dimention = 0;
    double epsilon = pow(10, -9);

    for(size_t idx = 0; idx < ROWS * COLS; idx++) {
        if(idx == diag_dimention * ROWS + diag_dimention) {
            diag_dimention++;
            EXPECT_EQ(unit_mat(idx), 1.0); 
        } else {
            EXPECT_EQ(unit_mat(idx) < epsilon, true);
        }
    }
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

