#include <iostream>
#include <cstdint>
#include "matrix.h"

int main() {
    Matrix::set_log_level(Matrix::LOG_INFO);
    try {
        Matrix mat(SIZE_MAX, SIZE_MAX);
        mat.set_identity();
        mat.print();
    } catch(const MatrixException& err) {
        std::cout << err.get_code() << " " << err.what() << std::endl;
    }
    return 0;
}

