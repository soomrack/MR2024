#include <stdio.h>
#include <math.h>


typedef unsigned long long int money; // в копейках

const double inflation_p = 9; // процент инфляции
const double indexation_p = 7; // процент инфляции
const double deposit_p = 20; // годовой процент депозита

const double mortgage_p = 16; // ставка по ипотеке

const int simulaton_time = 30; //длительность ипотеки

const money food_cost = 15 * 1000 * 100;
const money transport_cost = 3 * 1000 * 100;
const money house_bills = 7 * 1000 * 100;
const money personal = 10 * 1000 * 100;

struct Mortgage
{
money sum;
money first_pay;
money month_pay;
};

struct Rent
{
money month_pay;
};

typedef struct Person
{
    money bank_account;
    money earns;
    money month_expences;

    struct Mortgage mortgage;
    struct Rent rent;
} Person;

static Person alice;
static Person bob;

void alice_data()
{
    alice.bank_account = (1000 * 1000) * 100;
    alice.earns = (200 * 1000) * 100;
    alice.month_expences = food_cost + transport_cost + house_bills + personal;
    alice.mortgage.sum = (13 * 1000 * 1000) * 100;
    alice.mortgage.first_pay = (1000 * 1000) * 100;
}

void bob_data()
{
    bob.bank_account = (1000 * 1000) * 100;
    bob.earns = (200 * 1000) * 100;
    bob.rent.month_pay = (70 * 1000) * 100;
    bob.month_expences = food_cost + transport_cost + house_bills + personal;
}

void mortgage_month_pay()
{
    double month_percentage = mortgage_p / (100 * 12);

    alice.bank_account -= alice.mortgage.first_pay;
    
    alice.mortgage.month_pay = (alice.mortgage.sum - alice.mortgage.first_pay) \
    * month_percentage / (1 - ( 1/ pow((1 + month_percentage), simulaton_time*12)));

    alice.mortgage.month_pay = round (alice.mortgage.month_pay);
    (money)alice.mortgage.month_pay;
}

void earns()
{
    alice.bank_account += alice.earns;
    bob.bank_account += bob.earns;
}

void deposit_percents()
{
    alice.bank_account *= (1 + deposit_p / (100*12));

    bob.bank_account *= (1 + deposit_p / (100*12));
}

void month_expences()
{
    alice.bank_account -= alice.month_expences;
    alice.bank_account -= alice.mortgage.month_pay;

    bob.bank_account -= alice.month_expences;
    bob.bank_account -= bob.rent.month_pay;
}

void inflation_and_indexation()
{
    alice.month_expences *= (1 + inflation_p / 100);
    alice.earns *= (1 + indexation_p / 100);

    bob.month_expences *= (1 + inflation_p / 100);
    bob.rent.month_pay *= (1 + inflation_p / 100);
    bob.earns *= (1 + indexation_p / 100);
}

void print()
{
    alice.bank_account += alice.mortgage.sum;
    printf("Alice mortgage = %lld\n", alice.mortgage.month_pay);
    printf("Alice capital = %lld\nBob capital = %lld\n", alice.bank_account, bob.bank_account);
}

void simulation()
{
    mortgage_month_pay();
    
    const int start_year = 2024;
    const int start_month = 9;
    int year = start_year;
    int month = start_month;
    
    while( !((year == start_year + simulaton_time) && (month == start_month + 1)) ) {
        //printf("Alice capital = %lld\nBob capital =   %lld\n\n", alice->bank_account/100, bob.bank_account/100);
        //получение зарплаты
        earns();

        //траты
        month_expences();

        //получение процентов по депозиту
        if (!((year == start_year) && (month == start_month))) {
            deposit_percents();
        }

        //изменения
        if (month == 12) {
            inflation_and_indexation();
        }
        
        ++month;
        if(month == 13) {
            
            month = 1;
            ++year;
        }
    }
}

int main()
{
    alice_data();
    bob_data();

    simulation();

    print();
    
    return 0;
}
