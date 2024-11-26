#include <stdio.h>
#include <math.h>

typedef long long int Cash;

typedef double PercentagePt;

const double INDEXSATION_RATE = 0.07;
const double DEPOSITE_RATE = 0.2;
const double INFLATION_RATE = 0.09;


typedef struct {
    Cash salary;
    Cash flat;
    Cash monthly_motgage_pay;
    Cash loan_amount;
    Cash deposit;
    Cash food;
    Cash person_expense;
    Cash transport;
    Cash pet_expense;
} Person;


typedef struct {
    Cash cost;
    Cash maintenance_cost;
    Cash fuel_per_month_cost;
    Cash washing_per_month_cost;
    Cash car_tax;
    int month_of_purchasing;
    int year_of_purchasing;
} Car;


typedef struct
{
    Cash cost;
    Cash food_cost;
    Cash clean_cost;
    int year_of_buy;
    int month_of_buy;
    int year_of_death;
    int month_of_death;
} Pet;


typedef struct {
    PercentagePt mortgage_rate;
    Cash sum_of_mortgage;
    Cash monthly_pay;
    Cash first_pay;
    int duration_years;
    int start_year;
    int start_month;
} Mortgage;


Person bob;
Person alice;
Mortgage alice_mortgage;
Car bob_transport;
Pet alice_cat;

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
    bob.transport = 10 * 1000;
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


void transport_init() {
    bob_transport.cost = 30 * 1000 * 1000; 
    bob_transport.maintance_cost = 5 * 1000; 
    bob_transport.benzin_per_month_cost = 50 * 100; 
    bob_transport.washing_per_month_cost = 1 * 1000; 
    bob_transport.car_tax = 3 * 1000; // Car tax
    bob_transport.month_of_purchasing = 9;
    bob_transport.year_of_purchasing = 2042;
    
}


void cat_init(const int year_of_buy, const  int month_of_buy)
{
    alice_cat.cost = 10 * 1000;
    alice_cat.food_cost = 5 * 1000;
    alice_cat.clean_cost = 1000;
    alice_cat.month_of_buy = month_of_buy;
    alice_cat.year_of_buy = year_of_buy;
    alice_cat.year_of_death = year_of_buy + 15;
    alice_cat.month_of_death = month_of_buy + 9;
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
    bob.deposit -= bob.transport;
    if (month == 12) {
        bob.food *= 1. + INFLATION_RATE;
        bob.transport *= 1. + INFLATION_RATE;
        bob.person_expense *= 1. + INFLATION_RATE;
    }
}


void bob_deposit(const int month) {
    if (month == 12) {
        bob.deposit *= 1. + DEPOSITE_RATE;
    }
}


void alice_pet_expense(const int current_year, const int current_month)
{
    static int has_cat = 0;
    
    if (current_year == 2025 && current_month == 3) {
        cat_init(current_year, current_month);
        alice.deposit -= alice_cat.cost;
        has_cat = 1;
    }

    if (has_cat && alice_cat.year_of_death == current_year && alice_cat.month_of_death == current_month) {
        has_cat = 0;
    }
    
    if (has_cat) {
        alice.deposit -= (alice_cat.food_cost + alice_cat.clean_cost);

        if (current_month == 12) {
            alice_cat.food_cost *= (1. + INFLATION_RATE);
            alice_cat.clean_cost *= (1. + INFLATION_RATE);
            alice_cat.cost *= (1. + INFLATION_RATE);
        }
    }
}


void bob_car(const int month, const int year) {
    if (month == bob_transport.month_of_purchasing && year == bob_transport.year_of_purchasing) {
        bob.deposit -= bob_transport.cost; 
    }
    if (year > bob_transport.year_of_purchasing ||
        (year == bob_transport.year_of_purchasing && month >= bob_transport.month_of_purchasing)) {

        bob.deposit -= bob_transport.benzin_per_month_cost + bob_transport.washing_per_month_cost
            + bob_transport.maintance_cost;

        if (month == 12) {
            bob.deposit -= bob_gelic.car_tax;
            bob_transport.benzin_per_month_cost *= 1. + INFLATION_RATE;
            bob_transport.washing_per_month_cost *= 1. + INFLATION_RATE;
            bob_transport.maintance_cost *= 1. + INFLATION_RATE;
        }
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
    alice.deposit -= alice.cat;
    if (month == 12) {
        alice.food *= 1. + INFLATION_RATE;
        alice.person_expense *= 1. + INFLATION_RATE;
        alice.deposit -= alice.cat;
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
        bob_car(current_year, current_month);

        alice_salary(current_month);
        alice_expense(current_month);
        alice_deposit(current_month);
        alice_mortgage_monthly_pay();
        alice_pet_expense(current_year, current_month);

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
    transport_init();
    pet_init();

    simulation(2024, 9, 30);

    print_result();

    return 0;
}
