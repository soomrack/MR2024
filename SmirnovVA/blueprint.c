#include<stdio.h>


float ind = 1.07;
float infl = 1.13;

typedef double Money;  // RUB

typedef struct {
    Money account;
    Money salary;
    Money ipoteka_sum;
    Money ipoteka_perc;
    Money bytovuha;
    Money mesyachny_platezh;
    Money downplayment;


} Person;

Person alice;
Person bob;



void alice_init()
{
    alice.account = 1000000;
    alice.salary = 200 * 1000;
    alice.ipoteka_sum = 13*1000*1000;
    alice.downplayment = 900*1000;
    alice.ipoteka_perc = 16;
    alice.bytovuha = 30000;
    alice.mesyachny_platezh = 162715.6;
}


void alice_salary(const int month)
{
    alice.account += alice.salary;

    if(month == 12) {
        alice.salary *= ind;
    }
}



void alice_ipoteka_payment()
{
    //  na 12 mesyac
    alice.ipoteka_sum -= alice.mesyachny_platezh;
    alice.account -= alice.mesyachny_platezh;
}

void alice_traty(const int month)
{
    alice.account-= alice.bytovuha;
    if (month == 12)
    {
        alice.bytovuha*=infl;
    }
}


void alice_print()
{
    printf("Alice account = %f RUB\n", alice.account);
}

void simulation()
{
    int year = 2024;
    int month = 9;

    while( !((year == 2024 + 30) && (month == 10)) ) 
    {

        alice_print();

        alice_salary(month);
        alice_ipoteka_payment();
        
        alice_traty(month);

        alice_print();
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
