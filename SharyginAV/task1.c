#include <stdio.h>

const int YEAR_START = 2024;
const int MONTH_START = 9;
const int DURATION = 1;
const int inflation = 8;  // percent

typedef long long int Money;


struct person{
    Money bank_account;
    Money salary;
    Money monthly_expenses;
    Money rent;
} bob;


struct mortgage{
    Money start_installment;
    Money mortgage_payment;
    int interest_rate;
    int duration;
};


void bob_init()
{
    bob.bank_account = 0;
    bob.salary = 300 * 1000;
    bob.monthly_expenses = 40 * 1000;
    bob.rent = 30 * 1000;
}


void bob_salary(int month)
{
    bob.bank_account += bob.salary;

    if (month == 12){
        bob.salary *= 1 + inflation / 100;
    }
}


void bob_expenses(int month)
{
    bob.bank_account -= bob.monthly_expenses;
    bob.bank_account -= bob.rent;

    if (month == 12){
        bob.monthly_expenses *= 1 + inflation / 100;
        bob.rent *= 1 + inflation / 100;
    }
}


void bob_print()
{
    printf("Капитал Боба: %lld", bob.bank_account);
}


void simulation(int year, int month)
{
    while(year <= YEAR_START + DURATION)
    {
        while(month <= 12)
        {
            bob_salary(month);
            bob_expenses(month);
            //printf("Прошел %d месяц\n", month);
            month++;
        }
        month = 1;
        //printf("Прошел %d год\n", year);
        year++;
    }
}


int main()
{
    bob_init();

    simulation(YEAR_START, MONTH_START);

    bob_print();
}