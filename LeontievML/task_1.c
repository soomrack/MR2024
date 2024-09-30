#include <stdio.h>

const int ALICE_CAPITAL = 1000 * 1000 * 100;
const int BOB_CAPITAL = 1000 * 1000 * 100;

const int PRICE_HOME = 15 * 1000 * 1000 * 100;
const int MORTGAGE_TIME = 30;  // годы
const float MORTGAGE_RATE = 1.17;
const int MONTH_PAY = 19959455;  // ежемесячный платеж по ипотеке

const float INFLIATION = 1.08;
const float INDEXATION = 1.04;
const float DEPOSIT_RATE = 1.1;

typedef long long int Money;  // копейки


struct Person {
    unsigned long long int summ; // сумма процентов и стоимости квартиры по ипотеке
    unsigned long int account; // деньги на вкладе
    Money salary;
    Money cost_food;
    Money cost_entertainmants;
    Money cost_transport;
    Money cost_services;
    Money cost_home; // стоимость дома Алисы
    Money capital; // деньги на руках, не лежащие на вкладе
    Money rent;
};


struct Person alice;
struct Person bob;


void alice_init()
{
    alice.account = 0;
    alice.salary = 250 * 1000 * 100;
    alice.summ = 7185403800;
    alice.cost_food = 20 * 1000 * 100;
    alice.cost_entertainmants = 5 * 1000 * 100;
    alice.cost_transport = 15 * 1000 * 100;
    alice.cost_services = 10 * 1000 * 100;
    alice.cost_home = PRICE_HOME;
    alice.capital = 0;
}


void bob_init()
{
    bob.account = 0;
    bob.salary = 250 * 1000 * 100;
    bob.cost_food = 20 * 1000 * 100;
    bob.cost_entertainmants = 5 * 1000 * 100;
    bob.cost_transport = 15 * 1000 * 100;
    bob.cost_services = 10 * 1000 * 100;
    bob.capital = 0;
    bob.rent = 50 * 000;
}


void alice_salary(const int month)
{
    if (month == 1) alice.salary *= INDEXATION;
}


void bob_salary(const int month)
{
    if (month == 1) bob.salary *= INDEXATION;
}


void alice_account()
{
    alice.account += (alice.account * DEPOSIT_RATE / 12);

    alice.capital += alice.salary;
    alice.capital -= (alice.cost_food + alice.cost_entertainmants + alice.cost_transport + alice.cost_services);
    alice.account += alice.capital;
    alice.capital = 0;
}


void bob_account()
{
    bob.account += (bob.account * DEPOSIT_RATE / 12);
    
    bob.capital += bob.salary;
    bob.capital -= (bob.cost_food + bob.cost_entertainmants + bob.cost_transport + bob.cost_services + bob.rent);
    bob.account += bob.capital;
    bob.capital = 0;
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
        alice.summ -= MONTH_PAY;
        alice.capital -= MONTH_PAY;
    }
}


void bob_pets(const int year)
{
    if (year <= 2026) bob.account -= 2 * 1000;  // Боб завел хомяка
    if (year <= 2030 && year > 2026) bob.account -= 5 * 1000;  // Боб завел кошку
    if (year > 2030) bob.account -= 7 * 1000;  // Боб завел черепашку
}


void print()
{
    printf("Alice's capital = %llu kopeek\n", alice.account + alice.cost_home);
    printf("Bob's capital = %llu kopeek\n", bob.account);

    if ((alice.account + alice.cost_home) > bob.account)
        printf("An Alice's stratagy is more succesful\n");
    else
        printf("A Bob's stratagy is more succesful\n");
}


void simulation()
{
    // начало отсчета
    int year = 2024;
    int month = 9;

    bob.account += BOB_CAPITAL;

    while (!(year == 2024 + MORTGAGE_TIME && month == 9))
    {
        alice_salary(month);
        alice_infliation(month);
        bob_salary(month);
        bob_infliation(month);

        alice_mortgage();
        bob_pets(year);

        alice_account();
        bob_account();

        month += 1;
        if (month == 13)
        {
            month = 1;
            year += 1;
        }
    }
}


int main() {
    alice_init();
    bob_init();

    simulation();

    print();

    return 0;
}
