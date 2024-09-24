#include <stdio.h>
#include <math.h>


typedef unsigned long long int money; // в копейках

const double INFLATION_P = 9; // процент инфляции
const double IDEXATION_P = 7; // процент инфляции
const double DEPOSIT_P = 20; // годовой процент депозита
const double MORTGAGE_P = 16; // ставка по ипотеке
const int SIMULATION_TIME = 30; //длительность ипотеки

const money FOOD_COST = 15 * 1000 * 100;
const money TRANSPORT_COST = 3 * 1000 * 100;
const money HOUSE_BILLS = 7 * 1000 * 100;
const money PERSONAL = 10 * 1000 * 100;


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

struct Cat
{
    int year_of_purchase;
    int month_of_purchase;
    money cost;
    money food_cost;
    money medical_cost;
    money funeral_cost;
    int duration_of_life;
};

typedef struct Person
{
    money bank_account;
    money earns;
    money month_expences;

    struct Mortgage mortgage;
    struct Rent rent;
    struct Cat cat;
} Person;


    Person alice;
    Person bob;


void alice_data()
{
    alice.bank_account = (1000 * 1000) * 100;
    alice.earns = (200 * 1000) * 100;
    alice.month_expences = FOOD_COST + TRANSPORT_COST + HOUSE_BILLS + PERSONAL + alice.cat.food_cost + alice.cat.medical_cost;
    alice.mortgage.sum = (13 * 1000 * 1000) * 100;
    alice.mortgage.first_pay = (1000 * 1000) * 100;

    alice.cat.year_of_purchase = 2030;
    alice.cat.month_of_purchase = 9;
    alice.cat.cost = (50 * 1000) * 100;
    alice.cat.food_cost = (3 * 1000) * 100;
    alice.cat.medical_cost = (2 * 1000) * 100;
    alice.cat.funeral_cost = (30 * 1000) * 100;
    alice.cat.duration_of_life = 12;
    
}


void bob_data()
{
    bob.bank_account = (1000 * 1000) * 100;
    bob.earns = (200 * 1000) * 100;
    bob.rent.month_pay = (70 * 1000) * 100;
    bob.month_expences = FOOD_COST + TRANSPORT_COST + HOUSE_BILLS + PERSONAL;
}


void mortgage_month_pay()
{
    double month_percentage = MORTGAGE_P / (100 * 12);

    alice.bank_account -= alice.mortgage.first_pay;
    
    alice.mortgage.month_pay = (alice.mortgage.sum - alice.mortgage.first_pay) \
    * month_percentage / (1 - ( 1/ pow((1 + month_percentage), SIMULATION_TIME*12)));

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
    alice.bank_account *= (1 + DEPOSIT_P / (100*12));

    bob.bank_account *= (1 + DEPOSIT_P / (100*12));
}


void month_expences()
{
    alice.bank_account -= alice.month_expences;
    alice.bank_account -= alice.mortgage.month_pay;

    bob.bank_account -= alice.month_expences;
    bob.bank_account -= bob.rent.month_pay;
}

void cat_keeping(int year, int month)
{
    if ((year == alice.cat.year_of_purchase) && (month == alice.cat.month_of_purchase));
        alice.bank_account -= alice.cat.cost;
    if ((year == alice.cat.year_of_purchase + alice.cat.duration_of_life) && (month == alice.cat.month_of_purchase));
        alice.bank_account -= alice.cat.funeral_cost;
}

void inflation_and_indexation()
{
    alice.month_expences *= (1 + INFLATION_P / 100);
    alice.earns *= (1 + IDEXATION_P / 100);

    bob.month_expences *= (1 + INFLATION_P / 100);
    bob.rent.month_pay *= (1 + INFLATION_P / 100);
    bob.earns *= (1 + IDEXATION_P / 100);
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
    
    while( !((year == start_year + SIMULATION_TIME) && (month == start_month + 1)) ) {
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
