#include <stdio.h>


const int height = 5; // количество строк
const int lenght = 4; // количество столбцов
double array1[5 * 4], array2[5 * 4]; // Наша матрица


void array_init(double *arr, int i, int j) // Заполнение матрицы числами
{
    for (int k = 0; k < i; k++){
        for (int l = 0; l < j; l++)
        {
            arr[k * j + l] = (double) k * i + l;
        }
    }
}


void array_sum(double *arr1, double *arr2, int i, int j) //Функция для суммы матриц
{
    for (int k = 0; k < i; k++){
        for (int l = 0; l < j; l++)
        {
            printf("%.2f\t", arr1[k * j + l] + arr2[k * j + l]);
        }
        printf("\n");
    }
    printf("\n");
}

void array_sub(double *arr1, double *arr2, int i, int j)
{
    for (int k = 0; k < i; k++){
        for (int l = 0; l < j; l++)
        {
            printf("%.2f\t", arr1[k * j + l] - arr2[k * j + l]);
        }
        printf("\n");
    }
    printf("\n");
}



void array_output(double *arr, int i, int j)
{
    for (int k = 0; k < i; k++){
        for (int l = 0; l < j; l++){
            printf("%.2f\t", arr[k * j + l]); 
        }
        printf("\n"); 
    }
    printf("\n");
}


int main()
{
    array_init(array1, lenght, height);
    array_init(array2, lenght, height);

    array_output(array1, lenght, height);
    array_output(array2, lenght, height);

    array_sum(array1, array2, lenght, height);
    array_sub(array1, array2, lenght, height);

    return 0;
}
