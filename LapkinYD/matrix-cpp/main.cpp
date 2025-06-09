#include <iostream>
#include "matrix-cpp.h"

int main(){
    Matrix A(3, 3, std::vector<double>{
        4, 5, 6,
        7, 8, 9, 
        25, 25, 31
    });

    Matrix B(3, 3, std::vector<double>{
        1, 2, 0,
        7, -5, 9, 
        -9, 5, 3
    });

    Matrix C(3, 2, std::vector<double>{
        1, 2,
        7, -5,
        -9, 5
    });
    
    Matrix D(5, 5, std::vector<double>{
        1, 2, -5, 1, 32,
        7, -5, 23, 0, 5,
        -9, 5, 33, 1, -5,
        -4, -5, 2, 0, 0,
        33, 6, 7, 12, 34
    });

    double det = D.getDeterminant();
    std::cout << det << std::endl;
    return 0;
}