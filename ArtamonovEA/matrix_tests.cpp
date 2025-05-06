#include <iostream>
#include <sstream>

#include "matrix.cpp"
#include "logging.cpp"


void test_logger_set_level()
{
    try {
        Logger logger = Logger();
        logger.set_level(LogLevel::ERROR);
        if (logger.get_level() != LogLevel::ERROR) {
            throw std::runtime_error("");
        }
        std::cout << "Test `test_logger_set_level` passed." << std::endl;
    } catch (...) {
        std::cerr << "Test `test_logger_set_level` failed." << std::endl;
    }
}


void test_logger_match_level() 
{
    try {
        Logger logger = Logger(LogLevel::WARNING);
        if (logger.match_level(LogLevel::INFO)) {
            throw std::runtime_error("");
        }
        if (!logger.match_level(LogLevel::ERROR)) {
            throw std::runtime_error("");
        }
        std::cout << "Test `test_logger_matches_level` passed." << std::endl;
    } catch (...) {
        std::cerr << "Test `test_logger_matches_level` failed." << std::endl;
    }
}


void test_logger_log() 
{
    try {
        std::ostringstream captured_output;
        std::streambuf* original_cout = std::cout.rdbuf(captured_output.rdbuf());
        std::streambuf* original_cerr = std::cerr.rdbuf(captured_output.rdbuf());

        Logger logger = Logger();
        logger.log("Test INFO message.", LogLevel::INFO);
        logger.log("Test WARNING message.", LogLevel::WARNING);
        logger.log("Test ERROR message.", LogLevel::ERROR);

        std::cout.rdbuf(original_cout);
        std::cerr.rdbuf(original_cerr);

        std::string expected_output = 
            "[INFO] Test INFO message.\n"
            "[WARNING] Test WARNING message.\n"
            "[ERROR] Test ERROR message.\n";

        if (captured_output.str() != expected_output) {
            throw std::runtime_error("Logger output does not match expected output.");
        }

        std::cout << "Test `test_logger_log` passed." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test `test_logger_log` failed: " << e.what() << std::endl;
    }
}


void test_matrix_sum()
{
    try {
        Matrix A(2, 3);
        A(0, 0) = 1;
        A(0, 1) = 2;
        A(0, 2) = 3;
        A(1, 0) = 4;
        A(1, 1) = 5;
        A(1, 2) = 6;

        Matrix B(2, 3);
        B(0, 0) = 1;
        B(0, 1) = 2;
        B(0, 2) = 3;
        B(1, 0) = 4;
        B(1, 1) = 5;
        B(1, 2) = 6;

        Matrix Expected(2, 3);
        Expected(0, 0) = 2;
        Expected(0, 1) = 4;
        Expected(0, 2) = 6;
        Expected(1, 0) = 8;
        Expected(1, 1) = 10;
        Expected(1, 2) = 12;

        if (A + B != Expected) {
            throw std::runtime_error("");
        }
        std::cout << "Test `test_matrix_sum` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_matrix_sum` failed" << '\n';
    }
}

void test_matrix_sub()
{
    try {
        Matrix A(2, 3);
        A(0, 0) = 1;
        A(0, 1) = 2;
        A(0, 2) = 3;
        A(1, 0) = 4;
        A(1, 1) = 5;
        A(1, 2) = 6;

        Matrix B(2, 3);
        B(0, 0) = 1;
        B(0, 1) = 2;
        B(0, 2) = 3;
        B(1, 0) = 4;
        B(1, 1) = 5;
        B(1, 2) = 6;

        ZeroMatrix Expected(2, 3);

        if (A - B != Expected) {
            throw std::runtime_error("");
        }
        std::cout << "Test `test_matrix_sub` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_matrix_sub` failed" << '\n';
    }
}

void test_matrix_mul()
{
    try {
        Matrix A(2, 3);
        A(0, 0) = 1;
        A(0, 1) = 2;
        A(0, 2) = 3;
        A(1, 0) = 4;
        A(1, 1) = 5;
        A(1, 2) = 6;

        Matrix C(3, 2);
        C(0, 0) = 1;
        C(0, 1) = 2;
        C(1, 0) = 3;
        C(1, 1) = 4;
        C(2, 0) = 5;
        C(2, 1) = 6;

        Matrix Expected(2, 2);
        Expected(0, 0) = 22;
        Expected(0, 1) = 28;
        Expected(1, 0) = 49;
        Expected(1, 1) = 64;

        if (A * C != Expected) {
            throw std::runtime_error("");
        }
        std::cout << "Test `test_matrix_mul` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_matrix_mul` failed" << '\n';
    }
}


void test_matrix_scalar_sum()
{
    try {
        Matrix A(2, 3);
        A(0, 0) = 1;
        A(0, 1) = 2;
        A(0, 2) = 3;
        A(1, 0) = 4;
        A(1, 1) = 5;
        A(1, 2) = 6;

        Matrix Expected(2, 3);
        Expected(0, 0) = 3;
        Expected(0, 1) = 4;
        Expected(0, 2) = 5;
        Expected(1, 0) = 6;
        Expected(1, 1) = 7;
        Expected(1, 2) = 8;

        if (A + 2 != Expected) {
            throw std::runtime_error("");
        }
        std::cout << "Test `test_matrix_scalar_sum` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_matrix_scalar_sum` failed" << '\n';
    }
}


void test_matrix_scalar_sub()
{
    try {
        Matrix A(2, 3);
        A(0, 0) = 1;
        A(0, 1) = 2;
        A(0, 2) = 3;
        A(1, 0) = 4;
        A(1, 1) = 5;
        A(1, 2) = 6;

        Matrix Expected(2, 3);
        Expected(0, 0) = -1;
        Expected(0, 1) = 0;
        Expected(0, 2) = 1;
        Expected(1, 0) = 2;
        Expected(1, 1) = 3;
        Expected(1, 2) = 4;
        
        if (A - 2 != Expected) {
            throw std::runtime_error("");
        }
        std::cout << "Test `test_matrix_scalar_sub` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_matrix_scalar_sub` failed" << '\n';
    }
}

void test_matrix_scalar_mul()
{
    try {
        Matrix A(2, 3);
        A(0, 0) = 1;
        A(0, 1) = 2;
        A(0, 2) = 3;
        A(1, 0) = 4;
        A(1, 1) = 5;
        A(1, 2) = 6;

        Matrix Expected(2, 3);
        Expected(0, 0) = 2;
        Expected(0, 1) = 4;
        Expected(0, 2) = 6;
        Expected(1, 0) = 8;
        Expected(1, 1) = 10;
        Expected(1, 2) = 12;

        if (A * 2 != Expected) {
            throw std::runtime_error("");
        }
        std::cout << "Test `test_matrix_scalar_mul` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_matrix_scalar_mul` failed" << '\n';
    }
}

void test_matrix_scalar_div()
{
    try {
        Matrix A(2, 3);
        A(0, 0) = 1;
        A(0, 1) = 2;
        A(0, 2) = 3;
        A(1, 0) = 4;
        A(1, 1) = 5;
        A(1, 2) = 6;

        Matrix Expected(2, 3);
        Expected(0, 0) = 0.5;
        Expected(0, 1) = 1;
        Expected(0, 2) = 1.5;
        Expected(1, 0) = 2;
        Expected(1, 1) = 2.5;
        Expected(1, 2) = 3;

        if (A / 2 != Expected) {
            throw std::runtime_error("");
        }
        std::cout << "Test `test_matrix_scalar_div` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_matrix_scalar_div` failed" << '\n';
    }
}

void test_matrix_sum_inplace()
{
    try {
        Matrix A(2, 3);
        A(0, 0) = 1;
        A(0, 1) = 2;
        A(0, 2) = 3;
        A(1, 0) = 4;
        A(1, 1) = 5;
        A(1, 2) = 6;

        Matrix B(2, 3);
        B(0, 0) = 1;
        B(0, 1) = 2;
        B(0, 2) = 3;
        B(1, 0) = 4;
        B(1, 1) = 5;
        B(1, 2) = 6;

        Matrix Expected(2, 3);
        Expected(0, 0) = 2;
        Expected(0, 1) = 4;
        Expected(0, 2) = 6;
        Expected(1, 0) = 8;
        Expected(1, 1) = 10;
        Expected(1, 2) = 12;

        A += B;
        if (A != Expected) {
            throw std::runtime_error("");
        }
        std::cout << "Test `test_matrix_sum_inplace` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_matrix_sum_inplace` failed" << '\n';
    }
}

void test_matrix_sub_inplace()
{
    try {
        Matrix A(2, 3);
        A(0, 0) = 1;
        A(0, 1) = 2;
        A(0, 2) = 3;
        A(1, 0) = 4;
        A(1, 1) = 5;
        A(1, 2) = 6;

        Matrix B(2, 3);
        B(0, 0) = 1;
        B(0, 1) = 2;
        B(0, 2) = 3;
        B(1, 0) = 4;
        B(1, 1) = 5;
        B(1, 2) = 6;

        ZeroMatrix Expected(2, 3);
        A -= B;
        if (A != Expected) {
            throw std::runtime_error("");
        }
        std::cout << "Test `test_matrix_sub_inplace` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_matrix_sub_inplace` failed" << '\n';
    }
}

void test_matrix_mul_inplace()
{
    try {
        Matrix A(2, 3);
        A(0, 0) = 1;
        A(0, 1) = 2;
        A(0, 2) = 3;
        A(1, 0) = 4;
        A(1, 1) = 5;
        A(1, 2) = 6;

        Matrix C(3, 2);
        C(0, 0) = 1;
        C(0, 1) = 2;
        C(1, 0) = 3;
        C(1, 1) = 4;
        C(2, 0) = 5;
        C(2, 1) = 6;

        Matrix Expected(2, 2);
        Expected(0, 0) = 22;
        Expected(0, 1) = 28;
        Expected(1, 0) = 49;
        Expected(1, 1) = 64;

        A *= C;
        if (A != Expected) {
            throw std::runtime_error("");
        }
        std::cout << "Test `test_matrix_mul_inplace` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_matrix_mul_inplace` failed" << '\n';
    }
}


void test_matrix_scalar_sum_inplace()
{
    try {
        Matrix A(2, 3);
        A(0, 0) = 1;
        A(0, 1) = 2;
        A(0, 2) = 3;
        A(1, 0) = 4;
        A(1, 1) = 5;
        A(1, 2) = 6;

        Matrix Expected(2, 3);
        Expected(0, 0) = 3;
        Expected(0, 1) = 4;
        Expected(0, 2) = 5;
        Expected(1, 0) = 6;
        Expected(1, 1) = 7;
        Expected(1, 2) = 8;

        A += 2;
        if (A != Expected) {
            throw std::runtime_error("");
        }
        std::cout << "Test `test_matrix_scalar_sum_inplace` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_matrix_scalar_sum_inplace` failed" << '\n';
    }
}

void test_matrix_scalar_sub_inplace()
{
    try {
        Matrix A(2, 3);
        A(0, 0) = 1;
        A(0, 1) = 2;
        A(0, 2) = 3;
        A(1, 0) = 4;
        A(1, 1) = 5;
        A(1, 2) = 6;

        Matrix Expected(2, 3);
        Expected(0, 0) = -1;
        Expected(0, 1) = 0;
        Expected(0, 2) = 1;
        Expected(1, 0) = 2;
        Expected(1, 1) = 3;
        Expected(1, 2) = 4;

        A -= 2;
        if (A != Expected) {
            throw std::runtime_error("");
        }
        std::cout << "Test `test_matrix_scalar_sub_inplace` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_matrix_scalar_sub_inplace` failed" << '\n';
    }
}

void test_matrix_scalar_mul_inplace()
{
    try {
        Matrix A(2, 3);
        A(0, 0) = 1;
        A(0, 1) = 2;
        A(0, 2) = 3;
        A(1, 0) = 4;
        A(1, 1) = 5;
        A(1, 2) = 6;

        Matrix Expected(2, 3);
        Expected(0, 0) = 2;
        Expected(0, 1) = 4;
        Expected(0, 2) = 6;
        Expected(1, 0) = 8;
        Expected(1, 1) = 10;
        Expected(1, 2) = 12;

        A *= 2;
        if (A != Expected) {
            throw std::runtime_error("");
        }
        std::cout << "Test `test_matrix_scalar_mul_inplace` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_matrix_scalar_mul_inplace` failed" << '\n';
    }
}

void test_matrix_scalar_div_inplace()
{
    try {
        Matrix A(2, 3);
        A(0, 0) = 1;
        A(0, 1) = 2;
        A(0, 2) = 3;
        A(1, 0) = 4;
        A(1, 1) = 5;
        A(1, 2) = 6;

        Matrix Expected(2, 3);
        Expected(0, 0) = 0.5;
        Expected(0, 1) = 1;
        Expected(0, 2) = 1.5;
        Expected(1, 0) = 2;
        Expected(1, 1) = 2.5;
        Expected(1, 2) = 3;

        A /= 2;
        if (A != Expected) {
            throw std::runtime_error("");
        }
        std::cout << "Test `test_matrix_scalar_div_inplace` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_matrix_scalar_div_inplace` failed" << '\n';
    }
}

void test_matrix_eq()
{
    try {
        Matrix A(2, 3);
        A(0, 0) = 1;
        A(0, 1) = 2;
        A(0, 2) = 3;
        A(1, 0) = 4;
        A(1, 1) = 5;
        A(1, 2) = 6;

        Matrix B(2, 3);
        B(0, 0) = 1;
        B(0, 1) = 2;
        B(0, 2) = 3;
        B(1, 0) = 4;
        B(1, 1) = 5;
        B(1, 2) = 6;

        if (A != B) {
            throw std::runtime_error("");
        }
        std::cout << "Test `test_matrix_eq` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_matrix_eq` failed" << '\n';
    }
}

void test_matrix_neq()
{
    try {
        Matrix A(2, 3);
        A(0, 0) = 1;
        A(0, 1) = 2;
        A(0, 2) = 3;
        A(1, 0) = 4;
        A(1, 1) = 5;
        A(1, 2) = 6;

        Matrix C(3, 2);
        C(0, 0) = 1;
        C(0, 1) = 2;
        C(1, 0) = 3;
        C(1, 1) = 4;
        C(2, 0) = 5;
        C(2, 1) = 6;

        if (A == C) {
            throw std::runtime_error("");
        }
        std::cout << "Test `test_matrix_neq` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_matrix_neq` failed" << '\n';
    }
}

void test_matrix_transpose()
{
    try {
        Matrix A(2, 3);
        A(0, 0) = 1;
        A(0, 1) = 2;
        A(0, 2) = 3;
        A(1, 0) = 4;
        A(1, 1) = 5;
        A(1, 2) = 6;

        Matrix Expected(3, 2);
        Expected(0, 0) = 1;
        Expected(1, 0) = 2;
        Expected(2, 0) = 3;
        Expected(0, 1) = 4;
        Expected(1, 1) = 5;
        Expected(2, 1) = 6;
        
        Matrix T = A.transpose() ? *A.transpose() : Matrix();
        if (T != Expected) {
            throw std::runtime_error("");
        }
        std::cout << "Test `test_matrix_transpose` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_matrix_transpose` failed" << '\n';
    }
}

void test_matrix_transpose_inplace()
{
    try {
        Matrix A(2, 3);
        A(0, 0) = 1;
        A(0, 1) = 2;
        A(0, 2) = 3;
        A(1, 0) = 4;
        A(1, 1) = 5;
        A(1, 2) = 6;

        Matrix Expected(3, 2);
        Expected(0, 0) = 1;
        Expected(1, 0) = 2;
        Expected(2, 0) = 3;
        Expected(0, 1) = 4;
        Expected(1, 1) = 5;
        Expected(2, 1) = 6;
        
        A.transpose(true);
        if (A != Expected) {
            throw std::runtime_error("");
        }
        std::cout << "Test `test_matrix_transpose_inplace` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_matrix_transpose_inplace` failed" << '\n';
    }
}


void test_sq_matrix_identity()
{
    try {
        SquareMatrix S(2);
        
        SquareMatrix I = S.identity();

        for (size_t i = 0; i < I.get_rows(); ++i) {
            for (size_t j = 0; j < I.get_cols(); ++j) {
                if (I(i, j) != 1.0) {
                    throw std::runtime_error("");
                }
            }
        }
        std::cout << "Test `test_sq_matrix_identity` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_sq_matrix_identity` failed" << '\n';
    }
}

void test_sq_matrix_minor()
{
    try {
        SquareMatrix S(2);
        S(0, 0) = 1;
        S(0, 1) = 2;
        S(1, 0) = 3;
        S(1, 1) = 4;

        SquareMatrix Expected(1);
        Expected(0, 0) = 4;

        SquareMatrix M = S.minor(0, 0);
        if (M != Expected) {
            throw std::runtime_error("");
        }
        std::cout << "Test `test_sq_matrix_minor` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_sq_matrix_minor` failed" << '\n';
    }
}

void test_sq_matrix_det()
{
    try {
        SquareMatrix S(2);
        S(0, 0) = 1;
        S(0, 1) = 2;
        S(1, 0) = 3;
        S(1, 1) = 4;

        double expected_det = -2;

        if (S.determinant() != expected_det) {
            throw std::runtime_error("");
        }
        std::cout << "Test `test_sq_matrix_det` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_sq_matrix_det` failed" << '\n';
    }
}

void test_sq_matrix_inv()
{
    try {
        SquareMatrix S(2);
        S(0, 0) = 1;
        S(0, 1) = 2;
        S(1, 0) = 3;
        S(1, 1) = 4;

        SquareMatrix Expected(2);
        Expected(0, 0) = -2;
        Expected(0, 1) = 1;
        Expected(1, 0) = 1.5;
        Expected(1, 1) = -0.5;

        SquareMatrix I = S.inverse() ? *S.inverse() : SquareMatrix(2);
        
        for (size_t i = 0; i < I.get_rows(); ++i) {
            for (size_t j = 0; j < I.get_cols(); ++j) {
                if (I(i, j) != Expected(i, j)) {
                    throw std::runtime_error("");
                }
            }
        }
        std::cout << "Test `test_sq_matrix_inv` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_sq_matrix_inv` failed" << '\n';
    }
}

void test_sq_matrix_inv_inplace()
{
    try {
        SquareMatrix S(2);
        S(0, 0) = 1;
        S(0, 1) = 2;
        S(1, 0) = 3;
        S(1, 1) = 4;

        SquareMatrix Expected(2);
        Expected(0, 0) = -2;
        Expected(0, 1) = 1;
        Expected(1, 0) = 1.5;
        Expected(1, 1) = -0.5;

        S.inverse(true);
        
        for (size_t i = 0; i < S.get_rows(); ++i) {
            for (size_t j = 0; j < S.get_cols(); ++j) {
                if (S(i, j) != Expected(i, j)) {
                    throw std::runtime_error("");
                }
            }
        }
        std::cout << "Test `test_sq_matrix_inv_inplace` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_sq_matrix_inv_inplace` failed" << '\n';
    }
}

void test_sq_matrix_pow()
{
    try {
        SquareMatrix S(2);
        S(0, 0) = 1;
        S(0, 1) = 2;
        S(1, 0) = 3;
        S(1, 1) = 4;

        SquareMatrix Expected(2);
        Expected(0, 0) = 7;
        Expected(0, 1) = 10;
        Expected(1, 0) = 15;
        Expected(1, 1) = 22;

        SquareMatrix P = S.pow(2) ? *S.pow(2) : SquareMatrix(2);
        
        for (size_t i = 0; i < P.get_rows(); ++i) {
            for (size_t j = 0; j < P.get_cols(); ++j) {
                if (P(i, j) != Expected(i, j)) {
                    throw std::runtime_error("");
                }
            }
        }
        std::cout << "Test `test_sq_matrix_pow` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_sq_matrix_pow` failed" << '\n';
    }
}

void test_sq_matrix_pow_inplace()
{
    try {
        SquareMatrix S(2);
        S(0, 0) = 1;
        S(0, 1) = 2;
        S(1, 0) = 3;
        S(1, 1) = 4;

        SquareMatrix Expected(2);
        Expected(0, 0) = 7;
        Expected(0, 1) = 10;
        Expected(1, 0) = 15;
        Expected(1, 1) = 22;

        S.pow(2, true);
        for (size_t i = 0; i < S.get_rows(); ++i) {
            for (size_t j = 0; j < S.get_cols(); ++j) {
                if (S(i, j) != Expected(i, j)) {
                    throw std::runtime_error("");
                }
            }
        }
        std::cout << "Test `test_sq_matrix_pow_inplace` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_sq_matrix_pow_inplace` failed" << '\n';
    }
}

void test_zero_matrix()
{
    try {
        ZeroMatrix Z = ZeroMatrix(2, 3);
        for (size_t i = 0; i < Z.get_rows(); ++i) {
            for (size_t j = 0; j < Z.get_cols(); ++j) {
                if (Z(i, j) != 0) {
                    throw std::runtime_error("");
                }
            }
        }
        std::cout << "Test `test_zero_matrix` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_zero_matrix` failed" << '\n';
    }
}


void test_sq_matrix_div()
{
    try {
        SquareMatrix S(2);
        S(0, 0) = 1;
        S(0, 1) = 2;
        S(1, 0) = 3;
        S(1, 1) = 4;

        SquareMatrix B(2);
        B(0, 0) = 1;
        B(0, 1) = 2;
        B(1, 0) = 3;
        B(1, 1) = 4;

        SquareMatrix Expected(2);
        Expected(0, 0) = 1;
        Expected(0, 1) = 0;
        Expected(1, 0) = 0;
        Expected(1, 1) = 1;

        if (S / B != Expected) {
            throw std::runtime_error("");
        }
        std::cout << "Test `test_sq_matrix_div` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_sq_matrix_div` failed" << '\n';
    }
}

void test_sq_matrix_div_inplace()
{
    try {
        SquareMatrix S(2);
        S(0, 0) = 1;
        S(0, 1) = 2;
        S(1, 0) = 3;
        S(1, 1) = 4;

        SquareMatrix B(2);
        B(0, 0) = 1;
        B(0, 1) = 2;
        B(1, 0) = 3;
        B(1, 1) = 4;

        SquareMatrix Expected(2);
        Expected(0, 0) = 1;
        Expected(0, 1) = 0;
        Expected(1, 0) = 0;
        Expected(1, 1) = 1;

        S /= B;
        if (S != Expected) {
            throw std::runtime_error("");
        }
        std::cout << "Test `test_sq_matrix_div_inplace` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_sq_matrix_div_inplace` failed" << '\n';
    }
}

void test_sq_matrix_scalar_div()
{
    try {
        SquareMatrix S(2);
        S(0, 0) = 1;
        S(0, 1) = 2;
        S(1, 0) = 3;
        S(1, 1) = 4;

        SquareMatrix Expected(2);
        Expected(0, 0) = 0.5;
        Expected(0, 1) = 1;
        Expected(1, 0) = 1.5;
        Expected(1, 1) = 2;

        if (S / 2 != Expected) {
            throw std::runtime_error("");
        }
        std::cout << "Test `test_sq_matrix_div` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_sq_matrix_div` failed" << '\n';
    }
}


void test_sq_matrix_scalar_div_inplace()
{
    try {
        SquareMatrix S(2);
        S(0, 0) = 1;
        S(0, 1) = 2;
        S(1, 0) = 3;
        S(1, 1) = 4;

        SquareMatrix Expected(2);
        Expected(0, 0) = 0.5;
        Expected(0, 1) = 1;
        Expected(1, 0) = 1.5;
        Expected(1, 1) = 2;

        S /= 2;
        if (S != Expected) {
            throw std::runtime_error("");
        }
        std::cout << "Test `test_sq_matrix_div_inplace` passed." << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Test `test_sq_matrix_div_inplace` failed" << '\n';
    }
}


void run_tests()
{
    test_logger_set_level();
    test_logger_match_level();
    test_logger_log();

    test_matrix_sum();
    test_matrix_sub();
    test_matrix_mul();
    test_matrix_scalar_sum();
    test_matrix_scalar_sub();
    test_matrix_scalar_mul();
    test_matrix_scalar_div();
    test_matrix_sum_inplace();
    test_matrix_sub_inplace();
    test_matrix_mul_inplace();
    test_matrix_scalar_sum_inplace();
    test_matrix_scalar_sub_inplace();
    test_matrix_scalar_mul_inplace();
    test_matrix_scalar_div_inplace();
    test_matrix_eq();
    test_matrix_neq();
    test_matrix_transpose();
    test_matrix_transpose_inplace();

    test_sq_matrix_identity();
    test_sq_matrix_minor();
    test_sq_matrix_det();
    test_sq_matrix_inv();
    test_sq_matrix_inv_inplace();
    test_sq_matrix_pow();
    test_sq_matrix_pow_inplace();
    test_sq_matrix_div();
    test_sq_matrix_div_inplace();
    test_sq_matrix_scalar_div();
    test_sq_matrix_scalar_div_inplace();

    test_zero_matrix();
}

int main() 
{
    run_tests();
    return 0;
}