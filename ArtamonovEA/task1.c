#include <stdio.h>
#include <math.h>
#include <locale.h>

// ----------------------------------------------------  Settings  ---------------------------------------------------- //

int base_capital = 1000000;  // Initial capital of both Alice and Bob.
int salary = 200000;  // Initial monthly salary of both Alice and Bob.

int mortage_duration = 30;  // Duration of the mortgage in years.
int mortage_rate = 16;  // Annual interest rate for the mortgage.

int start_year = 2024;  // Year when the simulation starts.
int start_month = 1;  // Month when the simulation starts (1 for January, 2 for February, 3 for March, etc).

int appartment_cost = 13000000;  // Total cost of the apartment.
int appartment_rent_cost = 30000;  // Monthly rent cost of an apartment.

int base_deposit = 0;  // Initial deposit amount for both Alice and Bob.

int deposit_rate = 20;  // Annual interest rate for the deposit.
int inflation_rate = 9;  // Annual inflation rate.

int food_cost = 20000;  // Monthly expenses for food.
int service_cost = 6000;  // Monthly expenses for services.
int personal_cost = 15000;  // Monthly expenses for personal needs.

// ------------------------------------------------------  Types  ----------------------------------------------------- //

/**
 * @brief Structure representing a mortgage.
 * 
 * Contains information about the mortgage cost, duration, interest rate, and monthly payment.
 */
typedef struct 
{
    int cost;
    int period;
    int rate;
    int payment;
} mortageType;

/**
 * @brief Structure representing a deposit account.
 * 
 * Contains information about the deposit value and annual interest rate.
 */
typedef struct 
{
    long value;
    int rate;
} depositType;

/**
 * @brief Structure representing a person.
 * 
 * Contains information about the person's name, capital, salary, and deposit account.
 */
typedef struct
{
    char name[20];
    long capital;
    int salary;
    depositType deposit;
} personType;

/**
 * @brief Structure for managing simulation dates.
 * 
 * Tracks the start and end year/month of the simulation, as well as the current year/month.
 */
typedef struct
{
    int start_year;
    int start_month;
    int end_year;
    int end_month;
    int current_year;
    int current_month;
} datesType;


// ----------------------------------------------------  Functions  ---------------------------------------------------- //

/**
 * @brief Converts a percentage value to its decimal representation.
 * 
 * @param value Percentage value as integer.
 * @return Decimal representation of the percentage.
 */
double percent(int value) {
    return value / 100.0;
}

/**
 * @brief Calculates the monthly mortgage payment.
 * 
 * The calculation is based on the mortgage cost, duration, and interest rate.
 * The result is stored in the `payment` field of the provided `mortageType` struct.
 * 
 * @param m Pointer to the `mortageType` struct for which to calculate the payment.
 */
void calculate_motrtage_payment(mortageType* m) {
    float monthly_rate = percent(m->rate) / 12;
    float total_rate = pow(1 + monthly_rate, m->period * 12);
    m->payment = m->cost * monthly_rate * total_rate / (total_rate - 1);
}

/**
 * @brief Applies inflation to the cost of goods.
 * 
 * Increases the values of `appartment_cost`, `appartment_rent_cost`, `food_cost`, 
 * `service_cost`, and `personal_cost` based on the `inflation_rate`.
 */
void add_inflation_to_goods() {
    appartment_cost *= 1 + percent(inflation_rate);
    appartment_rent_cost *= 1 + percent(inflation_rate);
    food_cost *= 1 + percent(inflation_rate);
    service_cost *= 1 + percent(inflation_rate);
    personal_cost *= 1 + percent(inflation_rate);
}

/**
 * @brief Applies inflation to a person's salary.
 * 
 * Increases the `salary` field of the provided `personType` struct based on the `inflation_rate`.
 * 
 * @param p Pointer to the `personType` struct for which to apply the inflation.
 */
void add_inflation_to_salary(personType* p) {
    p->salary *= 1 + percent(inflation_rate);
}

/**
 * @brief Adds a person's salary to their capital.
 * 
 * @param p Pointer to the `personType` struct representing the person receiving the salary.
 */
void recieve_salary(personType* p) {
    p->capital += p->salary;
}

/**
 * @brief Deducts the mortgage payment from a person's capital.
 * 
 * @param p Pointer to the `personType` struct representing the person paying the mortgage.
 * @param m Pointer to the `mortageType` struct representing the mortgage being paid.
 */
void pay_mortgage(personType* p, mortageType* m) {
    p->capital -= m->payment;
}

/**
 * @brief Deducts the apartment rent cost from a person's capital.
 * 
 * @param p Pointer to the `personType` struct representing the person paying the rent.
 */
void pay_rent(personType* p) {
    p->capital -= appartment_rent_cost;
}

/**
 * @brief Deducts the combined cost of food, services, and personal expenses from a person's capital.
 * 
 * @param p Pointer to the `personType` struct representing the person paying the bills.
 */
void pay_bills(personType* p) {
    p->capital -= (food_cost + service_cost + personal_cost);
}

/**
 * @brief Increases the value of a person's deposit.
 * 
 * Adds the person's current capital to their deposit, then applies the deposit interest rate.
 * The person's capital is then reset to 0.
 * 
 * @param p Pointer to the `personType` struct for which to increase the deposit.
 */
void increase_deposit(personType* p) {
    p->deposit.value += p->capital;
    p->deposit.value *= 1 + percent(p->deposit.rate) / 12;
    p->capital = 0;
}

/**
 * @brief Prints the capital of a person.
 * 
 * Displays the person's name and their current capital.
 * 
 * @param p Pointer to the `personType` struct for which to display the capital.
 */
void show_capital(personType* p) {
    printf("%s\n", p->name);
    printf(" Capital is %.ld rub\n", p->capital);
}

/**
 * @brief Initializes a `datesType` struct with start and end dates.
 * 
 * Calculates the end year and month based on the provided start year, start month, and duration.
 * 
 * @param start_year Year when the simulation starts.
 * @param start_month Month when the simulation starts (1 for January, 2 for February, 3 for March, etc).
 * @param duration Duration of the simulation in years.
 * @return Initialized `datesType` struct.
 */
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

/**
 * @brief Advances the simulation date to the next year.
 * 
 * Increments the current year and resets the current month to January (1).
 * 
 * @param d Pointer to the `datesType` struct representing the simulation date.
 */
void move_to_next_year(datesType* d) {
    d->current_year++;
    d->current_month = 1;
}

/**
 * @brief Advances the simulation date to the next month.
 * 
 * Increments the current month.
 * 
 * @param d Pointer to the `datesType` struct representing the simulation date.
 */
void move_to_next_month(datesType* d) {
    d->current_month++;
}

/**
 * @brief Calculates the total capital of a person by summing up provided components.
 * 
 * @param p Pointer to the `personType` struct representing the person.
 * @param components Array of long integers representing different components of the capital.
 */
void calculate_capital(personType* p, long components[], int components_count) {
    for (int i = 0; i < components_count; i++) {
        p->capital += components[i];
    }
}



// -----------------------------------------------------  Structs  ----------------------------------------------------- //

/**
 * @brief Structure holding all data for the simulation.
 */
struct Simulation {
    personType alice;
    personType bob;
    mortageType mortage;
    datesType dates;
};

/**
 * @brief Initializes a `Simulation` struct with default values.
 * 
 * Sets up Alice and Bob with their initial capital, salary, and deposit.
 * Initializes the mortgage parameters and calculates the monthly payment.
 * Sets up the simulation dates based on the provided start year, start month, and mortgage duration.
 * 
 * @return Initialized `Simulation` struct.
 */
struct Simulation init_simulation() {
    setlocale(LC_NUMERIC, "");

    depositType deposit = {base_deposit, deposit_rate};
    personType alice = {"Alice", base_capital, salary, deposit};
    personType bob = {"Bob", base_capital, salary, deposit};
    datesType dates = init_dates(start_year, start_month, mortage_duration);

    mortageType mortage = {appartment_cost, mortage_duration, mortage_rate};

    calculate_motrtage_payment(&mortage);

    struct Simulation result = {alice, bob, mortage, dates};
    return result;
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
void run_simulation(struct Simulation simulation) {
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
        if (simulation.dates.current_year == simulation.dates.end_year && simulation.dates.current_month == simulation.dates.end_month) {
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
int main() {
    struct Simulation simulation = init_simulation();
    run_simulation(simulation);
    return 0;
}
