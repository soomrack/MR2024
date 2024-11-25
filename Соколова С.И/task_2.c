#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


struct Matrix {
    size_t cols;
    size_t rows;
    double *data;
};

typedef struct Matrix Matrix;

enum  MatrixExceptionLevel {ERROR, WARNING, INFO, DEBUG};


void matrix_exception(const enum MatrixExceptionLevel level, char *msg) {
    if (level == ERROR) {
        printf("ERROR %s", msg);
    }
    if (level == WARNING) {
        printf("WARNING %s", msg);
    }
}


Matrix matrix_allocreate(const size_t cols, const size_t rows) {
    struct Matrix A = {0, 0, NULL};

    if (cols == 0 || rows == 0) return (struct Matrix) {
        cols,rows,NULL
    };

    if(((double)SIZE_MAX / A.cols / A.rows / sizeof(double)) < 1.) {
        matrix_exception(ERROR, "OVERFLOW: Data overflow");
        return (struct Matrix) {
            0,0, NULL
        };
    }

    A.cols = cols;
    A.rows = rows;
    A.data = (double*)malloc(cols * rows * sizeof(double));
    if(A.data == NULL) {
        matrix_exception(ERROR, "Allocation memory fail");
        return (struct Matrix) {
            0,0, NULL
        };
    }
    return A;
}

int matrix_free(struct Matrix *A) {
    if(A == NULL) {
        matrix_exception(ERROR, "NULL matrix");
        return 1;
    }

    free(A->data);
    *A = (struct Matrix) {
        0,0, NULL
    };
}


void matrix_zeros(struct Matrix A) {
    memset(A.data, 0, A.cols * A.rows * sizeof(A.data));
}


void matrix_random(struct Matrix A) {
    for (size_t index = 0; index < A.cols * A.rows; index++)
        A.data[index] = (double)(rand() % 5);
}


void matrix_print(const struct Matrix A) {
    printf("\n");
    for (size_t row = 0; row < A.rows ; row++) {
        for (size_t col = 0; col < A.cols ; col++) {
            printf("%lf ", A.data[row * A.cols + col]);
        }
        printf("\n");
    }
    printf("\n");
}


int matrix_copy(Matrix C, const Matrix A) {
    if((C.cols != A.cols) || (C.rows != A.rows)) {
        matrix_exception(ERROR, "Cant copy matrix");
        return 1;
    }
    memcpy(C.data, A.data, A.cols * A.rows * sizeof(double));
}


Matrix matrix_add(struct Matrix sum_matrix, const struct Matrix A,const struct Matrix B) {
    if(!((A.cols == B.cols) && (A.rows == B.rows))) {
        matrix_exception(ERROR, "The size of the matrices does not correspond to the multiplication operation");
        return (Matrix) {
            0,0,NULL
        };
    }
    if(&A == &sum_matrix || &B == &sum_matrix || &B == &A) {
        matrix_exception(ERROR, "You cannot pass identical matrices to a function");
        return (Matrix) {
            0,0,NULL
        };
    }
    if(!((sum_matrix.cols == B.cols) && (sum_matrix.rows == B.rows))) {
        matrix_exception(ERROR, "The output matrix does not match in size");
        return (Matrix) {
            0,0,NULL
        };
    }

    for(size_t idx = 0; idx < A.cols * A.rows; idx++) {
        sum_matrix.data[idx] = A.data[idx] + B.data[idx];
    }
    return sum_matrix;
}


Matrix matrix_sub(struct Matrix sub_matrix, const struct Matrix A,const struct Matrix B) {
    if(!((A.cols == B.cols) && (A.rows == B.rows))) {
        matrix_exception(ERROR, "The size of the matrices does not correspond to the substract operation");
        return (Matrix) {
            0,0,NULL
        };
    }
    if(&A == &sub_matrix || &B == &sub_matrix || &B == &A) {
        matrix_exception(ERROR, "You cannot pass identical matrices to a function");
        return (Matrix) {
            0,0,NULL
        };
    }
    if(!((sub_matrix.cols == B.cols) && (sub_matrix.rows == B.rows))) {
        matrix_exception(ERROR, "The output matrix does not match in size");
        return (Matrix) {
            0,0,NULL
        };
    }


    for(size_t idx = 0; idx < A.cols * A.rows; idx++) {
        sub_matrix.data[idx] = A.data[idx] - B.data[idx];
    }
    return sub_matrix;
}


Matrix matrix_mul(struct Matrix mul_res, const struct Matrix A,const struct Matrix B) {
    if(!((A.cols == B.rows))) {
        matrix_exception(ERROR, "The size of the matrices does not correspond to the multiplication operation");
        return (Matrix) {
            0,0,NULL
        };
    }
    if(&A == &mul_res || &B == &mul_res || &B == &A) {
        matrix_exception(ERROR, "You cannot pass identical matrices to a function");
        return (Matrix) {
            0,0,NULL
        };
    }

    if(!((mul_res.cols == B.cols) && (mul_res.rows == A.rows))) {
        matrix_exception(ERROR, "The output matrix does not match in size");
        return (Matrix) {
            0,0,NULL
        };
    }

    matrix_zeros(mul_res);
    for (size_t B_col = 0; B_col< B.cols; B_col++) {
        for(size_t A_row = 0; A_row < A.rows; A_row++) {
            for(size_t B_row = 0; B_row< B.rows; B_row++) {
                mul_res.data[A_row * mul_res.cols + B_col] += A.data[A_row * A.cols + B_row] * B.data[B_row * B.cols + B_col];
            }
        }
    }
    return mul_res;
}


double matrix_gaus_det(const struct Matrix A) {
    double det = 1;
    if(!((A.cols == A.rows))) {
        matrix_exception(ERROR, "The size of the matrix does not correspond to the determinant operation");
        return NAN;
    }
    for(size_t col = 0; col < A.cols; col++) {
        double max = fabs(A.data[col * A.cols + col]);
        size_t index = col;
        for(size_t row = col + 1; row < A.rows; row++) {
            if(max < fabs(A.data[col * A.cols + row])) {
                max = fabs(A.data[col * A.cols + row]);
                index = row;
            }
        }
        if(index != col) { //  замена столбцов
            for(size_t row = col; row <A.rows; row++) {
                double dat = A.data[row * A.cols + col];
                A.data[row * A.cols + col] = A.data[row * A.cols + index];
                A.data[row * A.cols + index] = dat;
            }
            det *= -1;
        }
        for(size_t row = col + 1; row < A.rows; row++) {
            double t = A.data[row * A.cols + col] / A.data[col * A.cols + col];
            for(size_t idx = col; idx < A.cols; idx++) {
                A.data[row * A.cols + idx] = A.data[row * A.cols + idx] - t * A.data[col * A.cols + idx];
            }
        }
    }
    for(size_t idx = 0; idx <= A.cols - 1; idx++) {
        det *= A.data[idx * A.cols + idx];
    }
    return det;
}


Matrix matrix_identity(Matrix identity_res) {
    if(!((identity_res.cols == identity_res.rows))) {
        matrix_exception(ERROR, "The size of the matrix does not correspond to the matrix identity operation");
        return (Matrix) {
            0,0,NULL
        };
    }
    for (size_t row = 0; row < identity_res.rows; row++) {
        for (size_t col = 0; col < identity_res.cols; col++) {
            if (row == col)
                identity_res.data[row * identity_res.cols + col] = 1;
            else
                identity_res.data[row * identity_res.cols + col] = 0;
        }
    }
    return identity_res;
}


Matrix matrix_involution(Matrix involution_res,const Matrix A, const unsigned int level) {
    if(!((A.cols == A.rows))) {
        matrix_exception(ERROR, "The size of the matrix does not correspond to the involution operation");
        return (Matrix) {
            0,0,NULL
        };
    }
    if(&A == &involution_res) {
        matrix_exception(ERROR, "You cannot pass identical matrices to a function");
        return (Matrix) {
            0,0,NULL
        };
    }
    if(!((involution_res.cols == A.cols) && (involution_res.rows == A.rows))) {
        matrix_exception(ERROR, "The output matrix does not match in size");
        return (Matrix) {
            0,0,NULL
        };
    }

    if (level == 0) {
        matrix_identity(involution_res);
        return involution_res;
    }

    matrix_copy(involution_res, A);
    Matrix C = matrix_allocreate(A.cols,A.rows);

    for (unsigned int idx = 1; idx < level; idx++) {
        matrix_zeros(C);
        C = matrix_mul(C, involution_res, A);
        matrix_copy(involution_res, C);
    }
    matrix_free(&C);
    return involution_res;
}


Matrix matrix_div(struct Matrix div_matrix, const struct Matrix A,const double delitel) {
    if(delitel < 0.00000001 & delitel > 0.00000001) {
        matrix_exception(ERROR, "The delitel cant be = 0");
        return (Matrix) {
            0,0,NULL
        };
    }
    if(&A == &div_matrix) {
        matrix_exception(ERROR, "You cannot pass identical matrices to a function");
        return (Matrix) {
            0,0,NULL
        };
    }
    if(!((div_matrix.cols == A.cols) && (div_matrix.rows == A.rows))) {
        matrix_exception(ERROR, "The output matrix does not match in size");
        return (Matrix) {
            0,0,NULL
        };
    }

    for(size_t idx = 0; idx < A.cols * A.rows; idx++) {
        div_matrix.data[idx] = A.data[idx] / delitel;
    }
    return div_matrix;
}


static double fact(int num) {
    double f = 1;
    if(num == 0) return f;
    for(double i = 1; i <= num; i++)f *= i;
    return f;
}


Matrix matrix_exp(Matrix exp_res,const Matrix A, const unsigned int level) {
    if(!((A.cols == A.rows))) {
        matrix_exception(ERROR, "The size of the matrix does not correspond to the exponent operation");
        return (Matrix) {
            0,0,NULL
        };
    }
    if(&A == &exp_res) {
        matrix_exception(ERROR, "You cannot pass identical matrices to a function");
        return (Matrix) {
            0,0,NULL
        };
    }
    if(!((exp_res.cols == A.cols) && (exp_res.rows == A.rows))) {
        matrix_exception(ERROR, "The output matrix does not match in size");
        return (Matrix) {
            0,0,NULL
        };
    }

    Matrix D = matrix_allocreate(A.rows, A.cols);

    for (int idx = 0; idx <= level; idx++) {
        matrix_involution(D, A, idx);
        matrix_div(D, D, fact(idx));
        exp_res = matrix_add(exp_res, exp_res, D);
    }
    matrix_free(&D);
    return exp_res;
}


int main () {

    struct Matrix A;
    A = matrix_allocreate (2, 2);
    matrix_random(A);

    struct Matrix B;
    B = matrix_allocreate (2, 2);
    matrix_random(B);

    printf("%s ","Matrix A:");
    matrix_print(A);
    printf("%s ","Matrix B:");
    matrix_print(B);

    struct Matrix sum_result = matrix_allocreate(A.cols,A.rows);

    printf("%s ","Summ result:");
    sum_result = matrix_add(sum_result, A, B);
    matrix_print(sum_result);


    struct Matrix mul_result = matrix_allocreate(B.cols,A.rows);

    printf("%s ","Mul result:");
    mul_result = matrix_mul(mul_result, A, B);
    matrix_print(mul_result);


    struct Matrix sub_result = matrix_allocreate(A.cols,A.rows);

    printf("%s ","Sub result:");
    sub_result = matrix_sub(sub_result, A, B);
    matrix_print(sub_result);

    struct Matrix involution_result = matrix_allocreate(A.cols,A.rows);

    printf("%s ","Involution result:");
    involution_result = matrix_involution(involution_result, A, 3);
    matrix_print(involution_result);


    struct Matrix exp_result = matrix_allocreate(A.cols,A.rows);

    printf("%s ","Exponent result:");
    exp_result = matrix_exp(exp_result, A, 20);
    matrix_print(exp_result);

    double det;

    printf("%s ","Det A result:");
    det = matrix_gaus_det(A);
    printf("%f ", det);


    matrix_free(&A);
    matrix_free(&B);
    matrix_free(&mul_result);
    matrix_free(&sum_result);
    matrix_free(&involution_result);
    matrix_free(&exp_result);
}
