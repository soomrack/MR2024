#include <stdio.h>


typedef long long int Money; //RUB


typedef struct {
    Money account;
    Money salary;
    Money food;
    Money home_bills;
    Money mortage;
    Money rent;
    Money overhaul;
    Money overhaul_outcome;
} Person;


typedef struct {
    Money expenses;
    Money outcome;
    int overhaul_month;
} Overhaul;


Person alice;
Person bob;


Overhaul overhaul;


void alice_init()
{
    alice.account = 0;
    alice.salary = 200 * 1000 * 100;
    alice.food = 25 * 1000 * 100;
    alice.home_bills = 13 * 1000 * 1000 * 100 * 0.001;
    alice.mortage = 174818 * 100 + 41;    // calcus.ru, ипотека на 13 миллионов под 16% без первоначального взноса
}


void bob_init()
{
    bob.account = 0;
    bob.salary = 200 * 1000 * 100;
    bob.food = 25 * 1000 * 100;
    bob.home_bills = 13 * 1000 * 1000 * 100 * 0.001;
    bob.rent = 30 * 1000 * 100;
    bob.overhaul = 50 * 1000 * 100;
    bob.overhaul_outcome = 3 * 1000 * 1000 * 100;
}


void overhaul_init()
{
    overhaul.expenses = 50 * 1000 * 100;
    overhaul.outcome = 3 * 1000 * 1000 * 100;
    overhaul.overhaul_month = 0;
}


void alice_salary(const int month)
{
    alice.account += alice.salary;

    if(month == 12) {
        alice.salary *= 1.07;
    }
}


void bob_salary(const int month)
{
    bob.account += bob.salary;

    if(month == 12) {
        bob.salary *= 1.07;
    }
}


void alice_food(const int month)
{
    alice.account -= alice.food;

    if (month == 12) {
        alice.food *= 1.08;
    }
}


void bob_food(const int month)
{
    bob.account -= bob.food;

    if (month == 12) {
        bob.food *= 1.08;
    }
}


void alice_home_bills(const int month)
{
    if(month == 12) {
        alice.account -= alice.home_bills;
        alice.home_bills *= 1.08;
    }
}


void bob_home_bills(const int month)
{
    if(month == 12) {
        bob.account -= bob.home_bills;
        bob.home_bills *= 1.08;
    }
}


void alice_mortage_payment()
{
    alice.account -= alice.mortage;
}


void bob_rent(const int month, const int year)
{
    bob.account -= bob.rent;
    if(month == 12) {
        bob.rent *= 1.08;
    }
}

void bob_overhaul(const int month, const int year)
{
    bob.account -= overhaul.expenses;
    while(overhaul.overhaul_month != 24) {
        overhaul.overhaul_month += 1;
    }

    if(overhaul.overhaul_month == 24) {
        bob.account += overhaul.outcome;
        overhaul.overhaul_month == 0;
    }

    if(month == 12) {
        overhaul.expenses *= 1.08;
        overhaul.outcome *= 1.08;
    }
}


void alice_deposit()
{
    alice.account *= (1 + 0.2/12);
}


void bob_deposit()
{
    bob.account *= (1 + 0.2/12);
}


void alice_print()
{
    printf("Alice account = %lld RUB\n", alice.account);
}


void bob_print()
{
    printf("Bob account = %lld RUB\n", bob.account);
}


void final_print()
{
    if (alice.account > bob.account) {
        printf("Alice won\n");
    }

    else if (bob.account > alice.account) {
        printf("Bob won\n");
    }

    else {
        printf("Draw\n");
    }
}

void simulation()
{
    int year = 2024;
    int month = 9;

    while( !((year == 2024 + 30) && (month == 10)) ){

        alice_salary(month);
        alice_deposit();
        alice_mortage_payment();
        alice_food(month);
        alice_home_bills(month);

        bob_salary(month);
        bob_deposit();
        bob_rent(month, year);
        bob_food(month);
        bob_home_bills(month);
        bob_overhaul(month, year);

        ++month;
        if(month == 13) {
            month = 1;
            ++year;
        }
    }
}


int main()
{
    alice_init();
    overhaul_init();
    bob_init();

    simulation();

    alice_print();
    bob_print();

    final_print();
}