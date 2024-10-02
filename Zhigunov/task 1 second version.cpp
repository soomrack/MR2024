

#include <stdio.h>
#include<cmath>

typedef long long int Money; //рабоnаем в рублях

struct Person {
    Money salary;
    Money account;
    double inflation;
    Money rent;
    Money flat;
    const char* name;
    Money utility;
    Money vklad;

};


struct Credit {
    double interest_percentage;
    Money first_deposishion;
    Money cost;
    Money payment;
};


struct Person Alice;
struct Person Bob;
struct Credit Alice_credit;
int year = 2024;
int month = 10;


void credit_init()
{
    Alice_credit.first_deposishion = 100 * 1000 * 100;
    Alice_credit.cost = 13 * 1000 * 1000 * 100;
    Alice_credit.interest_percentage = 0.16;
}

void alice_init()
{
    Alice.salary = 200 * 1000 * 100;
    Alice.account = 1000 * 1000 * 100;
    Alice.rent = 0;
    Alice.inflation = 0.09;
    Alice.utility = 10 * 1000 * 100;
    Alice.flat = 13 * 1000 * 1000 * 100;
    Alice.name = "Alice";
    Alice.vklad = 0;
}


void bob_init()
{
    Bob.salary = 200 * 1000 * 100;
    Bob.account = 1000 * 1000 * 100;
    Bob.rent = 30 * 1000 * 100;
    Bob.inflation = 0.09;
    Bob.utility = 10 * 1000 * 100;
    Bob.flat = 0;
    Bob.name = "Bob";
    Bob.vklad = 0;
}


void bob_salary()
{
    if (year % 3 == 2 && month == 1) {
        Bob.salary += 30 * 1000 * 100;
    }
    Bob.account += Bob.salary;
}

void bob_utility_pay()
{
    if (month == 1) {
        Bob.utility *= Bob.inflation + 1;
    }
    Bob.account -= Bob.utility;
}

void bob_rent()
{
    if (year % 4 == 0 && month == 1) {
        Bob.rent += 5 * 1000 * 100;
    }
    Bob.account -= Bob.rent;
}

void bob_vklad()
{
    Bob.vklad += Bob.account;
    Bob.vklad *= 1 + (0.19 / 12);
    Bob.account = 0;
}

void alice_salary()
{
    if (year % 3 == 2 && month == 1) {
        Alice.salary += 30 * 1000 * 100;
    }
    Alice.account += Alice.salary;
}

void alice_utility_pay()
{
    if (month == 1) {
        Alice.utility *= Alice.inflation + 1;
    }
    Alice.account -= Alice.utility;
}

void alice_credit_payment()
{
    Alice.account -= Alice_credit.payment;
}

void alice_vklad()
{
    Alice.vklad += Alice.account;
    Alice.vklad *= 1 + (0.19 / 12);
    Alice.account = 0;
}

void ipoteka_calculation()
{
    Alice_credit.payment = (Alice_credit.cost - Alice_credit.first_deposishion) / (12 * 30) * (1 + Alice_credit.interest_percentage);
    Alice.account -= Alice_credit.first_deposishion;
}

void data_print()
{
    printf("Month: %i\n", month);
    printf("Year: %i\n", year);
}

void result_print()
{
    printf("%s", Alice.name);
    printf(" have account= %lli\n", Alice.vklad + Alice.flat);
    printf("%s", Bob.name);
    printf(" have account= %lli\n", Bob.vklad + Bob.flat);
    if (Alice.vklad + Alice.flat > Bob.vklad + Bob.flat) {
        printf("be liked %s\n", Alice.name);
    }
    else if (Alice.vklad + Alice.flat < Bob.vklad + Bob.flat) {
        printf("be liked %s\n", Bob.name);
    }
    else {
        printf("no differens");
    }

}


void simulation()
{
    credit_init();
    ipoteka_calculation();
    while (year != 2054 || month != 10) {
        alice_salary();
        alice_credit_payment();
        alice_utility_pay();
        alice_vklad();

        bob_salary();
        bob_rent();
        bob_utility_pay();
        bob_vklad();

        month++;

        if (month == 12) {
            month = 1;
            year++;
        }
    }
}

int main()
{
    bob_init();
    alice_init();
    credit_init();
    simulation();
    data_print();
    result_print();
    return 0;
}