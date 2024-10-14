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
    salary *= 1 + (inflation_rate / 100.0);
}


void mortgage(Person* p, Mortgage* m) {
    p->capital -= m->payment;
}


void rent(Person* p) {
    p->capital -= appartment_rent;
}


void life(Person* p) {
    p->capital -= (food_cost + service_cost + personal_cost);
    p->capital += salary;
}


void dacha(Person* p, Dates* d) {
    p->capital -= dacha_year_cost;
    if (d->current_month >= 6 && d->current_month <= 8) {
        p->capital += vegetables_cost;
    }
}

void cat(Person* p, Dates* d) {
    if (d->current_year <= d->cat_end_year) {
        p->capital -= cat_month_cost;
        if (d->current_month == 11) {
        p->capital -= cat_doctor_cost;
        }
        if (d->current_year == d->cat_end_year) {
            p->capital -= cat_funiral_cost;
            p->capital += cat_month_cost;
        }
    }
}


void deposit(Person* p) {
    p->deposit.value += p->capital;
    p->deposit.value *= 1 + (p->deposit.rate / 100.0) / 12;
    p->capital = 0;
}


void move_to_next_date(Dates* d) {
    if (d->current_month == 12) {
            life_inflation();
            d->current_year++;
            d->current_month = 1;
        } else {
            d->current_month++;
        }
}


void calculate_capital(Person* p, long components[], int components_count) {
    for (int i = 0; i < components_count; i++) {
        p->capital += components[i];
    }
}


void show(Person* p) {
    printf("%s\n", p->name);
    printf(" Balance is %.ld rub\n", p->capital);
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


struct Simulation {
    Person alice;
    Person bob;
    Mortgage mortgage;
    Dates dates;
};


struct Simulation init_simulation() {
    setlocale(LC_NUMERIC, "");

    Deposit deposit = {base_deposit, deposit_rate};
    Person alice = {"Alice", start_balance, deposit};
    Person bob = {"Bob", start_balance, deposit};
    Dates dates = init_dates(start_year, start_month, mortgage_duration, cat_life_expectancy);

    Mortgage mortgage = {appartment_cost, mortgage_duration, mortgage_rate};

    calculate_motrtage_payment(&mortgage);

    struct Simulation result = {alice, bob, mortgage, dates};
    return result;
}


void run_simulation(struct Simulation simulation) {
    while (!(simulation.dates.current_year == simulation.dates.end_year 
    && simulation.dates.current_month == simulation.dates.end_month)) {
        life(&simulation.alice);
        mortgage(&simulation.alice, &simulation.mortgage);
        dacha(&simulation.alice, &simulation.dates);

        life(&simulation.bob);
        rent(&simulation.bob);
        deposit(&simulation.bob);
        cat(&simulation.bob, &simulation.dates);

        move_to_next_date(&simulation.dates);
    }

    calculate_capital(&simulation.alice, (long[]) {appartment_cost, simulation.alice.deposit.value}, 2);
    calculate_capital(&simulation.bob, (long[]) {simulation.bob.deposit.value}, 1);
    show(&simulation.alice);
    show(&simulation.bob);
}


int main() {
    struct Simulation simulation = init_simulation();
    run_simulation(simulation);
    return 0;
}