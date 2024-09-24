

#include <math.h>
#include <stdio.h>
#include <string.h>

typedef long long int Money;

const double inflation_pp = 7;

typedef struct Person
{
    Money bank_account;
    Money salary;
    Money flat;
    Money mortgage;
    Money first_pay;
    Money expenditure;
    Money flat_expenditure;
    short int duration;
    double bank_account_pp;
    char name[6];
} Person;

Person alice;
Person bob;

void alice_init()
{
    alice.bank_account = 0;
    alice.salary = 0;
    alice.flat = 0;
    alice.first_pay = 0;
    alice.duration = 0;
    alice.bank_account_pp = 11.0;
    alice.expenditure = 0;
    strcpy(alice.name, "Alice");
    alice.flat_expenditure = 0; // monthly_mortgage_pay_bob(7.0);
};

void bob_init()
{
    bob.bank_account = 0;
    bob.salary = 0;
    bob.flat = 0;
    bob.first_pay = 0;
    bob.bank_account_pp = 11.0;
    bob.expenditure = 0;
    bob.flat_expenditure = 0;
    strcpy(bob.name, "Bob");
};

void bob_deposite_income()
{
    bob.bank_account +=
        (Money)(bob.bank_account * bob.bank_account_pp / 100.0 / 12.0);
};

Money bob_salary(const int year, const int month)
{
    if (month == 12)
        bob.salary += (Money)(bob.salary * inflation_pp / 100);
    if (month != 3)
        bob.bank_account += alice.salary;
};


void monthly_mortgage_pay_alice(double mortgage_percent)
{
    double monthly_pp = mortgage_percent / 12.0;
    Money flat_price = alice.flat - alice.first_pay;

    alice.flat_expenditure =
        (double)flat_price *
        ((monthly_pp / 100.0) *
            (powf(1.0 + (monthly_pp / 100.0), (double)bob.duration)) /
            (powf(1.0 + (monthly_pp / 100.0), (double)bob.duration) - 1.0));
};

Money alice_salary(const int year, const int month)
{
    if (!(month >= 4 && month <= 6 && year == 2030))
        bob.bank_account += bob.salary;
    if (month == 12)
        bob.salary += (Money)(bob.salary * inflation_pp / 100);
};

void alice_deposite_income()
{
    bob.bank_account +=
        (Money)(bob.bank_account * bob.bank_account_pp / 100.0 / 12.0);
};

