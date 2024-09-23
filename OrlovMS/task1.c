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
} Mortage;


typedef enum {
    ALICE,
    BOB
} Id;


typedef struct {
    Id person_id;

    Money money;  // month cash
    Money salary;
    Deposit deposit;

    Money estate_cost;

    Mortage mortage;

    Money rent_cost;

    Money food_cost;
    Money service_cost;
    Money personel_cost;
} Person;


const Money flat_cost = 15 * 1000 * 1000;
const double inflation = 0.08;
const double deposit_rate = 0.2;
const int duration = 30; // years


void alice_init(Person* person)
{
    *person = (Person){
        .person_id = ALICE,
        .money = 0,
        .salary = 300 * 1000,
        .estate_cost = flat_cost,
        .deposit = {
            .capital = 0,
            .rate = deposit_rate
        },
        .mortage = {
            .amount = flat_cost - (1000 * 1000),
            .remaining_month = 12 * duration,
            .rate = 0.17
        },
        .rent_cost = 0,
        .food_cost = 20 * 1000,
        .service_cost = 10 * 1000,
        .personel_cost = 15 * 1000
    };
}


void bob_init(Person* person)
{
    *person = (Person){
        .person_id = BOB,
        .money = 0,
        .salary = 300 * 1000,
        .estate_cost = 0,
        .deposit = {
            .capital = 1000 * 1000,
            .rate = deposit_rate
        },
        .mortage = {
            .amount = 0,
            .remaining_month = 0,
            .rate = 0
        },
        .rent_cost = 30 * 1000,
        .food_cost = 30 * 1000,
        .service_cost = 10 * 1000,
        .personel_cost = 15 * 1000
    };
}


void add_salary(Person* person, const Date* date)
{
    person->money += person->salary;
}


void pay_mortage(Person* person, const Date* date)
{
    if(person->mortage.amount != 0)
    {
        person->money -= (Money)((double)person->mortage.amount * person->mortage.rate / 12.0);  // mortage percentage payment
        Money mortage_payment = person->mortage.amount / person->mortage.remaining_month;
        person->money -= mortage_payment;
        person->mortage.amount -= mortage_payment;
        person->mortage.remaining_month--;
    }
}


void pay_rent(Person* person, const Date* date)
{
    person->money -= person->rent_cost;
}


void pay_bills(Person* person, const Date* date)
{
    person->money -= person->food_cost;
    person->money -= person->service_cost;
    person->money -= person->personel_cost;
}


void add_deposit(Person* person, const Date* date)
{
    person->deposit.capital += person->money;
    person->money = 0;
}


void deposit_growth(Person* person, const Date* date)
{
    person->deposit.capital += (Money)((double)person->deposit.capital * person->deposit.rate / 12.0);
}


void index_salary(Person* person, const Date* date)
{
    person->salary += (Money)((double)person->salary * inflation / 12.0);
}


void bills_inflation(Person* person, const Date* date)
{
    person->food_cost += (Money)((double)person->food_cost * inflation / 12.0);
    person->service_cost += (Money)((double)person->service_cost * inflation / 12.0);
    person->personel_cost += (Money)((double)person->personel_cost * inflation / 12.0);
}


void rent_rise(Person* person, const Date* date)
{
    person->rent_cost += (Money)((double)person->rent_cost * inflation / 12.0);
}


void estate_cost_rise(Person* person, const Date* date)
{
    person->estate_cost += (Money)((double)person->estate_cost * inflation / 12.0);
}


void additional_conditions(Person* person, const Date* date)
{
    if(person->person_id == ALICE)
    {
        if(date->year == 2040 && date->month == 5)
        {
            person->salary += (Money)((double)person->salary * 0.25);  // alice promotion
        }
    }
    else
    {
        if(date->year == 2032 && date->month == 2)
        {
            person->deposit.capital -= 700 * 1000;  // bob buys car
        }
    }
}


void month_operations(Person* person, const Date* date)
{
    additional_conditions(person, date);

    //person operations with money
    add_salary(person, date);
    pay_mortage(person, date);
    pay_rent(person, date);
    pay_bills(person, date);
    add_deposit(person, date);

    //processes
    deposit_growth(person, date);
    index_salary(person, date);
    bills_inflation(person, date);
    rent_rise(person, date);
    estate_cost_rise(person, date);
}


int is_date_equal(const Date* first, const Date* second)
{
    return (first->year == second->year) && (first->month == second->month);
}


void date_increment(Date* date)
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
        month_operations(alice, &date_current);
        month_operations(bob, &date_current);

        date_increment(&date_current);
    }
}


void print_comparison(const Person* alice, const Person* bob)
{
    if((alice->estate_cost + alice->deposit.capital) == bob->deposit.capital)
    {
        printf("Alice and Bob funds are equal: %ld\n", bob->deposit.capital);
    }
    else if((alice->estate_cost + alice->deposit.capital) > bob->deposit.capital)
    {
        printf("Alice funds: %ld are higher than Bob's %ld\n", alice->estate_cost + alice->deposit.capital, bob->deposit.capital);
    }
    else
    {
        printf("Alice funds: %ld are lower than Bob's %ld\n", alice->estate_cost + alice->deposit.capital, bob->deposit.capital);
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
