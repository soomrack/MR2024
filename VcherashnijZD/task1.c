#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


typedef long int Money;
typedef double Percent;

typedef struct
{
    char name[32];
    Money cost;
} Expenditure;

typedef struct
{
    Money money;
    Money salary;
    int vacation_month;

    Money estate_cost;
    Money deposit;
    Percent deposit_rate;
    Percent inflation;
 
    Money mortage_cap;
    Money mortage_remaining_month;
    Percent mortage_rate;

    Money rent;

    Expenditure* consumers;
} Person;

const Expenditure basic_consumer[] = {
    {"Food", 15*1000},
    {"Service", 20*1000},
    {"Personal", 20*1000},
    {"End", 0}
};
#define EXPED_SIZE sizeof(basic_consumer)

/*
 * Where is no need here for this function, OS will always free memory after proccess termination (if you are not in DOS). But for convenience.
 */
void deinit(Person* person)
{
    free(person->consumers);
}

void bob_init(Person* person)
{
    person->vacation_month = -1;
    person->money = 0;
    person->salary = 300000;

    person->estate_cost = 0;
    person->deposit = 1000000;

    person->mortage_cap = 0;
    person->mortage_remaining_month = 0;
    person->mortage_rate = 0;
    person->rent = 30000;
    person->deposit_rate = 0.1;


    person->inflation=0.07;
    person->consumers = malloc(EXPED_SIZE);
    memcpy(person->consumers, &basic_consumer, EXPED_SIZE);
}

void alice_init(Person* person, const int duration)
{
    person->vacation_month = 8;
    person->money = 0;
    person->salary = 300000;

    person->estate_cost = 15000000;
    person->deposit = 1000000;

    person->mortage_cap = person->estate_cost - person->deposit;
    person->mortage_remaining_month = 12*duration;
    person->mortage_rate = 0.24;
    person->deposit_rate = 0.1;

    person->rent = 0;

    person->inflation=0.07;
    person->consumers = malloc(EXPED_SIZE);
    memcpy(person->consumers, &basic_consumer, EXPED_SIZE);
}

void salary(Person* person, int month)
{
    if (month != person->vacation_month)
        person->money += person->salary;
}

void mortage(Person* person)
{
    if(person->mortage_cap != 0)
    {
        person->money -= (Money)((Percent)person->mortage_cap * person->mortage_rate / 12.0); // mortage percentage payment
        Money mortage_payment = person->mortage_cap / person->mortage_remaining_month;
        person->money -= mortage_payment;
        person->mortage_cap -= mortage_payment;
        person->mortage_remaining_month--;
    }
}

void rent(Person* person)
{
    person->money -= person->rent;
}

void consumers(Person* person)
{
    Expenditure* pointer = person->consumers;
    int index = 0;
    while (pointer[index].cost != 0) {
        person->money -= pointer[index].cost;
        index++;
    }
}

void add_deposit(Person* person)
{
    person->deposit += person->money;
    person->money = 0;
}

void deposit_growth(Person* person)
{
    person->deposit += (Money)((Percent)person->deposit * person->deposit_rate / 12.0);
}

void index_salary(Person* person)
{
    
    person->salary += (Money)((Percent)person->salary * person->inflation / 12.0);
}

void consumer_inflate(Person* person)
{
    Expenditure* pointer = person->consumers;
    uint16_t index = 0;
    while (pointer[index].cost != 0) {
        pointer[index].cost += (Money)((Percent)pointer[index].cost * person->inflation / 12.0);
        index++;
    }
}

void rent_inflate(Person* person)
{
    person->rent += (Money)((Percent)person->rent * person->inflation / 12.0);
}

void estate_inflate(Person* person)
{
    person->estate_cost += (Money)((Percent)person->estate_cost * person->inflation / 12.0);
}

void month_cycle(Person* person, int month)
{
    //person operations with money
    salary(person, month);
    mortage(person);
    rent(person);
    consumers(person);
    add_deposit(person);

    //processes
    deposit_growth(person);
    index_salary(person);
    consumer_inflate(person);
    rent_inflate(person);
    estate_inflate(person);
}

void compare(Person* alice, Person* bob)
{
    if ((alice->estate_cost + alice->deposit) == bob->deposit)
        printf("Bob and Alice funds are equal: %lu\n", bob->deposit);
    else if (alice->estate_cost + alice->deposit > bob->deposit)
        printf("Alice funds: %lu are higher than Bob's %lu\n", alice->estate_cost + alice->deposit, bob->deposit);
    else
        printf("Alice funds: %lu are lower than Bob's %lu\n", alice->estate_cost + alice->deposit, bob->deposit);
}

int main()
{
    const int duration = 25, current_year = 2024, month_start = 9;
    Person alice, bob;
    alice_init(&alice, duration);
    bob_init(&bob);
    uint16_t year = current_year;
    int month = month_start;
    while(year < current_year + duration || month < month_start)
    {
        if(++month > 12)
        {
            month = 1;
            year++;
        }

        month_cycle(&alice, month);
        month_cycle(&bob, month);
    }
    deinit(&alice);
    deinit(&bob);
    compare(&alice, &bob);

    return 0;
}

