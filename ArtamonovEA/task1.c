#include <stdio.h>
#include <math.h>
#include <locale.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


float inflation_rate = 9.0;
int duration = 30;

// ------------------------------------------------------  Types  ----------------------------------------------------- //

typedef long long int Money;

typedef struct {
    int year;
    int month;
} Date;

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
    Money rent_cost;
    Money food_cost;
    Money service_cost;
    Money personal_cost;
} Bills;

typedef struct {
    char name[20];
    Money capital;
    Deposit deposit;
    Mortage mortage;
    Money appartment_cost;
    Money salary;
    Money annual_bonus;
    Bills bills;
} Person;

typedef struct {
    Person person;
    Date start_date;
    Date current_date;
    Date end_date;
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


Simulation init_alice_simulation() 
{
    Bills bills = (Bills){
        .rent_cost = 0, 
        .food_cost = 20 * 1000, 
        .service_cost = 30 * 1000, 
        .personal_cost = 15 * 1000
    };

    Mortage mortage = init_mortage(15 * 1000 * 1000, 20, 30);

    Person person = (Person){
        .name = "Alice", 
        .capital = 1000 * 1000, 
        .deposit = (Deposit){
            .value = 0, 
            .rate = 24
        }, 
        .mortage = mortage,
        .bills = (Bills){
            .rent_cost = 0,
            .food_cost = 15 * 1000,
            .personal_cost = 7 * 1000,
            .service_cost = 10 * 1000
        },
        .salary = 200 * 1000,
        .annual_bonus = 1000 * 1000,
        .appartment_cost = 13 * 1000 * 1000
    };

    Date start = (Date) {
        .year = 2024,
        .month = 9
    };

    Date end = (Date) {
        .year = start.year + duration,
        .month = start.month - 1
    };

    if (end.month == 0) {
        end.year--;
        end.month = 12;
    }

    Simulation result = (Simulation){
        .person = person,
        .start_date = start,
        .current_date = start,
        .end_date = end
    };
    return result;
}

Simulation init_bob_simulation() 
{
    Person person = (Person){
        .name = "Bob", 
        .capital = 700 * 1000, 
        .deposit = (Deposit){
            .value = 0, 
            .rate = 22
        },
        .mortage = (Mortage){
            .cost = 0, 
            .period = 0, 
            .rate = 0, 
            .payment = 0
        },
        .bills = (Bills){
            .rent_cost = 30 * 1000,
            .food_cost = 15 * 1000,
            .personal_cost = 7 * 1000,
            .service_cost = 10 * 1000
        },
        .salary = 500 * 1000,
        .annual_bonus = 1000 * 1000,
        .appartment_cost = 0
    };

    Date start = (Date) {
        .year = 2024,
        .month = 9
    };

    Date end = (Date) {
        .year = start.year + duration,
        .month = start.month - 1
    };

    if (end.month == 0) {
        end.year--;
        end.month = 12;
    }

    Simulation result = (Simulation){
        .person = person,
        .start_date = start,
        .current_date = start,
        .end_date = end
    };
    return result;
}


void add_inflation_to_bills(const int month, Bills* bills) 
{
    if (month == 12) {
        bills->rent_cost *= 1 + inflation_rate /100;
        bills->food_cost *= 1 + inflation_rate / 100;
        bills->service_cost *= 1 + inflation_rate / 100;
        bills->personal_cost *= 1 + inflation_rate / 100;
    };
}


void salary(Person* person, int current_month) 
{   
    person->capital += person->salary;

    if (current_month == 12) {
        person->capital += person->annual_bonus;
        person->salary *= 1 + inflation_rate / 100;
        person->annual_bonus *= 1 + inflation_rate / 100;
    };
}


void pay_mortgage(Person* person) 
{
    person->capital -= person->mortage.payment;
}


void pay_bills(Person* person) 
{

    person->capital -= person->bills.food_cost;
    person->capital -= person->bills.service_cost;
    person->capital -= person->bills.personal_cost;
    person->capital -= person->bills.rent_cost;
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


void have_cat(Simulation* simulation) {
    if (simulation->current_date.year == 2028 && simulation->current_date.month == 9) {
        simulation->person.capital -= 100 * 1000;
        return;
    }

    if (simulation->current_date.year < 2030 
                && simulation->current_date.month < 9) {
        simulation->person.capital -= 10 * 1000;
    }
}


void have_dog(Simulation* simulation) {
    if (simulation->current_date.year == 2032 && simulation->current_date.month == 5) {
        simulation->person.capital -= 30 * 1000;
        return;
    }

    if (simulation->current_date.year < 2040 
                && simulation->current_date.month < 12) {
        simulation->person.capital += 15 * 1000;
    }
}


void move_to_next_month(Simulation* simulation) 
{
    if (simulation->current_date.month == 12) {
        simulation->current_date.year++;
        simulation->current_date.month = 1;
        inflation_change();

    } else {
        simulation->current_date.month++;
    }
}


void calculate_capital(Person* person) 
{
    person->capital += person->deposit.value;
    person->capital += person->appartment_cost;
}


// ----------------------------------------------------  Simulation  --------------------------------------------------- //


void run_simulation(Simulation simulation) 
{
    while( !(simulation.current_date.year == simulation.end_date.year 
            && simulation.current_date.month  == simulation.end_date.month) ) {

        salary(&simulation.person, simulation.current_date.month);

        pay_mortgage(&simulation.person);
        pay_bills(&simulation.person);

        if (strcmp(simulation.person.name, "Alice") != 0) {
            have_cat(&simulation);
            have_dog(&simulation);
        }

        increase_deposit(&simulation.person);

        add_inflation_to_bills(simulation.current_date.month, &simulation.person.bills);

        move_to_next_month(&simulation);
    }

    calculate_capital(&simulation.person);

    show_capital(&simulation.person);
}


// -------------------------------------------------------  Main  ------------------------------------------------------ //

int main() 
{
    srand(time(NULL));
    setlocale(LC_NUMERIC, "");

    Simulation alice_simulation = init_alice_simulation();

    run_simulation(alice_simulation);

    Simulation bob_simulation = init_alice_simulation();

    run_simulation(bob_simulation);

    return 0;
}
