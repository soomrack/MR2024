#include <stdio.h>
#include <math.h>

typedef long long int Money;  // в копейках

int MONTH = 9;
int YEAR = 2024;
double INFLATION = 0.08;  // процент годовой инфляции
double DEP_PERCENT_BOB = 0.2;  // процент по вкладу Боба
double DEP_PERCENT_ALICE = 0.2;  // процент по вкладу Алисы
int FLAG = 1;  // Флаг для изменения процентной ставки

Money IPOTEKA_AMOUNT;


struct Person {
    Money capital;
    Money salary;
    Money home_cost;  // стоимость квартиры
    Money month_payment;  // ежемесячный платеж по ипотеке
    Money food_expenses;
    Money utility_expenses;
    Money other_expenses;
    Money rent;  // арендная плата
    Money initial_deposit;  // начальный депозит в банк, по которому начисляются проценты(меняется каждый год)
    Money deposit;  // депозит в банк, на который начисляются проценты
};


struct Person alice;  // ипотека
struct Person bob;  // депозит


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

    //  вносим за Алису первоначальный платеж и обнуляем её банк
    IPOTEKA_AMOUNT = alice.capital;
    alice.capital = 0;
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

    //  вносим депозит в банк за Боба
    bob.deposit = bob.capital;
    bob.capital -= bob.deposit;
}


void alice_salary(const int month)
{
    alice.capital += alice.salary;
    if (month == 12) {
        alice.salary += alice.salary * INFLATION;
    }
}


void alice_expenses(const int month)
{
    alice.capital -= (alice.food_expenses + alice.utility_expenses + alice.other_expenses);
    if (month == 12) {
        alice.food_expenses += alice.food_expenses * INFLATION;
        alice.utility_expenses += alice.utility_expenses * INFLATION;
        alice.other_expenses += alice.other_expenses * INFLATION;
    }
}


void alice_ipoteka_pay(const int month)
{
    alice.capital -= alice.month_payment;
    IPOTEKA_AMOUNT += alice.month_payment;
    if (month == 12) {
        alice.home_cost += alice.home_cost * INFLATION;
    }
}


void alice_deposit_accrual(const int month, Money capital)
{
    if (month == 9) {
        alice.initial_deposit = alice.deposit;
    }
    alice.deposit += alice.initial_deposit * (DEP_PERCENT_ALICE / 12);
    alice.deposit += alice.capital;
}


void bob_salary(const int month)
{
    bob.capital += bob.salary;
    if (month == 12) {
        bob.salary += bob.salary * INFLATION;
    }
}
    

void bob_expenses(const int month)
{
    bob.capital -= (bob.food_expenses + bob.utility_expenses + bob.other_expenses);
    if (month == 12) {
        bob.food_expenses += bob.food_expenses * INFLATION;
        bob.utility_expenses += bob.utility_expenses * INFLATION;
        bob.other_expenses += bob.other_expenses * INFLATION;
    }
}


void bob_rent_pay(const int month)
{
    bob.capital -= bob.rent;
    if (month == 12) {
        bob.rent += bob.rent * INFLATION;
    }
}


void bob_deposit_accrual(const int month, Money capital)
{
    if (month == 9) {
        bob.initial_deposit = bob.deposit;
    }
    if (bob.deposit >= 6*1000*1000*100 && FLAG == 1) {
        DEP_PERCENT_BOB += 0.01;
        FLAG = 0;
    }
    bob.deposit += bob.initial_deposit * (DEP_PERCENT_BOB / 12);
    bob.deposit += capital;
}


void print_coclusion(const Money bob_loot, const Money alice_loot)
{
    if (alice_loot > bob_loot) {
        printf("Выгоднее стратегия Алисы\n");
    }
    else if (alice_loot < bob_loot) {
        printf("Выгоднее стратегия Боба\n");
    }
    else {
        printf("Обе стратегии выгодные\n");
    }
    printf("Боб сэкономил и заработал за 30 лет %lld рублей \n", (bob_loot) / 100);
    printf("Алиса сэкономила и заработал за 30 лет %lld рублей \n", (alice_loot) / 100);
}


void simulation(int month, int year)
{
    int end_year = year + 30;

    while (!((year == end_year) && (month == MONTH))) {
        alice_salary(month);
        alice_expenses(month);
        alice_ipoteka_pay(month);
        alice_deposit_accrual(month, alice.capital);

        bob_salary(month);
        bob_expenses(month);
        bob_rent_pay(month);
        bob_deposit_accrual(month, bob.capital);

        month += 1;
        if (month == 13) {
            year += 1;
            month = 1;
        }
    }
}


int main()
{
    alice_init();
    bob_init();

    simulation(MONTH, YEAR);

    print_coclusion(bob.capital + bob.deposit, alice.capital + alice.home_cost + alice.deposit);

    return 0;
}
