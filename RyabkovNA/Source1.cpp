#include <iostream>

class Matrix {
private:
    size_t rows;
    size_t cols;
    double* data;

public:
    // �����������
    Matrix(size_t r, size_t c) : rows(r), cols(c) {
        data = new double[rows * cols](); // ������������� ������
    }

    // ����������
    ~Matrix() {
        delete[] data;
    }

    // ����� ��� ������ �������
    void print() {
        for (size_t row = 0; row < rows; ++row) {
            for (size_t col = 0; col < cols; ++col) {
                std::cout << data[row * cols + col] << " ";
            }
            std::cout << std::endl;
        }
    }

    // ����� ��� ���������� ������� ���������� �������
    void fillRandom() {
        for (size_t i = 0; i < rows * cols; ++i) {
            data[i] = rand() % 10; // ��������� ����� �� 0 �� 9
        }
    }
};

int main() {
    Matrix A(2, 2);
    A.fillRandom();
    std::cout << "������� A:" << std::endl;
    A.print();

    Matrix B(2, 2);
    B.fillRandom();
    std::cout << "������� B:" << std::endl;
    B.print();

    return 0;
}