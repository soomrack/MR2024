#include <stdio.h>
#include <math.h>

typedef long long int Cash;  // rub

const float INFLATION = 1.1;
const float INDEXATION = 1.07;
const float DEPOSITE = 1.18;
const float TAX = 0.0001;

struct Person
{
    Cash salary;
    Cash check;
    Cash food;
    Cash transport;
    Cash person_expense;
    Cash expense;
    Cash flat_rent;
    Cash communal_bills;
    Cash flat_cost;
    Cash flat_tax;
};


struct Mortgage
{
    Cash monthly_pay;
    Cash loan_amount;
    float annual_rate;
    int term;
};


struct Person bob;
struct Person alice;
struct Mortgage alice_mortgage;


void bob_set()
{

    bob.salary = 200 * 1000;
    bob.food = 30 * 1000;
    bob.transport = 10 * 1000;
    bob.person_expense = 50 * 1000;
    bob.flat_rent = 30 * 1000;
    bob.check = 1000 * 1000;
    bob.expense = bob.food + bob.person_expense + bob.transport;
}


void alice_set()
{
    alice.flat_cost = 13 * 1000 * 1000;
    alice.salary = 200 * 1000;
    alice.food = 30 * 1000;
    alice.transport = 10 * 1000;
    alice.person_expense = 30 * 1000;
    alice.flat_rent = 5 * 1000;
    alice.check = 1000 * 1000;
    alice.flat_tax = 0;
    alice.expense = alice.food + alice.person_expense + alice.transport;
}


void mortgage_set()
{
    alice_mortgage.monthly_pay = 0;
    alice_mortgage.loan_amount = 13 * 1000 * 1000;
    alice_mortgage.annual_rate = 16;
    alice_mortgage.term = 30;
}


void bob_salary(const int month)
{
    bob.check += bob.salary;

    if (month == 12)
        bob.salary *= INDEXATION;
}


void bob_flat_rent(const int month)
{
    bob.check -= bob.flat_rent;

    if (month == 12) 
        bob.flat_rent *= INFLATION;
    
}


void bob_expense(const int month)
{
    bob.check -= bob.expense;

    if (month == 12) 
        bob.expense *= INFLATION;
    
}


void bob_deposite(const int month)
{
    if (month == 12)
        bob.check *= DEPOSITE;   
}


void alice_salary(const int month)
{
    alice.check += alice.salary;

    if (month == 12)
        alice.salary *= INDEXATION;
}


void alice_communal_bills(const int month)
{
    alice.check -= alice.flat_rent;

    if (month == 12) 
        alice.flat_rent *= INFLATION;
}


void alice_expense(const int month)
{
    alice.check -= alice.expense;

    if (month == 12)
        alice.expense *= INFLATION;
}


void alice_deposite(const int month)
{
    if (month == 12) 
        alice.check *= DEPOSITE;
}


void alice_flat_cost(const int month)
{
    if (month == 12)
        alice.flat_cost *= INFLATION;
}


void alice_mortgage_monthly_pay()
{
    double monthly_rate = alice_mortgage.annual_rate / 12.0 / 100.0;
    int total_month = alice_mortgage.term * 12;

    alice_mortgage.monthly_pay = alice_mortgage.loan_amount 
        * (monthly_rate * pow(1 + monthly_rate, total_month)) 
        / (pow(1 + monthly_rate, total_month) - 1);

    alice.check -= alice_mortgage.monthly_pay;
}


void alice_flat_tax(const int month, const int year)
{
    alice.check -= alice.flat_tax;

    if (month == 9)
        alice.flat_tax = alice.flat_cost * TAX;
   
    if (year == 2026)
        alice.check += alice.flat_tax;
}

void simulation(const int start_month,const int start_year)
{
    int year = start_year;
    int month = start_month;

    while (!(year == (start_year + 30) && month == start_month)) {

        bob_salary(month);
        bob_expense(month);
        bob_flat_rent(month);
        bob_deposite(month);

        alice_salary(month);
        alice_expense(month);
        alice_communal_bills(month);
        alice_deposite(month);
        alice_flat_cost(month);
        alice_flat_tax(month, year);
        alice_mortgage_monthly_pay();

        month++;
        if (month == 13) {
            month = 1;
            ++year;
        }
    }
    alice.check += alice.flat_cost;
}


void print_result()
{
    printf("Bob check = % lli rub\n", (Cash)(bob.check));
    printf("Alice check = % lli rub\n", (Cash)(alice.check));
}


int main()
{
    alice_set();
    bob_set();
    mortgage_set();

    simulation(9, 2024);

    print_result();

    return 0;
}
