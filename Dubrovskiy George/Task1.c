#include <stdio.h>
#include <math.h>

typedef long long int Coins; // рассчет в копейках

const double INFLATION = 1.09;
const double DEPOSIT_PERCENT = 1.0169; // при процентой ставке 20% годовых примерный процент в месяц 1.69%


struct  Mortgage {
    Coins house_cost;
    Coins sum;
    Coins first_payment;   //ipoteka
    Coins monthly_payment;
    double mortgage_percent;
} typedef Mortgage;


typedef struct Deposit {
    Coins deposit;          //vklad
    double deposit_percent; 
}Deposit;

typedef struct Household{
    Coins food;
    Coins person_expens; //household expens
}Household;

typedef struct Person {
    Coins salary;
    Mortgage mortgage;
    Coins deposit;
    Coins house_timeprice;
    Household household;
    Coins house_bills;
    Coins rent;
    Coins acaunt;
}Person;
Person alice;
Person bob;


void alice_start_parametrs(){
    alice.salary = 200 * 1000 * 100;
    alice.acaunt = 1000 * 1000 * 100;

    alice.mortgage.house_cost = 5600 * 1000 * 100; //s sayta
    alice.mortgage.first_payment = 1000 * 1000 * 100;
    alice.mortgage.sum = alice.mortgage.house_cost - alice.mortgage.first_payment;
    alice.mortgage.monthly_payment = 24694 * 100;						//l'gotnaya ipoteka 5%
    alice.house_bills =  2500 * 100; //s sayta, po kadastru
    alice.house_timeprice = alice.mortgage.house_cost = 5600 * 1000 * 100;
    
    alice.acaunt -= alice.mortgage.first_payment;

    alice.household.food = 10 * 000 * 100;
    alice.household.person_expens = 25 * 1000 * 100;
    alice.deposit = 0;
}

void bob_start_parametrs() {
    bob.salary = 200 * 1000 * 100;
    bob.acaunt = 1000 * 1000 * 100;

    bob.house_bills = 2500 * 100;
    bob.rent = 40*1000 * 100; 

    bob.household.food = 10 * 1000 * 100;
    bob.household.person_expens = 25 * 1000 * 100;
    bob.deposit = 0;
}


void alice_salary(int month) {
    alice.acaunt += alice.salary;
        if (month == 12)
        {
            alice.salary *= INFLATION;
        }
}

void bob_salary(int month) {
    bob.acaunt += bob.salary;
    if (month == 12)
    {
        bob.salary *= INFLATION;
    }
}


void alice_household(int month) {
    alice.acaunt -= (alice.mortgage.monthly_payment + alice.house_bills + alice.household.food + alice.household.person_expens);
    if (month == 12)
    {
        alice.household.food *= INFLATION;
        alice.household.person_expens *= INFLATION;
    }
}


void bob_household(int month) {
    bob.acaunt -= (bob.rent + bob.house_bills + bob.household.food + bob.household.person_expens);
    if (month == 12)
    {
        bob.household.food *= INFLATION;
        bob.household.person_expens *= INFLATION;
    }
}


void alice_deposit(int month) {
    alice.deposit += alice.acaunt;
    alice.acaunt = 0;
    if (month == 12)
    {
        alice.deposit *= DEPOSIT_PERCENT;
    }
}

void bob_deposit(int month) {
    bob.deposit += bob.acaunt;
    bob.acaunt = 0;
    if (month == 12)
    {
        bob.deposit *= DEPOSIT_PERCENT;
    }
}

void alice_print() {
    printf("Alce status: %lld rubles\n", (alice.deposit + alice.mortgage.house_cost) / 100);
}

void bob_print() {
    printf("Bob status: %lld rubles\n", bob.deposit / 100);
}



void life(){
    int start_month = 10;
    int start_year = 2024;
    int month = start_month;
    int year = start_year;
    while (!((year == start_year + 30) && (month == start_month))){
        alice_salary(month);
        alice_household(month);
        alice_deposit(month);
        
        bob_salary(month);
        bob_household(month);
        bob_deposit(month);
        month++;
        if (month == 13) {
            month = 1;
            year++;
        }
    }
}
int main() {
    alice_start_parametrs();
    bob_start_parametrs();
    
    life();
    
    alice_print();
    bob_print();
    return 0;
}
