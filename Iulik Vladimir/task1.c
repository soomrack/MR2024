#include <stdio.h>
#include <math.h>

typedef long long int Lli;

int MONTH = 9;
int YEAR = 2024;
double INFLATION = 0.08;
double DEP_PERCENT = 0.2;
Lli IPOTEKA_AMOUNT;

struct Person
{
    Lli bank;
	Lli salary;
	Lli home;
    Lli home_start;
	Lli	month_payment;
	Lli food_expenses;
	Lli utility_expenses;
	Lli other_expenses;
	Lli rent;
    Lli start_dep;
	int duration;
	Lli deposit;
	char * name;
};

struct Person alice;
struct Person bob;

void alice_init()
{
    alice.bank = 1000 * 1000 * 100;
    alice.salary = 250 * 1000 * 100;
    alice.month_payment = 200 * 1000 * 100; //ежемесячный платеж по ипотеке
	alice.home = 15 * 1000 * 1000 * 100;
    alice.home_start = 15 * 1000 * 1000 * 100;
	alice.food_expenses = 15 * 1000 * 100;
	alice.utility_expenses = 8 * 1000 * 100;
	alice.other_expenses = 17 * 1000 * 100;
	alice.name = "Alice";
}
void bob_init()
{
    bob.bank = 1000 * 1000 * 100;
	bob.salary = 250 * 1000 * 100;
	bob.food_expenses = 15 * 1000 * 100;
	bob.utility_expenses = 8 * 1000 * 100;
    bob.food_expenses = 15 * 1000 * 100;
	bob.other_expenses = 17 * 1000 * 100;
	bob.rent = 30 * 1000 * 100;
    bob.deposit = 0;
    bob.deposit = 0;
	bob.name = "Bob";
}

void alice_inflation()
{
    alice.salary += alice.salary*(INFLATION/12);
    alice.food_expenses += alice.food_expenses*(INFLATION/12);
    alice.utility_expenses += alice.utility_expenses*(INFLATION/12);
	alice.other_expenses += alice.other_expenses*(INFLATION/12);
    alice.home += alice.home_start*(INFLATION);
}
void bob_inflation()
{
    bob.salary += bob.salary*(INFLATION/12);
    bob.food_expenses += bob.food_expenses*(INFLATION/12);
    bob.utility_expenses += bob.utility_expenses*(INFLATION/12);
	bob.other_expenses += bob.other_expenses*(INFLATION/12);
    bob.rent += bob.rent*(INFLATION/12);
}
void alice_month()
{
    alice.bank += alice.salary;
    alice.bank -= (alice.food_expenses+alice.utility_expenses+alice.other_expenses+alice.month_payment);
    IPOTEKA_AMOUNT += alice.month_payment; //считаем общую сумму за ипотеку
}
void bob_month()
{
    bob.bank += bob.salary;
    bob.deposit += bob.start_dep*(DEP_PERCENT/12);
    bob.bank -= (bob.food_expenses+bob.utility_expenses+bob.other_expenses+bob.rent);
}
void simulation(int month, int year)
{
    int end_year = year+30;

    //вносим за Алису первоначальный платеж и обнуляем её банк
    IPOTEKA_AMOUNT = alice.bank;
    alice.bank = 0;

    //вносим депозит в банк за Боба 
    bob.deposit = bob.bank;
    bob.start_dep = bob.deposit;
    bob.bank -= bob.deposit;

    while (!((year == end_year) && (month == MONTH)))
    {
        alice_month();
        bob_month();

        month += 1; 
        if (month == 13)
        {
            alice_inflation();
            bob_inflation();
            year += 1;
            month = 1;
        }
        //printf("%d",year);
        //printf("___%d \n", month);
        //printf("___%d \n", bob.bank/100);
        //printf("%d \n",alice.bank);
    }
    printf("Алиса заплатила за 30 лет %lld рублей \n",IPOTEKA_AMOUNT/100);
    printf("Боб сэкономил и заработал за 30 лет %lld рублей \n",(bob.bank+bob.deposit)/100);
    printf("За 30 лет квартира подорожала с %lld рублей до %lld \n",alice.home_start/100,alice.home/100);
    printf("Алиса переплатила %lld рублей", (IPOTEKA_AMOUNT-alice.home_start)/100);
}
int main()
{
    alice_init();
	bob_init();

    simulation(MONTH, YEAR);

    printf("");
    return 0;
}
