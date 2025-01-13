#include <stdio.h>


typedef long long int Money; //RUB


typedef struct Mortgage {
    Money sum;
    Money first_pay;
    Money monthly_payment;
    double mortgage_rate;
} Mortgage;


typedef struct Person {
    Money account;
    Money salary;
    Mortgage mortgage;
    Money food;
    Money home_bills;
    Money mortage;
    Money rent;
} Person;


Person alice;
Person bob;


void alice_init()
{
    alice.account = 800 * 1000 * 100;
    alice.salary = 200 * 1000 * 100;

    alice.food = 25 * 1000 * 100;
    alice.home_bills = 13 * 1000 * 1000 * 100 * 0.001;

    alice.mortgage.sum = 13 * 1000 * 1000 * 100;
    alice.mortgage.first_pay = 1000 * 1000 * 100;
    alice.mortgage.monthly_payment = 16137084;  // https://calcus.ru/kalkulyator-ipoteki
    alice.mortgage.mortgage_rate = 0.16;
    alice.account -= alice.mortgage.first_pay;
}


void bob_init()
{
    bob.account = 800 * 1000 * 100;
    bob.salary = 200 * 1000 * 100;
    bob.food = 25 * 1000 * 100;
    bob.home_bills = 13 * 1000 * 1000 * 100 * 0.001;
    bob.rent = 30 * 1000 * 100;

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


void alice_mortgage()
{
    alice.account -= alice.mortgage.monthly_payment;
}


void bob_rent(const int month, const int year)
{
    bob.account -= bob.rent;
    if(month == 12) {
        bob.rent *= 1.08;
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


void simulation()
{
    int year = 2024;
    int month = 9;

    while( !((year == 2024 + 30) && (month == 10)) ){

        alice_salary(month);
        alice_deposit();
        alice_mortgage();
        alice_food(month);
        alice_home_bills(month);

        bob_salary(month);
        bob_deposit();
        bob_rent(month, year);
        bob_food(month);
        bob_home_bills(month);

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
    bob_init();

    simulation();

    alice_print();
    bob_print();
}