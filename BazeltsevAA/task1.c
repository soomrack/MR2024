#include <stdio.h>
#include <stdlib.h>

typedef unsigned long long int Money;

const int START_YEAR = 2024;
const int START_MONTH = 9;
const int PERIOD = 30;  // период симуляции

typedef struct Person
{
    long long int available_funds; //свободные деньги
    Money capital; //капитал
    Money salary; //зарплата
    Money monthly_payment; //ежемесячный платёж
    Money necessary_expenses; //необходимые траты
    Money contribution; //вклад
    Money cost_house; //стоимость дома
    double rate_contribution; //ставка по вкладу
} Person;


Person Alice;
Person Bob;


void Bob_init()
{
    Bob.capital = 0;
    Bob.available_funds = 0;
    Bob.salary = 300 * 1000;
    Bob.monthly_payment = 50 * 1000;
    Bob.necessary_expenses = 50 * 1000;
    Bob.contribution = 1500 * 1000;
    Bob.rate_contribution = 1.014; // вклад под 17% годовых
}


void Alice_init()
{
    Alice.capital = 0;
    Alice.available_funds = 0;
    Alice.salary = 300 * 1000;
    Alice.monthly_payment = 192466; //на 30 лет через калькулятор ипотеки от яндекса под 17%
    Alice.necessary_expenses = 50 * 1000;
    Alice.contribution = 0;
    Alice.cost_house = 15 * 1000 * 1000;
    Alice.rate_contribution = 1.014; // вклад под 17% годовых
}


Money search_inflation() // данные взяты с https://calcus.ru/inflyaciya с 2010-2023 год
{ 
    int probability = (rand() % 100);
    Money inflation;
    if (probability > 90) {
        inflation = (1001 + rand() % 2);
    }
    if (probability <= 90 && probability > 60) {
        inflation=(1004 + rand() % 2);
    }
    if (probability <= 60 && probability > 10) {
        inflation = (1007 + rand() % 2);
    }
    if (probability <= 10) {
        inflation = (1010 + rand() % 4);
    }
    return(inflation);
}


Money indexation()
{
    return((100 + rand() % 9));
}


void Bob_profit(int month)
{
    if (month == 1) {
        Bob.salary = ((Bob.salary * indexation()) / 100);
    }
    Bob.available_funds = Bob.salary;
}


void Alice_profit(int month)
{
    if (month == 1) {
        Alice.salary = ((Alice.salary * indexation()) / 100);
    }
    Alice.available_funds = Alice.salary;
}


void Bob_expenses(Money inflation)
{
    Bob.necessary_expenses = (Bob.necessary_expenses * inflation) / 1000;
    Bob.available_funds -= Bob.necessary_expenses;
    Bob.monthly_payment = (Bob.monthly_payment * inflation) / 1000;
    Bob.available_funds -= Bob.monthly_payment;
}


void Alice_expenses(Money inflation)
{
    Alice.necessary_expenses = (Alice.necessary_expenses * inflation) / 1000;
    Alice.available_funds -= Alice.necessary_expenses;
    Alice.available_funds -= Alice.monthly_payment;
}


void Bob_contribution()
{
    Bob.contribution = (Bob.contribution + Bob.available_funds) * Bob.rate_contribution;
}


void Alice_contribution()
{
    Alice.contribution = (Alice.contribution + Alice.available_funds) * Alice.rate_contribution;
}


void Alice_capital(Money inflation)
{
    Alice.cost_house = (Alice.cost_house * inflation) / 1000;
    Alice.capital = Alice.contribution + Alice.cost_house;
}


void Bob_capital(Money inflation)
{
    Bob.capital = Bob.contribution;
}


void Alice_count(int month, Money inflation)
{ 
    Alice_profit(month);
    Alice_expenses(inflation);
    Alice_contribution();
    Alice_capital(inflation);
}


void Bob_count(int month, Money inflation)
{ 
    Bob_profit(month);
    Bob_expenses(inflation);
    Bob_contribution();
    Bob_capital(inflation); 
}


void simulation()
{
    int month = START_MONTH, year = START_YEAR;
    Bob_init();
    Alice_init();
    while (year < START_YEAR + PERIOD)
    {
        Money inflation = search_inflation();
        Alice_count(month, inflation);
        Bob_count(month, inflation);
        month++;
        if (month > 12)  {
            year++;
            month = 1;
        }
    }
}


void print()
{
    printf("%lld\n", Alice.capital);
    printf("%lld\n", Bob.capital);
    if (Alice.capital > Bob.capital)  {
        printf("Alice win!\n");
    }
    else  {
        printf("Bob win!\n");
    }
}


int main()
{
    simulation();
    print();
}
