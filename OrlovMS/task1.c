#include <stdio.h>
#include <stdint.h>

#define ALICE_CAPITAL 1000*1000
#define BOB_CAPITAL 1000*1000

#define FLAT_COST 15*1000*1000
#define MORTAGE_DURATION 30 //years
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
#define DURATION 30 //years

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

    uint64_t food_cost;
    uint64_t service_cost;
    uint64_t personel_cost;
} Person;

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

    person->food_cost = FOOD_COST;
    person->service_cost = SERVICE_COST;
    person->personel_cost = PERSONEL_COST;
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

    person->food_cost = FOOD_COST;
    person->service_cost = SERVICE_COST;
    person->personel_cost = PERSONEL_COST;
}

void add_salary(Person* person)
{
    person->money += person->salary;
}

void pay_mortage(Person* person)
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

void pay_rent(Person* person)
{
    person->money -= person->rent;
}

void pay_bills(Person* person)
{
    person->money -= person->food_cost;
    person->money -= person->service_cost;
    person->money -= person->personel_cost;
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

void bills_inflation(Person* person)
{
    person->food_cost += (uint64_t)((double)person->food_cost * INFLATION / 12.0);
    person->service_cost += (uint64_t)((double)person->service_cost * INFLATION / 12.0);
    person->personel_cost += (uint64_t)((double)person->personel_cost * INFLATION / 12.0);
}

void rent_rise(Person* person)
{
    person->rent += (uint64_t)((double)person->rent * INFLATION / 12.0);
}

void estate_cost_rise(Person* person)
{
    person->estate_cost += (uint64_t)((double)person->estate_cost * INFLATION / 12.0);
}

void month_operations(Person* person)
{
    //person operations with money
    add_salary(person);
    pay_mortage(person);
    pay_rent(person);
    pay_bills(person);
    add_deposit(person);

    //processes
    deposit_growth(person);
    index_salary(person);
    bills_inflation(person);
    rent_rise(person);
    estate_cost_rise(person);
}

void comparison(Person* alice, Person* bob)
{
    if(alice->estate_cost + alice->deposit == bob->deposit)
    {
        printf("Alice and Bob funds are equal: %lu\n", bob->deposit);
    }
    else if(alice->estate_cost + alice->deposit > bob->deposit)
    {
        printf("Alice funds: %lu are higher than Bob's %lu\n", alice->estate_cost + alice->deposit, bob->deposit);
    }
    else
    {
        printf("Alice funds: %lu are lower than Bob's %lu\n", alice->estate_cost + alice->deposit, bob->deposit);
    }
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

        month_operations(&alice);
        month_operations(&bob);
    }

    comparison(&alice, &bob);

    return 0;
}