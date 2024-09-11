#include <stdio.h>
#include <math.h>
#include <locale.h>

// ----------------------------------------------------  Settings  ---------------------------------------------------- //

const int base_capital = 1000 * 1000;
const int salary = 200 * 1000;

const int mortage_duration = 30;  // Duration of the mortgage in years.
const int mortage_rate = 16;

const int start_year = 2024;
const int start_month = 1;  // 1 for January, 2 for February, 3 for March, etc.

int appartment_cost = 13 * 1000 * 1000;
int appartment_rent_cost = 30 * 1000;

const int base_deposit = 0;

const int deposit_rate = 20;
const int inflation_rate = 9;

int food_cost = 20 * 1000;
int service_cost = 6 * 1000;
int personal_cost = 15 * 1000;

// ------------------------------------------------------  Types  ----------------------------------------------------- //

typedef struct {
    int cost;
    int period;
    int rate;
    int payment;
} Mortage;

typedef struct {
    long value;
    const int rate;
} Deposit;

typedef struct {
    const char name[20];
    long capital;
    int salary;
    Deposit deposit;
} Person;

typedef struct {
    const int start_year;
    const int start_month;
    const int end_year;
    const int end_month;
    int current_year;
    int current_month;
} Dates;

// -----------------------------------------------------  Structs  ----------------------------------------------------- //

struct Simulation {
    Person alice;
    Person bob;
    Mortage mortage;
    Dates dates;
};

// ----------------------------------------------------  Functions  ---------------------------------------------------- //

/**
 * @brief Converts a percentage value to its decimal representation.
 * 
 * @param value Percentage value as integer.
 * @return Decimal representation of the percentage.
 */
double percent(int value) 
{
    return value / 100.0;
}

/**
 * @brief Calculates the monthly mortgage payment.
 * 
 * The calculation is based on the mortgage cost, duration, and interest rate.
 * The result is stored in the `payment` field of the provided `Mortage` struct.
 * 
 * @param m Pointer to the `Mortage` struct for which to calculate the payment.
 */
void calculate_motrtage_payment(Mortage* m) 
{
    float monthly_rate = percent(m->rate) / 12;
    float total_rate = pow(1 + monthly_rate, m->period * 12);
    m->payment = m->cost * monthly_rate * total_rate / (total_rate - 1);
}

/**
 * @brief Initializes a `Dates` struct with start and end dates.
 * 
 * Calculates the end year and month based on the provided start year, start month, and duration.
 * 
 * @param start_year Year when the simulation starts.
 * @param start_month Month when the simulation starts (1 for January, 2 for February, 3 for March, etc).
 * @param duration Duration of the simulation in years.
 * @return Initialized `Dates` struct.
 */
Dates init_dates(int start_year, int start_month, int duration) 
{
    int end_year = start_year + duration;
    int end_month = 12;

    if (start_month - 1 == 0) {
        end_year--;
        end_month = 12;
    } else {
        end_month = start_month - 1;
    }

    Dates result = {start_year, start_month, end_year, end_month, start_year, start_month};
    return result;
}

/**
 * @brief Initializes a `Simulation` struct with default values.
 * 
 * Sets up Alice and Bob with their initial capital, salary, and deposit.
 * Initializes the mortgage parameters and calculates the monthly payment.
 * Sets up the simulation dates based on the provided start year, start month, and mortgage duration.
 * 
 * @return Initialized `Simulation` struct.
 */
struct Simulation init_simulation() 
{
    setlocale(LC_NUMERIC, "");

    Deposit deposit = {base_deposit, deposit_rate};
    Person alice = {"Alice", base_capital, salary, deposit};
    Person bob = {"Bob", base_capital, salary, deposit};
    Dates dates = init_dates(start_year, start_month, mortage_duration);

    Mortage mortage = {appartment_cost, mortage_duration, mortage_rate};

    calculate_motrtage_payment(&mortage);

    struct Simulation result = {alice, bob, mortage, dates};
    return result;
}

void add_inflation_to_goods() 
{
    appartment_cost *= 1 + percent(inflation_rate);
    appartment_rent_cost *= 1 + percent(inflation_rate);
    food_cost *= 1 + percent(inflation_rate);
    service_cost *= 1 + percent(inflation_rate);
    personal_cost *= 1 + percent(inflation_rate);
}

void add_inflation_to_salary(Person* p) 
{
    p->salary *= 1 + percent(inflation_rate);
}

void recieve_salary(Person* p) 
{
    p->capital += p->salary;
}

void pay_mortgage(Person* p, Mortage* m) 
{
    p->capital -= m->payment;
}

void pay_rent(Person* p) 
{
    p->capital -= appartment_rent_cost;
}

void pay_bills(Person* p) 
{
    p->capital -= (food_cost + service_cost + personal_cost);
}

/**
 * @brief Increases the value of a person's deposit.
 * 
 * Adds the person's current capital to their deposit, then applies the deposit interest rate.
 * The person's capital is then reset to 0.
 * 
 * @param p Pointer to the `Person` struct for which to increase the deposit.
 */
void increase_deposit(Person* p) 
{
    p->deposit.value += p->capital;
    p->deposit.value *= 1 + percent(p->deposit.rate) / 12;
    p->capital = 0;
}

void show_capital(Person* p) 
{
    printf("%s\n", p->name);
    printf(" Capital is %.ld rub\n", p->capital);
}

void move_to_next_year(Dates* d) 
{
    d->current_year++;
    d->current_month = 1;
}

void move_to_next_month(Dates* d) 
{
    d->current_month++;
}

/**
 * @brief Calculates the total capital of a person by summing up provided components.
 * 
 * @param p Pointer to the `Person` struct representing the person.
 * @param components Array of long integers representing different components of the capital.
 */
void calculate_capital(Person* p, long components[], int components_count) 
{
    for (int i = 0; i < components_count; i++) {
        p->capital += components[i];
    }
}

// ----------------------------------------------------  Simulation  --------------------------------------------------- //

/**
 * @brief Runs the financial simulation.
 * 
 * Simulates the financial situation of Alice (buying an apartment with a mortgage and investing)
 * and Bob (renting an apartment and investing) over the specified duration.
 * 
 * @param simulation Initialized `Simulation` struct containing all necessary data.
 */
void run_simulation(struct Simulation simulation) 
{
    while (simulation.dates.current_year <= simulation.dates.end_year) {
        // Calculate monthly income
        recieve_salary(&simulation.alice);
        recieve_salary(&simulation.bob);

        // Calculate monthly expenses
        pay_bills(&simulation.alice);
        pay_bills(&simulation.bob);
        
        // Account for housing costs
        pay_mortgage(&simulation.alice, &simulation.mortage);
        pay_rent(&simulation.bob);

        // Increase deposit value
        increase_deposit(&simulation.alice);
        increase_deposit(&simulation.bob);

        // Check if simulation is finished
        if (simulation.dates.current_year == simulation.dates.end_year 
            && simulation.dates.current_month == simulation.dates.end_month) {
            break;
        } else if (simulation.dates.current_month == 12) {
            // Apply inflation at the end of the year
            add_inflation_to_salary(&simulation.alice);
            add_inflation_to_salary(&simulation.bob);
            add_inflation_to_goods();

            // Move to the next year
            move_to_next_year(&simulation.dates);
        } else {
            // Move to the next month
            move_to_next_month(&simulation.dates);
        }
    }

    // Calculate final capital
    calculate_capital(&simulation.alice, (long[]) {appartment_cost, simulation.alice.deposit.value}, 2);
    calculate_capital(&simulation.bob, (long[]) {simulation.bob.deposit.value}, 1);

    // Display results
    show_capital(&simulation.alice);
    show_capital(&simulation.bob);
}


// -------------------------------------------------------  Main  ------------------------------------------------------ //

/**
 * @brief Main function of the program.
 * 
 * Initializes the simulation and runs it.
 * 
 * @return 0 if the program runs successfully.
 */
int main() 
{
    struct Simulation simulation = init_simulation();
    run_simulation(simulation);
    return 0;
}
