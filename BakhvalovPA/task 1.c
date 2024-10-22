#include <stdio.h>

typedef long long int Money;    //КОП.
const double INFLATION_RATE = 1.09;
const double DEPOSIT_RATE = 0.2;


typedef struct Mortgage {
    Money sum;
    Money first_pay;
    Money monthly_payment;
    double mortgage_rate;
} Mortgage;



typedef struct Person {
    Money salary;
    Money account;
    Mortgage mortgage;
    Money house_price;
    Money expenses;
    Money deposit;
    Money rent;

} Person;


Person alice;
Person bob;


void alice_init()
{
    alice.salary = 200 * 1000 * 100;
    alice.account = 1000 * 1000 * 100;
    alice.expenses = 30 * 1000 * 100;
    alice.mortgage.sum = 20 * 1000 * 1000 * 100;
    alice.mortgage.first_pay = 1000 * 1000 * 100;
    alice.mortgage.monthly_payment = 25550383;  // https://calcus.ru/kalkulyator-ipoteki
    alice.mortgage.mortgage_rate = 0.16;
    alice.account -= alice.mortgage.first_pay;
    alice.house_price = alice.mortgage.sum;

}


void bob_init()
{
    bob.salary = 200 * 1000 * 100;
    bob.account =  1000 * 1000 * 100;
    bob.expenses = 30 * 1000 * 100;
    bob.rent = 30 * 1000 * 100;
}


void print_alice()
{
    printf("Alice account = %lld RUB\n", alice.account / 100);
    printf("Alice house price = %lld RUB\n", alice.house_price / 100);
    printf("Alice capital = %lld RUB\n", (alice.account + alice.house_price) / 100);
}


void print_bob()
{
    printf("Bob capital = %11d RUB\n", bob.account / 100);
}


void alice_salary(const int month)
{
    if (month == 12) {
        alice.salary *= INFLATION_RATE;
    }
    alice.account += alice.salary;
}


void bob_salary(const int month)
{
    if (month == 12) {
        bob.salary *= INFLATION_RATE;
    }
    bob.account += bob.salary;
}


void alice_mortgage()
{
    alice.account -= alice.mortgage.monthly_payment;
}


void bob_rent(const int month)
{
    if (month == 12) {
        bob.rent *= INFLATION_RATE;
    }
    bob.account -= bob.rent;
}


void alice_expenditure(const int month)
{
    if (month == 12) {
        alice.expenses *= INFLATION_RATE;
    }
    alice.account -= (alice.expenses);
}


void bob_expenditure(const int month)
{
    if (month == 12) {
        bob.expenses *= INFLATION_RATE;
    }
    bob.account -= (bob.expenses);
}


void alice_house_price(const int month)
{
    if (month == 1) alice.house_price *= INFLATION_RATE;
}


void alice_deposit(const int month)
{
    alice.account += alice.account * DEPOSIT_RATE / 12;
}


void bob_deposit(const int month)
{
    bob.account += bob.account * DEPOSIT_RATE / 12;
}


void simulation()
{
    int month = 10;
    int year = 2024;

    while (!((year == 2024 + 30) && (month == 10))) {
        alice_salary(month);
        alice_mortgage();
        alice_house_price(month);
        alice_expenditure(month);
        alice_deposit(month);

        bob_salary(month);
        bob_rent(month);
        bob_expenditure(month);
        bob_deposit(month);


        month++;
        if (month == 13) {
            month = 1;
            year++;
        }
    }
}


int main()
{
    alice_init();
    bob_init();

    simulation();

    print_alice();
    print_bob();
    return 0;
}
