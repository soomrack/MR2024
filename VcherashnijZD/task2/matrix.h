#ifndef MATRIX_H
#define MATRIX_H

#include "malloc.h"
#include <stdio.h>
#include <math.h>


typedef struct Matrix{
    unsigned int cols;
    unsigned int rows;
    double* values;
    double** item;
} Matrix;

extern Matrix EMPTY;  // return if error and in some other cases

Matrix create_zero_matrix(unsigned int rows, unsigned int cols);

Matrix create_one_matrix(unsigned int rows, unsigned int cols);

void fill_matrix_val(Matrix *matrix, const double* value);

Matrix copy_mat(Matrix matrix);

Matrix matrix_add(Matrix fst_matx, Matrix snd_matx);

Matrix matrix_subt(Matrix fst_matx, Matrix snd_matx);

Matrix matrix_mult(Matrix fst_matx, Matrix snd_matx);

Matrix matrix_mult_by_num(double a, Matrix matrix);

void matrix_change_rows(Matrix *matrix, int fst_row, int snd_row);

double matrix_det(Matrix matrix);

Matrix matrix_pow(Matrix matrix, int power);

double check_max_dif(Matrix fst_mat, Matrix snd_mat);

Matrix matrix_exp(Matrix matrix);

void free_mat(Matrix *matrix);

void print_matx(Matrix matrix);

void print_items(Matrix matrix);


#endif //MATRIX_H
