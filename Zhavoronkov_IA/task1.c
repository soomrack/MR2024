#include <stdio.h>

typedef unsigned long long int Money;  // расчёт в рублях

const int START_YEAR = 2024;
const int START_MONTH = 9;
const int PERIOD = 30;  // расчетный период симуляции


struct Mortgage
{
    Money sum;
    Money first_pay;
    Money monthly_payments;
    double rate;
};


struct Cat
{
    Money buy;
    Money cost;
    Money funeral;
};


struct Person
{
    Money salary;
    Money account;
    Money house_price;
    Money life_cost;
    Money monthly_rent;
    struct Mortgage mortgage;
    struct Cat cat;
    double deposit_rate;
    double inflation_index;
};

struct Person alice;
struct Person bob;


void alice_init()
{
    alice.account = 1 * 1000 * 1000;
    alice.salary = 270 * 1000;
    alice.life_cost = 30 * 1000;

    alice.deposit_rate = 0.2;
    alice.inflation_index = 0.08;

    alice.mortgage.sum = 15 * 1000 * 1000;
    alice.mortgage.first_pay = 1 * 1000 * 1000;
    alice.mortgage.rate = 0.17;
    alice.mortgage.monthly_payments = 200 * 1000;  //url: https://calcus.ru/kalkulyator-ipoteki?input=eyJjdXJyZW5jeSI6IlJVQiIsInR5cGUiOiIxIiwiY29zdCI6IjE1MDAwMDAwIiwic3RhcnRfc3VtIjoiMTAwMDAwMCIsInN0YXJ0X3N1bV90eXBlIjoiMSIsInBlcmlvZCI6IjMwIiwicGVyaW9kX3R5cGUiOiJZIiwicGVyY2VudCI6IjE3IiwicGF5bWVudF90eXBlIjoiMSJ9
    alice.account -= alice.mortgage.first_pay;
    alice.house_price = alice.mortgage.sum;
}


void alice_salary(const int month)
{
    if (month == 1) {
        alice.salary *= (1. + alice.inflation_index);
    }

    alice.account += alice.salary;
}


void alice_mortgage(const int month)
{
    alice.account -= alice.mortgage.monthly_payments;
}


void alice_life_cost(const int month)
{
    if (month == 1) {
        alice.life_cost *= (1. + alice.inflation_index);
    } 

    alice.account -= alice.life_cost;
}


void alice_house_price(const int month)
{
    if (month == 1) {
        alice.house_price *= (1. + alice.inflation_index);
    }
}


void alice_deposit()
{
    alice.account += alice.account * (alice.deposit_rate / 12);
}


void alice_print()
{
    printf("Alice summary capital: %lld \n", alice.account + alice.house_price);
}




void bob_init()
{
    bob.account = 1 * 1000 * 1000;
    bob.salary = 270 * 1000;
    bob.life_cost = 30 * 1000;
    bob.cat.buy = 30 * 1000;
    bob.cat.cost = 12 * 1000;
    bob.cat.funeral = 45 * 1000;

    bob.deposit_rate = 0.2;
    bob.inflation_index = 0.08;

    bob.monthly_rent = 35 * 1000; //url: https://spb.cian.ru/rent/flat/307489774/
}


void bob_salary(const int month)
{
    if (month == 1) {
        bob.salary *= (1. + bob.inflation_index);
    }

    bob.account += bob.salary;
}


void bob_rent(const int month)
{
    if (month == 1) {
        bob.monthly_rent *= (1. + bob.inflation_index);
    } 
    
    bob.account -= bob.monthly_rent;
}


void bob_life_cost(const int month)
{
    if (month == 1) {
        bob.life_cost *= (1. + bob.inflation_index);
    }
    
    bob.account -= bob.life_cost;
}


void bob_cat(const int month, const int year)
{
    static int is_cat = 0;

    if ((month == 5) && (year == 2036) ) {
        bob.account -= bob.cat.buy;
        is_cat = 1;
    }

    if (is_cat == 1) {
        if (month == 1) {
        bob.cat.cost *= (1. + bob.inflation_index);
        }

        bob.account -= bob.cat.cost;
    }

    if ((month == 11) && (year == 2050)) {
        bob.account -= bob.cat.funeral;
        is_cat = 0;
    }
}


void bob_deposit()
{
    bob.account += bob.account * (bob.deposit_rate / 12);
}


void bob_print()
{
    printf("Bob summary capital: %lld \n", bob.account);
}


void conclusion()
{
    printf("----------------------------------\n");
    if ((alice.account + alice.house_price) > bob.account) {
        printf("Alice is winner\n");
    } else {
        if ((alice.account + alice.house_price) == bob.account) {
            printf("Alice and Bob are equal\n");
        } else {
            printf("Bob is winner\n");
        }
    }

}


void simulation()          
{
    int year = START_YEAR;
    int month = START_MONTH;
    
    while (!((year == START_YEAR + PERIOD) && (month == START_MONTH + 1))) {
        alice_salary(month);
        alice_mortgage(month);
        alice_life_cost(month);
        alice_house_price(month);
        alice_deposit();
        
        bob_salary(month);
        bob_rent(month);
        bob_life_cost(month);
        bob_cat(month, year);
        bob_deposit();

        month++;
        if (month == 13) {
            year++;
            month = 1;
        }
    }
}

void print_output()
{
    printf("Results for %d.%d are:\n",START_MONTH, START_YEAR + PERIOD);
    printf("----------------------------------\n");
    alice_print();
    bob_print();
    conclusion();
}

int main()
{
    alice_init();
    bob_init();
    simulation();
    print_output();
    return 0;
}



/*
Условия задачи:

Alice и Bob, стартовый капитал - 1.000.000 Р 
Alice эту сумму как первый взнос по ипотеке (15.000.000 Р) на 30 лет под 17% годовых (ставку рассчитать по онлайн-калькулятору)
Bob копит на квартиру и живет в съемной - ~30.000 Р в месяц
Зарплата обоих составляет ~200.000 Р в месяц
Стоит учитывать базовые потребности типа еды, комуналки, 
Все цены и зарплаты подвержены инфляции в 8% в год
дописать покупку кота
*/
