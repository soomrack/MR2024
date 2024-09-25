#include <stdio.h>
#include <math.h>
#include <stdbool.h>

typedef long long int Money;

int Year_Start=2024;
int Year_Stop=2054;
int Month_Start=9;
int Month_Stop=9;

//Общие переменные
typedef struct Person
    {
    bool    Bank_Way;
    Money  Balance;
    Money  Salary;
    Money  Credit;
    Money  Rate;
    Money  Expense;
    Money  Rent;
    Money  Purpose;
    double  Inflation;
    }Person;

    Person Alice; 
    Person Bob;
    
void Alice_Data()
{
    Alice= (Person)
    {
    .Bank_Way = 1,
    .Balance = pow(10,9),
    .Salary = pow(10,6),
    .Credit = 15*pow(10,9),
    .Rate = 20,
    .Expense = 30*1000,
    .Purpose = 0,
    .Inflation = 8,
    .Credit = Alice.Credit - Alice.Balance,
    .Balance = 0,
    .Rate = Alice.Rate/(30*12*100),
    .Inflation = 1 + Alice.Inflation/(100*12), // 
    .Rent = Alice.Credit*(Alice.Rate*pow((1+Alice.Rate),(Year_Stop-Year_Start)*12)/(pow((1+Alice.Rate),(Year_Stop-Year_Start)*12)-1)),
    };
}

void Bob_Data()
{
    Bob = (Person)
    {
    .Bank_Way = 0,
    .Balance = pow(10,9),
    .Salary = pow(10,6),
    .Credit = 0,
    .Rate = 0,
    .Expense = 30*1000,
    .Rent = 30*1000,
    .Purpose = 15*pow(10,9),
    .Inflation = 8,
    .Inflation = 1 + Bob.Inflation/(100*12),
    };
}

void Calculations_Alice() //?? Как Сделать одну расчтеную функцию на двоих
{
    for (int t=0; t<=(Year_Stop-Year_Start); t++){
        for (int m=0; m<=12; m++){
            Alice.Balance = Alice.Balance + Alice.Salary; 
            Alice.Balance = Alice.Balance - Alice.Rent - Alice.Expense;
            Alice.Salary = Alice.Salary*Alice.Inflation;
            Alice.Expense = Alice.Expense*Alice.Inflation;
            if (Alice.Bank_Way!=1)
            {
                Alice.Purpose = Alice.Purpose*Alice.Inflation;
            }
    }    
}

void Calculations_Bob()
{
    for (int t=0; t<=(Year_Stop-Year_Start); t++){
        for (int m=0; m<=12; m++){
            Bob.Balance = Bob.Balance + Bob.Salary; 
            Bob.Balance = Bob.Balance - Bob.Rent - Bob.Expense;
            Bob.Salary = Bob.Salary*Bob.Inflation;
            Bob.Expense = Bob.Expense*Bob.Inflation;
            if (Bob.Bank_Way!=1)
            {
                Bob.Purpose = Bob.Purpose*Bob.Inflation;
            }
       }
    }
}

void Result ()
{
if (Bob.Balance > Alice.Balance+Bob.Purpose){
        printf("The winner is Bob\n"); 
        printf("The looser is Alice\n");
    }
    else{
        printf("The winner is Alice\n"); 
        printf("The looser is Bob\n");
    }

    printf("Bob.Balance: %3.0f\n", round(Bob.Balance));
    printf("Alice.Balance: %3.0f\n", round(Alice.Balance));
}

int main()
{

Alice_Data();
Bob_Data();

Calculations_Alice();
Calculations_Bob();

Result();  

    return 0;
}
