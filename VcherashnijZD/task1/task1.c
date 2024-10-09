#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>


typedef long int Money;
typedef double Percent;


typedef struct
{
    char name[32];
    Money cost;
} Expenditure;


typedef struct {
    Money estate_cost;
    Money cap;
    Money remaining_month;
    Percent rate;
} Mortgage;  // mortgage


typedef struct {
    Money money;
    Percent rate;
} Deposit;


typedef struct
{
    Money money;
    Money salary;
    uint16_t vacation_month;  // bit field vacation, no payment
    Mortgage mortgage;
    Deposit deposit;
    Percent inflation;
    Money rent;
    Expenditure consumers[4];
} Person;


void bob_init(Person* person)
{
    person->vacation_month = 0;
    person->money = 0;
    person->salary = 300000;

    person->deposit.money = 1000000;
    person->deposit.rate = 0.1;

    person->rent = 30000;

    person->inflation=0.07;
    const Expenditure consumers[] = {
        {"Food", 15000},
        {"Service", 20000},
        {"Personal", 20000},
        {"End", 0}
    };
    memcpy(person->consumers, consumers, sizeof(person->consumers));
}


void alice_init(Person* person, const int duration)
{
    person->vacation_month = 8;
    person->money = 0;
    person->salary = 500000;

    person->deposit.money = 1000000;
    person->deposit.rate = 0.1;

    person->mortgage.estate_cost = 15000000;
    person->mortgage.cap = person->mortgage.estate_cost - person->deposit.money;
    person->deposit.money = 0;
    person->mortgage.remaining_month = 12 * duration;
    person->mortgage.rate = 0.24;

    person->inflation=0.07;
    const Expenditure consumers[] = {
        {"Food", 15000},
        {"Service", 20000},
        {"Personal", 10000},
        {"End", 0}
    };
    memcpy(person->consumers, consumers, sizeof(person->consumers));
}


void salary_alice(Person* person, int month, int year, const int current_year)
{
    // Alice lost job after five years for one month (task on lection).
    if (year == (current_year + 5)) {
        person->salary = (Money)((Percent)person->salary * 2.0F);
        return;
    }

    uint16_t mask = 1 << (month - 1);  // 0b0000000000000010 feb
    if (!(person->vacation_month & mask))
        person->money += person->salary;
    
}


void salary_bob(Person* person, int month, int year, const int current_year)
{
    // Bob get promotion
    if (year == (current_year + 10))
        person->salary = (Money)((Percent)person->salary * 4.0F);

    uint16_t mask = 1 << (month - 1);  // 0b0000000000000010 feb
    if (!(person->vacation_month & mask))
        person->money += person->salary;
}


void mortgage_payment(Person* person)
{
    if(person->mortgage.cap >= 0)
    {
        // mortgage percentage payment
        person->money -= (Money)((Percent)person->mortgage.cap
            * person->mortgage.rate / 12.0);
        Money mortgage_payment = person->mortgage.cap
            / person->mortgage.remaining_month;
        person->money -= mortgage_payment;
        person->mortgage.cap -= mortgage_payment;
        person->mortgage.remaining_month--;
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
    if (person->money < 0) return;
    person->deposit.money += person->money;
    person->money = 0;
}


void deposit_growth(Person* person)
{
    person->deposit.money += (Money)((Percent)person->deposit.money
        * person->deposit.rate / 12.0D);
}


void index_salary(Person* person)
{    
    person->salary += (Money)((Percent)person->salary
        * person->inflation / 12.0D);
}


void consumer_inflate(Person* person)
{
    Expenditure* pointer = person->consumers;
    uint16_t index = 0;
    while (pointer[index].cost != 0) {
        pointer[index].cost += (Money)((Percent)pointer[index].cost
            * person->inflation / 12.0);
        index++;
    }
}


void rent_inflate(Person* person)
{
    person->rent += (Money)((Percent)person->rent * person->inflation / 12.0);
}


void estate_inflate(Person* person)
{
    person->mortgage.estate_cost += (Money)((Percent)person->mortgage.estate_cost
        * person->inflation / 12.0);
}


void head_loop(Person* alice, Person* bob, const int current_year,
    const int current_month, const int duration_years)
{
    int year = current_year;
    int month = current_month;
    while(year < current_year + duration_years || month < current_month) {
        // person operations with money 

        salary_alice(alice, month, year, current_year);
        mortgage_payment(alice);
        consumers(alice);
        add_deposit(alice);

        salary_bob(bob, month, year, current_year);
        rent(bob);
        consumers(bob);
        add_deposit(bob);

        // processes

        deposit_growth(alice);
        index_salary(alice);
        consumer_inflate(alice);
        estate_inflate(alice);

        deposit_growth(bob);
        index_salary(bob);
        consumer_inflate(bob);
        rent_inflate(bob);
        
        // loop proccess
        if(++month > 12) {
            month = 1;
            year++;
        }
    }
}

void print_comparation(Person* alice, Person* bob)
{
    if ((alice->mortgage.estate_cost + alice->deposit.money) == bob->deposit.money)
        printf("Bob and Alice funds are equal: %lu\n", bob->deposit.money);
    else if (((Money) (alice->mortgage.estate_cost + alice->deposit.money))
            > bob->deposit.money)
        printf("Alice funds: %lu are higher than Bob's %lu\n", alice->mortgage.estate_cost
            + alice->deposit.money, bob->deposit.money);
    else
        printf("Alice funds: %lu are lower than Bob's %lu\n", alice->mortgage.estate_cost
            + alice->deposit.money, bob->deposit.money);
}

int main()
{
    const int duration_years = 25;
    Person alice, bob;
    alice_init(&alice, duration_years);
    bob_init(&bob);

    head_loop(&alice, &bob, 2024, 9, duration_years);

    print_comparation(&alice, &bob);

    return 0;
}

