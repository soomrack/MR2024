#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

typedef int64_t Money;  // В копейках
const double INFLATION_RATE = 0.08;
const double DEPOSIT_RATE = 0.2;
const double MORTGAGE_RATE = 0.17;
const double RENT_INFLATION_RATE = 0.09;
const int SIMULATION_YEARS = 30;

typedef struct {
    int year;
    int month;
} Date;

typedef struct {
    Money amount;
    double rate;
    int remaining_months;
} Mortgage;

typedef struct {
    Money capital;
    double rate;
} Deposit;

typedef struct {
    char name[10];
    Money salary;
    Money capital;
    Money food_expenses;
    Money personal_expenses;
    Money utility_expenses;
    Money rent_expenses;
    Money monthly_payment;
    Deposit deposit;
    Mortgage mortgage;
} Person;

void date_increment(Date *date) {
    date->month++;
    if (date->month == 13) {
        date->month = 1;
        date->year++;
    }
}

void person_init(Person *person, const char *name, Money salary, Money initial_capital, Money food, Money personal, Money utility, Money rent, double deposit_rate, Mortgage mortgage) {
    strcpy(person->name, name);
    person->salary = salary;
    person->capital = initial_capital;
    person->food_expenses = food;
    person->personal_expenses = personal;
    person->utility_expenses = utility;
    person->rent_expenses = rent;
    person->monthly_payment = 0;
    person->deposit.capital = 0;
    person->deposit.rate = deposit_rate;
    person->mortgage = mortgage;
}

double calculate_monthly_mortgage_payment(Mortgage mortgage) {
    double monthly_rate = mortgage.rate / 12.0;
    double total_rate = pow((1.0 + monthly_rate), mortgage.remaining_months);
    return (mortgage.amount * monthly_rate * total_rate) / (total_rate - 1);
}

void update_salary(Person *person, int month) {
    if (month == 1) {
        person->salary += person->salary * INFLATION_RATE;
    }
    person->capital += person->salary;
}

void update_expenses(Person *person, int month) {
    if (month == 1) {
        person->food_expenses += person->food_expenses * INFLATION_RATE;
        person->personal_expenses += person->personal_expenses * INFLATION_RATE;
        person->utility_expenses += person->utility_expenses * INFLATION_RATE;
        if (person->rent_expenses > 0) {
            person->rent_expenses += person->rent_expenses * RENT_INFLATION_RATE;
        }
    }
    person->capital -= (person->food_expenses + person->personal_expenses + person->utility_expenses + person->monthly_payment);
}

void update_deposit(Person *person) {
    person->deposit.capital += person->capital;
    person->deposit.capital *= (1.0 + person->deposit.rate / 12.0);
    person->capital = 0;
}

void pay_mortgage(Person *person) {
    if (person->mortgage.amount > 0) {
        double monthly_payment = calculate_monthly_mortgage_payment(person->mortgage);
        person->capital -= monthly_payment;
        person->mortgage.amount -= monthly_payment;
        person->mortgage.remaining_months--;
    }
}

void simulation(Person *alice, Person *bob, Date start_date, Date end_date) {
    Date current_date = start_date;

    while (current_date.year < end_date.year || (current_date.year == end_date.year && current_date.month < end_date.month)) {
        update_salary(alice, current_date.month);
        update_salary(bob, current_date.month);

        update_expenses(alice, current_date.month);
        update_expenses(bob, current_date.month);

        pay_mortgage(alice);

        update_deposit(alice);
        update_deposit(bob);

        date_increment(&current_date);
    }
}

void print_results(Person *person) {
    printf("%s's final capital: %lld Rub\n", person->name, person->deposit.capital / 100);
}

int main() {
    Mortgage alice_mortgage = {.amount = 1300000000, .rate = MORTGAGE_RATE, .remaining_months = SIMULATION_YEARS * 12};
    Mortgage bob_mortgage = {.amount = 0, .rate = 0, .remaining_months = 0};

    Person alice, bob;
    person_init(&alice, "Alice", 30000000, 100000000, 1500000, 1700000, 800000, 0, DEPOSIT_RATE, alice_mortgage);
    person_init(&bob, "Bob", 30000000, 100000000, 1500000, 1700000, 800000, 3000000, DEPOSIT_RATE, bob_mortgage);

    Date start_date = {.year = 2024, .month = 9};
    Date end_date = {.year = start_date.year + SIMULATION_YEARS, .month = start_date.month};

    simulation(&alice, &bob, start_date, end_date);

    print_results(&alice);
    print_results(&bob);

    return 0;
}
