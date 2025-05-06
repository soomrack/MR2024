#include <stdio.h>
#include <stdint.h>

typedef struct {
    int year;
    int month;
} Date;

typedef int64_t Money;  // Rub

typedef struct {
    Money capital;
    double rate;
} Deposit;

typedef struct {
    Money amount;
    unsigned remaining_month;
    double rate;
} Mortgage;

typedef struct {
    Money cash_on_hand;  // month cash
    Money salary;
    Deposit deposit;
    Money property_value;
    Mortgage mortgage;
    Money rent_cost;
    Money food_cost;
    Money service_cost;
    Money personal_expenses;
} Person;

const Money flat_cost = 15 * 1000 * 1000;
const double inflation = 0.08;
const double deposit_rate = 0.2;
const int duration = 30; // years

void alice_init(Person* person)
{
    *person = (Person){
        .cash_on_hand = 0,
        .salary = 300 * 1000,
        .property_value = flat_cost,
        .deposit = {
            .capital = 0,
            .rate = deposit_rate
        },
        .mortgage = {
            .amount = flat_cost - (1000 * 1000),
            .remaining_month = 12 * duration,
            .rate = 0.17
        },
        .rent_cost = 0,
        .food_cost = 20 * 1000,
        .service_cost = 10 * 1000,
        .personal_expenses = 15 * 1000
    };
}

void bob_init(Person* person)
{
    *person = (Person){
        .cash_on_hand = 0,
        .salary = 300 * 1000,
        .property_value = 0,
        .deposit = {
            .capital = 1000 * 1000,
            .rate = deposit_rate
        },
        .mortgage = {
            .amount = 0,
            .remaining_month = 0,
            .rate = 0
        },
        .rent_cost = 30 * 1000,
        .food_cost = 30 * 1000,
        .service_cost = 10 * 1000,
        .personal_expenses = 15 * 1000
    };
}

void alice_salary(Person* alice, const Date* date)
{
    if(date->year == 2040 && date->month == 5)
    {
        alice->salary += (Money)((double)alice->salary * 0.25);  // alice promotion
    }

    alice->cash_on_hand += alice->salary;

    alice->salary += (Money)((double)alice->salary * inflation / 12.0);
}

void bob_salary(Person* bob, const Date* date)
{
    bob->cash_on_hand += bob->salary;

    bob->salary += (Money)((double)bob->salary * inflation / 12.0);
}

void alice_pay_mortgage(Person* person, const Date* date)
{
    if(person->mortgage.amount != 0)
    {
        person->cash_on_hand -= (Money)((double)person->mortgage.amount * person->mortgage.rate / 12.0);  // mortgage percentage payment
        Money mortgage_payment = person->mortgage.amount / person->mortgage.remaining_month;
        person->cash_on_hand -= mortgage_payment;
        person->mortgage.amount -= mortgage_payment;
        person->mortgage.remaining_month--;
    }
}

void rent(Person* person, const Date* date)
{
    person->cash_on_hand -= person->rent_cost;

    person->rent_cost += (Money)((double)person->rent_cost * inflation / 12.0);
}

void bills(Person* person, const Date* date)
{
    person->cash_on_hand -= person->food_cost;
    person->cash_on_hand -= person->service_cost;
    person->cash_on_hand -= person->personal_expenses;

    person->food_cost += (Money)((double)person->food_cost * inflation / 12.0);
    person->service_cost += (Money)((double)person->service_cost * inflation / 12.0);
    person->personal_expenses += (Money)((double)person->personal_expenses * inflation / 12.0);
}

void deposit(Person* person, const Date* date)
{
    person->deposit.capital += person->cash_on_hand;
    person->cash_on_hand = 0;

    person->deposit.capital += (Money)((double)person->deposit.capital * person->deposit.rate / 12.0);
}

void property_value_increase(Person* person, const Date* date)
{
    person->property_value += (Money)((double)person->property_value * inflation / 12.0);
}

int is_date_equal(const Date* first, const Date* second)
{
    return (first->year == second->year) && (first->month == second->month);
}

void date_month_increment(Date* date)
{
    date->month++;
    if(date->month == 13)
    {
        date->month = 1;
        date->year++;
    }
}

void simulation(Person* alice, Person* bob)
{
    Date date_current = {.year = 2024, .month = 9}; // starting date
    Date date_end = date_current;
    date_end.year += duration;

    while(!is_date_equal(&date_current, &date_end))
    {
        alice_salary(alice, &date_current);
        alice_pay_mortgage(alice, &date_current);
        bills(alice, &date_current);
        deposit(alice, &date_current);
        property_value_increase(alice, &date_current);

        bob_salary(bob, &date_current);
        bills(bob, &date_current);
        rent(bob, &date_current);
        deposit(bob, &date_current);

        date_month_increment(&date_current);
    }
}

void print_comparison(const Person* alice, const Person* bob)
{
    if((alice->property_value + alice->deposit.capital) == bob->deposit.capital)
    {
        printf("Alice and Bob funds are equal: %ld\n", bob->deposit.capital);
    }
    else if((alice->property_value + alice->deposit.capital) > bob->deposit.capital)
    {
        printf("Alice funds: %ld are higher than Bob's %ld\n", alice->property_value + alice->deposit.capital, bob->deposit.capital);
    }
    else
    {
        printf("Alice funds: %ld are lower than Bob's %ld\n", alice->property_value + alice->deposit.capital, bob->deposit.capital);
    }
}

int main()
{
    Person alice, bob;
    alice_init(&alice);
    bob_init(&bob);

    simulation(&alice, &bob);

    print_comparison(&alice, &bob);

    return 0;
}
