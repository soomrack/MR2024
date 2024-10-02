#include <stdio.h>
#include <math.h>
#include <locale.h>

unsigned long int start_balance = 1000000;
unsigned long int salary = 200000; 

int mortgage_duration = 30;
int mortgage_rate = 16;

int start_year = 2024;
int start_month = 9;

unsigned long int appartment_cost = 13000000;
unsigned long int appartment_rent = 30000;

int cat_month_cost = 1500;
int cat_doctor_cost = 3000;
int cat_life_expectancy = 16;
int cat_funiral_cost = 12000;

unsigned long int dacha_year_cost = 60000;
unsigned long int vegetables_cost = 25000;

unsigned long int base_deposit = 0;

int deposit_rate = 20;
int inflation_rate = 9; 

unsigned long int personal_cost = 15000; 
unsigned long int food_cost = 20000; 
unsigned long int service_cost = 6000; 


typedef struct {
    int start_year;
    int start_month;
    int end_year;
    int end_month;
    int current_year;
    int current_month;
    int cat_end_year;
} Dates;

typedef struct  {
    int cost;
    int period;
    int rate;
    int payment;
} Mortgage;

typedef struct  {
    long value;
    int rate;
} Deposit;

typedef struct {
    char name[20];
    long capital;
    int salary;
    Deposit deposit;
} Person;


void calculate_motrtage_payment(Mortgage* m) {
    float monthly_rate = (m->rate / 100.0) / 12;
    float total_rate = pow(1 + monthly_rate, m->period * 12);
    m->payment = m->cost * monthly_rate * total_rate / (total_rate - 1);
}


void life_inflation() {
    appartment_cost *= 1 + (inflation_rate / 100.0);
    appartment_rent *= 1 + (inflation_rate / 100.0);
    food_cost *= 1 + (inflation_rate / 100.0);
    service_cost *= 1 + (inflation_rate / 100.0);
    personal_cost *= 1 + (inflation_rate / 100.0);
    cat_month_cost *= 1 + (inflation_rate / 100.0);
    cat_doctor_cost *= 1 + (inflation_rate / 100.0);
    vegetables_cost *= 1 + (inflation_rate / 100.0);
    dacha_year_cost *= 1 + (inflation_rate / 100.0);
    cat_funiral_cost *= 1 + (inflation_rate / 100.0);
}


void salary_inflation(Person* p) {
    p->salary *= 1 + (inflation_rate / 100.0);
}


void recieve_salary(Person* p) {
    p->capital += p->salary;
}


void pay_mortgage(Person* p, Mortgage* m) {
    p->capital -= m->payment;
}


void pay_rent(Person* p) {
    p->capital -= appartment_rent;
}


void pay_bills(Person* p) {
    p->capital -= (food_cost + service_cost + personal_cost);
}


void pay_dacha(Person* p) {
    p->capital -= dacha_year_cost;
}

void vegetables_sell(Person* p) {
    p->capital += vegetables_cost;
}


void cat_expenses(Person* p) {
    p->capital -= cat_month_cost;
}


void cat_doctor_expenses(Person* p) {
    p->capital -= cat_doctor_cost;
}


void cat_funiral_expenses(Person* p) {
    p->capital -= cat_funiral_cost;
}


void increase_deposit(Person* p) {
    p->deposit.value += p->capital;
    p->deposit.value *= 1 + (p->deposit.rate / 100.0) / 12;
    p->capital = 0;
}


void show_capital(Person* p) {
    printf("%s\n", p->name);
    printf(" Balance is %.ld rub\n", p->capital);
}


void show_date(Dates* d) {
    printf("Current year is %d\n", d->current_year);
    printf("Current month is %d\n", d->current_month);
}


Dates init_dates(int start_year, int start_month, int duration, int cat_life_expectancy) {
    int end_year = start_year + duration;
    int cat_end_year = start_year + cat_life_expectancy;
    int end_month = 12;

    if (start_month - 1 == 0) {
        end_year--;
        end_month = 12;
    } else {
        end_month = start_month - 1;
    }

    Dates result = {start_year, start_month, end_year, end_month, start_year, start_month, cat_end_year};
    return result;
}


void move_to_next_year(Dates* d) {
    d->current_year++;
    d->current_month = 1;
}


void move_to_next_month(Dates* d) {
    d->current_month++;
}


void calculate_capital(Person* p, long components[], int components_count) {
    for (int i = 0; i < components_count; i++) {
        p->capital += components[i];
    }
}


struct Simulation {
    Person alice;
    Person bob;
    Mortgage mortgage;
    Dates dates;
};


struct Simulation init_simulation() {
    setlocale(LC_NUMERIC, "");

    Deposit deposit = {base_deposit, deposit_rate};
    Person alice = {"Alice", start_balance, salary, deposit};
    Person bob = {"Bob", start_balance, salary, deposit};
    Dates dates = init_dates(start_year, start_month, mortgage_duration, cat_life_expectancy);

    Mortgage mortgage = {appartment_cost, mortgage_duration, mortgage_rate};

    calculate_motrtage_payment(&mortgage);

    struct Simulation result = {alice, bob, mortgage, dates};
    return result;
}


void run_simulation(struct Simulation simulation) {
    while (simulation.dates.current_year <= simulation.dates.end_year && !(simulation.dates.current_year == simulation.dates.end_year && simulation.dates.current_month == simulation.dates.end_month)) {
        recieve_salary(&simulation.alice);
        pay_bills(&simulation.alice);
        pay_mortgage(&simulation.alice, &simulation.mortgage);
        increase_deposit(&simulation.alice);

        pay_dacha(&simulation.alice);
        if (simulation.dates.current_month >= 6 && simulation.dates.current_year <= 8) {
            vegetables_sell(&simulation.alice);
        }


        recieve_salary(&simulation.bob);
        pay_bills(&simulation.bob);
        pay_rent(&simulation.bob);
        increase_deposit(&simulation.bob);

        if (simulation.dates.current_year <= simulation.dates.cat_end_year) {
            cat_expenses(&simulation.bob);
            if (simulation.dates.current_month == 11) {
                cat_doctor_expenses(&simulation.bob);
            }
            if (simulation.dates.current_year == simulation.dates.cat_end_year) {
            cat_funiral_expenses(&simulation.bob);
            }
        }

        if (simulation.dates.current_month == 12) {
            salary_inflation(&simulation.alice);
            salary_inflation(&simulation.bob);
            life_inflation();
            move_to_next_year(&simulation.dates);
        } else {
            move_to_next_month(&simulation.dates);
        }
    }

    calculate_capital(&simulation.alice, (long[]) {appartment_cost, simulation.alice.deposit.value}, 2);
    calculate_capital(&simulation.bob, (long[]) {simulation.bob.deposit.value}, 1);
    show_date(&simulation.dates);
    show_capital(&simulation.alice);
    show_capital(&simulation.bob);
}


int main() {
    struct Simulation simulation = init_simulation();
    run_simulation(simulation);
    return 0;
}