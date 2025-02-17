#include "matrix.hpp"
#include <utility>



int main(int argc, char **argv) {
    using namespace MTL;
    Matrix A(3, 3);
    A.to_unit();
    A.print();
    A = std::move(A);
    A.print();

    Matrix B(A);
    B.print();
    Matrix C(A);
    C.print();
    Matrix D;

    D = A * B + C;
    D.print();

    A = D;
    A.print();
}

