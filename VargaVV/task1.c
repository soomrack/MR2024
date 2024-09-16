#include <stdio.h>
#include <math.h>
int main(){
    //Переменные Алисы
    float  Alice_Balance = 100000000;
    float  Alice_Salary = 20000000;
    float  Alice_Credit = 1500000000;
    float  Alice_Expense = 3500000;
    //Переменные Боба
    float  Bob_Balance = 100000000;
    float  Bob_Salary = 20000000;
    float  Bob_Purpose = 1500000000;
    float  Bob_Rent = 3000000;
    float  Bob_Expense = 3500000;
    //Общие переменные 
    int Time = 30*12;
    float  Inf = 8;
    Inf = 1 + Inf/(12*100);
    float Rate = 15;
    Rate = Rate/(30*12*100);

    for (int t=0; t<=Time; t++){
         Bob_Balance = Bob_Balance + Bob_Salary; 
         Bob_Balance = Bob_Balance - Bob_Rent - Bob_Expense;
         Bob_Salary = Bob_Salary*Inf;
         Bob_Rent = Bob_Rent*Inf;
         Bob_Expense = Bob_Expense*Inf;
    }

    Alice_Credit = Alice_Credit - Alice_Balance;
    Alice_Balance = 0;
    float Alice_Rent =  Alice_Credit*(Rate*pow((1+Rate),30*12)/(pow((1+Rate),30*12)-1));
    
    for (int t=0; t<=Time; t++){
         Alice_Balance = Alice_Balance + Alice_Salary; 
         Alice_Balance = Alice_Balance - Alice_Rent - Alice_Expense;
         Alice_Salary = Alice_Salary*Inf;
         Alice_Expense = Alice_Expense*Inf;
    }
    
    if (Bob_Balance > Alice_Balance+Bob_Purpose){
        printf("Bob is the winner\n"); 
    }
    else{
        printf("Bob is the looser\n");
    }

    printf("Bob_Balance: %3.0f\n", round(Bob_Balance));
    printf("Alice_Balance: %3.0f\n", round(Alice_Balance));

    return 0;
}
