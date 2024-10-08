#include <stdio.h>

typedef long long int Money;

struct Mortgage {
    Money sum;
    Money first_pay;
    Money monthly_payments;
};

struct Rent{
    Money monthly_rent;
    Money all;
};

struct Cat
{
    Money buy;
    Money live;
    Money funeral;
};


struct Person {
    Money account;
    Money salary;
    Money house_price;
    Money food;
    Money expenses;
    Money utility_bills;
    Money deposit_account;
    double deposit_rate;

    struct Mortgage mortgage;
    struct Rent rent;
    struct Cat cat;
};


struct Person alice;
struct Person bob;


void alice_init()
{  
    alice.salary = 250 * 1000;
    alice.account = 1000 * 1000;

    alice.food = 20 * 1000;
    alice.utility_bills = 4 * 1000;
    alice.expenses = 28 * 1000;

    alice.mortgage.sum = 13 * 1000 * 1000;
    alice.mortgage.first_pay = 1000 * 1000;
    alice.mortgage.monthly_payments = 174 * 1000;
    alice.account -= alice.mortgage.first_pay;
    alice.house_price = alice.mortgage.sum;

    alice.deposit_rate = 0.2;
}

void bob_init()
{  
    bob.salary = 250 * 1000;
    bob.account = 0;
    bob.deposit_account = 1000*1000;
    bob.rent.monthly_rent = 30 * 1000;
    bob.rent.all = 0;

    bob.deposit_rate = 0.2;

    bob.food = 22 * 1000;
    bob.utility_bills = 5 * 1000;
    bob.expenses = 25 * 1000;

    bob.cat.buy = 30 * 1000;
    bob.cat.live = 10 *1000;
    bob.cat.funeral =15 *1000;
}


void alice_print()
{
    printf("Alice account = %lld RUB\n", alice.account);
    printf("Alice housse price = %lld RUB\n", alice.house_price);
    printf("---------------------\n");
    printf("Alice capital = %lld RUB\n", alice.account + alice.house_price);
}

void bob_print()
{
    printf("---------------------\n");
    printf("Bob account = %lld RUB\n", bob.account);
    printf("Bob rent = %lld RUB\n", bob.rent.all);
    printf("---------------------\n");
    printf("Bob capital = %lld RUB\n", bob.deposit_account + bob.account);
}

void alice_salary(const int month)
{
    if(month == 1) {
        alice.salary *= 1.07;
    }
    alice.account += alice.salary;
}

void bob_salary(const int month)
{
    if(month == 1) {
        bob.salary *= 1.07;
    }
    bob.account += bob.salary;
}


void alice_expenses(const int month)
{
    if(month == 1) {
        alice.food *= 1.09;
        alice.utility_bills *= 1.09;
    }
    alice.account -= (alice.food + alice.utility_bills + alice.expenses);
}

void bob_expenses(const int month)
{
    if(month == 1) {
        bob.food *= 1.09;
        bob.utility_bills *= 1.09;
        bob.expenses *= 1.09;
    }
    bob.account -= (bob.food + bob.utility_bills + bob.expenses);
}

void alice_mortgage()
{
    alice.account -= alice.mortgage.monthly_payments;
}

void alice_deposit(const int month, const int year)
{
    if(month == 4 && year == 2025) {
        alice.deposit_rate *= 0.09;
    }
    alice.account += alice.account * alice.deposit_rate / 12;
}

void bob_deposit(const int month, const int year)
{
    if(month == 4 && year == 2025) {
        bob.deposit_rate *= 0.09;
    }
    bob.deposit_account += bob.deposit_account * bob.deposit_rate / 12;
}

void bob_house(const int month)
{
    if(month == 1) {
        bob.rent.monthly_rent *= 1.09;
    }
    bob.account -= bob.rent.monthly_rent;
    bob.rent.all += bob.rent.monthly_rent;
}


void alice_house_price(const int month)
{
    if(month == 1) alice.house_price *= 1.07;
}

void bob_cat(const int month, const int year)
{
    if(month == 4 && year == 2026) {
        bob.account -= bob.cat.buy;
    }
    
    if(((year <= 2038 && month == 3) || (year >= 2026 && month >=4))) {
        bob.account -= bob.cat.live;
    }
    
    if(month == 4 && year == 2038) {
        bob.account -= bob.cat.funeral;
    }

}

void simulation()
{
    int year = 2024;
    int month = 9;

    while( !((year == 2024 + 30) && (month == 10)) ) {

        alice_salary(month);
        alice_mortgage();
        alice_house_price(month);
        alice_deposit(month, year);
        alice_expenses(month);

        bob_salary(month);
        bob_house(month);
        bob_deposit(month, year);
        bob_expenses(month);
        bob_cat(month, year);
        
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