#include <stdio.h>

typedef long long int Money; //коп.
const double INFLATION = 1.09;

typedef struct  {
    Money sum;
    Money first_pay;
    Money monthly_payment;
    double rate;
} Mortgage;

typedef struct  {
    Money pledge;
    Money monthly_chek;
} Rent;

typedef struct  {
    Money price;
    Money petrol;
    Money technical;
    Money insurance;
    double tax; 
} Car;

typedef struct  {
    Money salary;
    Money count;

    Mortgage mortgage;
    Rent rent;
    Car car;
    Money house_price;
    double house_tax;
    Money utilities;

    Money food_cost;
    Money selth_expenses;

    double deposit_rate;
    double inflation_rate;
} Person; 

Person alice;
Person bob;

//Alice

void alice_init()
{
    alice.salary = 200 * 1000 * 100;  
    alice.count = 1000 * 1000 * 100;

    alice.mortgage.sum = 14 * 1000 * 1000 * 100;
    alice.mortgage.first_pay = 1000 * 1000 * 100;
    alice.mortgage.monthly_payment = 241582 * 100;   //source: https://www.sberbank.ru/ru/person/credits/home/ipotechniy-kalkulyator
    alice.count -= alice.mortgage.first_pay;

    alice.house_price = alice.mortgage.sum;
    alice.house_tax = 0.001;                         //Налог на имущество
    alice.utilities = 4 * 1000 * 100;                //Коммунальные услуги

    alice.food_cost = 15 * 1000 * 100;
    alice.selth_expenses = 10 * 1000 * 100;          //Расходы на товары для дома, одежду, косметику и пр.
    
    alice.deposit_rate = 0.2;
    alice.inflation_rate = 0.09;
}


void print_alice()
{
    printf("Alice capital - %lld rub \n", (Money)(alice.count / 100)); //руб.
    printf("Alice capital - %lld mln rub \n", (Money)(alice.count / 100 / 1000 / 1000));
}


void alice_salary(const int month)
{
    if(month == 1){
        alice.salary *= 1.07;                        //Индексация ЗП
    }
    alice.count += alice.salary;
}

void alice_mortgage(const int month)
{
    alice.count -= alice.mortgage.monthly_payment;
}


void alice_computer(const int month, const int year)
{
    if( month == 11 && year == 2027) {
        alice.count -= 150 * 1000 * 100;             //Покупка ноутбука
    }
}


void alice_monthly_payments(const int month)
{
    if (month == 12) {
        alice.food_cost *= INFLATION;
        alice.utilities *= INFLATION;
        alice.selth_expenses *= INFLATION;
    }
    alice.count -= alice.food_cost; 
    alice.count -= alice.utilities;
    alice.count -= alice.selth_expenses;
}


void alice_house(const int month, const int year)
{
    if (month == 1) alice.house_price *= 1.07;
    if (month == 12) alice.count -= alice.house_price * alice.house_tax;
}

void alice_deposit()
{
    alice.count += alice.count * alice.deposit_rate / 12;
}

//Bob

void bob_init()
{
    bob.salary = 200 * 1000 * 100; 
    bob.count = 1000 * 1000 *100;

    bob.rent.pledge = 30 * 1000 * 100;
    bob.rent.monthly_chek = 34 * 1000 * 100;            //Аренда
    bob.count -= bob.rent.pledge;
    bob.utilities = 4 * 1000 * 100;                     //КУ

    bob.food_cost = 18 * 1000 * 100;
    bob.selth_expenses = 5 * 1000 * 100;                //Оплата бытовых покупок, одежды и пр.

    bob.car.price = 2 * 1000 * 1000 * 100;
    bob.car.petrol = 15 * 1000 * 100;
    bob.car.technical = 10 * 1000 * 100;                //Техническое обслуживание машины
    bob.car.insurance = 7 * 1000 * 100;                 //Страховка
    bob.car.tax = 4 * 1000 * 100;                       //Налог на машину

    bob.deposit_rate = 0.2;
    bob.inflation_rate = 0.09;
}


void bob_salary(const int month)
{
    if(month == 1){
        bob.salary *= 1.07;                             //Индексация ЗП
    }
    
    bob.count += bob.salary;
}

void bob_rent(const int month)
{
    if (month == 12) {
        bob.rent.monthly_chek *= INFLATION;
    }
    bob.count -= bob.rent.monthly_chek;
}

void bob_house_cost(const int month)
{
    if (month == 12) {
        bob.utilities *= INFLATION;
    }
    bob.count -= bob.utilities;
}

void bob_monthly_payments(const int month)
{
    if (month == 12) {
        bob.food_cost *= INFLATION;
        bob.selth_expenses *= INFLATION;
    }
    bob.count -= bob.food_cost;
    bob.count -= bob.selth_expenses;
}

void bob_car(const int month, const int year)
{
    if (month == 5 && year == 2026) {
        bob.count -= bob.car.price;
    }
    while (month > 5 && 2030 > year > 2026) 
    {
        if (month == 12) {
            bob.car.petrol *= INFLATION;
        }
        bob.count -= bob.car.petrol;
        if (month == 11 || month == 5) {
            bob.count -= bob.car.technical;
        }
        if (month == 12) {
            bob.count -= bob.car.tax;
            bob.count -= bob.car.insurance;
        }
    }
    if (month == 1) {
        bob.car.price *= 1.01;
    }
    
}

void bob_deposit() 
{
    bob.count += bob.count * bob.deposit_rate / 12;
}

void print_bob()
{
    printf("Bob capital - %lld rub \n", (Money)(bob.count / 100));                    //руб.
    printf("Bob capital - %lld mln rub \n", (Money)(bob.count / 100 / 1000 / 1000));
}


void simulation()
{
    int month = 9;
    int year = 2024;

    while ( !((year == 2024 + 30) && (month == 9)) ) {
        alice_salary(month);
        alice_mortgage(month);
        alice_computer(month, year);
        alice_house(month, year);
        alice_monthly_payments(month);

        alice_deposit();

        bob_salary(month);
        bob_rent(month);
        bob_monthly_payments(month);
        bob_house_cost(month);
        bob_car(month, year);

        bob_deposit();
        
        month ++;
        if(month == 13) {
            month = 1;
            year++;
        }
    }
}

//End

int main() 
{
    alice_init();
    bob_init();

    simulation();

    print_alice();
    print_bob();
    return 0;
}
