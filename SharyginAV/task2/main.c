#include "matrix.h"

int main()
{
    Matrix M_res;
    Matrix* M_res_ptr = &M_res;
    matrix_alloc(M_res_ptr, 3, 3);

    Matrix M_1;
    Matrix* M_1_ptr = &M_1;
    matrix_alloc(M_1_ptr, 3, 3);
    matrix_random(M_1, 0, 5);

    Matrix M_2;
    Matrix* M_2_ptr = &M_2;
    matrix_alloc(M_2_ptr, 3, 3);
    matrix_random(M_2, 0, 5);

    matrix_sum(M_res_ptr, M_1, M_2);

    matrix_free(M_1_ptr);
    matrix_free(M_2_ptr);
    matrix_free(M_res_ptr);











    matrix_print(M1);
    printf("%lf", matrix_det(M1));
    matrix_free(M1_ptr);


/*
    Matrix M2;
    Matrix* M2_ptr = &M2;

    matrix_alloc(M1_ptr, 3, 3);
    matrix_alloc(M2_ptr, 3, 3);

    matrix_random(M1, 0, 5);
    matrix_print(M1);
    
    matrix_random(M2, 6, 12);
    matrix_print(M2);

    matrix_add(M1, M2);

    matrix_print(M1);

    matrix_free(M1_ptr);
    matrix_free(M2_ptr);
*/

    return 0;
}