#include <stdio.h>
#include <stdint.h>

#define ALICE_CAPITAL 1000*1000
#define BOB_CAPITAL 1000*1000

#define FLAT_COST 15*1000*1000
#define MORTAGE_DURATION 30 // years
#define MORTAGE_RATE 0.17

#define RENT_COST 30*1000

#define ALICE_SALARY 300*1000
#define BOB_SALARY 300*1000

#define FOOD_COST 20*1000
#define SERVICE_COST 10*1000
#define PERSONEL_COST 15*1000

#define INFLATION 0.08

#define DEPOSIT_RATE 0.2

#define YEAR_START 2024
#define MONTH_START 9
#define DURATION 30 // years

typedef struct
{
    int year;
    int month;
} Date;

typedef int64_t Money; // Rub

typedef struct
{
    Money capital;
    double rate;
} Deposit;

typedef struct
{
    Money amount;
    uint16_t remaining_month;
    double rate;
} Mortage;

typedef struct
{
    Money money; // month cash
    Money salary;
    Deposit deposit;

    Money estate_cost;

    Mortage mortage;

    Money rent_cost;

    Money food_cost;
    Money service_cost;
    Money personel_cost;
} Person;


void alice_init(Person* person)
{
    *person = (Person){
        .money = 0,
        .salary = ALICE_SALARY,
        .estate_cost = FLAT_COST,
        .deposit = {
            .capital = 0,
            .rate = DEPOSIT_RATE
        },
        .mortage = {
            .amount = FLAT_COST - ALICE_CAPITAL,
            .remaining_month = 12 * DURATION,
            .rate = MORTAGE_RATE
        },
        .rent_cost = 0,
        .food_cost = FOOD_COST,
        .service_cost = SERVICE_COST,
        .personel_cost = PERSONEL_COST
    };
}


void bob_init(Person* person)
{
    *person = (Person){
        .money = 0,
        .salary = BOB_SALARY,
        .estate_cost = 0,
        .deposit = {
            .capital = BOB_CAPITAL,
            .rate = DEPOSIT_RATE
        },
        .mortage = {
            .amount = 0,
            .remaining_month = 0,
            .rate = 0
        },
        .rent_cost = RENT_COST,
        .food_cost = FOOD_COST,
        .service_cost = SERVICE_COST,
        .personel_cost = PERSONEL_COST
    };
}


void add_salary(Person* person, Date* date)
{
    person->money += person->salary;
}


void pay_mortage(Person* person, Date* date)
{
    if(person->mortage.amount != 0)
    {
        person->money -= (uint64_t)((double)person->mortage.amount * person->mortage.rate / 12.0); //mortage percentage payment
        uint64_t mortage_payment = person->mortage.amount / person->mortage.remaining_month;
        person->money -= mortage_payment;
        person->mortage.amount -= mortage_payment;
        person->mortage.remaining_month--;
    }
}


void pay_rent(Person* person, Date* date)
{
    person->money -= person->rent_cost;
}


void pay_bills(Person* person, Date* date)
{
    person->money -= person->food_cost;
    person->money -= person->service_cost;
    person->money -= person->personel_cost;
}


void add_deposit(Person* person, Date* date)
{
    person->deposit.capital += person->money;
    person->money = 0;
}


void deposit_growth(Person* person, Date* date)
{
    person->deposit.capital += (uint64_t)((double)person->deposit.capital * DEPOSIT_RATE / 12.0);
}


void index_salary(Person* person, Date* date)
{
    person->salary += (uint64_t)((double)person->salary * INFLATION / 12.0);
}


void bills_inflation(Person* person, Date* date)
{
    person->food_cost += (uint64_t)((double)person->food_cost * INFLATION / 12.0);
    person->service_cost += (uint64_t)((double)person->service_cost * INFLATION / 12.0);
    person->personel_cost += (uint64_t)((double)person->personel_cost * INFLATION / 12.0);
}


void rent_rise(Person* person, Date* date)
{
    person->rent_cost += (uint64_t)((double)person->rent_cost * INFLATION / 12.0);
}


void estate_cost_rise(Person* person, Date* date)
{
    person->estate_cost += (uint64_t)((double)person->estate_cost * INFLATION / 12.0);
}


void month_operations(Person* person, Date* date)
{
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


int is_date_equal(Date* first, Date* second)
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
    date_end.year += 30; // duration - 30 years

    while(!is_date_equal(&date_current, &date_end))
    {
        month_operations(alice, &date_current);
        month_operations(bob, &date_current);

        date_increment(&date_current);
    }
}


void print_comparison(Person* alice, Person* bob)
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
