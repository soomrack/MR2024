#include<stdio.h>


typedef long long int Money;  // RUB

typedef struct {
    Money account;
    Money salary;
} Person;

Person alice;



void alice_init()
{
    alice.account = 0;
    alice.salary = 200 * 1000;
}


void alice_salary(const int month, const int year)
{
    alice.account += alice.salary;

    if(month == 12) {
        alice.salary *= 1.07;
    }
}


void alice_print()
{
    printf("Alice account = %lld RUB\n", alice.account);
}

void simulation()
{
    int year = 2024;
    int month = 9;

    while( !((year == 2024 + 30) && (month == 10)) ) {

        alice_salary(month, year);
        // alice_deposite_pp();
        // alice_mortage_payment();
        // alice_food();
        // alice_home_bills();
        
        ++month;
        if(month == 13) {
            month = 1;
            ++year;
        }
    }
}


int main()
{
    alice_init();

    simulation();

    alice_print();
}
