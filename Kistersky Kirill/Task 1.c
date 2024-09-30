#include <stdio.h>

/*
Условия задачи:
Alice и Bob
стартовый капитал - 1.000.000 Р 
Alice эту сумму как первый взнос по ипотеке (15.000.000 Р) на 30 лет под 17% годовых (ставку рассчитать по онлайн-калькулятору)
Bob копит на квартиру и живет в съемной - ~30.000 Р в месяц
Зарплата обоих составляет ~200.000 Р в месяц
Учитывать базовые потребности типа еды, комуналки, 
Все цены подвержены инфляции в 8% в год, а зарплата такому же индексированию

Дополнительное условие:
Alice и Bob получают годовые премии
Bob получает дополнительно квартальные премии
Bob устраивает вечеринку каждый год и тратит 50.000 Р с инфляцией
*/


typedef long long int Money; //RUB

const int START_YEAR = 2024;
const int START_MONTH = 9;
const int PERIOD = 30;  // расчетный период симуляции


struct Mortgage {
    Money sum;
    Money first_pay;
    Money monthly_payments;
    double rate;
};


struct Person {
    Money salary;
    Money account;
    Money life_cost;
    Money flat_price;
    Money monthly_rent;
    Money newyear_party;
    struct Mortgage mortgage;
    double deposit_rate;
    double inflation_index;
};


struct Person alice;
struct Person bob;


void alice_init()
{
    alice.salary = 200 * 1000;
    alice.account = 1000 * 1000;
    alice.life_cost = 50 * 1000;
    alice.deposit_rate = 0.2;
    alice.inflation_index = 0.08;
    alice.mortgage.sum = 15 * 1000 * 1000;
    alice.mortgage.first_pay = 1000 * 1000;
    alice.mortgage.rate = 0.17;
    alice.mortgage.monthly_payments = 200 * 1000; // source: https://calcus.ru/kalkulyator-ipoteki?input=eyJjdXJyZW5jeSI6IlJVQiIsInR5cGUiOiIxIiwiY29zdCI6IjE1MDAwMDAwIiwic3RhcnRfc3VtIjoiMTAwMDAwMCIsInN0YXJ0X3N1bV90eXBlIjoiMSIsInBlcmlvZCI6IjMwIiwicGVyaW9kX3R5cGUiOiJZIiwicGVyY2VudCI6IjE3IiwicGF5bWVudF90eXBlIjoiMSJ9
    alice.account -= alice.mortgage.first_pay;
    alice.flat_price = alice.mortgage.sum;
}


void alice_salary(const int month)
{
    if(month == 1){
        alice.salary*=(1. + alice.inflation_index);
    }
    alice.account += alice.salary;
}


void alice_year_bonus(const int month)
{
    if(month == 12){
        alice.account += alice.salary;
    }
}


void alice_mortgage()
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


void alice_flat_price(const int month)
{
    if (month == 1) {
        alice.flat_price *= (1. + alice.inflation_index);
    }
}


void alice_deposit()
{
    alice.account += alice.account * (alice.deposit_rate / 12);
}


void alice_print()
{
    printf("Alice summary capital: %lld \n", alice.account + alice.flat_price);
}


void bob_init()
{
    bob.account = 1 * 1000 * 1000;
    bob.salary = 200 * 1000;
    bob.life_cost = 50 * 1000;
    bob.newyear_party = 50 * 1000;
    bob.deposit_rate = 0.2;
    bob.inflation_index = 0.08;
    bob.monthly_rent = 36*1000; // source: https://www.avito.ru/sankt-peterburg/kvartiry/1-k._kvartira_31_m_55_et._4269985596
}


void bob_salary (const int month)
{
    if (month == 1){
        bob.salary *= (1. + bob.inflation_index);
    }
    bob.account += bob.salary;
}


void bob_quarterly_bonus(const int month)
{
    if(month == 1 || month == 4 || month == 7 || month == 10){
        bob.account += 0.5 * bob.salary;
    }
}


void bob_year_bonus(const int month)
{
    if(month == 12){
        bob.account += bob.salary;
    }
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


void bob_newyear_party(const int month)
{
    if (month == 12){
        bob.newyear_party *= (1. + bob.inflation_index);
        bob.account -= bob.newyear_party;
    }
}


void bob_deposit()
{
    bob.account += bob.account * (bob.deposit_rate / 12);
}


void bob_print()
{
    printf("Bob summary capital:   %lld \n", bob.account);
}


void conclusion()
{
    printf("----------------------------------\n");
    if ((alice.account + alice.flat_price) > bob.account) {
        printf("Alice is winner\n");
    } else {
        if ((alice.account + alice.flat_price) == bob.account) {
            printf("Alice and Bob are equal\n");
        } else {
            printf("Bob is winner\n");
        }
    }

}

void simulation ()
{
    int month = START_MONTH;
    int year = START_YEAR;

    while(!((year == START_YEAR + PERIOD) && (month == START_MONTH + 1))) {
        alice_salary(month);
        alice_mortgage(month);
        alice_year_bonus(month);
        alice_life_cost(month);
        alice_flat_price(month);
        alice_deposit();
        
        bob_salary(month);
        bob_quarterly_bonus(month);
        bob_year_bonus(month);
        bob_rent(month);
        bob_life_cost(month);
        bob_newyear_party(month);
        bob_deposit();

        month++;
        if(month == 13){
            month = 1;
            year++;
        }
    }
}


void print_output()
{
    printf("Results for %d.%d:\n",START_MONTH, START_YEAR + PERIOD);
    printf("==================================\n");
    alice_print();
    bob_print();
    conclusion();
}


int main() {
    alice_init();
    bob_init();
    simulation();
    print_output();
    return 0;
}
