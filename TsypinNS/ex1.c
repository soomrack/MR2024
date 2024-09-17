#include <stdio.h>

typedef long long int Money;

typedef struct {
    Money account;
    Money salary;
    Money food;
    Money deposit;
    Money home_bills;
} Person;

Person alice;

void alice_init()
{
    alice.account = 0;
    alice.salary = 200 * 1000 * 100;
    alice.food = 25 * 1000 * 100;
    alice.home_bills = 13 * 1000 * 1000 * 100 * 0.001;
}

void alice_salary(const int month, const int year)
{
    alice.account += alice.salary;

    if(month == 12) {
        alice.salary *= 1.07;
    }
}

void alice_food(const int month, const int year)
{
    alice.account -= alice.food;

    if (month == 12) {
        alice.food *= 1.08;
    }
}

void alice_home_bills(const int month)
{
    if(month == 12) {
        alice.account -= alice.home_bills;
        alice.home_bills *= 1.07;
    }
}

void alice_print()
{
    printf("Alice account = %lld RUB\n", alice.account);
}

void simulation()
{
    int year = 2024;
    int month = 9;

    while( !((year == 2024 + 30) && (month == 10)) ){

        alice_salary(month, year);
        //alice_deposit_pp();
        //alice_mortage_payment();
        alice_food(month, year);
        alice_home_bills(month);

        ++month;
        if(month==13) {
            month = 1;
            ++year;
        }
    }
}

int main()
{
    alice_init();
    simulation();
    alice_print();
    return 0;

}