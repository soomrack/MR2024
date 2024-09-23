#include <stdio.h>
#include <math.h>

typedef long long int Cash;
typedef double PercentagePt;
const double INFLATION_RATE = 0.09;
const double INDEXSATION_RATE = 0.07;
const double DEPOSITE_RATE = 0.2;

typedef struct {
    Cash salary;
    Cash deposit;
    Cash food;
    Cash transport;
    Cash person_expense;
    Cash flat;
    Cash communal_bills;
    Cash monthly_motgage_pay;
    Cash loan_amount;
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

typedef struct {
    Cash cost;
    Cash benzin_per_month_cost;
    Cash utility_cost;
    Cash car_tax;
    Cash washing_per_month_cost;
    int year_of_purchasing;
    int month_of_purchasing;
} Car;

Person bob;
Person alice;
Mortgage alice_mortgage;
Car gelic;

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
    bob.transport = 10 * 1000;
    bob.person_expense = 50 * 1000;
    bob.flat = 30 * 1000;
    bob.deposit = 1000 * 1000;
}

void alice_init() {
    alice.salary = 200 * 1000;
    alice.food = 30 * 1000;
    alice.transport = 10 * 1000;
    alice.person_expense = 30 * 1000;
    alice.flat = 5 * 1000;
    alice.deposit = 1000 * 1000;
    alice_mortgage_init(2024, 9);
    alice.monthly_motgage_pay = alice_mortgage.monthly_pay;
}

void gelic_init() {
    gelic.cost = 30 * 1000 * 1000; // Car cost in rubles
    gelic.benzin_per_month_cost = 50 * 100; 
    gelic.utility_cost = 50 * 1000; 
    gelic.car_tax = 30 * 1000; 
    gelic.washing_per_month_cost = 1 * 1000; 
    gelic.year_of_purchasing = 2038;
    gelic.month_of_purchasing = 7;
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
    bob.deposit -= bob.transport;
    bob.deposit -= bob.person_expense;
    if (month == 12) {
        bob.food *= 1. + INFLATION_RATE;
        bob.transport *= 1. + INFLATION_RATE;
        bob.person_expense *= 1. + INFLATION_RATE;
    }
}

void bob_disaster(const int month, const int year) {
    if (month == gelic.month_of_purchasing && year == gelic.year_of_purchasing) {
        bob.deposit -= gelic.cost;  
    }
}

void bob_car_annual_expense(const int year, const int month) {
    if (year > gelic.year_of_purchasing ||
        (year == gelic.year_of_purchasing && month > gelic.month_of_purchasing)) {
       
        Cash total_car_expense = (gelic.benzin_per_month_cost + gelic.washing_per_month_cost) * 12
            + gelic.car_tax + gelic.utility_cost;

        if (month == 12) {  
            bob.deposit -= total_car_expense;
        }
        if (month == 12) {
            gelic.benzin_per_month_cost *= 1. + INFLATION_RATE;
            gelic.washing_per_month_cost *= 1. + INFLATION_RATE;
            gelic.car_tax *= 1. + INFLATION_RATE;
            gelic.utility_cost *= 1. + INFLATION_RATE;
        }
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

void alice_communal_bills(const int month) {
    alice.deposit -= alice.communal_bills;

    if (month == 12) {
        alice.communal_bills *= 1. + INFLATION_RATE;
    }
}

void alice_expense(const int month) {
    alice.deposit -= alice.food;
    alice.deposit -= alice.transport;
    alice.deposit -= alice.person_expense;
    if (month == 12) {
        alice.food *= 1. + INFLATION_RATE;
        alice.transport *= 1. + INFLATION_RATE;
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
        bob_disaster(current_month, current_year);
        bob_car_annual_expense(current_year, current_month);

        alice_salary(current_month);
        alice_expense(current_month);
        alice_communal_bills(current_month);
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
    gelic_init();

    simulation(2024, 9, 30);

    print_result();

    return 0;
}