#include <stdio.h>
#include <math.h>
#include <locale.h>

int start_balance = 1000000;
int salary = 200000; 

int mortage_duration = 30;
int mortage_rate = 16;

int start_year = 2024;
int start_month = 1;

int appartment_cost = 13000000;
int appartment_rent = 30000;

int base_deposit = 0; 

int deposit_rate = 20;
int inflation_rate = 9; 

int personal_cost = 15000; 
int food_cost = 20000; 
int service_cost = 6000; 

typedef struct
{
    int start_year;
    int start_month;
    int end_year;
    int end_month;
    int current_year;
    int current_month;
} datesType;

typedef struct 
{
    int cost;
    int period;
    int rate;
    int payment;
} mortageType;

typedef struct 
{
    long value;
    int rate;
} depositType;

typedef struct
{
    char name[20];
    long capital;
    int salary;
    depositType deposit;
} personType;

double percent(int value) {
    return value / 100.0;
}

void calculate_motrtage_payment(mortageType* m) {
    float monthly_rate = percent(m->rate) / 12;
    float total_rate = pow(1 + monthly_rate, m->period * 12);
    m->payment = m->cost * monthly_rate * total_rate / (total_rate - 1);
}

void life_inflation() {
    appartment_cost *= 1 + percent(inflation_rate);
    appartment_rent *= 1 + percent(inflation_rate);
    food_cost *= 1 + percent(inflation_rate);
    service_cost *= 1 + percent(inflation_rate);
    personal_cost *= 1 + percent(inflation_rate);
}

void salary_inflation(personType* p) {
    p->salary *= 1 + percent(inflation_rate);
}

void recieve_salary(personType* p) {
    p->capital += p->salary;
}

void pay_mortgage(personType* p, mortageType* m) {
    p->capital -= m->payment;
}

void pay_rent(personType* p) {
    p->capital -= appartment_rent;
}

void pay_bills(personType* p) {
    p->capital -= (food_cost + service_cost + personal_cost);
}

void increase_deposit(personType* p) {
    p->deposit.value += p->capital;
    p->deposit.value *= 1 + percent(p->deposit.rate) / 12;
    p->capital = 0;
}

void show_capital(personType* p) {
    printf("%s\n", p->name);
    printf(" Balance is %.ld rub\n", p->capital);
}

datesType init_dates(int start_year, int start_month, int duration) {
    int end_year = start_year + duration;
    int end_month = 12;

    if (start_month - 1 == 0) {
        end_year--;
        end_month = 12;
    } else {
        end_month = start_month - 1;
    }

    datesType result = {start_year, start_month, end_year, end_month, start_year, start_month};
    return result;
}

void move_to_next_year(datesType* d) {
    d->current_year++;
    d->current_month = 1;
}

void move_to_next_month(datesType* d) {
    d->current_month++;
}

void calculate_capital(personType* p, long components[], int components_count) {
    for (int i = 0; i < components_count; i++) {
        p->capital += components[i];
    }
}

struct Simulation {
    personType alice;
    personType bob;
    mortageType mortage;
    datesType dates;
};

struct Simulation init_simulation() {
    setlocale(LC_NUMERIC, "");

    depositType deposit = {base_deposit, deposit_rate};
    personType alice = {"Alice", start_balance, salary, deposit};
    personType bob = {"Bob", start_balance, salary, deposit};
    datesType dates = init_dates(start_year, start_month, mortage_duration);

    mortageType mortage = {appartment_cost, mortage_duration, mortage_rate};

    calculate_motrtage_payment(&mortage);

    struct Simulation result = {alice, bob, mortage, dates};
    return result;
}

void run_simulation(struct Simulation simulation) {
    while (simulation.dates.current_year <= simulation.dates.end_year) {
        recieve_salary(&simulation.alice);
        recieve_salary(&simulation.bob);

        pay_bills(&simulation.alice);
        pay_bills(&simulation.bob);
        
        pay_mortgage(&simulation.alice, &simulation.mortage);
        pay_rent(&simulation.bob);

        increase_deposit(&simulation.alice);
        increase_deposit(&simulation.bob);

        if (simulation.dates.current_year == simulation.dates.end_year && simulation.dates.current_month == simulation.dates.end_month) {
            break;
        } else if (simulation.dates.current_month == 12) {
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
    show_capital(&simulation.alice);
    show_capital(&simulation.bob);
}

int main() {
    struct Simulation simulation = init_simulation();
    run_simulation(simulation);
    return 0;
}