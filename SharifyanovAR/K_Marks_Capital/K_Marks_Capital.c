#include<stdio.h>
#include<math.h>


typedef long long int Money;


struct animal {
    Money buy;
    Money cost;
    int lifetime; // months
}cat, parrot;

struct dude {
    Money deposit;
    Money capital;
    Money salary;
    Money expenses;
    Money unx_expeses; 
    Money flat_payment;
    Money overpayment;
    struct animal cat;
    struct animal parrot;
}Alice, Bob;

const Money START_DEPOSIT = 0;
const Money START_CAPITAL = 1100000 *100;
const Money INIT_SALARY = 270000 * 100;
const Money INIT_EXPENSES = 50000 * 100;
const Money INIT_UNX_EXPENSES = 10000 * 100;
const Money FLAT_PRICE = 10650000 * 100;
const Money RENT_PRICE = 45000 * 100;
const Money MORTRAGE_MOUNTH_PRICE = 170000 * 100;
const Money MORTRAGE_FIST_PAYMENT = 106500 * 100;

const Money ANIMAL_BUY_COST = 10000 * 100;
const Money ANIMAL_KEEPING_COST = 2000 * 100; // per month

const int SIMULATION_DURATION = 30;
const int START_YEAR = 2024;
const int START_MONTH = 9;

double inflation = 0.10;
double invest_rate = 0.18;

void Alice_init() {
    Alice.deposit = START_DEPOSIT;
    Alice.capital = START_CAPITAL- MORTRAGE_FIST_PAYMENT;
    Alice.salary = INIT_SALARY;
    Alice.expenses = INIT_EXPENSES;
    Alice.unx_expeses = INIT_UNX_EXPENSES;
    Alice.flat_payment = MORTRAGE_MOUNTH_PRICE;
    //Alice.overpayment = MORTRAGE_MOUNTH_PRICE * SIMULATION_DURATION - FLAT_PRICE;
}

void Bob_init() {
    Bob.deposit = START_DEPOSIT;
    Bob.capital = START_CAPITAL;
    Bob.salary = INIT_SALARY;
    Bob.expenses = INIT_EXPENSES;
    Bob.unx_expeses = INIT_UNX_EXPENSES;
    Bob.flat_payment = RENT_PRICE;
    Bob.cat.buy = ANIMAL_BUY_COST;
    Bob.cat.cost = ANIMAL_KEEPING_COST;
    Bob.cat.lifetime = 96;
    Bob.parrot.buy = ANIMAL_BUY_COST;
    Bob.parrot.cost = ANIMAL_KEEPING_COST;
    Bob.parrot.lifetime = 600;
    //Bob.overpayment = RENT_PRICE * pow((1.0 + inflation), SIMULATION_DURATION) - FLAT_PRICE;
}



void alice_salary(const int month)
{
    if (month == 1) {
        Alice.salary *= (1. + inflation);
    }

    Alice.capital += Alice.salary;
}


void alice_mortgage(const int month)
{
    Alice.capital -= Alice.flat_payment;
}


void alice_expenses(const int month)
{
    if (month == 1) {
        Alice.expenses *= (1. + inflation);
    }

    Alice.capital -= Alice.expenses;
}

void alice_unx_expenses(const int month)
{
    if (month == 1) {
        Alice.unx_expeses *= (1. + inflation);
    }

    Alice.capital -= Alice.unx_expeses;
}

void alice_house_price(const int month)
{
    if (month == 1) {
        Alice.flat_payment *= (1. + inflation);
    }
}


void alice_deposit()
{
    if (Alice.capital > 0) {
        Alice.deposit += Alice.capital * (invest_rate / 12);
    }
}


void alice_print()
{
    printf("Alice summary capital: %lld \n", Alice.capital / 100 + Alice.deposit / 100);
}




void bob_salary(const int month, const int year)
{
    if (month == 1) {
        Bob.salary *= (1. + inflation);
    }
    if (month == 1 && year == 2034) {
        Bob.salary *= 1.5; // extra condition
    }
    if (month != 8) {
     Bob.capital += Bob.salary; // extra condition
    }
}


void bob_rent(const int month)
{
    if (month == 1) {
        Bob.flat_payment *= (1. + inflation);
    }

    Bob.capital -= Bob.flat_payment;
}


void bob_expenses(const int month, const int year)
{
    if (month == 1) {
        Bob.expenses *= (1. + inflation);
    }
    Bob.capital -= Bob.expenses;
}

void bob_animal_cost(const int month, const int year)
{
    if ((month >= 3 && year >= 2027) && !(year == cat.lifetime / 12 && month == cat.lifetime % 12 + 3)) {
        Bob.capital -= Bob.cat.cost;
    }
    if ((month >= 6 && year >= 2034) && !(year == parrot.lifetime / 12 && month == parrot.lifetime % 12 + 6)) {
        Bob.capital -= Bob.parrot.cost;
    }
    if (month == 1) {
        Bob.parrot.cost *= (1. + inflation);
        Bob.cat.cost *= (1. + inflation);
    }
}

void bob_animal_buy(const int month, const int year)
{
    if (month == 3 && year == 2027) {
        Bob.capital -= Bob.cat.buy;
    }
    if (month == 6 && year == 2034) {
        Bob.capital -= Bob.parrot.buy;
    }
}

void bob_unx_expenses(const int month, const int year)
{
    if (month == 1) {
        Bob.unx_expeses *= (1. + inflation);
    }
    Bob.capital -= Bob.unx_expeses;
}



void bob_deposit()
{
    if (Bob.capital > 0) {
        Bob.deposit += Bob.capital * (invest_rate / 12);
    }
}


void bob_print()
{
    printf("Bob summary capital: %lld \n", Bob.capital / 100 + Bob.deposit / 100);
}


void conclusion()
{
    printf(".......................\n");
    if (Alice.capital > Bob.capital) {
        printf("Alice is winner\n");
    }
    else {
        if (Alice.capital == Bob.capital) {
            printf("Alice and Bob are equal\n");
        }
        else {
            printf("Bob is winner\n");
        }
    }

}


void simulation()
{
    int year = START_YEAR;
    int month = START_MONTH;

    while ((year != START_YEAR + SIMULATION_DURATION) && (month != START_MONTH + 1)) {
        alice_salary(month);
        alice_mortgage(month);
        alice_unx_expenses(month);
        alice_expenses(month);
        alice_house_price(month);
        alice_deposit();

        bob_salary(month, year);
        bob_rent(month);
        bob_unx_expenses(month,year);
        bob_expenses(month, year);
        bob_deposit();

        month++;
        if (month == 13) {
            year++;
            month = 1;
        }
    }
}

void print_output()
{
    printf("Results for %d.%d are:\n", START_MONTH, START_YEAR + SIMULATION_DURATION);
    printf(".......................\n");
    alice_print();
    bob_print();

    conclusion();
}

int main()
{
    Alice_init();
    Bob_init();

    simulation();

    print_output();
    return 0;
}
