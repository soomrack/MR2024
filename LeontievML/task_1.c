#include <stdio.h>
#include <math.h>

const int ALICE_CAPITAL = 1000 * 1000 * 100;
const int BOB_CAPITAL = 1000 * 1000 * 100;

const int PRICE_HOME = 15 * 1000 * 1000 * 100;
const int MORTGAGE_TIME = 30; // YEARS
const double MORTGAGE_RATE = 1.17;
const int MONTH_PAY = 19959455; // ежемесячный платеж по ипотеке

const double INFLIATION = 1.08;
const double INDEXATION = 1.04;
const double DEPOSIT_RATE = 1.1;

typedef long long int Money; // копейки

struct Person
{
    unsigned long int summ;
    Money salary;
    unsigned long int account;
    Money cost_food;
    Money cost_entertainmants;
    Money cost_transport;
    Money cost_services;
    Money cost_home; // стоимость дома Алисы
    Money capital;
    Money rent;
};

struct Person alice;
struct Person bob;

void alice_init()
{
    alice.account = 0; // деньги на счету в банке
    alice.salary = 250 * 1000 * 100;
    alice.summ = 71854038 * 100;
    alice.cost_food = 20 * 1000 * 100;
    alice.cost_entertainmants = 5 * 1000 * 100;
    alice.cost_transport = 15 * 1000 * 100;
    alice.cost_services = 10 * 1000 * 100;
    alice.cost_home = PRICE_HOME;
    alice.capital = 0; // деньги с зарплаты
}

void bob_init()
{
    bob.account = 0; // деньги на счету в банке
    bob.salary = 250 * 1000 * 100;
    bob.cost_food = 20 * 1000 * 100;
    bob.cost_entertainmants = 5 * 1000 * 100;
    bob.cost_transport = 15 * 1000 * 100;
    bob.cost_services = 10 * 1000 * 100;
    bob.capital = 0; // деньги с зарплаты
    bob.rent = 50 * 000;
}

void alice_salary(const int month)
{
    if (month == 1)
    {
        alice.salary *= INDEXATION;
    }
}

void bob_salary(const int month)
{
    if (month == 1)
    {
        bob.salary *= INDEXATION;
    }
}

void alice_infliation(const int month)
{
    if (month == 1)
    {
        alice.cost_food *= INFLIATION;
        alice.cost_entertainmants *= INFLIATION;
        alice.cost_transport *= INFLIATION;
        alice.cost_services *= INFLIATION;
        alice.cost_home *= INFLIATION;
    }
}

void bob_infliation(const int month)
{
    if (month == 1)
    {
        bob.cost_food *= INFLIATION;
        bob.cost_entertainmants *= INFLIATION;
        bob.cost_transport *= INFLIATION;
        bob.cost_services *= INFLIATION;
        bob.rent *= INFLIATION;
    }
}

void alice_mortgage()
{
    if (alice.summ > 0)
    {
        if (alice.summ > MONTH_PAY)
        {
            alice.summ -= MONTH_PAY;
            alice.capital -= MONTH_PAY;
        }
        else
        {
            alice.capital -= alice.summ;
            alice.summ = 0;
        }
    }
}

void print()
{
    printf("Alice's capital = %llu kopeek\n", alice.account + alice.cost_home);
    printf("Bob's capital = %llu kopeek\n", bob.account);

    if ((alice.account + alice.cost_home) > bob.account)
    {
        printf("An Alice's stratagy is more succesful\n");
    }
    else
    {
        printf("A Bob's stratagy is more succesful\n");
    }
}

void simulation()
{
    int year = 2024;
    int month = 9;

    bob.account += BOB_CAPITAL;

    while (!(year == 2024 + MORTGAGE_TIME && month == 9))
    {
        alice_salary(month);
        alice_infliation(month);
        bob_salary(month);
        bob_infliation(month);

        month += 1;
        if (month == 13)
        {
            month = 1;
            year += 1;
        }

        alice.account += (alice.account * DEPOSIT_RATE / 12);
        alice.capital += alice.salary;
        alice.capital -= (alice.cost_food + alice.cost_entertainmants + alice.cost_transport + alice.cost_services);
        bob.account += (bob.account * DEPOSIT_RATE / 12);
        bob.capital += bob.salary;
        alice.capital -= (alice.cost_food + alice.cost_entertainmants + alice.cost_transport + alice.cost_services);

        alice_mortgage();

        alice.account += alice.capital;
        alice.capital = 0;
        bob.account += bob.capital;
        bob.capital = 0;
    }
}

int main()
{
    alice_init();
    bob_init();

    simulation();

    print();

    return 0;
}