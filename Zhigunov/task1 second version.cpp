

#include <stdio.h>
#include<cmath>

typedef long long int Money; //рабоnаем в рублях

struct Person {
    Money salary;
    Money capital;
    double inflation;
    Money rent;
    Money flat;
    const char* name;
    Money utility;

};


struct Credit {
    double interest_percentage;
    Money deposit;
    Money cost;
    Money payment;
};


struct Person Alice, Bob;
struct Credit Alice_credit;
int year = 2024;
int month = 10;

void credit_init() {
    Alice_credit.deposit = 100 * 1000 * 100;
    Alice_credit.cost = 13 * 1000 * 1000 * 100;
    Alice_credit.interest_percentage = 0.16;
}

void alice_init() {
    Alice.salary = 200 * 1000 * 100;
    Alice.capital = 1000 * 1000 * 100;
    Alice.rent = 0;
    Alice.inflation = 0.09;
    Alice.utility = 10 * 1000 * 100;
    Alice.flat = 13 * 1000 * 1000 * 100;
    Alice.name = "Alice";
}


void bob_init() {
    Bob.salary = 200 * 1000 * 100;
    Bob.capital = 1000 * 1000 * 100;
    Bob.rent = 30 * 1000 * 100 ;
    Bob.inflation = 0.09;
    Bob.utility = 10 * 1000 * 100;
    Bob.flat = 0;
    Bob.name = "Bob";
}


void bob_salary() {
    if ((year - 2024) % 3 == 0 && month==1) {
        client.salary += 30 * 1000 * 100;
    }
    client.capital += client.salary;
}

void bob_utility_pay() {
    client.capital -= client.utility;
}

void bob_rent() {
    if ((year - 2024) % 4 == 0 && month == 1) {
        client.rent += 5 * 1000 * 100;
    }
    client.capital -= client.rent;
}

void bob_inflation() {
    client.utility *= client.inflation + 1;
}

void alice_salary() {
    if ((year - 2024) % 3 == 0 && month == 1) {
        client.salary += 30 * 1000 * 100;
    }
    client.capital += client.salary;
}

void alice_utility_pay() {
    client.capital -= client.utility;
}

void alice_credit_payment() {
    client.capital -= credit.payment;
}

void alice_inflation() {
    client.utility *= client.inflation + 1;
}

void ipoteka_calculation() {
    credit.payment = (credit.cost - credit.deposit) / (12 * 30) * (1 + credit.interest_percentage);
    client.capital -= credit.deposit;
}

void data_print() {
    printf("Month: %i\n", month);
    printf("Year: %i\n", year);
}

void result_print() 
{
    printf("%s", client1.name);
    printf(" have capital= %lli\n", client1.capital + client1.flat);
    printf("%s", client2.name);
    printf(" have capital= %lli\n", client2.capital + client2.flat);
    if (client1.capital+client1.flat > client2.capital+client2.flat) {
        printf("be liked %s\n", client1.name);
    } else if (client1.capital + client1.flat < client2.capital + client2.flat) {
        printf("be liked %s\n", client2.name);
    }
    else {
        printf("no differens");
    }

}


void simulation() {
    credit_init();
    ipoteka_calculation();
    while (year!=2054 || month!=10) {
        alice_salary();
        alice_credit_payment();
        alice_utility_pay();

        bob_salary();
        bob_rent();
        bob_utility_pay();

        month++;
        if (month == 12) {
            month = 1;
            year++;
            bob_inflation();
            alice_inflation();
        }
    }
    data_print();
    result_print();
}

int main()
{   
    bob_init();
    alice_init();
    simulation();
    return 0;
}