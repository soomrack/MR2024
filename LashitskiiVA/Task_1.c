#include <stdio.h>
#include <math.h>


typedef unsigned long long int Money; // копейки

const double IdePer = 7.;
const double InfPer = 9.;
const double MorPer = 16.;
const double DepPer = 20.;
const int t = 30;
const Money Tran = 3 * 1000 * 100; 
const Money House = 7 * 1000 * 100;
const Money Pers = 10 * 1000 * 100;
const Money Food = 15 * 1000 * 100;

struct Rent 
{
    Money month_pay;
};

struct Mortgage 
{
    Money sum;
    Money first_pay;
    Money month_pay;
};

struct Cat 
{
    int Year;
    int Month;
    Money cost;
    Money Food;
    Money Med;
    Money Funeral;
    int Dur;
};

typedef struct
 {
    Money bank_account;
    Money deposit;
    Money salary;
    Money month_expences;

    struct Mortgage mortgage;
    struct Rent rent;
    struct Cat cat;
} Person;


Person alice;
Person bob;


void alice_data_init() 
{
    alice.bank_account = (1000 * 1000) * 100;
    alice.deposit = 0;
    alice.salary = (200 * 1000) * 100;
    alice.month_expences = 0;
    alice.mortgage.sum = (13 * 1000 * 1000) * 100;
    alice.mortgage.first_pay = (1000 * 1000) * 100;


    alice.cat.Year = 2030;
    alice.cat.Month = 9;
    alice.cat.cost = (50 * 1000) * 100;
    alice.cat.Food = (3 * 1000) * 100;
    alice.cat.Med = (2 * 1000) * 100;
    alice.cat.Funeral = (30 * 1000) * 100;
    alice.cat.Dur = 12;
}


void bob_data_init() 
{
    bob.bank_account = (1000 * 1000) * 100;
    bob.deposit = 0;
    bob.salary = (200 * 1000) * 100;
    bob.rent.month_pay = (70 * 1000) * 100;
    bob.month_expences = 0;
}


void mortgage_month_pay() 
{
    double month_percentage = MorPer / (100 * 12);

    alice.bank_account -= alice.mortgage.first_pay;
    
    alice.mortgage.month_pay = (alice.mortgage.sum - alice.mortgage.first_pay) \
    * month_percentage / (1 - ( 1/ pow((1 + month_percentage), t*12)));

    alice.mortgage.month_pay = round (alice.mortgage.month_pay);
    (Money)alice.mortgage.month_pay;
}


void alice_cat_expences(const int year, const int month) 
{
    if ((year == alice.cat.Year) && (month == alice.cat.Month));
        alice.bank_account -= alice.cat.cost;

    if ((year == alice.cat.Year + alice.cat.Dur) && (month == alice.cat.Month));
        alice.bank_account -= alice.cat.Funeral;

    if ((year >= alice.cat.Year + alice.cat.Dur) && (month >= alice.cat.Month));
        alice.bank_account -= alice.cat.Food;
        alice.bank_account -= alice.cat.Med;

    if (month == 12) {
            alice.cat.cost *= (1. + IdePer / (100*12));
            alice.cat.Funeral *= (1. + IdePer / (100*12));
            alice.cat.Food = (1. + IdePer / (100*12));
            alice.cat.Med = (1. + IdePer / (100*12));
        }
}


void alice_salary(const int year, const int month) 
{
    
    alice.bank_account += alice.salary;
    if (month == 12) {
            alice.salary *= (1. + IdePer / (100*12));
        }
}


void bob_salary(const int year, const int month)
{
    bob.bank_account += bob.salary;
    if (month == 12) {
            bob.salary *= (1. + IdePer / (100*12));
        }
}


void alice_Pers_expences(const int year, const int month, const int start_year, const int start_month)
{
    if ((year == start_year) && (month == start_month)) {
        alice.month_expences = Food + Tran + House + Pers;
    }

    alice.bank_account -= alice.month_expences;

    if (month == 12) {
            alice.month_expences *= (1. + InfPer / (100*12));
        }
}


void alice_mortgage_expences(const int year, const int month) 
{
    alice.bank_account -= alice.mortgage.month_pay;
}


void bob_Pers_expences(const int year, const int month, const int start_year, const int start_month) 
{
    if ((year == start_year) && (month == start_month)) {
        bob.month_expences = Food + Tran + House + Pers;
    }

    bob.bank_account -= bob.month_expences;

    if (month == 12) {
            bob.month_expences *= (1. + InfPer / (100*12));
        }
}


void bob_rent_expences(const int year, const int month) 
{
    bob.bank_account -= bob.rent.month_pay;

    if (month == 12) {
            bob.rent.month_pay *= (1. + InfPer / (100*12));
        }
}


void alice_deposit() 
{
    alice.deposit *= (1. + DepPer / (100*12));
    alice.deposit += alice.bank_account;
    alice.bank_account = 0;
}


void bob_deposit() 
{
    bob.deposit *= (1. + DepPer / (100*12));
    bob.deposit += bob.bank_account;
    bob.bank_account = 0;
}


void print() 
{
    alice.deposit += alice.mortgage.sum;

    printf("Alice capital = %lld\nBob capital   = %lld\n", alice.deposit, bob.deposit);
}


void simulation() 
{
    mortgage_month_pay();
    
    const int start_year = 2024;
    const int start_month = 9;

    int year = start_year;
    int month = start_month;

    int end_year = start_year + t;

    int end_month = (start_month == 12) ? (end_year++, 1) : (start_month + 1);

    while( !((year == end_year) && (month == end_month)) ) {

        alice_salary(year, month);  
        alice_Pers_expences(year, month, start_year, start_month);  
        alice_mortgage_expences(year, month);
        alice_cat_expences(year, month);
        alice_deposit(); 
        
        bob_salary(year, month); 
        bob_Pers_expences(year, month, start_year, start_month);
        bob_rent_expences(year, month);
        bob_deposit();
        
        ++month;
        if(month == 13) {
            month = 1;
            ++year;
        }
    }
}


int main()
{
    alice_data_init();
    bob_data_init();

    simulation();

    print();
    
    return 0;
}