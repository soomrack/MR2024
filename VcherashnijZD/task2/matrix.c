#ifndef MATRIX_C
#define MATRIX_C
#include "matrix.h"

Matrix EMPTY = {0, 0, NULL, NULL};
static const double EQUAL_ACCURACY = 0.000001;
static const double DETERMINANT_ACCURACY = 0.000001;
static const double EXPONENT_ACCURACY = 0.001;
static const int EXPONENT_STEPS = 100;


static enum Message {ERR, ADD, SUB, MULT, DET, EX};


static void message(int ind) {
    const char *msgs[6] = {"\nMistake in matrix sizes!\n",
                      "\nError in addition\n",
                      "\nError in subtraction\n",
                      "\nError in multiplication\n",
                      "\nError in determinant\n",
                      "\nError in exponent\n"};
    printf("%s", msgs[ind]);
}


int is_mat_equal(const Matrix A, const Matrix B) {
    if (A.cols != B.cols) {
        return 0;
    }
    if (A.rows != B.rows) {
        return 0;
    }
    for (int k = 0; k < A.cols * A.rows; k++) {
        if (fabs(A.values[k] - B.values[k]) > EQUAL_ACCURACY) {
            return 0;
        }
    }
    return 1;
}


Matrix copy_mat(Matrix matrix) {
    unsigned int rows = matrix.rows;
    unsigned int cols = matrix.cols;
    Matrix copy = create_zero_matrix(rows,cols);
    for(int k = 0; k < rows * cols; k++) {
        copy.values[k] = matrix.values[k];
    }
    return copy;
}


Matrix create_zero_matrix(const unsigned int rows, const unsigned int cols) {
    Matrix zero;
    zero.cols = cols;
    zero.rows = rows;
    zero.values = malloc(rows * cols * sizeof(double));
    zero.item = malloc(rows * sizeof(double*));
    if ((zero.values == NULL) || (zero.item == NULL)) {
        free_mat(&zero);
        return EMPTY;
    }
    for(int k = 0; k < rows * cols; k++){
        zero.values[k] = 0.0;
    }
    for(int j = 0; j < rows; j++){
        zero.item[j] = zero.values + j * cols;
    }
    return zero;
}


Matrix create_one_matrix(const unsigned int rows, const unsigned int cols) {
    Matrix one;
    one.cols = cols;
    one.rows = rows;
    one.values = malloc(rows * cols * sizeof(double));
    one.item = malloc(rows * sizeof(double*));
    if ((one.values == NULL) || (one.item == NULL)) {
        free_mat(&one);
        return EMPTY;
    }
    for (int k = 0; k < rows * cols; k++) {
        if((int)(k / cols) == (k % cols)){
            one.values[k] = 1.0;
        }
        else{
            one.values[k] = 0.0;
        }
    }
    for(int j = 0; j < rows; j++){
        one.item[j] = one.values + j * cols;
    }
    return one;
}


void fill_matrix_val(Matrix *matrix, const double *value) {
    for(int k = 0; k < matrix->rows * matrix->cols; k++){
        matrix->values[k] = value[k];
    }
}


Matrix matrix_add(const Matrix fst_matx, const Matrix snd_matx) {
    if (fst_matx.rows != snd_matx.rows || fst_matx.cols != snd_matx.cols){
        message(ADD);
        message(ERR);
        return EMPTY;
    }
    if (is_mat_equal(fst_matx, EMPTY) || is_mat_equal(snd_matx, EMPTY)) {
        return EMPTY;
    }
    unsigned int cols = snd_matx.cols;
    unsigned int rows = fst_matx.rows;
    Matrix res_matx = create_zero_matrix(rows,cols);
    if (is_mat_equal(res_matx, EMPTY)) {
        return EMPTY;
    }
    for(int k = 0; k < rows * cols; k++){
        res_matx.values[k] = fst_matx.values[k] + snd_matx.values[k];
    }
    return res_matx;
}


Matrix matrix_subt(const Matrix fst_matx, const Matrix snd_matx) {
    if (fst_matx.rows != snd_matx.rows || fst_matx.cols != snd_matx.cols){
        message(SUB);
        message(ERR);
        return EMPTY;
    }
    if (is_mat_equal(fst_matx, EMPTY) || is_mat_equal(snd_matx, EMPTY)) {
        return EMPTY;
    }
    unsigned int cols = snd_matx.cols;
    unsigned int rows = fst_matx.rows;
    Matrix res_matx = create_zero_matrix(rows,cols);
    if (is_mat_equal(res_matx, EMPTY)) {
        return EMPTY;
    }
    for(int k = 0; k < rows * cols; k++){
        res_matx.values[k] = fst_matx.values[k] - snd_matx.values[k];
    }
    return res_matx;
}


Matrix matrix_mult(const Matrix fst_matx, const Matrix snd_matx) {
    if (fst_matx.cols != snd_matx.rows){
        message(MULT);
        message(ERR);
        return EMPTY;
    }
    if (is_mat_equal(fst_matx, EMPTY) || is_mat_equal(snd_matx, EMPTY)) {
        return EMPTY;
    }
    unsigned int cols = snd_matx.cols;
    unsigned int rows = fst_matx.rows;
    Matrix res_matx = create_zero_matrix(rows,cols);
    if (is_mat_equal(res_matx, EMPTY)) {
        return EMPTY;
    }
    for(int k = 0; k < rows; k++) {
        for (int j = 0; j < cols; j++) {
            for (int n = 0; n < fst_matx.cols; n++) {
                res_matx.item[k][j] += fst_matx.item[k][n] * snd_matx.item[n][j];
            }
        }
    }
    return res_matx;
}


Matrix matrix_mult_by_num(const double a, const Matrix matrix) {
    if (is_mat_equal(matrix, EMPTY)) {
        return EMPTY;
    }
    unsigned int cols = matrix.cols;
    unsigned int rows = matrix.rows;
    Matrix res_matx = create_zero_matrix(rows,cols);
    if (is_mat_equal(res_matx, EMPTY)) {
        return EMPTY;
    }
    //message(MULT);
    for(int k = 0; k < rows * cols; k++) {
        res_matx.values[k] = a * matrix.values[k];
    }
    //printf("\nMultiplicated Matrix");
    //print_matx(res_matx);
    return res_matx;
}


void matrix_change_rows(Matrix *matrix, const int fst_row, const int snd_row) {
    double item_buff;
    if (is_mat_equal(*matrix, EMPTY)) {
        return;
    }
    for (int k = 0; k < matrix->cols; k++) {
        item_buff = matrix->item[snd_row][k];
        matrix->item[snd_row][k] = matrix->item[fst_row][k];
        matrix->item[fst_row][k] = item_buff;
    }
}


double matrix_det(const Matrix matrix) {
    if (is_mat_equal(matrix, EMPTY)) {
        return 0.0D;
    }
    if(matrix.rows != matrix.cols){
        message(DET);
        message(ERR);
        return NAN;
    }
    double det = 1;
    Matrix trian_mat = copy_mat(matrix);
    for(int row_fix = 1; row_fix < trian_mat.rows; row_fix++) { // row that will be subtracted
        for (int row = row_fix; row < trian_mat.rows; row++) { // start from the next row
            if (fabs(trian_mat.item[row_fix - 1][row_fix - 1]) < DETERMINANT_ACCURACY) { // division by zero check
                matrix_change_rows(&trian_mat, row_fix - 1, row_fix);
                det = -det;
            }
            double koef = trian_mat.item[row][row_fix - 1] / trian_mat.item[row_fix - 1][row_fix - 1];
            for (int col = 0; col < trian_mat.cols; col++) {
                trian_mat.item[row][col] -= trian_mat.item[row_fix - 1][col] * koef;
            }
        }
        det *= trian_mat.item[row_fix][row_fix];
    }
    free_mat(&trian_mat);
    return det;
}


Matrix matrix_pow(Matrix matrix, const int power){
    if (is_mat_equal(matrix, EMPTY)) {
        return EMPTY;
    }
    if(power == 0){
        return create_one_matrix(matrix.rows,matrix.cols);
    }
    Matrix res_mat = copy_mat(matrix);
    for(int k = 1; k < power; k++){
        double* val_mem = res_mat.values;
        double** item_mem = res_mat.item;
        res_mat = matrix_mult(res_mat, matrix);
        free(val_mem);
        free(item_mem);
    }
    //printf("\nPowered Matrix");
    //print_matx(res_mat);
    return res_mat;
}


double check_max_dif(const Matrix fst_mat, const Matrix snd_mat){
    
    if (is_mat_equal(fst_mat, EMPTY) || is_mat_equal(snd_mat, EMPTY)) {
        return 0.0D;
    }
    double dif = 0.0;
    for(int k = 0; k < fst_mat.cols * fst_mat.rows; k++){
        if(fabs(fst_mat.values[k] - snd_mat.values[k]) > dif){
            dif = fst_mat.values[k] - snd_mat.values[k];
        }
    }
    //printf("\n%f \n",dif);
    //print_matx(snd_mat);
    //print_matx(fst_mat);
    return dif;
}


Matrix matrix_exp(const Matrix matrix){
    if(matrix.rows != matrix.cols) {
        message(EX);
        message(ERR);
        return EMPTY;
    }
    if (is_mat_equal(matrix, EMPTY)) {
        return EMPTY;
    }
    Matrix res_mat = create_one_matrix(matrix.rows,matrix.cols);
    Matrix n_member = create_one_matrix(matrix.rows,matrix.cols);
    Matrix prev_mat = create_zero_matrix(matrix.rows,matrix.cols); // Initialize with correct size
    
    if (is_mat_equal(res_mat, EMPTY) || 
        is_mat_equal(n_member, EMPTY) ||
        is_mat_equal(prev_mat, EMPTY)) {
        free_mat(&prev_mat);
        free_mat(&n_member);
        free_mat(&res_mat);
        return EMPTY;
    }
    
    for(int m = 1; m <= EXPONENT_STEPS; m++) {
        free_mat(&prev_mat);
        prev_mat = copy_mat(res_mat);

        Matrix mult_mat = matrix_mult_by_num(1.0 / m, matrix); // Create temporary matrix

        Matrix n1_member = matrix_mult(n_member, mult_mat); // Create temporary matrix

        free_mat(&res_mat);
        res_mat = matrix_add(prev_mat,n1_member); // Use prev_mat instead of mem_mat

        free_mat(&n_member);
        n_member = copy_mat(n1_member); // Use n1_member instead of n_member

        free_mat(&n1_member); // Free temporary matrices
        free_mat(&mult_mat);

        if(fabs(check_max_dif(res_mat,prev_mat)) < EXPONENT_ACCURACY){
            free_mat(&prev_mat);
            return res_mat;
        }
    }
    free_mat(&prev_mat);
    return res_mat;
}

void free_mat(Matrix *matrix) {
    free(matrix->item);
    free(matrix->values);
}


void print_matx(Matrix matrix) {
    if (is_mat_equal(matrix, EMPTY)) {
        printf("\n");
        printf("()\n");
        return;
    }
    printf("\n");
    for(int k = 0; k < matrix.rows; k++){
        printf("(");
        for(int j = 0; j < matrix.cols; j++){
            printf("%f; ",matrix.values[k * matrix.cols + j]);
        }
        printf(")\n");
    }
}


void print_items(Matrix matrix) {
    if (is_mat_equal(matrix, EMPTY)) {
        return;
    }
    printf("\n");
    for(int k = 0; k < matrix.rows; k++){
        printf("(");
        for(int j = 0; j < matrix.cols; j++){
            printf("%f; ",matrix.item[k][j]);
        }
        printf(")\n");
    }
}

	
#endif //MATRIX_C
