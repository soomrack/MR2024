#include <stdio.h>
#include <math.h>
#include <stdbool.h>
typedef long long int Money;  
int START_YEAR = 2024;
int SIMULATION_TIME = 30;
int MONTH_START = 9;
//Общие переменные
struct Person {
    Money  Balance;
    Money  Salary;
    Money  Credit;
    double Rate;
    Money  Expense;
    Money  Rent;
    Money  Purpose;
    double Inflation;
    double Deposit;
};

struct Person Alice; 
struct Person Bob;

//Данные Алисы
void Alice_Data()
{
    Alice.Balance = pow(10,6);
    Alice.Salary = 2*pow(10,5);
    Alice.Credit = 15*pow(10,6);
    Alice.Rate = 0.2/12;
    Alice.Deposit = 0.12/12;
    Alice.Expense = 30 * 1000;
    Alice.Purpose = 0;
    Alice.Inflation = 0.08/12;
    Alice.Rent = (Alice.Credit - Alice.Balance) * (((Alice.Rate)*(pow((1. + Alice.Rate),(SIMULATION_TIME)*12.)))/(pow(1. + (Alice.Rate),(SIMULATION_TIME)*12.) - 1.));
    Alice.Balance = 0;
}

//Данные Боба
void Bob_Data()
{
    Bob.Balance = pow(10,6);
    Bob.Salary = 2*pow(10,5);
    Bob.Credit = 0;
    Bob.Rate = 0;
    Bob.Deposit = 0.12/12;
    Bob.Expense = 30 * 1000;
    Bob.Rent = 30 * 1000;
    Bob.Purpose = 15 * pow(10,6);
    Bob.Inflation = 0.08/12;
}

//Зарплата
void Alice_Salary()
{
    Alice.Balance +=  Alice.Salary;
}

void Bob_Salary()
{
    Bob.Balance += Bob.Salary;
}

//Расходы
void Alice_Expenses()
{
    Alice.Balance = Alice.Balance - Alice.Rent - Alice.Expense;
}

void Bob_Expenses()
{
    Bob.Balance = Bob.Balance - Bob.Rent - Bob.Expense;
}

//Инфляция
void Alice_Inflation()
{
    Alice.Salary += Alice.Salary * Alice.Inflation;
    Alice.Expense += Alice.Expense * Alice.Inflation;
}

void Bob_Inflation()
{
    Bob.Salary += Bob.Salary * Bob.Inflation;
    Bob.Expense += Bob.Expense * Bob.Inflation;
    Bob.Purpose += Bob.Purpose * Bob.Inflation;
}

//Вклады
void Alice_Deposit()
{
    Alice.Balance +=  Alice.Balance * Alice.Deposit;
}

void Bob_Deposit()
{
    Bob.Balance += Bob.Balance * Bob.Deposit;
}

//Расчеты
void Calculations()
{
    int Month = MONTH_START;
    for (int year=START_YEAR ; year<=(START_YEAR + SIMULATION_TIME); year++){
        printf("Year\n"); 
        for (;Month<=12; Month++){

            Alice_Salary(); 
            Alice_Expenses(); 
            Alice_Inflation(); 
            Alice_Deposit(); 
            printf("Bob.Balance: %lld\n\n", Bob.Balance);
            Bob_Salary(); 
            Bob_Expenses(); 
            Bob_Inflation(); 
            Bob_Deposit(); 
            printf("Alice.Balance: %lld\n\n", Alice.Balance);
        }
        Month=1;
    } 
}

//Вывод результатов
void Result ()
{
if (Bob.Balance > Alice.Balance+Bob.Purpose){
        printf("The winner is Bob\n"); 
        printf("Bob.Balance: %lld\n", Bob.Balance);
        printf("Alice.Balance: %lld\n", Alice.Balance);
        printf("Bob.Purpose: %lld\n", Bob.Purpose);
    }
    else{
        printf("The winner is Alice\n");
        printf("Alice.Balance:%lld\n",Alice.Balance);
        printf("Bob.Balance: %lld\n", Bob.Balance);
        printf("Bob.Purpose: %lld\n", Bob.Purpose);
    }
}

int main()
{

Alice_Data();
Bob_Data();

Calculations();

Result();  

    return 0;
}
