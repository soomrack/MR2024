#include "Matrix.h"
#include <iostream>

int main() 
{
    Matrix A(3, 3);
    Matrix B(3, 3);

    double data_A[9] = {3, 1, 7, 0, 5, 7, 2, 5, 8};
    double data_B[9] = {5, 0, 8, 1, 9, 6, 3, 2, 1};

    for (size_t i = 0; i < 9; ++i) {
        A.set(i / 3, i % 3, data_A[i]);
        B.set(i / 3, i % 3, data_B[i]);
    }

    std::cout << "Матрица A:" << std::endl;
    A.print();
    std::cout << "Матрица B:" << std::endl;
    B.print();

    Matrix result = A + B;
    std::cout << "Сумма A и B:" << std::endl;
    result.print();

    result = A - B;
    std::cout << "Разность A и B:" << std::endl;
    result.print();

    result = A * B;
    std::cout << "Произведение A и B:" << std::endl;
    result.print();

    double scalar = 2.5;
    result = A * scalar;
    std::cout << "Матрица A, умноженная на " << scalar << ":" << std::endl;
    result.print();

    int power = 4;
    result = A.power(power);
    std::cout << "Матрица A, возведенная в степень " << power << ":" << std::endl;
    result.print();

    Matrix copyA = A;
    std::cout << "Копия матрицы A:" << std::endl;
    copyA.print();
    
    return 0;
}
