#include <stdio.h>
#include <math.h>
#include <stdbool.h>
typedef unsigned long long int Money;  
int SIMULATION_TIME = 30;
int Month_Point = 9;
//Общие переменные
struct Person
    {
    Money  Balance;
    Money  Salary;
    Money  Credit;
    double  Rate;
    Money  Expense;
    double  Rent;
    Money  Purpose;
    double Inflation;
    double  Rate_Deposit;
    };

struct Person Alice; 
struct Person Bob;
    
void Alice_Data()
{
    
    Alice.Balance = pow(10,9);
    Alice.Salary = pow(10,6);
    Alice.Credit = 15*pow(10,9);
    Alice.Rate = 20;
    Alice.Rate_Deposit = 12;
    Alice.Expense = 30 * 1000;
    Alice.Purpose = 0;
    Alice.Inflation = 8;
    Alice.Credit = Alice.Credit - Alice.Balance;
    Alice.Balance = 0;
    Alice.Rate = 1 + Alice.Rate / (100*12);
    Alice.Rate_Deposit = 1 + Alice.Rate_Deposit / (100*12);
    Alice.Inflation = Alice.Inflation / (100*12); // 
    Alice.Rent = Alice.Credit * (((Alice.Rate - 1)*(pow((Alice.Rate),(SIMULATION_TIME)*12)))/(pow((Alice.Rate),(SIMULATION_TIME)*12) - 1));

}

void Bob_Data()
{
    Bob.Balance = pow(10,9);
    Bob.Salary = pow(10,6);
    Bob.Credit = 0;
    Bob.Rate = 0;
    Alice.Rate_Deposit = 12;
    Bob.Expense = 30 * 1000;
    Bob.Rent = 30 * 1000;
    Bob.Purpose = 15 * pow(10,9);
    Bob.Inflation = 8;
    Bob.Inflation = Bob.Inflation / (100*12);
    Bob.Rate_Deposit = 1 + Bob.Rate_Deposit / (100*12);
}


void Calculations_Alice() //?? Как Сделать одну расчтеную функцию на двоих
{
    for (int year=2024; year<(2024+SIMULATION_TIME); year++){
        for (int mounth=0; mounth<12; mounth++){
            Month_Point++;
            Alice.Balance += Alice.Salary; 
            Alice.Balance = Alice.Balance - Alice.Rent - Alice.Expense;
            Alice.Salary += Alice.Salary*Alice.Inflation;
            Alice.Expense += Alice.Expense*Alice.Inflation;
            Alice.Balance =  Alice.Balance*Alice.Rate_Deposit;
            printf("Alice.Balance:%d\n",Alice.Balance);
        
        }
    }    
}

void Calculations_Bob()
{
     for (int year=2024; year<(2024+SIMULATION_TIME); year++){
        for (int mounth=0; mounth<12; mounth++){
            Month_Point++;
            Bob.Balance = Bob.Balance + Bob.Salary; 
            Bob.Balance = Bob.Balance - Bob.Rent - Bob.Expense;
            Bob.Salary += Bob.Salary*Bob.Inflation;
            Bob.Expense += Bob.Expense*Bob.Inflation;
            Bob.Purpose += Bob.Purpose*Bob.Inflation;
            Bob.Balance =  Bob.Balance*Bob.Rate_Deposit;
            printf("Bob.Balance:%d\n",Bob.Balance);
       }
    }
}

void Result ()
{
if (Bob.Balance > Alice.Balance+Bob.Purpose){
        printf("The winner is Bob\n"); 
        printf("Bob.Balance: %d\n", round(Bob.Balance));
        printf("Alice.Balance: %d\n", round(Alice.Balance));
        printf("Bob.Purpose: %d\n", round(Bob.Purpose));
    }
    else{
        printf("The winner is Alice\n");
        printf("Alice.Balance:%d\n",Alice.Balance);
        printf("Bob.Balance: %d\n", Bob.Balance);
        printf("Bob.Purpose: %d\n", Bob.Purpose);
    }
//
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
