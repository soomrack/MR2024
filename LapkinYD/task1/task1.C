#include <stdio.h>

typedef unsigned long long int Money; //calculating in the rubles

struct Mortgage
{
    double rate;
    Money home_price;
    Money first_payment;
    Money monthly_payment;
};

typedef struct 
{
    Money salary; //after taxes || montly
    Money food_expenses; //monthly
    Money entertainment_expenses;
    Money utilities; //town services for a home such as electricity, gas, etc. || montly
    Money bank_account; //monthly
    Mortgage mortgage;
    double deposite_rate;
    Money apartment_rent;
} Person;

void unit_alice(Person *alice)
{
    alice->salary = 300000;
    alice->food_expenses = 21000; //https://www.banki.ru/news/lenta/?id=10999620
    alice->entertainment_expenses = 21151; //https://secrets.tinkoff.ru/blogi-kompanij/skolko-tratyat-na-razvlecheniya/
    alice->utilities = 5545;//https://journal.tinkoff.ru/zakroy-vodu-stat-2024/
    alice->bank_account = 1000000;
    alice->deposite_rate = 0.2;
    alice->mortgage.first_payment = 1000000;
    alice->mortgage.home_price = 15*1000000;
    alice->mortgage.rate = 0.17;
    alice->mortgage.monthly_payment = 199595; //https://calcus.ru/kalkulyator-ipoteki?input=eyJjdXJyZW5jeSI6IlJVQiIsInR5cGUiOiIxIiwiY29zdCI6IjE1MDAwMDAwIiwic3RhcnRfc3VtIjoiMTAwMDAwMCIsInN0YXJ0X3N1bV90eXBlIjoiMSIsInBlcmlvZCI6IjMwIiwicGVyaW9kX3R5cGUiOiJZIiwicGVyY2VudCI6IjE3IiwicGF5bWVudF90eXBlIjoiMSJ9

}

void unit_bob(Person *bob)
{
    bob->salary = 200000;
    bob->food_expenses = 21000; //https://www.banki.ru/news/lenta/?id=10999620
    bob->entertainment_expenses = 21151; //https://secrets.tinkoff.ru/blogi-kompanij/skolko-tratyat-na-razvlecheniya/
    bob->utilities = 5545; //https://journal.tinkoff.ru/zakroy-vodu-stat-2024/
    bob->bank_account = 1000000;
    bob->deposite_rate = 0.2;
    bob->apartment_rent = 40000;
}

void get_salary(Person *person, double inflation)
{
    person->salary*(1.+inflation/12);
    person->bank_account += person->salary;
}

void spend_money(Person *person, double inflation)
{
    person->bank_account -= person->food_expenses*(1. + inflation/12);
    person->bank_account -= person->entertainment_expenses*(1. + inflation/12);
    person->bank_account -= person->utilities*(1. + inflation/12);
}

void mortgage_payment(Person *person)
{
    person->bank_account -= person->mortgage.monthly_payment;
}

void increase_deposite(Person *person)
{
    person->bank_account*(1. + person->deposite_rate);
}

void home_rent(Person *person, double inflation)
{
    person->bank_account -= person->apartment_rent*(1.+inflation/12);
}

void simulation(Person *alice, Person *bob, int starting_year, int starting_month, 
int simulation_years, double inflation)
{
    int year = starting_year;
    int last_year = starting_year + simulation_years;
    int month = starting_month;
    alice->bank_account -= alice->mortgage.first_payment;
    while(year < last_year || month < starting_month)
    {
        get_salary(alice, inflation);
        spend_money(alice, inflation);
        mortgage_payment(alice);
        increase_deposite(alice);

        get_salary(bob, inflation);
        home_rent(bob, inflation);
        spend_money(bob, inflation);
        increase_deposite(bob);

        if (month == 12)
        {
          month = 1;
          year++;
        }
        else 
            month++;
    }
}

void compare(Person *alice, Person *bob)
{
    Money alice_sum = alice->bank_account + alice->mortgage.home_price;
    Money bob_sum = bob->bank_account;
    if (alice_sum == bob_sum)
        printf("Alice and Bob have the same money, so mortgage is not an advantage\n");
    else if (alice_sum > bob_sum)
        printf("Alice has more money than Bob, so mortgage is the best choice for buying an apartment. Difference is %ll\n", alice_sum - bob_sum);
    else
        printf("Bob has more money than Alice, so simply accumulating money on the deposite is the best way for buying an apartment. Difference is %ll\n", bob_sum - alice_sum);
}

int main()
{
    Person alice, bob;
    int starting_year = 2024;
    int starting_month = 9; //semptember
    int simulation_years = 30; //how many years it will run
    double inflation = 0.08; //annual
    unit_alice(&alice);
    unit_bob(&bob);
    simulation(&alice, &bob, starting_year, starting_month, simulation_years, inflation);
    compare(&alice, &bob);

    return 0;
}


/*
Условия задачи:

Alice и Bob, стартовый капитал - 1.000.000 Р 
Alice эту сумму как первый взнос по ипотеке (15.000.000 Р) на 30 лет под 17% годовых (ставку рассчитать по онлайн-калькулятору)
Bob копит на квартиру и живет в съемной - 30.000 Р в месяц
Зарплата обоих составляет 200.000 Р в месяц
Стоит учитывать базовые потребности типа еды, комуналки, 
Все цены и зарплаты подвержены инфляции в 8% в год
*/