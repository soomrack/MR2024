#include <stdio.h>

typedef long long int Money;

struct Mortgage{
    Money first_pay; 
    Money pay_a_mmonth; 
    Money sum; 
    double rate;
 };
 struct Person {
    Money account; 
    Money salary;
    Money expenses; 
    Money house; 
    Money rent;
    Money food; 
    Money cat;
    struct Mortgage mortgage; 
};


struct Person alice;
struct Person bob;


void alice_init() {
    alice.account = 1000 * 1000;
    alice.salary = 250 * 1000;
    alice.food = 15 * 1000;
    alice.expenses = 10 * 1000;
    alice.house = 5 * 1000;

    alice.mortgage.sum = 15 * 1000 * 1000;
    alice.mortgage.rate = 0.17;
    alice.mortgage.pay_a_mmonth = 199595; // посчитано в калькуляторе ипотеки
    alice.mortgage.first_pay = 1000 * 1000; 
    alice.account -= alice.mortgage.first_pay;  
 }


void alice_account(const int month, const int year)
{
    alice.account *= 1 + 0.20 / 12;
}


void alice_salary(const int month, const int year) 
{
    alice.account += alice.salary;

    if (month == 12) {
        alice.salary *= 1.08;
    }
}

void alice_mortgage_pay() 
{
    alice.account -= alice.mortgage.pay_a_mmonth;
}


void alice_food_pay(const int month, const int year)
{
   alice.account -= alice.food;

   if (month == 12) {
       alice.food *= 1.08;
   }
}


void alice_house_pay(const int month, const int year) 
{
   alice.account -= alice.house;

   if (month == 12) {
       alice.house *= 1.08;
   }
}


void alice_expenses_pay(const int month, const int year) 
{
   alice.account -= alice.expenses;

   if (month == 12) {
       alice.expenses *= 1.08;
   }
}


void alice_print()
{
    printf("Alice account = %lld RUB\n", alice.account);
}

void bob_init() 
{
   bob.account = 1000 * 1000;
   bob.salary = 250 * 1000;
   bob.rent = 70 * 1000;
   bob.food = 15 * 1000;
   bob.expenses = 10 * 1000;
   bob.house = 5 * 1000;
   bob.cat = 20 * 1000; 
}


void bob_account(const int month, const int year) 
{
   bob.account *= 1+0.2/12;
}


void bob_salary(const int month, const int year) 
{
    bob.account += bob.salary;

    if (month == 12) {
        bob.salary *= 1.08;
    }
    else if (year == 2030) {
      bob.salary = 300 * 1000;
    }
}


void bob_food_pay(const int month, const int year) 
{
   bob.account -= bob.food;

   if (month == 12) {
       bob.food *= 1.08;
   }
}

void bob_house_pay(const int month, const int year) 
{
   bob.account -= bob.house;

   if (month == 12) {
      bob.house *= 1.08;
   }
}

void bob_expenses_pay(const int month, const int year) 
{
   bob.account -= bob.expenses;

   if (month == 12) {
      bob.expenses *= 1.08;
   }
}


void bob_rent(const int month, const int year) 
{
   bob.account -= bob.rent;

   if (month == 12) {
      bob.rent *= 1.08;
   }
}


void bob_buy_a_cat(const int month, const int year) 
{
    if ((year == 2030) && (month == 3)) {
       bob.account -= bob.cat;
    }
}


void bob_print()
{
    printf("Bob capital = %lld RUB\n", bob.account);
}


void simulation() 
{
   int year = 2024;
   int month = 9;

   while (!((year == 2024+30) && (month == 10))) {
      alice_salary(month,year);
      alice_account(month,year);
      alice_food_pay(month,year);
      alice_mortgage_pay();
      alice_house_pay(month,year);
      alice_expenses_pay(month,year);

      bob_account(month,year);
      bob_salary(month,year);
      bob_food_pay(month,year);
      bob_house_pay(month,year);
      bob_expenses_pay(month,year);
      bob_rent(month,year);
      bob_buy_a_cat(month,year);

      month += 1;
      if (month == 13) {
         month = 1;
         year += 1; 
      }
   }
}

int main()
{
    alice_init();
    
    bob_init();

    simulation();

    alice_print();

    bob_print();
}