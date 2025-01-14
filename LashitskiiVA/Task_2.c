#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <locale.h>

struct Matrix 
{
    size_t rows;
    size_t cols;
    double* data;
};
typedef struct Matrix Matrix;

enum MatrixExceptionLevel {ERROR, WARNING, INFO, DEBUG};

const Matrix MATRIX_NULL = {0, 0, NULL};


// ��������� �� ������
void exM(const enum MatrixExceptionLevel level, char *msg)
{
    if(level == ERROR) {
        printf("ERROR: %s", msg);
    }

    if(level == WARNING) {
        printf("WARNING: %s", msg);
    }
    
    if(level == INFO) {
        printf("INFO: %s", msg);
    }
}


Matrix allM(const size_t rows, const size_t cols) 
{
    Matrix M;
    
    if (rows == 0 || cols == 0) {
        exM(INFO, "������� �������� 0 �������� ��� �����");
        return (Matrix) {rows, cols, NULL};
    }
    
    size_t size = rows * cols;
    if (size / rows != cols) {
        exM(ERROR, "OVERFLOW: ������������ ���������� ���������.");
        return MATRIX_NULL;
    }
    
    size_t size_in_bytes = size * sizeof(double);
    
    if (size_in_bytes / sizeof(double) != size) {
        exM(ERROR, "OVERFLOW: ������������ ���������� ������");
        return MATRIX_NULL;
    }
    
    M.data = malloc(rows * cols * sizeof(double));
    
    if (M.data == NULL) {
        exM(ERROR, "���� ��������� ������");
        return MATRIX_NULL;
    }
    
    M.rows = rows;
    M.cols = cols;
    return M;
}


void freeM(Matrix* M)  // ������� ��� ������������ ������ �������
{
    if (M == NULL){
        exM(ERROR, "��������� � ����������� ������� ������");
        return;
    }
    
    free(M->data);
    *M = MATRIX_NULL;
}


// ������� �������
void zM(const Matrix M)
{
    memset(M.data, 0, M.cols * M.rows * sizeof(double));
}


// ��������� �������
Matrix iM(size_t size)
{
    Matrix M = allM(size, size);

    zM(M);

    for (size_t idx = 0; idx < size; idx++) {
        M.data[idx * size + idx] = 1.0;
    }

    return M; 
}



void prM(const Matrix M) // ������� ��� ������ �������
{
    for (size_t row = 0; row < M.rows; row++) {
        for (size_t col = 0; col < M.cols; col++) {
            printf("%.2f ", M.data[row * M.cols + col]);
        }
        printf("\n");
    }
}


// C = A + B 
Matrix sumM(const Matrix A, const Matrix B) // �������� ������
{
    if (A.rows != B.rows || A.cols != B.cols) {
        exM(WARNING, "������� ������ �� �������� ��� ��������.\n");
        return MATRIX_NULL;
    }

    Matrix C = allM(A.rows, A.cols);
    
    for (size_t idx = 0; idx < C.rows * C.cols; idx++) {
        C.data[idx] = A.data[idx] + B.data[idx];
    }
    return C;
}


// C = A - B 
Matrix subM(const Matrix A, const Matrix B) // ��������� ������
{
    if (A.rows != B.rows || A.cols != B.cols) {
        exM(WARNING, "������� ������ �� �������� ��� ���������.\n");
        return MATRIX_NULL;
    }

    Matrix C = allM(A.rows, A.cols);
    for (size_t idx = 0; idx < C.rows * C.cols; idx++) {
        C.data[idx] = A.data[idx] - B.data[idx];
    }
    return C;
}


// C = A * B
Matrix mulM(const Matrix A, const Matrix B) // ��������� ������
{
    if (A.cols != B.rows) {
        exM(WARNING,"����� �������� ������ ������� �� ����� ����� ����� ������ �������.\n");
        return MATRIX_NULL;
    }

    Matrix C = allM(A.rows, B.cols);
    
    for (size_t row = 0; row < C.rows; row++) {
        for (size_t col = 0; col < C.cols; col++) {
            C.data[row * B.cols + col] = 0;
            for (size_t idx = 0; idx < A.cols; idx++) {
                C.data[row * C.cols + col] += A.data[row * A.cols + idx] * B.data[idx * B.cols + col];
            }
        }
    }
    return C;
}


Matrix TM(const Matrix A) // ���������������� �������
{
    Matrix T = allM(A.cols, A.rows);
    
    for (size_t row = 0; row < T.rows; row++) {
        for (size_t col = 0; col < T.cols; col++) {
            T.data[col * T.cols + row] = A.data[row * A.cols + col];
        }
    }
    return T;
}


Matrix powM(const Matrix A, int power)  // ���������� ������� � �������
{
    if (A.rows != A.cols) {
        exM(WARNING, "������� ������ ���� ���������� ��� ���������� � �������.\n");
        return MATRIX_NULL;
    }
    
    Matrix result = iM(A.rows); // ������� ��������� �������

    for (int n = 0; n < power; n++) {
        Matrix temp = mulM(result, A);
        freeM(&result);
        result = temp;
    }

    return result;
}

// C = A * k
Matrix scalM(const Matrix A, double scalar) // ��������� ������� �� �����
{
    Matrix C = allM(A.rows, A.cols);
    
    for (size_t idx = 0; idx < C.rows * C.cols; idx++) {
        C.data[idx] = A.data[idx] * scalar;
    }
    return C;
}


double detMt(const Matrix A) // ������������ ������� (��� 2x2 � 3x3)
{
    if (A.rows != A.cols) {
        exM(WARNING, "������� ������ ���� ���������� ��� ����������������.\n");
        return NAN;
    }
    
    if (A.rows == 1 && A.cols == 1) {
        return A.data[0];
    } 
    
    if (A.rows == 2 && A.cols == 2){
        return A.data[0] * A.data[3] - A.data[1] * A.data[2];
    }
    
    if (A.rows == 3 && A.cols == 3) {
        return A.data[0] * (A.data[4] * A.data[8] - A.data[5] * A.data[7]) -
               A.data[1] * (A.data[3] * A.data[8] - A.data[5] * A.data[6]) +
               A.data[2] * (A.data[3] * A.data[7] - A.data[4] * A.data[6]);
    }
    return 0; 
}


double fakt (const unsigned int f) 
{
    unsigned long long int res = 1;
    for (unsigned int idx = 1; idx <= f; idx++) {
        res *= idx;
    }

    return res;
}


// e ^ A
Matrix expM(const Matrix A, const unsigned int num)
{
    if (A.rows != A.cols) {
        exM(WARNING, "������� ������ ���� ���������� ��� ���������� ����������");
        return MATRIX_NULL;
    }

    Matrix E = iM(A.rows);

    if (E.data == NULL) {
        return MATRIX_NULL; // �������� �� �������� ��������� ������
    }

    if (num == 1) {
        return E;
    }
    
    for (size_t cur_num = 1; cur_num < num; ++cur_num) {
        Matrix tmp = powM(A, cur_num);
        if (tmp.data == NULL) {
            freeM(&E); 
            return MATRIX_NULL;
        }

        Matrix scaled_tmp = scalM(tmp, 1.0 / fakt(cur_num)); 

        Matrix new_E = sumM(E, scaled_tmp);
        freeM(&E); 
        E = new_E; 

        freeM(&tmp); 
        freeM(&scaled_tmp);
    }
    
    return E; // ���������� �������������� �������
}


int main() 
{
    setlocale(LC_ALL, "Rus");
    Matrix A = allM(3,3);
    Matrix B = allM(3, 3);

    double data_A[9] = {3, 1, 7, 0, 5, 7, 2, 5, 8};
    double data_B[9] = {5, 0, 8, 1, 9, 6, 3, 2, 1};

    memcpy(A.data, data_A, 9 * sizeof(double));
    memcpy(B.data, data_B, 9 * sizeof(double));

    // ������ �������� ������
    printf("������� A:\n");
    prM(A);

    printf("������� B:\n");
    prM(B);

    // ��������
    Matrix C = sumM(A, B);
    printf("��������� ��������:\n");
    prM(C);

    // ���������
    Matrix D = subM(A, B);
    printf("��������� ���������:\n");
    prM(D);

    // ���������
    Matrix E = mulM(A, B);
    printf("��������� ���������:\n");
    prM(E);

    // ����������������
    Matrix T = TM(A);
    printf("����������������� ������� A:\n");
    prM(T);

    // ���������� � �������
    int power = 3;
    Matrix F = powM(A, power);
    printf("������� A � ������� %d:\n", power);
    prM(F);

    // ��������� �� �����
    double scalar = 5;
    Matrix G = scalM(A, scalar);
    printf("������� A ���������� �� %2.f:\n", scalar);
    prM(G);
    
    // ������������
    printf("������������ ������� A: %2.f \n", detMt(A));
    
    //��������� ����������
    Matrix exponent_A = expM(A, 3);
    printf("��������� ���������� �� A:\n");
    prM(exponent_A);

    // ������������ ������
    freeM(&A);
    freeM(&B);
    freeM(&C);
    freeM(&D);
    freeM(&E);
    freeM(&T);
    freeM(&F);
    freeM(&G);
    freeM(&exponent_A);

    return 0;
}