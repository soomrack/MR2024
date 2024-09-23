#include <stdio.h>
#include <stdlib.h>

typedef unsigned long long int Money;

typedef struct Person
{
    Money capital; //капитал
    long long int available_funds; //свободные деньги
    Money salary; //зарплата
    Money monthly_payment; //ежемесячный платёж
    Money necessary_expenses; //необходимые траты
    Money contribution; //вклад
    Money cost_house; //стоимость дома
} Person;

Person Alice;
Person Bob;

void Bob_init()
{
    Bob.capital=0;
    Bob.available_funds=0;
    Bob.salary=300*1000;
    Bob.monthly_payment=50*1000;
    Bob.necessary_expenses=50*1000;
    Bob.contribution=1500*1000;
}


void Alice_init()
{
    Alice.capital=0;
    Alice.available_funds=0;
    Alice.salary=300*1000;
    Alice.monthly_payment=177767;
    Alice.necessary_expenses=50*1000;
    Alice.contribution=0;
    Alice.cost_house=1500*1000;
}

int inflation()
{
    return(2 + rand() % 15);
}

Money indexation(Money salary)
{
    salary*=((100 + rand() % 109)/100);
    return(salary);
}

Money contribution(Money contribution, long long int available_funds)
{
    contribution+=available_funds;
    contribution*=1.0125;
    return(contribution);
}

void Human_profit(short month)
{
    if (month==1)
    {
        Alice.salary=indexation(Alice.salary);
        Bob.salary=indexation(Bob.salary);
    }
    Alice.available_funds=Alice.salary;
    Bob.available_funds=Bob.salary;
}

void Human_expenses()
{
    double infl=1+(inflation()/100);
    printf("%d\n", infl);
    Alice.necessary_expenses*=infl;
    Alice.available_funds-=Alice.necessary_expenses;
    Alice.available_funds-=Alice.monthly_payment;
    Bob.necessary_expenses*=infl;
    Bob.available_funds-=Bob.necessary_expenses;
    Bob.available_funds-=(Bob.monthly_payment*infl);
}

void Human_contribution()
{
    Alice.contribution=contribution(Alice.contribution, Alice.available_funds);
    Bob.contribution=contribution(Bob.contribution, Bob.available_funds);
}

void count(short month)
{ 
    Human_profit(month);
    Human_expenses();
    Human_contribution();
    Bob.capital=Bob.contribution;
    Alice.cost_house+=37500;
    Alice.capital=Alice.contribution+Alice.cost_house;
}

void print()
{
    printf("Bob.capital %lld\n", Bob.capital);
    printf("Bob.available_funds %lld\n", Bob.available_funds);
    printf("Bob.salary %lld\n", Bob.salary);
    printf("Bob.monthly_payment %lld\n", Bob.monthly_payment);
    printf("Bob.necessary_expenses %lld\n", Bob.necessary_expenses);
    printf("Bob.contribution %lld\n", Bob.contribution);
    printf("Alice.capital %lld\n", Alice.capital);
    printf("Alice.available_funds %lld\n", Alice.available_funds);
    printf("Alice.salary %lld\n", Alice.salary);
    printf("Alice.monthly_payment %lld\n", Alice.monthly_payment);
    printf("Alice.necessary_expenses %lld\n", Alice.necessary_expenses);
    printf("Alice.contribution %lld\n", Alice.contribution);
    printf("Alice.cost_house %lld\n", Alice.cost_house);
}

int main()
{
    short month = 9, year = 2024;
    Bob_init();
    Alice_init();
    while (year < 2055)
    {
        count(month);
        print();
        month++;
        if (month > 12)
        {
            year++;
            month=1;
        }
    }
    printf("%lld\n", Alice.capital);
    printf("%lld\n", Bob.capital);
}
