#include <stdio.h>

const int ALICE_CAPITAL = 1000 * 1000 * 100;
const int BOB_CAPITAL = 1000 * 1000 * 100;

const int PRICE_HOME = 15 * 1000 * 1000 * 100;
const int MORTGAGE_TIME = 30; // годы
const float MORTGAGE_RATE = 1.17;
const int MONTH_PAY = 19959455; // ежемесячный платеж по ипотеке

const float INFLIATION = 1.08;
const float INDEXATION = 1.04;
const float DEPOSIT_RATE = 1.1;

typedef long long int Money; // копейки

Money HUMSTER_COST = 2 * 1000 * 100;
Money CAT_COST = 5 * 1000 * 100;
Money TURTLE_COST = 7 * 1000 * 100;


struct Person {
    unsigned long long int summ; // сумма процентов и стоимости квартиры по ипотеке
    unsigned long int depozit;   // деньги на вкладе
    Money salary;
    Money cost_food;
    Money cost_entertainmants;
    Money cost_transport;
    Money cost_services;
    Money cost_home; // стоимость дома Алисы
    Money account;   // деньги на руках, не лежащие на вкладе
    Money rent;
};


struct Person alice;
struct Person bob;


void alice_init()
{
    alice.depozit = 0;
    alice.account = 0;
    alice.salary = 250 * 1000 * 100;
    alice.summ = 7185403800;
    alice.cost_food = 20 * 1000 * 100;
    alice.cost_entertainmants = 5 * 1000 * 100;
    alice.cost_transport = 15 * 1000 * 100;
    alice.cost_services = 10 * 1000 * 100;
    alice.cost_home = PRICE_HOME;
}


void bob_init()
{
    bob.depozit = BOB_CAPITAL;
    bob.account = 0;
    bob.salary = 250 * 1000 * 100;
    bob.cost_food = 20 * 1000 * 100;
    bob.cost_entertainmants = 5 * 1000 * 100;
    bob.cost_transport = 15 * 1000 * 100;
    bob.cost_services = 10 * 1000 * 100;
    bob.rent = 50 * 1000;
}


void alice_salary(const int month)
{
    if (month == 1) {
        alice.salary *= INDEXATION;
        alice.account += alice.salary;
    }
}


void bob_salary(const int month)
{
    if (month == 1) {
        bob.salary *= INDEXATION;
        bob.account += bob.salary;
    }
}


void alice_expences(const int month)
{
    if (month == 1) {
        alice.cost_food *= INFLIATION;
        alice.cost_entertainmants *= INFLIATION;
        alice.cost_transport *= INFLIATION;
        alice.cost_services *= INFLIATION;
    }
    alice.account -= (alice.cost_food + alice.cost_entertainmants 
        + alice.cost_transport + alice.cost_services);
}


void bob_expences(const int month)
{
    if (month == 1) {
        bob.cost_food *= INFLIATION;
        bob.cost_entertainmants *= INFLIATION;
        bob.cost_transport *= INFLIATION;
        bob.cost_services *= INFLIATION;
        bob.rent *= INFLIATION;
    }
    bob.account -= (bob.cost_food + bob.cost_entertainmants 
        + bob.cost_transport + bob.cost_services + bob.rent);
}


void alice_depozit()
{
    alice.depozit += alice.account;
    alice.account = 0;
    alice.depozit += (alice.account * DEPOSIT_RATE / 12);
}


void bob_depozit()
{
    bob.depozit += bob.account;
    bob.account = 0;
    bob.depozit += (bob.account * DEPOSIT_RATE / 12);
}


void alice_mortgage(const int month)
{
    if (alice.summ > 0) {
        alice.summ -= MONTH_PAY;
        alice.account -= MONTH_PAY;
    }
    if (month == 1) alice.cost_home *= INFLIATION;
}


void bob_pets(const int year, const int month)
{
    if ((year > 2024 || (year == 2024 && month >= 9)) && // Боб завел хомяка
        (year < 2026 || (year == 2026 && month <= 8))) {
        bob.account -= HUMSTER_COST;
    }
    if ((year > 2026 || (year == 2026 && month >= 9)) && // Боб завел кота
        (year < 2030 || (year == 2030 && month <= 8))) {
        bob.account -= CAT_COST;
    }
    if (year > 2030 || (year == 2030 && month >= 9)) { // Боб завел черепашку
        bob.account -= TURTLE_COST;
    }
    if (month == 1) {
        HUMSTER_COST *= INFLIATION;
        CAT_COST *= INFLIATION;
        TURTLE_COST *= INFLIATION;
    }
}


void print()
{
    printf("Alice's capital = %llu kopeek\n", alice.depozit + alice.cost_home);
    printf("Bob's capital = %llu kopeek\n", bob.depozit);
    if ((alice.depozit + alice.cost_home) > bob.depozit)
        printf("An Alice's stratagy is more succesful\n");
    else
        printf("A Bob's stratagy is more succesful\n");
}


void simulation()
{
    // начало отсчета
    int year = 2024;
    int month = 9;

    while (!(year == 2024 + MORTGAGE_TIME && month == 9)) {
        alice_salary(month);
        alice_expences(month);
        alice_mortgage(month);
        alice_depozit();

        bob_salary(month);
        bob_expences(month);
        bob_pets(year);
        bob_depozit();

        month += 1;
        if (month == 13) {
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
