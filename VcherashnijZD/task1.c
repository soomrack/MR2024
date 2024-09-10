#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define ALICE_CAPITAL 1000*1000
#define BOB_CAPITAL 1000*1000
#define ALICE_SALARY 300*1000
#define BOB_SALARY 300*1000

#define FLAT_COST 15*1000*1000
#define RENT_COST 30*1000

#define MORTAGE_DURATION 25
#define MORTAGE_RATE 0.24
#define INFLATION 0.20
#define DEPOSIT_RATE 0.1

#define YEAR_START 2024
#define MONTH_START 9
#define DURATION 25 // in years
// Also constants in basic_consumer struct, line 48//


typedef struct
{
    char name[32];
    uint64_t cost;
} Expenditure;

typedef struct
{
    int64_t money;
    uint64_t salary;

    uint64_t estate_cost;
    int64_t deposit;

    uint64_t mortage;
    uint16_t mortage_remaining_month;
    double mortage_rate;

    uint64_t rent;

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
    person->money = 0;
    person->salary = BOB_SALARY;

    person->estate_cost = 0;
    person->deposit = BOB_CAPITAL;

    person->mortage = 0;
    person->mortage_remaining_month = 0;
    person->mortage_rate = 0;

    person->rent = RENT_COST;

    person->consumers = malloc(EXPED_SIZE);
    memcpy(person->consumers, &basic_consumer, EXPED_SIZE);
}

void alice_init(Person* person)
{
    person->money = 0;
    person->salary = ALICE_SALARY;

    person->estate_cost = FLAT_COST;
    person->deposit = 0;

    person->mortage = FLAT_COST - ALICE_CAPITAL;
    person->mortage_remaining_month = 12*DURATION;
    person->mortage_rate = MORTAGE_RATE;

    person->rent = 0;

    person->consumers = malloc(EXPED_SIZE);
    memcpy(person->consumers, &basic_consumer, EXPED_SIZE);
}

void salary(Person* person)
{
    person->money += person->salary;
}

void mortage(Person* person)
{
    if(person->mortage != 0)
    {
        person->money -= (uint64_t)((double)person->mortage * person->mortage_rate / 12.0); //mortage percentage payment
        uint64_t mortage_payment = person->mortage / person->mortage_remaining_month;
        person->money -= mortage_payment;
        person->mortage -= mortage_payment;
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
    uint16_t index = 0;
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
    person->deposit += (uint64_t)((double)person->deposit * DEPOSIT_RATE / 12.0);
}

void index_salary(Person* person)
{
    person->salary += (uint64_t)((double)person->salary * INFLATION / 12.0);
}

void consumer_inflate(Expenditure* pointer)
{
    uint16_t index = 0;
    while (pointer[index].cost != 0) {
        pointer[index].cost += (uint64_t)((double)pointer[index].cost * INFLATION / 12.0);
        index++;
    }
}

void rent_inflate(Person* person)
{
    person->rent += (uint64_t)((double)person->rent * INFLATION / 12.0);
}

void estate_inflate(Person* person)
{
    person->estate_cost += (uint64_t)((double)person->estate_cost * INFLATION / 12.0);
}

void month_cycle(Person* person)
{
    //person operations with money
    salary(person);
    mortage(person);
    rent(person);
    consumers(person);
    add_deposit(person);

    //processes
    deposit_growth(person);
    index_salary(person);
    consumer_inflate(person->consumers);
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
    Person alice, bob;
    alice_init(&alice);
    bob_init(&bob);

    uint16_t year = YEAR_START;
    uint8_t month = MONTH_START;
    while(year < YEAR_START + DURATION || month < MONTH_START)
    {
        if(++month > 12)
        {
            month = 1;
            year++;
        }

        month_cycle(&alice);
        month_cycle(&bob);
    }
    deinit(&alice);
    deinit(&bob);
    compare(&alice, &bob);

    return 0;
}
