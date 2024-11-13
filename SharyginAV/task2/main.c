#include "matrix.h"

int main()
{
    Matrix M1;
    Matrix* M1_ptr = &M1;

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

    return 0;
}