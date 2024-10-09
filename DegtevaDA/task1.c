#include <stdio.h>

typedef long long int Money;    //коп.
const double INFLATION_RATE = 1.09;
const double DEPOSIT_RATE = 0.2;


typedef struct Mortgage {
    Money sum;
    Money first_pay;
    Money monthly_payment;
    double mortgage_rate;
} Mortgage;


typedef struct Bike {
    Money cost;
    int month_bye;
} Bike;


typedef struct Person {
    Money salary;
    Money account;
    Mortgage mortgage;
    Money house_price;
    Money food;
    Money house_bills;
    Money personal_expens;
    Money deposit;
    Money rent;
    Bike bike;

} Person;


Person alice;
Person bob;


void alice_init()
{
    alice.salary = 200 * 1000 * 100;
    alice.account = 1000 * 1000 * 100;

    alice.food = 15 * 1000 *100;
    alice.house_bills = 7 * 1000 * 100;
    alice.personal_expens = 16 * 1000 * 100;
    alice.mortgage.sum = 13 * 1000 * 1000 * 100;
    alice.mortgage.first_pay = 1000 * 1000 * 100;
    alice.mortgage.monthly_payment = 16137084;  // https://calcus.ru/kalkulyator-ipoteki
    alice.mortgage.mortgage_rate = 0.16;
    alice.account -= alice.mortgage.first_pay;
    alice.house_price = alice.mortgage.sum;

    alice.bike.cost = 30 * 1000 * 100;
    alice.bike.month_bye = 0;
}


void bob_init()
{
    bob.salary = 200 * 1000 * 100;
    bob.account = 1000 * 1000 * 100;

    bob.food = 15 * 1000 * 100;
    bob.house_bills = 7 * 1000 * 100;
    bob.personal_expens = 16 * 1000 * 100;
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
    if (month == 12){
        bob.rent *= INFLATION_RATE;
    }
    bob.account -= bob.rent;
}


void alice_expenditure(const int month)
{
    if (month == 12){
        alice.food *= INFLATION_RATE;
        alice.house_bills *= INFLATION_RATE;
        alice.personal_expens *= INFLATION_RATE;
    }
    alice.account -= (alice.food + alice.house_bills + alice.personal_expens);
}


void bob_expenditure(const int month)
{
    if (month == 12) {
        bob.food *= INFLATION_RATE;
        bob.house_bills *= INFLATION_RATE;
        bob.personal_expens *= INFLATION_RATE;
    }
    bob.account -= (bob.food + bob.house_bills + bob.personal_expens);
}


void alice_house_price(const int month)
{
    if (month == 1) alice.house_price *= INFLATION_RATE;
}


void alice_bike(const int month, const int year)
{
    alice.bike.month_bye += 1;

    if (alice.bike.month_bye == 36) {
        alice.account -= alice.bike.cost;
        alice.bike.month_bye = 0;
    }

    if (month == 1) alice.bike.cost *= INFLATION_RATE;
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
    int month = 9;
    int year = 2024;

    while( !((year == 2024 + 30) && (month == 9)) ) {
        alice_salary(month);
        alice_mortgage();
        alice_house_price(month);
        alice_expenditure(month);
        alice_bike(year, month);
        alice_deposit(month);

        bob_salary(month);
        bob_rent(month);
        bob_expenditure(month);
        bob_deposit(month);


        month++;
        if(month == 13) {
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
