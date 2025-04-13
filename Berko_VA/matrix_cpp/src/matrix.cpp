#include <algorithm>
#include <sstream>
#include <atomic>
#include <cmath>

#include "matrix.hpp"

static std::atomic<LogLevel> CURRENT_LEVEL{LogLevel::NONE};

static void print_log(LogLevel level, const std::string& message) {
#ifdef MATRIX_LOG_ENABLE
    if (level <= CURRENT_LEVEL.load(std::memory_order_relaxed)) {
        switch (level) {
            case LogLevel::ERR:
                std::cout << "ERROR: ";
                break;
            case LogLevel::WARN:
                std::cout << "WARNING: ";
                break;
            case LogLevel::INFO:
                std::cout << "INFO: ";
                break;
            default:
                break;
        }
        std::cout << message << std::endl;
    }
#endif
}

static bool is_valid(double value) {
    return !std::isnan(value) && !std::isinf(value);
}

Matrix::Matrix() : m_rows(0), m_cols(0), m_data() {}

Matrix::Matrix(std::size_t rows, std::size_t cols) : m_rows(rows), m_cols(cols) {
    std::stringstream ss;
    ss << "allocate matrix with size: " << rows << "x" << cols << "\n";
    print_log(LogLevel::INFO, ss.str());
    if (rows == 0 || cols == 0) {
        m_data.clear();
        return;
    }
    if (rows * cols > m_data.max_size()) {
        print_log(LogLevel::ERR, "matrix size exceeds maximum allowed size\n");
        throw std::invalid_argument("Matrix size too large");
    }
    try {
        m_data.resize(rows * cols, 0.0);
    } catch (const std::bad_alloc&) {
        print_log(LogLevel::ERR, "memory allocation failed\n");
        throw;
    }
}

Matrix::Matrix(const Matrix& other) : m_rows(other.m_rows), m_cols(other.m_cols), m_data(other.m_data) {
    print_log(LogLevel::INFO, "matrix copy\n");
}

Matrix::Matrix(Matrix&& other) noexcept : m_rows(other.m_rows), m_cols(other.m_cols), m_data(std::move(other.m_data)) {
    print_log(LogLevel::INFO, "matrix move\n");
    other.m_rows = 0;
    other.m_cols = 0;
}

Matrix& Matrix::operator=(const Matrix& other) {
    print_log(LogLevel::INFO, "matrix copy assign\n");
    if (this != &other) {
        m_rows = other.m_rows;
        m_cols = other.m_cols;
        m_data = other.m_data;
    }
    return *this;
}

Matrix& Matrix::operator=(Matrix&& other) noexcept {
    print_log(LogLevel::INFO, "matrix move assign\n");
    if (this != &other) {
        m_rows = other.m_rows;
        m_cols = other.m_cols;
        m_data = std::move(other.m_data);
        other.m_rows = 0;
        other.m_cols = 0;
    }
    return *this;
}

Matrix Matrix::operator+(const Matrix& other) const {
    print_log(LogLevel::INFO, "matrix sum\n");
    if (m_rows != other.m_rows || m_cols != other.m_cols) {
        print_log(LogLevel::ERR, "matrix size not equals\n");
        throw std::runtime_error("Matrix sizes not equal");
    }
    try {
        Matrix result(m_rows, m_cols);
        for (std::size_t i = 0; i < m_rows * m_cols; ++i) {
            if (!is_valid(m_data[i]) || !is_valid(other.m_data[i])) {
                print_log(LogLevel::ERR, "invalid matrix element\n");
                throw std::runtime_error("Invalid matrix element");
            }
            result.m_data[i] = m_data[i] + other.m_data[i];
        }
        return result;
    } catch (const std::bad_alloc&) {
        print_log(LogLevel::ERR, "memory allocation failed\n");
        throw;
    }
}

Matrix Matrix::operator-(const Matrix& other) const {
    print_log(LogLevel::INFO, "matrix subtraction\n");
    if (m_rows != other.m_rows || m_cols != other.m_cols) {
        print_log(LogLevel::ERR, "matrix size not equals\n");
        throw std::runtime_error("Matrix sizes not equal");
    }
    try {
        Matrix result(m_rows, m_cols);
        for (std::size_t i = 0; i < m_rows * m_cols; ++i) {
            if (!is_valid(m_data[i]) || !is_valid(other.m_data[i])) {
                print_log(LogLevel::ERR, "invalid matrix element\n");
                throw std::runtime_error("Invalid matrix element");
            }
            result.m_data[i] = m_data[i] - other.m_data[i];
        }
        return result;
    } catch (const std::bad_alloc&) {
        print_log(LogLevel::ERR, "memory allocation failed\n");
        throw;
    }
}


Matrix Matrix::operator*(const Matrix& other) const {
    if (m_cols != other.m_rows) {
        print_log(LogLevel::ERR, "A.cols, B.rows not equals\n");
        throw std::runtime_error("Invalid matrix dimensions for multiplication");
    }

    int n = std::max({m_rows, m_cols, other.m_cols});
	try {
		if (n < 100) {
			return standard_multiply(other);
		} else {
			return block_multiply(other);
		}
	} catch (const std::bad_alloc&) {
        print_log(LogLevel::ERR, "memory allocation failed\n");
        throw;
    }
}

Matrix Matrix::operator*(double number) const {
    print_log(LogLevel::INFO, "matrix multiplication by number\n");
    if (!is_valid(number)) {
        print_log(LogLevel::ERR, "invalid scalar value\n");
        throw std::runtime_error("Invalid scalar value");
    }
    try {
        Matrix result(m_rows, m_cols);
        for (std::size_t i = 0; i < m_rows * m_cols; ++i) {
            if (!is_valid(m_data[i])) {
                print_log(LogLevel::ERR, "invalid matrix element\n");
                throw std::runtime_error("Invalid matrix element");
            }
            result.m_data[i] = m_data[i] * number;
        }
        return result;
    } catch (const std::bad_alloc&) {
        print_log(LogLevel::ERR, "memory allocation failed\n");
        throw;
    }
}

Matrix operator*(double number, const Matrix& matrix) {
    return matrix * number;
}

double& Matrix::operator()(std::size_t row, std::size_t col) {
    if (row >= m_rows || col >= m_cols) {
        print_log(LogLevel::ERR, "index out of bound\n");
        throw std::out_of_range("Matrix index out of bounds");
    }
    return m_data[row * m_cols + col];
}

const double& Matrix::operator()(std::size_t row, std::size_t col) const {
    if (row >= m_rows || col >= m_cols) {
        print_log(LogLevel::ERR, "index out of bound\n");
        throw std::out_of_range("Matrix index out of bounds");
    }
    return m_data[row * m_cols + col];
}

bool Matrix::operator==(const Matrix& other) const {
    if (m_rows != other.m_rows || m_cols != other.m_cols) return false;
    if (is_empty() && other.is_empty()) return true;
    return m_data == other.m_data;
}

void Matrix::set_log_level(LogLevel level) {
    CURRENT_LEVEL.store(level, std::memory_order_relaxed);
}

MatrixStatus Matrix::fill_random(double min, double max) {
    print_log(LogLevel::INFO, "make random matrix\n");
    if (is_empty()) {
        print_log(LogLevel::WARN, "matrix is empty\n");
        return MatrixStatus::OK;
    }
    if (min > max) {
        throw std::invalid_argument("Minimum value must not exceed maximum value");
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(min, max);

    for (int i = 0; i < rows(); ++i) {
        for (int j = 0; j < cols(); ++j) {
            (*this)(i, j) = dist(gen);
        }
    }
	return MatrixStatus::OK;

}

MatrixStatus Matrix::set_zeros() {
    print_log(LogLevel::INFO, "make zero matrix\n");
    if (is_empty()) {
        print_log(LogLevel::WARN, "matrix is empty\n");
        return MatrixStatus::OK;
    }
    std::fill(m_data.begin(), m_data.end(), 0.0);
    return MatrixStatus::OK;
}

MatrixStatus Matrix::set_identity() {
    print_log(LogLevel::INFO, "make identity matrix\n");
    if (!is_square()) {
        print_log(LogLevel::ERR, "matrix not square\n");
        return MatrixStatus::SIZE_ERR;
    }
    set_zeros();
    for (std::size_t idx = 0; idx < m_rows; ++idx) {
        m_data[idx * m_cols + idx] = 1.0;
    }
    return MatrixStatus::OK;
}

MatrixStatus Matrix::transpose() {
    print_log(LogLevel::INFO, "matrix transposition\n");
    if (!is_square()) {
        print_log(LogLevel::ERR, "matrix not square\n");
        return MatrixStatus::SIZE_ERR;
    }
    for (std::size_t row = 1; row < m_rows; ++row) {
        for (std::size_t idx = 0; idx < row; ++idx) {
            if (!is_valid(m_data[row * m_cols + idx]) || !is_valid(m_data[idx * m_cols + row])) {
                print_log(LogLevel::ERR, "invalid matrix element\n");
                return MatrixStatus::SIZE_ERR; // Заменили INVALID_ERR
            }
            std::swap(m_data[row * m_cols + idx], m_data[idx * m_cols + row]);
        }
    }
    return MatrixStatus::OK;
}

MatrixStatus Matrix::pow(unsigned int power) {
    print_log(LogLevel::INFO, "matrix power\n");
    if (!is_square()) {
        print_log(LogLevel::ERR, "matrix not square\n");
        return MatrixStatus::SIZE_ERR;
    }
    try {
        Matrix result(m_rows, m_cols);
        result.set_identity();
        if (power == 0) {
            *this = std::move(result);
            return MatrixStatus::OK;
        }
        Matrix temp(*this);
        for (std::size_t i = 0; i < m_rows * m_cols; ++i) {
            if (!is_valid(temp.m_data[i])) {
                print_log(LogLevel::ERR, "invalid matrix element\n");
                return MatrixStatus::SIZE_ERR;
            }
        }
        while (power > 0) {
            if (power % 2 == 1) {
                result = result * temp;
            }
            temp = temp * temp;
            power /= 2;
        }
        *this = std::move(result);
        return MatrixStatus::OK;
    } catch (const std::bad_alloc&) {
        print_log(LogLevel::ERR, "memory allocation failed\n");
        return MatrixStatus::ALLOC_ERR;
    }
}

MatrixStatus Matrix::exp() {
    print_log(LogLevel::INFO, "matrix exponent\n");
    if (!is_square()) {
        print_log(LogLevel::ERR, "matrix not square\n");
        return MatrixStatus::SIZE_ERR;
    }
    try {
        Matrix result(m_rows, m_cols);
        result.set_identity();
        Matrix term(m_rows, m_cols);
        term.set_identity();
        Matrix temp(m_rows, m_cols);
        for (std::size_t i = 0; i < m_rows * m_cols; ++i) {
            if (!is_valid(m_data[i])) {
                print_log(LogLevel::ERR, "invalid matrix element\n");
                return MatrixStatus::SIZE_ERR;
            }
        }
        const double tolerance = 1e-10;
        for (unsigned num = 1; num < 100; ++num) {
            temp = term * *this;
            term = temp;
            term = term * (1.0 / static_cast<double>(num));
            Matrix old_result = result;
            result = result + term;
            bool converged = true;
            for (std::size_t i = 0; i < m_rows * m_cols; ++i) {
                if (!Matrix::is_equal(result.m_data[i], old_result.m_data[i])) {
                    converged = false;
                    break;
                }
            }
            if (converged) break;
        }
        *this = std::move(result);
        return MatrixStatus::OK;
    } catch (const std::bad_alloc&) {
        print_log(LogLevel::ERR, "memory allocation failed\n");
        return MatrixStatus::ALLOC_ERR;
    }
}

MatrixStatus Matrix::det(double& result) const {
    print_log(LogLevel::INFO, "matrix determinant\n");
    if (is_empty()) {
        result = 0.0;
        print_log(LogLevel::WARN, "empty matrix\n");
        return MatrixStatus::OK;
    }
    if (!is_square()) {
        print_log(LogLevel::ERR, "matrix not square\n");
        return MatrixStatus::SIZE_ERR;
    }
    try {
        Matrix T(*this);
        for (std::size_t i = 0; i < m_rows * m_cols; ++i) {
            if (!is_valid(T.m_data[i])) {
                print_log(LogLevel::ERR, "invalid matrix element\n");
                return MatrixStatus::SIZE_ERR;
            }
        }
        result = 1.0;
        for (std::size_t row_base = 0; row_base < T.m_rows - 1; ++row_base) {
            if (Matrix::is_equal(T(row_base, row_base), 0.0)) {
                std::size_t idx_non_zero = row_base;
                for (std::size_t row = row_base + 1; row < T.m_rows; ++row) {
                    if (!Matrix::is_equal(T(row, row_base), 0.0)) {
                        idx_non_zero = row;
                        break;
                    }
                }
                if (idx_non_zero == row_base) {
                    result = 0.0;
                    return MatrixStatus::OK;
                }
                for (std::size_t idx = 0; idx < T.m_cols; ++idx) {
                    std::swap(T(row_base, idx), T(idx_non_zero, idx));
                }
                result *= -1.0;
            }
            if (std::abs(T(row_base, row_base)) < 1e-10) {
                print_log(LogLevel::WARN, "small pivot in determinant calculation\n");
            }
            for (std::size_t row = row_base + 1; row < T.m_rows; ++row) {
                double mult = T(row, row_base) / T(row_base, row_base);
                if (!is_valid(mult)) {
                    print_log(LogLevel::ERR, "invalid computation in determinant\n");
                    return MatrixStatus::SIZE_ERR;
                }
                for (std::size_t idx = 0; idx < T.m_cols; ++idx) {
                    T(row, idx) -= mult * T(row_base, idx);
                }
            }
        }
        for (std::size_t idx = 0; idx < T.m_rows; ++idx) {
            result *= T(idx, idx);
        }
        return MatrixStatus::OK;
    } catch (const std::bad_alloc&) {
        print_log(LogLevel::ERR, "memory allocation failed\n");
        return MatrixStatus::ALLOC_ERR;
    }
}

Matrix Matrix::standard_multiply(const Matrix& other) const {
    Matrix result(m_rows, other.m_cols);
    for (size_t i = 0; i < m_rows; ++i) {
        for (size_t j = 0; j < other.m_cols; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < m_cols; ++k) {
                sum += (*this)(i, k) * other(k, j);
            }
            result(i, j) = sum;
        }
    }
    return result;
}

void Matrix::multiply_block(const Matrix& A, const Matrix& B, Matrix& result,
                           int i_start, int i_end, int j_start, int j_end) {
    for (int i = i_start; i < i_end; ++i) {
        for (int j = j_start; j < j_end; ++j) {
            double sum = 0.0;
            for (int k = 0; k < A.m_cols; ++k) {
                sum += A(i, k) * B(k, j);
            }
            result(i, j) = sum;
        }
    }
}

Matrix Matrix::block_multiply(const Matrix& other) const {
    Matrix result(m_rows, other.m_cols);
    const int block_size = 64;
    const int num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    for (int i = 0; i < m_rows; i += block_size) {
        for (int j = 0; j < other.m_cols; j += block_size) {
            int i_end = std::min(i + block_size, static_cast<int>(m_rows));
            int j_end = std::min(j + block_size, static_cast<int>(other.m_cols));
            threads.emplace_back(multiply_block, std::cref(*this), std::cref(other),
                                std::ref(result), i, i_end, j, j_end);
        }
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return result;
}

Matrix Matrix::multiply(const Matrix& other, MultiplyMode mode) const {
    print_log(LogLevel::INFO, mode == MultiplyMode::STANDARD ? 
              "matrix multiplication (standard mode)\n" : 
              "matrix multiplication (block mode)\n");
    if (m_cols != other.m_rows) {
        print_log(LogLevel::ERR, "A.cols, B.rows not equals\n");
        throw std::runtime_error("Invalid matrix dimensions for multiplication");
    }

    try {
        switch (mode) {
            case MultiplyMode::STANDARD:
                return standard_multiply(other);
            case MultiplyMode::BLOCK:
                return block_multiply(other);
            case MultiplyMode::AUTO:
            default: {
                int n = std::max({m_rows, m_cols, other.m_cols});
                return n < 100 ? standard_multiply(other) : block_multiply(other);
            }
        }
    } catch (const std::bad_alloc&) {
        print_log(LogLevel::ERR, "memory allocation failed\n");
        throw;
    }
}

void Matrix::print() const {
    if (is_empty()) {
        std::cout << "Matrix is empty\n";
        return;
    }
    std::cout << "Matrix size: " << m_rows << "x" << m_cols << "\n";
    for (std::size_t row = 0; row < m_rows; ++row) {
        std::cout << "[ ";
        for (std::size_t col = 0; col < m_cols - 1; ++col) {
            std::cout << m_data[row * m_cols + col] << ", ";
        }
        std::cout << m_data[row * m_cols + m_cols - 1] << " ]\n";
    }
}
