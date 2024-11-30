#include <stdio.h>

typedef unsigned long long int Money;  // расчёт в рублях

const int START_YEAR = 2024;
const int START_MONTH = 9;
const int PERIOD = 30;  // расчетный период симуляции


struct Mortgage
{
    Money sum;
    Money first_pay;
    Money monthly_payments;
    double rate;
};

struct Deposite
{
    Money account;
    double rate;
};


struct Cat
{
    Money buy;
    Money cost;
    Money funeral;
};


struct Person
{
    Money salary;
    Money capital;
    Money house_price;
    Money monthly_cost;
    Money monthly_rent;
    struct Mortgage mortgage;
    struct Cat cat;
    struct Deposite deposite;
    double inflation_rate;
};

struct Person alice;
struct Person bob;


void alice_init()
{
    alice.capital = 1 * 1000 * 1000;
    alice.salary = 280 * 1000;
    alice.monthly_cost = 30 * 1000;

    alice.deposite.rate = 0.2;
    alice.inflation_rate = 0.08;

    alice.mortgage.sum = 17 * 1000 * 1000;
    alice.mortgage.first_pay = 1 * 1000 * 1000;
    alice.mortgage.rate = 0.17;
    alice.mortgage.monthly_payments = 230 * 1000; 
    alice.capital -= alice.mortgage.first_pay;
    alice.house_price = alice.mortgage.sum;
}


void alice_salary(const int month)
{
    if (month == 1) {
        alice.salary *= (1. + alice.inflation_rate);
    }

    alice.capital += alice.salary;
}


void alice_mortgage(const int month)
{
    alice.capital -= alice.mortgage.monthly_payments;
}


void alice_monthly_cost(const int month)
{
    if (month == 1) {
        alice.monthly_cost *= (1. + alice.inflation_rate);
    } 

    alice.capital -= alice.monthly_cost;
}


void alice_house_price(const int month)
{
    if (month == 1) {
        alice.house_price *= (1. + alice.inflation_rate);
    }
}


void alice_deposit()
{
    alice.capital += alice.capital * (alice.deposite.rate/12);
}


void alice_print()
{
    printf("Alice total capital: %lld \n", alice.capital + alice.house_price);
}




void bob_init()
{
    bob.capital = 1 * 1000 * 1000;
    bob.salary = 280 * 1000;
    bob.monthly_cost = 30 * 1000;
    bob.cat.buy = 30 * 1000;
    bob.cat.cost = 12 * 1000;
    bob.cat.funeral = 45 * 1000;
    bob.deposite.account = 0;

    bob.deposite.rate = 0.2;
    bob.inflation_rate = 0.08;

    bob.monthly_rent = 40 * 1000;
}


void bob_salary(const int month)
{
    if (month == 1) {
        bob.salary *= (1. + bob.inflation_rate);
    }

    bob.capital += bob.salary;
}


void bob_rent(const int month)
{
    if (month == 1) {
        bob.monthly_rent *= (1. + bob.inflation_rate);
    } 
    
    bob.capital -= bob.monthly_rent;
}


void bob_monthly_cost(const int month)
{
    if (month == 1) {
        bob.monthly_cost *= (1. + bob.inflation_rate);
    }
    
    bob.capital -= bob.monthly_cost;
}


void bob_cat(const int month, const int year)
{
    static int is_cat = 0;

    if ((month == 2) && (year == 2026) ) {
        bob.capital -= bob.cat.buy;
        is_cat = 1;
    }

    if (is_cat == 1) {
        if (month == 1) {
        bob.cat.cost *= (1. + bob.inflation_rate);
        }

        bob.capital -= bob.cat.cost;
    }

    if ((month == 7) && (year == 2038)) {
        bob.capital -= bob.cat.funeral;
        is_cat = 0;
    }
}


void bob_deposit(month, year)
{
   if ((month == START_MONTH) && (year == START_YEAR)){
       bob.deposite.account += bob.capital;
       bob.capital = 0;
   }
    bob.deposite.account += bob.capital;
    bob.deposite.account += bob.deposite.account * (bob.deposite.rate / 12);
    bob.capital = 0;
    
}


void bob_print()
{
    printf("Bob total capital: %lld \n", bob.deposite.account);
}


void conclusion()
{
    printf("----------------------------------\n");
    if ((alice.capital + alice.house_price) > bob.deposite.account) {
        printf("Alice's life is pofitapler\n");
    } else {
        if ((alice.capital + alice.house_price) == bob.deposite.account) {
            printf("Alice's and Bob's lifes are similar\n");
        } else {
            printf("Bob's life is profitable\n");
        }
    }

}


void simulation()          
{
    int year = START_YEAR;
    int month = START_MONTH;
    
    while (!((year == START_YEAR + PERIOD) && (month == START_MONTH + 1))) {
        alice_salary(month);
        alice_mortgage(month);
        alice_monthly_cost(month);
        alice_house_price(month);
        alice_deposit();
        
        bob_salary(month);
        bob_rent(month);
        bob_monthly_cost(month);
        bob_cat(month, year);
        bob_deposit(month, year);

        month++;
        if (month == 13) {
            year++;
            month = 1;
        }
    }
}

void print_output()
{
    printf("Results for %d.%d are:\n",START_MONTH, START_YEAR + PERIOD);
    printf("----------------------------------\n");
    alice_print();
    bob_print();
    conclusion();
}

int main()
{
    alice_init();
    bob_init();
    simulation();
    print_output();
    return 0;
}

