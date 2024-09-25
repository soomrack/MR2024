#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef long long int Money; // RUB

typedef struct {
    Money savings;
    Money salary;
    Money monthly_fee; 
    Money expenses; 
    Money property_value; 
}Person;

Person alice;
Person bob;


static void alice_init()
{
    alice.savings = 0;
    alice.salary = 200000;
    alice.monthly_fee = 161370;  // monthly mortgage payment, taken from the website banki.ru
    alice.expenses = 0;
    alice.property_value = 13000000;
}


static void bob_init()
{
    bob.savings = 1000000;
    bob.salary = 200000;
    bob.monthly_fee = 60000;
    bob.expenses = 0;
}


static void alice_salary(const int month)
{
    alice.savings += alice.salary;
        if (month == 12) {
            alice.salary *= 1.02;
        }
        if (month == 12) {
            int chance_carier = (rand() % 15);
            if (chance_carier == 7)
            {
                alice.salary *= 1.2;
            }
            if (chance_carier == 14)
            {
                alice.salary *= 0.8;
            }
        }
}


static void alice_expenses(const int month)
{
    alice.expenses = 0;
    Money utility_expenses = 10000;
    Money food_expenses = alice.salary * 0.12;
    Money other_expenses = alice.salary * 0.05;
    alice.expenses += utility_expenses + food_expenses + other_expenses + alice.monthly_fee;

    if (month == 12) {
        utility_expenses *= 1.02;
        food_expenses *= 1.02;
        other_expenses *= 1.02;
        alice.expenses += utility_expenses + food_expenses + other_expenses + alice.monthly_fee;
    }
    int chance_random_expenses = (rand() % 10);
    int sum_random_expenses = (rand() % 100);
    if (chance_random_expenses == 8) {
        Money expense = sum_random_expenses * 1000;
        if (alice.savings >= expense) {
            alice.savings -= expense;
        }
        else {
            alice.savings = 0;
        }
    }
}


static void alice_savings(const int month)
{
    alice.savings -= alice.expenses;
    if (month == 12) {
        alice.savings *= 1.2;
    }
}


static void bob_salary(const int month)
{
    bob.savings += bob.salary;
    if (month == 12) {
        bob.salary *= 1.02;
    }
    if (month == 12) {
        int chance_carier = (rand() % 15);
        if (chance_carier == 7)
        {
            bob.salary *= 1.2;
        }
        if (chance_carier == 14)
        {
            bob.salary *= 0.8;
        }
    }
}


static void bob_expenses(const int month)
{
    bob.expenses = 0;
    Money utility_expenses = 10000;
    Money food_expenses = bob.salary * 0.12;
    Money other_expenses = bob.salary * 0.05;
    bob.expenses += utility_expenses + food_expenses + other_expenses + bob.monthly_fee;

    if (month == 12) {
        utility_expenses *= 1.02;
        food_expenses *= 1.02;
        other_expenses *= 1.02;
        bob.expenses += utility_expenses + food_expenses + other_expenses + bob.monthly_fee;
    }
    int chance_random_expenses = (rand() % 10);
    int sum_random_expenses = (rand() % 100);
    if (chance_random_expenses == 8) {
        Money expense = sum_random_expenses * 1000;
        if (bob.savings >= expense) {
            bob.savings -= expense;
        }
        else {
            bob.savings = 0;
        }
    }
}


static void bob_savings(const int month)
{
    bob.savings -= bob.expenses;
    if (month == 12) {
        bob.savings *= 1.2;
    }
}


static void bob_cat_expenses(const int year, const int month)
{
    bob.savings -= 5000;
    if (month == 12) {
        bob.savings -= 20000;  // veterinarian
    }

}


static void bob_cat_income(const int year, const int month)
{
    while (!((year > 2024 + 6) && (year < 2024 + 14))) {
        if (month == 12) {
            int number_kittens = (4 + rand() % 5)
                bob.savings += number_kittens * 10 * 1000
        }

    }

}


static void bob_cat(const int year, const int month)
{
    while (!((year > 2024 + 5) && (year < 2024 + 21))) {
        if (!((year == 2024 + 6) && (month == 1))) {
            bob.savings -= 25 * 1000;
        }
        bob_cat_expenses(year, month);
        bob_cat_income(year, month);
    }
}


static void print_info_alice() {
    printf("Alice savings = %lld RUB\n", alice.savings + alice.property_value);
}


static void print_info_bob() {
    printf("Bob savings = %lld RUB\n", bob.savings);
}


static void simulation() 
{
    int year = 2024;
    int month = 9;

    while (!((year == 2024 + 30) && (month == 10))) {

        alice_salary(month); 
        alice_expenses(month);  // коммунальные услуги, еда, ежемесячная плата
        alice_random_expenses();
        alice_savings(month);

        bob_salary(month);
        bob_expenses(month);
        bob_random_expenses();
        bob_savings(month);
        bob_cat(year, month);

        ++month;
        if (month == 13)
        {
            month = 1;
            ++year;
        }
    }
}


int main() {
    srand(time(NULL));

    alice_init();
    bob_init();

    simulation();

    print_info_alice();
    print_info_bob();

    return 0;
}
