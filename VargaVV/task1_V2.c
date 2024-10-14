#include <stdio.h>
#include <math.h>
#include <stdbool.h>
typedef long long int Money;  
int START_YEAR = 2024;
int SIMULATION_TIME = 30;
int MONTH_START = 9;


struct Mortgage{
    double rate;  // ставка по ипотеке
    Money payment;  // ежемесячный платёж по ипотеке
};


struct Car{
    Money cost;  
    Money mounthly_service;  
    Money winter_rubber;
};


//Общие переменные
struct Person {
    Money  balance;
    Money  salary;
    Money  credit;
    Money  expense;
    Money  flat;
    double inflation;
    double deposit_rate;
    struct Mortgage mortgage;
    bool is_car;
};

struct Person alice; 
struct Person bob;
struct Car lada_granta;



void alice_init()  
{
    alice.balance = pow(10,6); 
    alice.salary = 2 * pow(10,5);
    alice.credit = 15 * pow(10,6);
    alice.mortgage.rate = 0.2 / 12;
    alice.deposit_rate = 0.12 / 12;
    alice.expense = 30 * 1000;
    alice.inflation = 0.08 / 12;
    alice.mortgage.payment = (alice.credit - alice.balance) * 
    (((alice.mortgage.rate)*(pow((1. + alice.mortgage.rate),(SIMULATION_TIME)*12.))) 
    / (pow(1. + (alice.mortgage.rate),(SIMULATION_TIME)*12.) - 1.));
    alice.balance = 0;
}


//Данные Боба
void bob_init()
{
    bob.balance = pow(10,6);
    bob.salary = 2 * pow(10,5);
    bob.credit = 0;
    bob.mortgage.rate = 0;
    bob.deposit_rate = 0.12 / 12;
    bob.expense = 30 * 1000;
    bob.mortgage.payment = 30 * 1000;
    bob.flat = 15 * pow(10,6);
    bob.inflation = 0.08 / 12;
    bob.is_car = 0;
}

void car_init()
{
lada_granta.cost = 2 * pow(10,6);
lada_granta.mounthly_service = 20 * 1000;
lada_granta.winter_rubber = 24 * 1000;
}


void alice_salary(const int month)
{
    alice.balance += alice.salary;
    alice.salary += alice.salary * alice.inflation;
     if(month % 4 == 0){
        alice.balance += alice.salary * 0.2;
    }
     if(month == 12){
        alice.balance += alice.salary * 0.5;
    }
}


void bob_salary(const int month)
{
    bob.balance += bob.salary;
    bob.salary += bob.salary * bob.inflation;
     if(month % 4 == 0){
        bob.balance += bob.salary * 0.2;
    }
     if(month == 12){
        bob.balance += bob.salary * 0.5;
    }
}


//Расходы
void alice_expenses()
{
    alice.balance = alice.balance - alice.mortgage.payment - alice.expense;
    alice.expense += alice.expense * alice.inflation;
}


void bob_expenses()
{
    bob.balance = bob.balance - bob.mortgage.payment - bob.expense;
    bob.flat += bob.flat * bob.inflation; 
    bob.expense += bob.expense * bob.inflation;
}


void bob_car(const int month)
{
    lada_granta.cost += lada_granta.cost * bob.inflation;
    lada_granta.mounthly_service += lada_granta.mounthly_service * bob.inflation;
    lada_granta.winter_rubber += lada_granta.winter_rubber * bob.inflation;

    if (bob.balance > (lada_granta.cost + lada_granta.cost * 0.5) && (bob.is_car!=1)){
        bob.is_car = true;
        bob.balance -= lada_granta.cost;
    }

    if (bob.is_car!=1){
        bob.balance -= lada_granta.mounthly_service;

        if (month == 10){
            bob.balance -= lada_granta.winter_rubber;  
         }
    }
}


//Вклады
void alice_deposit_rate()
{
    alice.balance +=  alice.balance * alice.deposit_rate;
}


void bob_deposit_rate()
{
    bob.balance += bob.balance * bob.deposit_rate;
}


void calculations()
{
    int month = MONTH_START;
    int year=START_YEAR;
    while((month < MONTH_START) || (year < START_YEAR+SIMULATION_TIME)){

            alice_salary(month); 
            alice_expenses(); 
            alice_deposit_rate(); 

            bob_salary(month); 
            bob_expenses(); 
            bob_car(month);
            bob_deposit_rate(); 

            month++;

            if(month==12){year++; month=1;}
        }
        
}


void result()
{
if (bob.balance > alice.balance+bob.flat) {
        printf("The winner is bob\n"); 
        printf("bob.balance: %lld\n", bob.balance);
        printf("alice.balance: %lld\n", alice.balance);
        printf("bob.flat: %lld\n", bob.flat);
        printf("lada_granta: %lld\n", lada_granta.cost);
    }
    else {
        printf("The winner is alice\n");
        printf("alice.balance:%lld\n",alice.balance);
        printf("bob.balance: %lld\n", bob.balance);
        printf("bob.flat: %lld\n", bob.flat);
        printf("lada_granta: %lld\n", lada_granta.cost);
    }
}


int main()
{

    alice_init();
    bob_init();
    car_init();

    calculations();

    result();  

return 0;
}

