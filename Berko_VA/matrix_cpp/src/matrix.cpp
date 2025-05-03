#include "matrix.hpp"
#include <algorithm>
#include <sstream>
#include <atomic>
#include <cmath>
#include <mutex>
#include <iostream>
#include <future>

static std::atomic<LogLevel> currentLevel{LogLevel::None};
static std::mutex logMutex;

static void printLog(LogLevel level, const std::string& message) {
#ifdef MATRIX_LOG_ENABLE
    if (level <= currentLevel.load(std::memory_order_relaxed)) {
        std::lock_guard<std::mutex> lock(logMutex);
        switch (level) {
            case LogLevel::Error:
                std::cout << "ERROR: ";
                break;
            case LogLevel::Warning:
                std::cout << "WARNING: ";
                break;
            case LogLevel::Info:
                std::cout << "INFO: ";
                break;
            default:
                break;
        }
        std::cout << message << std::endl;
    }
#endif
}

static bool isValid(double value) noexcept {
    return !std::isnan(value) && !std::isinf(value);
}

Matrix::Matrix() : m_rows(0), m_cols(0), m_data() {}

Matrix::Matrix(std::size_t rows, std::size_t cols) : m_rows(rows), m_cols(cols) {
    std::stringstream ss;
    ss << "Allocate matrix with size: " << rows << "x" << cols << "\n";
    printLog(LogLevel::Info, ss.str());

    if (rows == 0 || cols == 0) {
        m_data.clear();
        return;
    }

    if (rows > std::numeric_limits<std::size_t>::max() / cols) {
        printLog(LogLevel::Error, "Matrix size causes overflow\n");
        throw MatrixException("Matrix size causes overflow", MatrixException::ErrorType::InvalidArgument);
    }
    if (rows * cols > m_data.max_size()) {
        printLog(LogLevel::Error, "Matrix size exceeds maximum allowed size\n");
        throw MatrixException("Matrix size too large", MatrixException::ErrorType::InvalidArgument);
    }

    m_data.resize(rows * cols, 0.0);
}

Matrix::Matrix(const Matrix& other) : m_rows(other.m_rows), m_cols(other.m_cols), m_data(other.m_data) {
    printLog(LogLevel::Info, "Matrix copy\n");
}

Matrix::Matrix(Matrix&& other) noexcept
    : m_rows(std::exchange(other.m_rows, 0)),
      m_cols(std::exchange(other.m_cols, 0)),
      m_data(std::move(other.m_data)) {
    printLog(LogLevel::Info, "Matrix move\n");
}

Matrix& Matrix::operator=(const Matrix& other) {
    printLog(LogLevel::Info, "Matrix copy assign\n");
    if (this != &other) {
        m_rows = other.m_rows;
        m_cols = other.m_cols;
        m_data = other.m_data;
    }
    return *this;
}

Matrix& Matrix::operator=(Matrix&& other) noexcept {
    printLog(LogLevel::Info, "Matrix move assign\n");
    if (this != &other) {
        m_rows = std::exchange(other.m_rows, 0);
        m_cols = std::exchange(other.m_cols, 0);
        m_data = std::move(other.m_data);
    }
    return *this;
}

Matrix Matrix::operator+(const Matrix& other) const {
    printLog(LogLevel::Info, "Matrix sum\n");

    if (m_rows == other.m_rows && m_cols == other.m_cols) {
        Matrix result(m_rows, m_cols);
        for (std::size_t i = 0; i < m_rows * m_cols; ++i) {
            if (isValid(m_data[i]) && isValid(other.m_data[i])) {
                result.m_data[i] = m_data[i] + other.m_data[i];
            } else {
                printLog(LogLevel::Error, "Invalid matrix element\n");
                throw MatrixException("Invalid matrix element", MatrixException::ErrorType::InvalidData);
            }
        }
        return result;
    }

    printLog(LogLevel::Error, "Matrix sizes not equal\n");
    throw MatrixException("Matrix sizes not equal", MatrixException::ErrorType::SizeMismatch,
                         {m_rows, m_cols}, {other.m_rows, other.m_cols});
}

Matrix Matrix::operator-(const Matrix& other) const {
    printLog(LogLevel::Info, "Matrix subtraction\n");

    if (m_rows == other.m_rows && m_cols == other.m_cols) {
        Matrix result(m_rows, m_cols);
        for (std::size_t i = 0; i < m_rows * m_cols; ++i) {
            if (isValid(m_data[i]) && isValid(other.m_data[i])) {
                result.m_data[i] = m_data[i] - other.m_data[i];
            } else {
                printLog(LogLevel::Error, "Invalid matrix element\n");
                throw MatrixException("Invalid matrix element", MatrixException::ErrorType::InvalidData);
            }
        }
        return result;
    }

    printLog(LogLevel::Error, "Matrix sizes not equal\n");
    throw MatrixException("Matrix sizes not equal", MatrixException::ErrorType::SizeMismatch,
                         {m_rows, m_cols}, {other.m_rows, other.m_cols});
}

Matrix Matrix::operator*(const Matrix& other) const {
    printLog(LogLevel::Info, "Matrix multiplication\n");

    if (m_cols == other.m_rows) {
        std::size_t n = std::max({m_rows, m_cols, other.m_cols});
        return n < 100 ? transposeMultiply(other) : parallelOptimizedMultiply(other);
    }

    printLog(LogLevel::Error, "Invalid dimensions for multiplication\n");
    throw MatrixException("Invalid dimensions for multiplication", MatrixException::ErrorType::SizeMismatch,
                         {m_rows, m_cols}, {other.m_rows, other.m_cols});
}

Matrix Matrix::operator*(double number) const {
    printLog(LogLevel::Info, "Matrix multiplication by scalar\n");

    if (isValid(number)) {
        Matrix result(m_rows, m_cols);
        for (std::size_t i = 0; i < m_rows * m_cols; ++i) {
            if (isValid(m_data[i])) {
                result.m_data[i] = m_data[i] * number;
            } else {
                printLog(LogLevel::Error, "Invalid matrix element\n");
                throw MatrixException("Invalid matrix element", MatrixException::ErrorType::InvalidData);
            }
        }
        return result;
    }

    printLog(LogLevel::Error, "Invalid scalar value\n");
    throw MatrixException("Invalid scalar value", MatrixException::ErrorType::InvalidArgument);
}

Matrix operator*(double number, const Matrix& matrix) {
    return matrix * number;
}

double& Matrix::operator()(std::size_t row, std::size_t col) {
    if (row < m_rows && col < m_cols) {
        return m_data[row * m_cols + col];
    }

    printLog(LogLevel::Error, "Index out of bounds\n");
    throw MatrixException("Index out of bounds", MatrixException::ErrorType::OutOfBounds, row, col);
}

const double& Matrix::operator()(std::size_t row, std::size_t col) const {
    if (row < m_rows && col < m_cols) {
        return m_data[row * m_cols + col];
    }

    printLog(LogLevel::Error, "Index out of bounds\n");
    throw MatrixException("Index out of bounds", MatrixException::ErrorType::OutOfBounds, row, col);
}

bool Matrix::operator==(const Matrix& other) const {
    if (m_rows != other.m_rows || m_cols != other.m_cols) {
        return false;
    }
    if (isEmpty() && other.isEmpty()) {
        return true;
    }
    return m_data == other.m_data;
}

void Matrix::setLogLevel(LogLevel level) {
    currentLevel.store(level, std::memory_order_relaxed);
}

void Matrix::setZeros() {
    printLog(LogLevel::Info, "Set matrix to zeros\n");

    if (!isEmpty()) {
        std::fill(m_data.begin(), m_data.end(), 0.0);
        return;
    }

    printLog(LogLevel::Error, "Matrix is empty\n");
    throw MatrixException("Matrix is empty", MatrixException::ErrorType::EmptyMatrix);
}

void Matrix::fillRandom(double min, double max) {
    printLog(LogLevel::Info, "Fill matrix with random values\n");

    if (!isEmpty()) {
        if (min <= max) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<double> dist(min, max);
            for (std::size_t i = 0; i < m_rows; ++i) {
                for (std::size_t j = 0; j < m_cols; ++j) {
                    (*this)(i, j) = dist(gen);
                }
            }
            return;
        }
        printLog(LogLevel::Error, "Invalid range for random values\n");
        throw MatrixException("Minimum value exceeds maximum value", MatrixException::ErrorType::InvalidArgument);
    }

    printLog(LogLevel::Error, "Matrix is empty\n");
    throw MatrixException("Matrix is empty", MatrixException::ErrorType::EmptyMatrix);
}

void Matrix::setIdentity() {
    printLog(LogLevel::Info, "Set matrix to identity\n");

    if (isSquare()) {
        setZeros();
        for (std::size_t i = 0; i < m_rows; ++i) {
            m_data[i * m_cols + i] = 1.0;
        }
        return;
    }

    printLog(LogLevel::Error, "Matrix is not square\n");
    throw MatrixException("Matrix is not square", MatrixException::ErrorType::SizeMismatch, {m_rows, m_cols});
}

void Matrix::transpose() {
    printLog(LogLevel::Info, "Matrix transposition (block algorithm)\n");

    Matrix result(m_cols, m_rows);
    const std::size_t block_size = 64;
    const double* src_data = m_data.data();
    double* dst_data = result.m_data.data();

    for (std::size_t i = 0; i < m_rows; i += block_size) {
        for (std::size_t j = 0; j < m_cols; j += block_size) {
            std::size_t i_end = std::min(i + block_size, m_rows);
            std::size_t j_end = std::min(j + block_size, m_cols);
            for (std::size_t ii = i; ii < i_end; ++ii) {
                for (std::size_t jj = j; jj < j_end; ++jj) {
                    if (isValid(src_data[ii * m_cols + jj])) {
                        dst_data[jj * m_rows + ii] = src_data[ii * m_cols + jj];
                    } else {
                        printLog(LogLevel::Error, "Invalid matrix element\n");
                        throw MatrixException("Invalid matrix element", MatrixException::ErrorType::InvalidData, ii, jj);
                    }
                }
            }
        }
    }

    *this = std::move(result);
}

void Matrix::pow(unsigned int power) {
    printLog(LogLevel::Info, "Matrix power\n");

    if (isSquare()) {
        for (std::size_t i = 0; i < m_rows * m_cols; ++i) {
            if (!isValid(m_data[i])) {
                printLog(LogLevel::Error, "Invalid matrix element\n");
                throw MatrixException("Invalid matrix element", MatrixException::ErrorType::InvalidData);
            }
        }
        Matrix result(m_rows, m_cols);
        result.setIdentity();
        if (power == 0) {
            *this = std::move(result);
            return;
        }
        Matrix temp(*this);
        while (power > 0) {
            if (power % 2 == 1) {
                result = result * temp;
            }
            temp = temp * temp;
            power /= 2;
        }
        *this = std::move(result);
        return;
    }

    printLog(LogLevel::Error, "Matrix is not square\n");
    throw MatrixException("Matrix is not square", MatrixException::ErrorType::SizeMismatch, {m_rows, m_cols});
}

void Matrix::exp() {
    printLog(LogLevel::Info, "Matrix exponent\n");

    if (isSquare()) {
        for (std::size_t i = 0; i < m_rows * m_cols; ++i) {
            if (!isValid(m_data[i])) {
                printLog(LogLevel::Error, "Invalid matrix element\n");
                throw MatrixException("Invalid matrix element", MatrixException::ErrorType::InvalidData);
            }
        }
        Matrix result(m_rows, m_cols);
        result.setIdentity();
        Matrix term(m_rows, m_cols);
        term.setIdentity();
        for (unsigned n = 1; n < 100; ++n) {
            term = (term * *this) * (1.0 / static_cast<double>(n));
            Matrix oldResult = result;
            result = result + term;
            bool converged = true;
            for (std::size_t i = 0; i < m_rows * m_cols; ++i) {
                if (!Matrix::isEqual(result.m_data[i], oldResult.m_data[i])) {
                    converged = false;
                    break;
                }
            }
            if (converged) break;
        }
        *this = std::move(result);
        return;
    }

    printLog(LogLevel::Error, "Matrix is not square\n");
    throw MatrixException("Matrix is not square", MatrixException::ErrorType::SizeMismatch, {m_rows, m_cols});
}

double Matrix::det() const {
    printLog(LogLevel::Info, "Matrix determinant\n");

    if (isEmpty()) {
        printLog(LogLevel::Warning, "Empty matrix\n");
        return 0.0;
    }
    if (isSquare()) {
        Matrix temp(*this);
        for (std::size_t i = 0; i < m_rows * m_cols; ++i) {
            if (!isValid(temp.m_data[i])) {
                printLog(LogLevel::Error, "Invalid matrix element\n");
                throw MatrixException("Invalid matrix element", MatrixException::ErrorType::InvalidData);
            }
        }
        double result = 1.0;
        for (std::size_t rowBase = 0; rowBase < temp.m_rows - 1; ++rowBase) {
            if (std::abs(temp.m_data[rowBase * m_cols + rowBase]) < 1e-10) {
                std::size_t idxNonZero = rowBase;
                for (std::size_t row = rowBase + 1; row < temp.m_rows; ++row) {
                    if (!Matrix::isEqual(temp.m_data[row * m_cols + rowBase], 0.0)) {
                        idxNonZero = row;
                        break;
                    }
                }
                if (idxNonZero == rowBase) {
                    return 0.0;
                }
                for (std::size_t idx = 0; idx < temp.m_cols; ++idx) {
                    std::swap(temp.m_data[rowBase * m_cols + idx], temp.m_data[idxNonZero * m_cols + idx]);
                }
                result *= -1.0;
            }
            for (std::size_t row = rowBase + 1; row < temp.m_rows; ++row) {
                double mult = temp.m_data[row * m_cols + rowBase] / temp.m_data[rowBase * m_cols + rowBase];
                if (!isValid(mult)) {
                    printLog(LogLevel::Error, "Invalid computation in determinant\n");
                    throw MatrixException("Invalid computation in determinant", MatrixException::ErrorType::InvalidData);
                }
                for (std::size_t idx = 0; idx < temp.m_cols; ++idx) {
                    temp.m_data[row * m_cols + idx] -= mult * temp.m_data[rowBase * m_cols + idx];
                }
            }
        }
        for (std::size_t idx = 0; idx < temp.m_rows; ++idx) {
            result *= temp.m_data[idx * m_cols + idx];
        }
        return result;
    }

    printLog(LogLevel::Error, "Matrix is not square\n");
    throw MatrixException("Matrix is not square", MatrixException::ErrorType::SizeMismatch, {m_rows, m_cols});
}

Matrix Matrix::standardMultiply(const Matrix& other) const {
    printLog(LogLevel::Info, "Standard matrix multiplication\n");
    Matrix result(m_rows, other.m_cols);
    const double* a_data = m_data.data();
    const double* b_data = other.m_data.data();
    double* c_data = result.m_data.data();
    for (std::size_t i = 0; i < m_rows; ++i) {
        for (std::size_t j = 0; j < other.m_cols; ++j) {
            double sum = 0.0;
            for (std::size_t k = 0; k < m_cols; ++k) {
                sum += a_data[i * m_cols + k] * b_data[k * other.m_cols + j];
            }
            c_data[i * other.m_cols + j] = sum;
        }
    }
    return result;
}

Matrix Matrix::transposeMultiply(const Matrix& other) const {
    printLog(LogLevel::Info, "Matrix multiplication with transpose\n");
    Matrix transposed = other;
    transposed.transpose();
    Matrix result(m_rows, other.m_cols);
    const double* a_data = m_data.data();
    const double* b_data = transposed.m_data.data();
    double* c_data = result.m_data.data();
    for (std::size_t i = 0; i < m_rows; ++i) {
        for (std::size_t j = 0; j < other.m_cols; ++j) {
            double sum = 0.0;
            for (std::size_t k = 0; k < m_cols; ++k) {
                sum += a_data[i * m_cols + k] * b_data[j * m_cols + k];
            }
            c_data[i * other.m_cols + j] = sum;
        }
    }
    return result;
}

void Matrix::multiplyBlock(const Matrix& a, const Matrix& b, Matrix& result,
                          std::size_t iStart, std::size_t iEnd, std::size_t jStart, std::size_t jEnd) {
    const double* a_data = a.m_data.data();
    const double* b_data = b.m_data.data();
    double* c_data = result.m_data.data();
    const std::size_t unroll_factor = 4;
    const std::size_t block_rows = 3;
    const std::size_t block_cols = 4;

    for (std::size_t i = iStart; i < iEnd; i += block_rows) {
        for (std::size_t j = jStart; j < jEnd; j += block_cols) {
            std::size_t i_end = std::min(i + block_rows, iEnd);
            std::size_t j_end = std::min(j + block_cols, jEnd);
            for (std::size_t k = 0; k < a.m_cols; ++k) {
                for (std::size_t ii = i; ii < i_end; ++ii) {
                    double a_ik = a_data[ii * a.m_cols + k];
                    for (std::size_t jj = j; jj < j_end; jj += unroll_factor) {
                        if (jj + unroll_factor <= j_end) {
                            c_data[ii * result.m_cols + jj] += a_ik * b_data[k * b.m_cols + jj];
                            c_data[ii * result.m_cols + jj + 1] += a_ik * b_data[k * b.m_cols + jj + 1];
                            c_data[ii * result.m_cols + jj + 2] += a_ik * b_data[k * b.m_cols + jj + 2];
                            c_data[ii * result.m_cols + jj + 3] += a_ik * b_data[k * b.m_cols + jj + 3];
                        } else {
                            for (std::size_t jj_rem = jj; jj_rem < j_end; ++jj_rem) {
                                c_data[ii * result.m_cols + jj_rem] += a_ik * b_data[k * b.m_cols + jj_rem];
                            }
                        }
                    }
                }
            }
        }
    }
}

Matrix Matrix::blockMultiply(const Matrix& other) const {
    printLog(LogLevel::Info, "Block matrix multiplication\n");
    Matrix result(m_rows, other.m_cols);
    std::fill(result.m_data.begin(), result.m_data.end(), 0.0);
    const std::size_t blockSize = 64;
    const std::size_t maxThreads = std::min(static_cast<std::size_t>(std::thread::hardware_concurrency()), static_cast<std::size_t>(8));
    std::vector<std::thread> threads;
    threads.reserve(maxThreads);

    for (std::size_t i = 0; i < m_rows; i += blockSize) {
        for (std::size_t j = 0; j < other.m_cols; j += blockSize) {
            std::size_t iEnd = std::min(i + blockSize, m_rows);
            std::size_t jEnd = std::min(j + blockSize, other.m_cols);
            if (threads.size() >= maxThreads) {
                for (auto& thread : threads) {
                    thread.join();
                }
                threads.clear();
            }
            threads.emplace_back(multiplyBlock, std::cref(*this), std::cref(other),
                                std::ref(result), i, iEnd, j, jEnd);
        }
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return result;
}

Matrix Matrix::blockTransposeMultiply(const Matrix& other) const {
    printLog(LogLevel::Info, "Block matrix multiplication with transpose\n");
    Matrix transposed = other;
    transposed.transpose();
    Matrix result(m_rows, other.m_cols);
    std::fill(result.m_data.begin(), result.m_data.end(), 0.0);
    const std::size_t blockSize = 64;
    const std::size_t maxThreads = std::min(static_cast<std::size_t>(std::thread::hardware_concurrency()), static_cast<std::size_t>(8));
    std::vector<std::thread> threads;
    threads.reserve(maxThreads);

    for (std::size_t i = 0; i < m_rows; i += blockSize) {
        for (std::size_t j = 0; j < other.m_cols; j += blockSize) {
            std::size_t iEnd = std::min(i + blockSize, m_rows);
            std::size_t jEnd = std::min(j + blockSize, other.m_cols);
            if (threads.size() >= maxThreads) {
                for (auto& thread : threads) {
                    thread.join();
                }
                threads.clear();
            }
            threads.emplace_back(multiplyBlock, std::cref(*this), std::cref(transposed),
                                std::ref(result), i, iEnd, j, jEnd);
        }
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return result;
}

Matrix Matrix::optimizedMultiply(const Matrix& other) const {
    printLog(LogLevel::Info, "Optimized matrix multiplication (ikj, unrolled, register blocked)\n");
    Matrix result(m_rows, other.m_cols);
    const double* a_data = m_data.data();
    const double* b_data = other.m_data.data();
    double* c_data = result.m_data.data();
    const std::size_t unroll_factor = 4;
    const std::size_t block_rows = 3;
    const std::size_t block_cols = 4;

    std::fill(c_data, c_data + m_rows * other.m_cols, 0.0);

    for (std::size_t i = 0; i < m_rows; i += block_rows) {
        for (std::size_t j = 0; j < other.m_cols; j += block_cols) {
            std::size_t i_end = std::min(i + block_rows, m_rows);
            std::size_t j_end = std::min(j + block_cols, other.m_cols);
            for (std::size_t k = 0; k < m_cols; ++k) {
                for (std::size_t ii = i; ii < i_end; ++ii) {
                    double a_ik = a_data[ii * m_cols + k];
                    for (std::size_t jj = j; jj < j_end; jj += unroll_factor) {
                        if (jj + unroll_factor <= j_end) {
                            c_data[ii * other.m_cols + jj] += a_ik * b_data[k * other.m_cols + jj];
                            c_data[ii * other.m_cols + jj + 1] += a_ik * b_data[k * other.m_cols + jj + 1];
                            c_data[ii * other.m_cols + jj + 2] += a_ik * b_data[k * other.m_cols + jj + 2];
                            c_data[ii * other.m_cols + jj + 3] += a_ik * b_data[k * other.m_cols + jj + 3];
                        } else {
                            for (std::size_t jj_rem = jj; jj_rem < j_end; ++jj_rem) {
                                c_data[ii * other.m_cols + jj_rem] += a_ik * b_data[k * other.m_cols + jj_rem];
                            }
                        }
                    }
                }
            }
        }
    }

    return result;
}

Matrix Matrix::optimizedTransposeMultiply(const Matrix& other) const {
    printLog(LogLevel::Info, "Optimized matrix multiplication with transpose (ikj, unrolled, register blocked)\n");
    Matrix transposed = other;
    transposed.transpose();
    Matrix result(m_rows, other.m_cols);
    const double* a_data = m_data.data();
    const double* b_data = transposed.m_data.data();
    double* c_data = result.m_data.data();
    const std::size_t unroll_factor = 4;
    const std::size_t block_rows = 3;
    const std::size_t block_cols = 4;

    std::fill(c_data, c_data + m_rows * other.m_cols, 0.0);

    for (std::size_t i = 0; i < m_rows; i += block_rows) {
        for (std::size_t j = 0; j < other.m_cols; j += block_cols) {
            std::size_t i_end = std::min(i + block_rows, m_rows);
            std::size_t j_end = std::min(j + block_cols, other.m_cols);
            for (std::size_t k = 0; k < m_cols; ++k) {
                for (std::size_t ii = i; ii < i_end; ++ii) {
                    double a_ik = a_data[ii * m_cols + k];
                    for (std::size_t jj = j; jj < j_end; jj += unroll_factor) {
                        if (jj + unroll_factor <= j_end) {
                            c_data[ii * other.m_cols + jj] += a_ik * b_data[jj * m_cols + k];
                            c_data[ii * other.m_cols + jj + 1] += a_ik * b_data[(jj + 1) * m_cols + k];
                            c_data[ii * other.m_cols + jj + 2] += a_ik * b_data[(jj + 2) * m_cols + k];
                            c_data[ii * other.m_cols + jj + 3] += a_ik * b_data[(jj + 3) * m_cols + k];
                        } else {
                            for (std::size_t jj_rem = jj; jj_rem < j_end; ++jj_rem) {
                                c_data[ii * other.m_cols + jj_rem] += a_ik * b_data[jj_rem * m_cols + k];
                            }
                        }
                    }
                }
            }
        }
    }

    return result;
}

Matrix Matrix::parallelOptimizedMultiply(const Matrix& other) const {
    printLog(LogLevel::Info, "Parallel optimized matrix multiplication (static tasks, no transpose copy)\n");
    Matrix result(m_rows, other.m_cols);
    std::fill(result.m_data.begin(), result.m_data.end(), 0.0);

    const std::size_t blockSize = m_rows <= 256 ? 64 : m_rows <= 512 ? 96 : 128;
    const std::size_t kBlockSize = m_rows <= 512 ? 32 : 48;
    const std::size_t jBlockSize = m_rows > 512 ? 128 : other.m_cols;
    const std::size_t maxThreads = std::min(static_cast<std::size_t>(std::thread::hardware_concurrency()), static_cast<std::size_t>(8));

    std::vector<std::future<void>> futures;
    futures.reserve(maxThreads);
    const std::size_t rowsPerThread = (m_rows + maxThreads - 1) / maxThreads;

    for (std::size_t t = 0; t < maxThreads; ++t) {
        std::size_t iStart = t * rowsPerThread;
        std::size_t iEnd = std::min(iStart + rowsPerThread, m_rows);
        if (iStart < m_rows) {
            futures.push_back(std::async(std::launch::async, [this, &other, &result, iStart, iEnd, blockSize, kBlockSize, jBlockSize]() {
                const double* a_data = m_data.data();
                const double* b_data = other.m_data.data();
                double* c_data = result.m_data.data();
                const std::size_t unroll_factor = 8;
                const std::size_t block_rows = 4;
                const std::size_t block_cols = 8;

                std::vector<double> local_buffer(block_rows * block_cols, 0.0);

                for (std::size_t ii = iStart; ii < iEnd; ii += block_rows) {
                    for (std::size_t j = 0; j < result.m_cols; j += jBlockSize) {
                        std::size_t j_end_block = std::min(j + jBlockSize, result.m_cols);
                        for (std::size_t jj = j; jj < j_end_block; jj += block_cols) {
                            std::size_t ii_end = std::min(ii + block_rows, iEnd);
                            std::size_t jj_end = std::min(jj + block_cols, j_end_block);
                            std::fill(local_buffer.begin(), local_buffer.end(), 0.0);
                            for (std::size_t k = 0; k < m_cols; k += kBlockSize) {
                                std::size_t k_end = std::min(k + kBlockSize, m_cols);
                                for (std::size_t iii = ii; iii < ii_end; ++iii) {
                                    for (std::size_t kk = k; kk < k_end; ++kk) {
                                        double a_ik = a_data[iii * m_cols + kk];
                                        for (std::size_t jjj = jj; jjj < jj_end; jjj += unroll_factor) {
                                            if (jjj + unroll_factor <= jj_end) {
                                                local_buffer[(iii - ii) * block_cols + (jjj - jj)] += a_ik * b_data[jjj * m_cols + kk];
                                                local_buffer[(iii - ii) * block_cols + (jjj - jj) + 1] += a_ik * b_data[(jjj + 1) * m_cols + kk];
                                                local_buffer[(iii - ii) * block_cols + (jjj - jj) + 2] += a_ik * b_data[(jjj + 2) * m_cols + kk];
                                                local_buffer[(iii - ii) * block_cols + (jjj - jj) + 3] += a_ik * b_data[(jjj + 3) * m_cols + kk];
                                                local_buffer[(iii - ii) * block_cols + (jjj - jj) + 4] += a_ik * b_data[(jjj + 4) * m_cols + kk];
                                                local_buffer[(iii - ii) * block_cols + (jjj - jj) + 5] += a_ik * b_data[(jjj + 5) * m_cols + kk];
                                                local_buffer[(iii - ii) * block_cols + (jjj - jj) + 6] += a_ik * b_data[(jjj + 6) * m_cols + kk];
                                                local_buffer[(iii - ii) * block_cols + (jjj - jj) + 7] += a_ik * b_data[(jjj + 7) * m_cols + kk];
                                            } else {
                                                for (std::size_t jjj_rem = jjj; jjj_rem < jj_end; ++jjj_rem) {
                                                    local_buffer[(iii - ii) * block_cols + (jjj_rem - jj)] += a_ik * b_data[jjj_rem * m_cols + kk];
                                                }
                                            }
                                        }
                                    }
                                }
                            }

                            for (std::size_t iii = ii; iii < ii_end; ++iii) {
                                for (std::size_t jjj = jj; jjj < jj_end; ++jjj) {
                                    c_data[iii * result.m_cols + jjj] += local_buffer[(iii - ii) * block_cols + (jjj - jj)];
                                }
                            }
                        }
                    }
                }
            }));
        }
    }

    for (auto& future : futures) {
        future.get();
    }

    return result;
}


void Matrix::recursiveMultiplyView(const MatrixView& a, const MatrixView& b, Matrix& result) {
    if (a.rows <= 64 || a.cols <= 64 || b.cols <= 64) {
        Matrix temp_a(a.rows, a.cols);
        Matrix temp_b(b.rows, b.cols);
        for (std::size_t i = 0; i < a.rows; ++i) {
            for (std::size_t j = 0; j < a.cols; ++j) {
                temp_a.m_data[i * a.cols + j] = a.data[(i + a.row_offset) * a.stride + j + a.col_offset];
            }
        }
        for (std::size_t i = 0; i < b.rows; ++i) {
            for (std::size_t j = 0; j < b.cols; ++j) {
                temp_b.m_data[i * b.cols + j] = b.data[(i + b.row_offset) * b.stride + j + b.col_offset];
            }
        }
        Matrix temp_result = temp_a.optimizedMultiply(temp_b);
        for (std::size_t i = 0; i < a.rows; ++i) {
            for (std::size_t j = 0; j < b.cols; ++j) {
                result.m_data[i * result.m_cols + j] = temp_result.m_data[i * b.cols + j];
            }
        }
        return;
    }

    std::size_t m = a.rows / 2;
    std::size_t n = a.cols / 2;
    std::size_t p = b.cols / 2;

    MatrixView A11(a.data, m, n, a.row_offset, a.col_offset, a.stride);
    MatrixView A12(a.data, m, a.cols - n, a.row_offset, a.col_offset + n, a.stride);
    MatrixView A21(a.data, a.rows - m, n, a.row_offset + m, a.col_offset, a.stride);
    MatrixView A22(a.data, a.rows - m, a.cols - n, a.row_offset + m, a.col_offset + n, a.stride);

    MatrixView B11(b.data, n, p, b.row_offset, b.col_offset, b.stride);
    MatrixView B12(b.data, n, b.cols - p, b.row_offset, b.col_offset + p, b.stride);
    MatrixView B21(b.data, b.rows - n, p, b.row_offset + n, b.col_offset, b.stride);
    MatrixView B22(b.data, b.rows - n, b.cols - p, b.row_offset + n, b.col_offset + p, b.stride);

    Matrix C11(m, p), C12(m, b.cols - p), C21(a.rows - m, p), C22(a.rows - m, b.cols - p);
    std::fill(C11.m_data.begin(), C11.m_data.end(), 0.0);
    std::fill(C12.m_data.begin(), C12.m_data.end(), 0.0);
    std::fill(C21.m_data.begin(), C21.m_data.end(), 0.0);
    std::fill(C22.m_data.begin(), C22.m_data.end(), 0.0);

    const std::size_t maxThreads = std::min(static_cast<std::size_t>(std::thread::hardware_concurrency()), static_cast<std::size_t>(8));
    std::vector<std::thread> threads;
    threads.reserve(4);

    threads.emplace_back([&]() {
        Matrix temp1(m, p), temp2(m, p);
        recursiveMultiplyView(A11, B11, temp1);
        recursiveMultiplyView(A12, B21, temp2);
        for (std::size_t i = 0; i < m * p; ++i) {
            C11.m_data[i] = temp1.m_data[i] + temp2.m_data[i];
        }
    });

    threads.emplace_back([&]() {
        Matrix temp1(m, b.cols - p), temp2(m, b.cols - p);
        recursiveMultiplyView(A11, B12, temp1);
        recursiveMultiplyView(A12, B22, temp2);
        for (std::size_t i = 0; i < m * (b.cols - p); ++i) {
            C12.m_data[i] = temp1.m_data[i] + temp2.m_data[i];
        }
    });

    threads.emplace_back([&]() {
        Matrix temp1(a.rows - m, p), temp2(a.rows - m, p);
        recursiveMultiplyView(A21, B11, temp1);
        recursiveMultiplyView(A22, B21, temp2);
        for (std::size_t i = 0; i < (a.rows - m) * p; ++i) {
            C21.m_data[i] = temp1.m_data[i] + temp2.m_data[i];
        }
    });

    threads.emplace_back([&]() {
        Matrix temp1(a.rows - m, b.cols - p), temp2(a.rows - m, b.cols - p);
        recursiveMultiplyView(A21, B12, temp1);
        recursiveMultiplyView(A22, B22, temp2);
        for (std::size_t i = 0; i < (a.rows - m) * (b.cols - p); ++i) {
            C22.m_data[i] = temp1.m_data[i] + temp2.m_data[i];
        }
    });

    for (auto& thread : threads) {
        thread.join();
    }

    for (std::size_t i = 0; i < m; ++i) {
        for (std::size_t j = 0; j < p; ++j) {
            result.m_data[i * result.m_cols + j] = C11.m_data[i * p + j];
        }
        for (std::size_t j = p; j < b.cols; ++j) {
            result.m_data[i * result.m_cols + j] = C12.m_data[i * (b.cols - p) + (j - p)];
        }
    }
    for (std::size_t i = m; i < a.rows; ++i) {
        for (std::size_t j = 0; j < p; ++j) {
            result.m_data[i * result.m_cols + j] = C21.m_data[(i - m) * p + j];
        }
        for (std::size_t j = p; j < b.cols; ++j) {
            result.m_data[i * result.m_cols + j] = C22.m_data[(i - m) * (b.cols - p) + (j - p)];
        }
    }
}

Matrix Matrix::recursiveMultiply(const Matrix& other) const {
    printLog(LogLevel::Info, "Recursive matrix multiplication (parallel, view-based)\n");

    if (m_cols != other.m_rows) {
        printLog(LogLevel::Error, "Invalid dimensions for multiplication\n");
        throw MatrixException("Invalid dimensions for multiplication", MatrixException::ErrorType::SizeMismatch,
                             {m_rows, m_cols}, {other.m_rows, other.m_cols});
    }

    Matrix result(m_rows, other.m_cols);
    std::fill(result.m_data.begin(), result.m_data.end(), 0.0);
    MatrixView a_view(m_data.data(), m_rows, m_cols, 0, 0, m_cols);
    MatrixView b_view(other.m_data.data(), other.m_rows, other.m_cols, 0, 0, other.m_cols);
    recursiveMultiplyView(a_view, b_view, result);
    return result;
}

Matrix Matrix::multiply(const Matrix& other, MultiplyMode mode) const {
    printLog(LogLevel::Info, mode == MultiplyMode::Standard ? "Matrix multiplication (standard mode)\n" :
                             mode == MultiplyMode::Block ? "Matrix multiplication (block mode)\n" :
                             mode == MultiplyMode::Transpose ? "Matrix multiplication (transpose mode)\n" :
                             mode == MultiplyMode::BlockTranspose ? "Matrix multiplication (block transpose mode)\n" :
                             mode == MultiplyMode::Optimized ? "Matrix multiplication (optimized mode)\n" :
                             mode == MultiplyMode::Recursive ? "Matrix multiplication (recursive mode)\n" :
                             mode == MultiplyMode::ParallelOptimized ? "Matrix multiplication (parallel optimized mode)\n" :
                             mode == MultiplyMode::OptimizedTranspose ? "Matrix multiplication (optimized transpose mode)\n" :
                             "Matrix multiplication (auto mode)\n");

    if (m_cols == other.m_rows) {
        switch (mode) {
            case MultiplyMode::Standard:
                return standardMultiply(other);
            case MultiplyMode::Block:
                return blockMultiply(other);
            case MultiplyMode::Transpose:
                return transposeMultiply(other);
            case MultiplyMode::BlockTranspose:
                return blockTransposeMultiply(other);
            case MultiplyMode::Optimized:
                return optimizedMultiply(other);
            case MultiplyMode::Recursive:
                return recursiveMultiply(other);
            case MultiplyMode::ParallelOptimized:
                return parallelOptimizedMultiply(other);
            case MultiplyMode::OptimizedTranspose:
                return optimizedTransposeMultiply(other);
            case MultiplyMode::Auto:
            default: {
                std::size_t n = std::max({m_rows, m_cols, other.m_cols});
                return n < 100 ? optimizedTransposeMultiply(other) : parallelOptimizedMultiply(other);
            }
        }
    }

    printLog(LogLevel::Error, "Invalid dimensions for multiplication\n");
    throw MatrixException("Invalid dimensions for multiplication", MatrixException::ErrorType::SizeMismatch,
                         {m_rows, m_cols}, {other.m_rows, other.m_cols});
}

void Matrix::print() const {
    if (isEmpty()) {
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
