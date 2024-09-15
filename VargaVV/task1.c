#include <stdio.h>
#include <math.h>
int main(){
    //Переменные Алисы
    unsigned int Alice_Balance = 100000000;
    unsigned int Alice_Salary = 20000000;
    unsigned int Alice_Credit = 1500000000;
    unsigned int Alice_Expense = 3500000;
    //Переменные Боба
    unsigned int Bob_Balance = 100000000;
    unsigned int Bob_Salary = 20000000;
    unsigned int Bob_Purpose = 1500000000;
    unsigned int Bob_Rent = 3000000;
    unsigned int Bob_Expense = 3500000;
    //Общие переменные 
    int Time = 30*12;
    float Inf = 1.08/12;

int Bob_Calculate();{
    
    for (int t=0; t<=Time; t++){
     Bob_Balance = Bob_Balance + Bob_Salary;  
    }
    printf("Bob_Balance = %d", Bob_Balance);
}



    return 0;
}
