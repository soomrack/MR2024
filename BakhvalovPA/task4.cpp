#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <cstdlib>
#include <ctime>

using namespace std;

class Matrix {
private:
    vector<vector<double>> data;
    int rows, cols;

public:
    
    Matrix(int r, int c) : rows(r), cols(c), data(r, vector<double>(c, 0)) {}
        
    void fillSequential() {
        double val = 1.0;
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                data[i][j] = val++;
            }
        }
    }

    
    void fillRandom() {
        srand(time(0));
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                data[i][j] = rand() % 19 - 9; // -9 to 9
            }
        }
    }

    void print() const {
        for (const auto& row : data) {
            for (double val : row) {
                cout << setw(8) << fixed << setprecision(2) << val << " ";
            }
            cout << endl;
        }
    }

    Matrix operator+(const Matrix& other) const {
        Matrix result(rows, cols);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                result.data[i][j] = data[i][j] + other.data[i][j];
            }
        }
        return result;
    }

    Matrix operator-(const Matrix& other) const {
        Matrix result(rows, cols);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                result.data[i][j] = data[i][j] - other.data[i][j];
            }
        }
        return result;
    }

    Matrix operator*(const Matrix& other) const {
        Matrix result(rows, other.cols);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < other.cols; ++j) {
                for (int k = 0; k < cols; ++k) {
                    result.data[i][j] += data[i][k] * other.data[k][j];
                }
            }
        }
        return result;
    }

    Matrix operator*(double scalar) const {
        Matrix result(rows, cols);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                result.data[i][j] = data[i][j] * scalar;
            }
        }
        return result;
    }

    Matrix transpose() const {
        Matrix result(cols, rows);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                result.data[j][i] = data[i][j];
            }
        }
        return result;
    }

    Matrix pow(int n) const {
        if (rows != cols) throw "Matrix must be square!";

        Matrix result(rows, cols);
       
        for (int i = 0; i < rows; ++i) {
            result.data[i][i] = 1;
        }

        Matrix power = *this;
        while (n > 0) {
            if (n % 2 == 1) {
                result = result * power;
            }
            power = power * power;
            n /= 2;
        }
        return result;
    }

    double determinant() const {
        if (rows != 3 || cols != 3) throw "Method works only for 3x3 matrices!";

        return data[0][0] * (data[1][1] * data[2][2] - data[1][2] * data[2][1])
            - data[0][1] * (data[1][0] * data[2][2] - data[1][2] * data[2][0])
            + data[0][2] * (data[1][0] * data[2][1] - data[1][1] * data[2][0]);
    }

    Matrix exp(int terms = 10) const {
        if (rows != cols) throw "Matrix must be square!";

        Matrix result(rows, cols);
        Matrix term(rows, cols);

        for (int i = 0; i < rows; ++i) {
            term.data[i][i] = 1;
            result.data[i][i] = 1;
        }

        for (int n = 1; n <= terms; ++n) {
            term = term * (*this) * (1.0 / n);
            result = result + term;
        }

        return result;
    }
};

int main() {
    try {
        Matrix A(3, 3), B(3, 3);
        
        A.fillSequential();
        cout << "Matrix A:\n";
        A.print();
                
        B.fillRandom();
        cout << "\nMatrix B:\n";
        B.print();

        cout << "\nSum A + B:\n";
        (A + B).print();

        cout << "\nDifference A - B:\n";
        (A - B).print();

        cout << "\nProduct A * B:\n";
        (A * B).print();

        double scalar = 2.5;
        cout << "\nScalar product A * " << scalar << ":\n";
        (A * scalar).print();

        cout << "\nTranspose of A:\n";
        A.transpose().print();

        int power = 3;
        cout << "\nMatrix A to the power of " << power << ":\n";
        A.pow(power).print();

        cout << "\nDeterminant of matrix A: " << A.determinant() << endl;

        cout << "\nExponential of matrix A:\n";
        A.exp().print();

    }
    catch (const char* msg) {
        cerr << "Error: " << msg << endl;
        return 1;
    }

    return 0;
}
