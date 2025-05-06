#include <iostream>
#include <vector>
#include <stdexcept>
#include <cmath>

using namespace std;

class Matrix {
private:
    size_t rows;
    size_t cols;
    vector<vector<double>> data;
public:
    Matrix(size_t r, size_t c) : rows(r), cols(c), data(r, vector<double>(c, 0)) {}

    //Деструктор, копирование, присваивание, перенос


    //Заполнение матрицы значениями из массива
    void matrix_fill(const vector<double>& values)
    {
        if (values.size() != rows * cols) {
            throw runtime_error("Неверное количество элементов для заполнения матрицы.");
        }
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                data[i][j] = values[i * cols + j];
            }
        }
    }

    void matrix_print() const
    {
        for (const auto& row : data) {
            for (double val : row) {
                cout << val << " ";
            }
            cout << endl;
        }
        cout << endl;
    }

    //Сложение
    Matrix operator+(const Matrix& B)
    {
        if (cols != B.cols || rows != B.rows) {
            throw runtime_error("Размеры матриц не совпадают.");
        }

        Matrix C(rows, cols);

        for (size_t i = 0; i < rows * cols; i++) {
            for (size_t j = 0; j < cols; j++) {
                C.data[i][j] = data[i][j] + B.data[i][j];
            }

        }

        return C;
    }

    //Вычитание
    Matrix operator-(const Matrix& B)
    {
        if (cols != B.cols || rows != B.rows) {
            throw runtime_error("Размеры матриц не совпадают.");
        }

        Matrix C(rows, cols);

        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                C.data[i][j] = data[i][j] - B.data[i][j];
            }

        }

        return C;
    }

    //Умножение
    Matrix operator*(const Matrix& B)                                 //Дважды выделяется память
    {
        if (cols != B.rows) {
            throw runtime_error("Число столбцов не соответствует числу строк.");
        }

        Matrix C(rows, B.cols);

        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < B.cols; j++) {
                for (size_t k = 0; k < cols; k++) {
                    C.data[i][j] += data[i][k] * B.data[k][j];
                }
            }
        }
        return C;
    }

    //Умножение на число
    Matrix operator*(double number) const
    {
        Matrix C(rows, cols);

        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                C.data[i][j] = data[i][j] * number;
            }
        }
        return C;
    }

    //Транспонирование
    Matrix matrix_trans() const
    {
        Matrix T(cols, rows);

        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                T.data[j][i] = data[i][j];
            }
        }
        return T;
    }

    //Получение подматрицы
    Matrix matrix_submatrix(size_t row_exclude, size_t col_exclude) const
    {
        Matrix submatrix(rows - 1, cols - 1);
        size_t submatrix_i = 0, submatrix_j = 0;

        for (size_t i = 0; i < rows; i++) {
            if (i == row_exclude) continue;
            submatrix_j = 0;
            for (size_t j = 0; j < cols; j++) {
                if (j == col_exclude) continue;
                submatrix.data[submatrix_i][submatrix_j] = data[i][j];
                submatrix_j++;
            }
            submatrix_i++;
        }
        return submatrix;
    }

    //Определитель
    double matrix_determinant() const
    {
        if ((cols != rows) || rows == 0 || cols == 0) {
            throw runtime_error("Не квадратная матрица.");
        }
        if (rows == 1) {
            return data[0][0];
        }
        if (rows == 2) {
            return data[0][0] * data[1][1] - data[0][1] * data[1][0];
        }

        double det = 0;
        double sign = 1;

        for (size_t j = 0; j < cols; j++) {

            Matrix submatrix = matrix_submatrix(0, j);

            det += sign * data[0][j] * submatrix.matrix_determinant();
            sign = -sign;
        }
        return det;
    }

    //Единичная матрица
    static Matrix matrix_identity(const size_t size)
    {
        Matrix identity_matrix(size, size);

        for (size_t i = 0; i < size; ++i) {
            identity_matrix.data[i][i] = 1;
        }

        return identity_matrix;
    }


    //Возведение в степень
    Matrix operator^(unsigned int n)
    {
        if (rows != cols) {
            throw runtime_error("Матрица не квадратная.");
        }

        if (n == 0) return matrix_identity(rows);                    //Нулевая степень

        Matrix matrix_powered_to_n = matrix_identity(rows);
        Matrix temp = *this;

        while (n > 0) {
            if (n % 2 == 1) {
                matrix_powered_to_n = matrix_powered_to_n * temp;
            }
            temp = temp * temp;
            n /= 2;
        }

        return matrix_powered_to_n;
    }


    //Инверсия
    Matrix matrix_inverse() const
    {
        double det = matrix_determinant();

        if (abs(det) < 0.00000001) {
            throw runtime_error("Определитель = 0.");
        }

        if (rows == 1) {                                               //1 на 1
            return Matrix(1, 1).matrix_fill({ 1.0 / data[0][0] });
        }

        Matrix additional_matrix(rows, cols);

        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                Matrix submatrix = matrix_submatrix(i, j);
                additional_matrix.data[i][j] = (i + j) % 2 == 0 ? submatrix.matrix_determinant() : -submatrix.matrix_determinant();
            }
        }

        Matrix inverse_matrix = additional_matrix * (1.0 / det);

        return inverse_matrix;
    }


    //Экспонента
    Matrix matrix_exponent(int n) const
    {
        if (rows != cols) {
            throw runtime_error("Матрица не квадратная.");
        }

        Matrix result = matrix_identity(rows);
        Matrix temp(rows, cols);
        Matrix temp2(rows, cols);
        Matrix exp_result = matrix_identity(rows);
        double factorial = 1.0;

        for (int i = 1; i <= n; i++) {
            temp = *this ^ i;  // temp = A^i
            factorial *= i;
            temp2 = temp * (1.0 / factorial);  // temp2 = (A^i) / i!
            exp_result = exp_result + temp2;  // exp_result += temp2
        }

        return exp_result;
    }
};

//Итоговые вычисления
void calculations()
{
    Matrix A(2, 2);
    Matrix B(2, 2);

    A.matrix_fill({ 1, 2, 3, 4 });
    B.matrix_fill({ 5, 6, 7, 8 });

    cout << "Matrix A:\n";
    A.matrix_print();

    cout << "Matrix B:\n";
    B.matrix_print();

    cout << "Addition:\n";
    (A + B).matrix_print();

    cout << "Subtraction:\n";
    (A - B).matrix_print();

    cout << "Multiplication:\n";
    (A * B).matrix_print();

    cout << "Multiplication by 2:\n";
    (A * 2).matrix_print();

    cout << "Transpose A:\n";
    A.matrix_trans().matrix_print();

    cout << "Determinant of A: " << A.matrix_determinant() << endl;

    cout << "A ^ 2:\n";
    (A ^ 2).matrix_print();

    cout << "Inverse of A:\n";
    A.matrix_inverse().matrix_print();

    cout << "Matrix Exponential of A:\n";
    A.matrix_exponent().matrix_print();

}


int main()
{
    calculations();
    return 0;
}