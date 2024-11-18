
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
int INITIAL_YEAR = 2024;
int INITIAL_MONTH = 9;
int SIMULATION_TIME = 30;


struct Mortgage {
    double rate;  // ставка по ипотеке
    long long int payment;  // ежемесячный платёж по ипотеке
};


struct Deposit {
    double rate;
    long long int balance;
};

struct Person {
    long long int balance;
    long long int salary;
    long long int credit;
    long long int expense;
    long long int flat;
    double inflation;
    struct Deposit deposit;
    struct Mortgage mortgage;
};


struct Person alice;
struct Person bob;


void alice_init() {
    alice.balance = pow(10,6); 
    alice.salary = 2 * pow(10,5);
    alice.credit = 15 * pow(10,6);
    alice.mortgage.rate = 0.2 / 12;
    alice.deposit.rate = 0.12 / 12;
    alice.expense = 30 * 1000;
    alice.inflation = 0.08 / 12;
    alice.mortgage.payment = (alice.credit - alice.balance) * (((alice.mortgage.rate)
        *(pow((1. + alice.mortgage.rate),(SIMULATION_TIME)*12.))) / (pow(1. + (alice.mortgage.rate),(SIMULATION_TIME)*12.) - 1.));
    alice.balance = 0;
}


void bob_init() {
    bob.balance = pow(10,6);
    bob.salary = 2 * pow(10,5);
    bob.credit = 0;
    bob.mortgage.rate = 0;
    bob.deposit.rate = 0.12 / 12;
    bob.expense = 30 * 1000;
    bob.flat = 15 * pow(10,6);
    bob.inflation = 0.08 / 12;
    bob.mortgage.payment = 30 * 1000;
}


//Зарплата


void alice_salary(const int month) {
    alice.balance += alice.salary;
    if (month == 12) {
        alice.salary += alice.salary * 1.08;
    }
}


void bob_salary(const int month) {
    bob.balance += bob.salary;
    if (month == 12) {
        bob.salary += bob.salary * 1.08;
    }
}


//Расходы и вклады


void alice_expenses() {
    alice.expense += alice.expense * alice.inflation;
    alice.balance = alice.balance - alice.expense - alice.mortgage.payment;
}


void bob_expenses() {
    bob.expense += bob.expense * bob.inflation;
    bob.balance = bob.balance - bob.expense - bob.mortgage.payment;
    bob.flat += bob.flat * bob.inflation; 
}

void alice_deposite(){
    alice.deposit.balance += alice.balance;
    alice.deposit.balance += alice.deposit.balance * alice.deposit.rate;
    alice.balance = 0;
}

void bob_deposite() {
    bob.deposit.balance += bob.balance;
    bob.deposit.balance += bob.deposit.balance * bob.deposit.rate;
    bob.balance = 0;
}

void calculations() {
    int month = INITIAL_MONTH;
    int year = INITIAL_YEAR;
    while (year < INITIAL_YEAR + SIMULATION_TIME || month < INITIAL_MONTH) {
        alice_salary(month); 
        alice_expenses();
        alice_deposite(); 

        bob_salary(month); 
        bob_expenses();
        bob_deposite();
        
        month++;
        if (month > 12) {
            year++;
            month = 1;
        }
    }
}


void result() {
if (bob.balance > alice.balance+bob.flat) {
        printf("Bob: %lld\n", bob.balance);
        printf("Alice: %lld\n", alice.balance);
        printf("Bob's flat: %lld\n", bob.flat);
        printf("Bob win\n"); 
    }
    else {
        printf("Bob: %lld\n", bob.balance);
        printf("Alice: %lld\n", alice.balance);
        printf("Bob's flat: %lld\n", bob.flat);
        printf("Alice win\n"); 
    }
}


int main() {

    alice_init();
    bob_init();
    calculations();
    result();  
    return 0;
}