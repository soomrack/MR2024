#include <stdio.h>
#include <math.h>
#include <locale.h>
#include <string.h>

typedef long long int Money;
const double INFLATION_RATE = 0.0075; // 0.75% per month (approx. 9% annual inflation)
const double DEPOSIT_RATE = 0.2;   // Annual deposit interest rate (20% per year)

typedef struct {
    char name[10];
    Money capital;
    Money salary;
    Money food_cost;
    Money service_cost;
    Money personal_cost;
    Money rent_cost;
    Money deposit;
} Person;

typedef struct {
    int year_start;
    int month_start;
    int year_end;
    int month_end;
    Money credit_cost;
    double credit_rate;
    Money credit_payment;
} Credit;

Person alice;
Person bob;
Credit alice_apartment_mortgage;

// Calculate monthly credit payment (mortgage or loan)
double calc_credit_payment(Credit c) {
    Money cost = c.credit_cost;
    double rate = c.credit_rate;
    int duration = (c.year_end * 12 + c.month_end) - (c.year_start * 12 + c.month_start); // Calculate loan term in months

    double month_rate = rate / 12;                                      // Monthly interest rate
    double whole_rate = pow((1 + month_rate), duration);                // Total rate
    double payment = cost * month_rate * whole_rate / (whole_rate - 1); // Monthly payment
    return payment;
}

void alice_init() {
    strcpy(alice.name, "Alice");
    alice.capital = 1000 * 1000 * 100;    // Initial capital in kopecks
    alice.salary = 200 * 1000 * 100;      // Monthly salary in kopecks
    alice.food_cost = 15 * 1000 * 100;    // Monthly food expenses
    alice.service_cost = 8 * 1000 * 100;  // Monthly service expenses
    alice.personal_cost = 17 * 1000 * 100; // Monthly personal expenses
    alice.deposit = 0;                    // Initial deposit
    alice.rent_cost = 0;                  // Alice doesn't pay rent (has a mortgage)
}

void bob_init() {
    strcpy(bob.name, "Bob");
    bob.capital = 1000 * 1000 * 100;    // Initial capital in kopecks
    bob.salary = 200 * 1000 * 100;      // Monthly salary in kopecks
    bob.food_cost = 15 * 1000 * 100;    // Monthly food expenses
    bob.service_cost = 8 * 1000 * 100;  // Monthly service expenses
    bob.personal_cost = 17 * 1000 * 100; // Monthly personal expenses
    bob.deposit = 0;                    // Initial deposit
    bob.rent_cost = 30 * 1000 * 100;    // Monthly rent in kopecks
}

void alice_mortgage_init() {
    alice_apartment_mortgage.year_start = 2024;
    alice_apartment_mortgage.month_start = 9;
    alice_apartment_mortgage.year_end = 2054;
    alice_apartment_mortgage.month_end = 9;
    alice_apartment_mortgage.credit_cost = 13 * 1000 * 1000 * 100; // Mortgage cost in kopecks
    alice_apartment_mortgage.credit_rate = 0.16; // Annual mortgage interest rate (16%)
    alice_apartment_mortgage.credit_payment = calc_credit_payment(alice_apartment_mortgage);
}

// Manage salary adjustments (apply inflation every 12 months)
void manage_salary(Person* p, int month) {
    if (month == 1) { // Adjust salary annually (in January)
        p->salary *= 1 + INFLATION_RATE * 12; // Apply annual salary inflation
    }
    p->capital += p->salary; // Add salary to capital
}

// Manage regular monthly expenses
void manage_expenses(Person* p) {
    p->capital -= (p->food_cost + p->service_cost + p->personal_cost);
}

// Manage rent payments (with monthly inflation)
void manage_rent(Person* p) {
    p->capital -= p->rent_cost;
}

// Handle deposit growth based on monthly interest rate
void manage_deposit(Person* p) {
    if (p->capital > 0) {
        p->deposit += p->capital; // Move leftover capital into the deposit
        p->capital = 0;           // Clear capital
    }
    p->deposit *= 1 + DEPOSIT_RATE / 12; // Apply monthly interest to deposits
}

// Manage mortgage payments for Alice
void manage_credit(Person* p, Credit* c, int current_month, int current_year) {
    if (current_year >= c->year_start && current_year <= c->year_end) {
        if (current_year == c->year_end && current_month > c->month_end) {
            return; // No payment after mortgage term ends
        }
        p->capital -= c->credit_payment; // Subtract monthly mortgage payment
    }
}

// Print the final capital and deposit of a person
void print_capital(Person* p) {
    setlocale(LC_NUMERIC, ""); // Enable locale for number formatting

    printf("%s\n", p->name);
    printf("Final capital: %'.lld Rub\n", p->capital / 100);   // Capital in Rubles
    printf("Final deposit: %'.lld Rub\n\n", p->deposit / 100); // Deposit in Rubles
}

// Simulation of financial operations over a period of 30 years
void simulation(int month, int year) {
    int current_year = year;
    int current_month = month;

    while (!(year == (current_year + 30) && month == current_month)) {
        manage_salary(&alice, month);
        manage_salary(&bob, month);

        manage_expenses(&alice);
        manage_expenses(&bob);

        manage_credit(&alice, &alice_apartment_mortgage, current_month, current_year);

        manage_rent(&bob);

        manage_deposit(&alice);
        manage_deposit(&bob);

        // Move to the next month
        ++month;

        if (month == 13) { // Move to the next year
            month = 1;
            ++year;
        }
    }
}

int main() {
    alice_init();
    bob_init();
    alice_mortgage_init();

    simulation(9, 2024); // Start simulation from September 2024

    print_capital(&alice);
    print_capital(&bob);

    return 0;
}