#include <stdio.h>
#include <math.h>

int year=2024;
int month=9;
int simulaton_time=30;

typedef long long int money; // в копейках

const int inflation_p = 9; // процент инфляции

const int deposit_p = 20; // годовой процент депозита

const float mortgage_p = 16; // ставка по ипотеке

typedef struct person
{
    money bank_account;
    money earns;
    money rent;
    money mortgage;
    money month_expences;
} person;

person alice;
person bob;


void alice_data()
{
    alice.bank_account=(1000*1000)*100;
    alice.earns=(200*1000)*100;
    alice.rent=0;
    alice.mortgage=(13*1000*1000)*100;
    alice.month_expences=(20*1000)*100;
}

void bob_data()
{
    bob.bank_account=(1000*1000)*100;
    bob.earns=(200*1000)*100;
    bob.rent=(30*1000*1000)*100;
    bob.mortgage=0;
    bob.month_expences=(20*1000)*100;
}

money mortgage_pay()
{
    float month_percentage = mortgage_p/(100*12);

    money mortgage_payment = (alice.mortgage - alice.bank_account) * month_percentage / (1 - ( 1/ pow((1 + month_percentage), simulaton_time*12)));
    
    return mortgage_payment;
}

int main()
{
    alice_data();
    bob_data();

    sim();

    compare();

    return 0;
}
