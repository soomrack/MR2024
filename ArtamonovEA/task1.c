#include <stdio.h>
#include <math.h>
#include <locale.h>
#include <stdlib.h>
#include <time.h>

// ----------------------------------------------------  Settings  ---------------------------------------------------- //

float inflation_rate = 9.0;

// ------------------------------------------------------  Types  ----------------------------------------------------- //

typedef long long int Money;

typedef struct {
    Money cost;
    int period;
    int rate;
    Money payment;
} Mortage;

typedef struct {
    Money value;
    int rate;
} Deposit;

typedef struct {
    Money appartment_cost;
    Money appartment_rent_cost;
    Money food_cost;
    Money service_cost;
    Money personal_cost;
} Goods;

typedef struct {
    char name[20];
    Money capital;
    Deposit deposit;
    Money salary;
    Money annual_bonus;
    int is_working;
    Goods goods;
} Person;

typedef struct {
    const int start_year;
    const int start_month;
    const int end_year;
    const int end_month;
    int current_year;
    int current_month;
    int alice_fired_in_months;
    int bob_fired_in_months;
} Dates;

typedef struct {
    Person alice;
    Person bob;
    Mortage mortage;
    Dates dates;
} Simulation;

// ----------------------------------------------------  Functions  ---------------------------------------------------- //

Mortage init_mortage(Money cost, int rate, int duration) 
{
    float monthly_rate = rate / 100 / 12;
    float total_rate = pow(1 + monthly_rate, duration * 12);
    Money payment = cost * monthly_rate * total_rate / (total_rate - 1);
    Mortage result = {cost, duration, rate, payment};
    return result;
}

Dates init_dates(int start_year, int start_month, int duration, int alice_fired_in_months, int bob_fired_in_months) 
{
    int end_year = start_year + duration;
    int end_month = 12;

    if (start_month - 1 == 0) {
        end_year--;
        end_month = 12;
    } else {
        end_month = start_month - 1;
    }

    Dates result = {
        start_year, start_month, 
        end_year, end_month, 
        start_year, start_month, 
        alice_fired_in_months, bob_fired_in_months
    };

    return result;
}

Simulation init_simulation() 
{
    setlocale(LC_NUMERIC, "");

    Deposit alice_deposit = {0, 24};
    Goods alice_goods = {15 * 1000 * 1000, 0, 15 * 1000, 8 * 1000, 17 * 1000};
    Person alice = {"Alice", 1000 * 1000, alice_deposit, 200 * 1000, 300 * 1000, 1};

    Deposit bob_deposit = {0, 22};
    Goods bob_goods = {0, 30 * 1000, 12 * 1000, 10 * 1000, 7 * 1000};
    Person bob = {"Bob", 700 * 1000, bob_deposit, 300 * 1000, 500 * 1000, 1};
    Dates dates = init_dates(2024, 1, 30, 57, 96);

    Mortage mortage = init_mortage(15 * 1000 * 1000, 30, 20);

    Simulation result = {alice, bob, mortage, dates};
    return result;
}

void add_inflation_to_goods(const int month, Goods* goods) 
{
    if (month == 12) {
        goods->appartment_cost *= 1 + inflation_rate / 100;
        goods->appartment_rent_cost *= 1 + inflation_rate /100;
        goods->food_cost *= 1 + inflation_rate / 100;
        goods->service_cost *= 1 + inflation_rate / 100;
        goods->personal_cost *= 1 + inflation_rate / 100;
    };
}

void salary(Person* person, int month) 
{   
    if (person->is_working == 0) {
        return;
    }

    person->capital += person->salary;

    if (month == 12) {
        person->capital += person->annual_bonus;
        person->salary *= 1 + inflation_rate / 100;
        person->annual_bonus *= 1 + inflation_rate / 100;
    };
}

void pay_mortgage(Person* person, Mortage* mortage) 
{
    person->capital -= mortage->payment;
}

void pay_bills(Person* person) 
{

    person->capital -= person->goods.food_cost;
    person->capital -= person->goods.service_cost;
    person->capital -= person->goods.personal_cost;
    person->capital -= person->goods.appartment_rent_cost;
}

void increase_deposit(Person* person) 
{
    person->deposit.value += person->capital;
    person->deposit.value *= 1 + person->deposit.rate / 100 / 12;
    person->capital = 0;
}

void show_capital(Person* person) 
{
    printf("%s\n", person->name);
    printf(" Capital is %.lld rub\n", person->capital);
}

void inflation_change() 
{
    int change_direction = arc4random() % 2 ? 1 : -1;
    int change_value = random() / (RAND_MAX / 6);
    inflation_rate *= (100 + change_direction * change_value) * 1.0 / 100;
}

void move_to_next_month(Simulation* simulation) 
{
    if (simulation->dates.current_month == 12) {
        simulation->dates.current_year++;
        simulation->dates.current_month = 1;
        inflation_change();

    } else {
        simulation->dates.current_month++;
    }

    simulation->dates.alice_fired_in_months--;
    simulation->dates.bob_fired_in_months--;
    
    if (simulation->dates.alice_fired_in_months == 0) {
        simulation->alice.is_working = 0;
    }

    if (simulation->dates.bob_fired_in_months == 0) {
        simulation->bob.is_working = 0;
    }
}

void calculate_capital(Person* p, long components[], int components_count) 
{
    for (int i = 0; i < components_count; i++) {
        p->capital += components[i];
    }
}

// ----------------------------------------------------  Simulation  --------------------------------------------------- //

void run_simulation(Simulation simulation) 
{
    while( !(
        simulation.dates.current_year == simulation.dates.end_year && 
        simulation.dates.current_month == simulation.dates.end_month
        ) ) {
        salary(&simulation.alice, simulation.dates.current_month);
        salary(&simulation.bob, simulation.dates.current_month);

        pay_mortgage(&simulation.alice, &simulation.mortage);
        pay_bills(&simulation.alice);
        pay_bills(&simulation.bob);

        increase_deposit(&simulation.alice);
        increase_deposit(&simulation.bob);

        add_inflation_to_goods(simulation.dates.current_month, &simulation.alice.goods);
        add_inflation_to_goods(simulation.dates.current_month, &simulation.bob.goods);

        move_to_next_month(&simulation);
    }

    calculate_capital(&simulation.alice, (long[]) {simulation.alice.goods.appartment_cost, simulation.alice.deposit.value}, 2);
    calculate_capital(&simulation.bob, (long[]) {simulation.bob.deposit.value}, 1);

    show_capital(&simulation.alice);
    show_capital(&simulation.bob);
}


// -------------------------------------------------------  Main  ------------------------------------------------------ //

int main() 
{
    srand(time(NULL));
    Simulation simulation = init_simulation();
    run_simulation(simulation);
    return 0;
}
