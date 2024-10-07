#include <stdio.h>

    const int height = 5; // количество строк
    const int lenght = 4; // количество столбцов
    double array[5 * 4]; // Наша матрица


void array_init(double *arr, int i, int j)
{
    for (int k = 0; k < i; k++){
        for (int l = 0; l < j; l++)
        {
            arr[k * j + l] = (double) k * i + l;
        }
    }
}




void array_output(double *arr, int i, int j)
{
    for (int k = 0; k < i; k++){
        for (int l = 0; l < j; l++){
            printf("%.2f\t", arr[k * j + l]); 
        }
        printf("\n"); 
    }
}


int main()
{

    array_init(array, lenght, height);
    array_output(array, lenght, height);
  
    
    return 0;
}
