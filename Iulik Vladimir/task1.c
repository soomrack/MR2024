#include <stdio.h>
#include <math.h>

typedef long long int Money; // в копейках

int MONTH = 9;
int YEAR = 2024;
double INFLATION = 0.08;  // процент годовой инфляции
double DEP_PERCENT = 0.2; // процент по вкладу
Money IPOTEKA_AMOUNT;

struct Person
{
    Money capital; // капитал
    Money salary;
    Money home_cost;     // стоимость квартиры
    Money month_payment; // ежемесячный платеж по ипотеке
    Money food_expenses;
    Money utility_expenses;
    Money other_expenses;
    Money rent;            // арендная плата
    Money initial_deposit; // начальный депозит в банк, по которому начисляются проценты(меняется каждый год)
    Money deposit;         // депозит в банк, на который начисляются проценты
};

struct Person alice; // ипотека
struct Person bob;   // депозит

void alice_init()
{
    alice.capital = 1000 * 1000 * 100;
    alice.salary = 250 * 1000 * 100;
    alice.home_cost = 15 * 1000 * 1000 * 100;
    alice.month_payment = 200 * 1000 * 100;
    alice.food_expenses = 15 * 1000 * 100;
    alice.utility_expenses = 8 * 1000 * 100;
    alice.other_expenses = 17 * 1000 * 100;
    alice.deposit = 0;
}

void bob_init()
{
    bob.capital = 1000 * 1000 * 100;
    bob.salary = 250 * 1000 * 100;
    bob.food_expenses = 15 * 1000 * 100;
    bob.utility_expenses = 8 * 1000 * 100;
    bob.food_expenses = 15 * 1000 * 100;
    bob.other_expenses = 17 * 1000 * 100;
    bob.rent = 30 * 1000 * 100;
    bob.deposit = 0;
}

void alice_salary()
{
    alice.capital += alice.salary;
}

void alice_expenses()
{
    alice.capital -= (alice.food_expenses + alice.utility_expenses + alice.other_expenses);
}

void alice_ipoteka_pay()
{
    alice.capital -= alice.month_payment;
    IPOTEKA_AMOUNT += alice.month_payment;
}

void alice_deposit_accrual(int month, Money capital)
{
    if (month == 9)
    {
        alice.initial_deposit = alice.deposit;
    }
    alice.deposit += alice.initial_deposit * (DEP_PERCENT / 12);
    alice.deposit += alice.capital;
}

void alice_inflation()
{
    alice.salary += alice.salary * INFLATION;
    alice.food_expenses += alice.food_expenses * INFLATION;
    alice.utility_expenses += alice.utility_expenses * INFLATION;
    alice.other_expenses += alice.other_expenses * INFLATION;
    alice.home_cost += alice.home_cost * INFLATION;
}

void bob_salary()
{
    bob.capital += bob.salary;
}
void bob_expenses()
{
    bob.capital -= (bob.food_expenses + bob.utility_expenses + bob.other_expenses);
}

void bob_rent_pay()
{
    bob.capital -= bob.rent;
}

void bob_deposit_accrual(int month, Money capital)
{
    if (month == 9)
    {
        bob.initial_deposit = bob.deposit;
    }
    bob.deposit += bob.initial_deposit * (DEP_PERCENT / 12);
    bob.deposit += capital;
}

void bob_inflation()
{
    bob.salary += bob.salary * INFLATION;
    bob.food_expenses += bob.food_expenses * INFLATION;
    bob.utility_expenses += bob.utility_expenses * INFLATION;
    bob.other_expenses += bob.other_expenses * INFLATION;
    bob.rent += bob.rent * INFLATION;
}

void simulation(int month, int year)
{
    int end_year = year + 30;

    // вносим за Алису первоначальный платеж и обнуляем её банк
    IPOTEKA_AMOUNT = alice.capital;
    alice.capital = 0;

    // вносим депозит в банк за Боба
    bob.deposit = bob.capital;
    bob.capital -= bob.deposit;

    while (!((year == end_year) && (month == MONTH)))
    {
        alice_salary();
        alice_expenses();
        alice_ipoteka_pay();
        alice_deposit_accrual(month, alice.capital);

        bob_salary();
        bob_expenses();
        bob_rent_pay();
        bob_deposit_accrual(month, bob.capital);

        month += 1;
        if (month == 12)
        {
            alice_inflation();
            bob_inflation();
        }
        if (month == 13)
        {
            year += 1;
            month = 1;
        }
    }
    if (alice.capital + alice.home_cost + alice.deposit > bob.capital + bob.deposit)
    {
        printf("Выгоднее стратегия Алисы\n");
    }
    else if (alice.capital + alice.home_cost < bob.capital + bob.deposit)
    {
        printf("Выгоднее стратегия Боба\n");
    }
    else
    {
        printf("Обе стратегии выгодные\n");
    }
    printf("Боб сэкономил и заработал за 30 лет %lld рублей \n", (bob.capital + bob.deposit) / 100);
    printf("Алиса сэкономила и заработал за 30 лет %lld рублей \n", (alice.capital + alice.home_cost + alice.deposit) / 100);
}

int main()
{
    alice_init();
    bob_init();

    simulation(MONTH, YEAR);
    return 0;
}
