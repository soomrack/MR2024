#include <stdio.h>
#include <math.h>

typedef long long int Cash;
typedef double PercentagePt;
const double INFLATION_RATE = 0.09;
const double INDEXSATION_RATE = 0.07;
const double DEPOSITE_RATE = 0.2;

typedef struct {
    Cash salary;
    Cash flat;
    Cash monthly_motgage_pay;
    Cash loan_amount;
    Cash deposit;
    Cash food;
    Cash person_expense;
} Person;

typedef struct {
    PercentagePt mortgage_rate;
    Cash sum_of_mortgage;
    Cash first_pay;
    Cash monthly_pay;
    int duration_years;
    int start_year;
    int start_month;
} Mortgage;

Person bob;
Person alice;
Mortgage alice_mortgage;

Cash calculate_mortgage_pay(Mortgage mortgage) {
    int number_of_months = mortgage.duration_years * 12;
    PercentagePt monthly_percent = mortgage.mortgage_rate / 12;
    Cash payment = (mortgage.sum_of_mortgage - mortgage.first_pay) *
        (monthly_percent * pow((1. + monthly_percent), number_of_months) /
            (pow((1. + monthly_percent), number_of_months) - 1));

    return payment;
}

void alice_mortgage_init(const int year, const int month) {
    alice_mortgage.duration_years = 30;
    alice_mortgage.first_pay = 1000 * 1000;
    alice_mortgage.mortgage_rate = 0.17;
    alice_mortgage.sum_of_mortgage = 13 * 1000 * 1000;
    alice_mortgage.monthly_pay = calculate_mortgage_pay(alice_mortgage);
    alice_mortgage.start_year = year;
    alice_mortgage.start_month = month;
}

void bob_init() {
    bob.salary = 200 * 1000;
    bob.food = 30 * 1000;
    bob.person_expense = 50 * 1000;
    bob.flat = 30 * 1000;
    bob.deposit = 1000 * 1000;
}

void alice_init() {
    alice.salary = 200 * 1000;
    alice.food = 30 * 1000;
    alice.person_expense = 30 * 1000;
    alice.flat = 5 * 1000;
    alice.deposit = 1000 * 1000;
    alice_mortgage_init(2024, 9);
    alice.monthly_motgage_pay = alice_mortgage.monthly_pay;
}

void bob_salary(const int month) {
    bob.deposit += bob.salary;

    if (month == 12) {
        bob.salary *= 1. + INDEXSATION_RATE;
    }
}

void bob_flat(const int month) {
    bob.deposit -= bob.flat;

    if (month == 12) {
        bob.flat *= 1. + INFLATION_RATE;
    }
}

void bob_expense(const int month) {
    bob.deposit -= bob.food;
    bob.deposit -= bob.person_expense;
    if (month == 12) {
        bob.food *= 1. + INFLATION_RATE;
        bob.person_expense *= 1. + INFLATION_RATE;
    }
}


void bob_deposit(const int month) {
    if (month == 12) {
        bob.deposit *= 1. + DEPOSITE_RATE;
    }
}

void alice_salary(const int month) {
    alice.deposit += alice.salary;

    if (month == 12) {
        alice.salary *= 1. + INDEXSATION_RATE;
    }
}

void alice_expense(const int month) {
    alice.deposit -= alice.food;
    alice.deposit -= alice.person_expense;
    if (month == 12) {
        alice.food *= 1. + INFLATION_RATE;
        alice.person_expense *= 1. + INFLATION_RATE;
    }
}

void alice_mortgage_monthly_pay() {
    alice.deposit -= alice.monthly_motgage_pay;
}

void alice_deposit(const int month) {
    if (month == 12) {
        alice.deposit *= 1. + DEPOSITE_RATE;
    }
}

void simulation(int start_year, int start_month, int years_to_simulate) {
    int current_year = start_year;
    int current_month = start_month;

    while (!(current_year == start_year + years_to_simulate && current_month == start_month)) {

        bob_salary(current_month);
        bob_expense(current_month);
        bob_flat(current_month);
        bob_deposit(current_month);

        alice_salary(current_month);
        alice_expense(current_month);
        alice_deposit(current_month);
        alice_mortgage_monthly_pay();

        current_month += 1;
        if (current_month == 13) {
            current_year++;
            current_month = 1;
        }
    }
}


void print_result() {
    printf("Bob capital = %lli Rub\n", (Cash)(bob.deposit));
    printf("Alice capital = %lli Rub\n", (Cash)(alice.deposit));
}


int main() {
    alice_init();
    bob_init();

    simulation(2024, 9, 30);

    print_result();

    return 0;
}