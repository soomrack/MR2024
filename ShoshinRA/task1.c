#include <stdio.h>
#include <math.h>

typedef long long int Money;  // RUB

struct Mortgage {
    Money sum;  // сумма ипотеки
    Money first_pay;  // сумма первого платежа
    Money monthly_payments;  // ежемесячный платёж
    double rate;
};


struct Person
{
    Money house_cost;  // Цена квартиры
    Money salary;  //  зарплата
    Money account;  //  капитал
    Money cost_food;  //  ежемесячный расход на еду
    Money rent;  // Ежемесячная плата за квартиру
    struct Mortgage mortgage;
} alice, bob;


void alice_init()
{
    alice.account = 1000 * 1000;  // Изначальный капитал Элис
    alice.salary = 200 * 1000;  // ЗП Элис
    alice.cost_food = 20 * 1000;  // Траты на еду за месяц

    alice.mortgage.sum = 15 * 1000 * 1000;  // Сумма ипотеки
    alice.house_cost = 15 * 1000 * 1000;  // Стоимость квартиры на момент покупки
    alice.mortgage.first_pay = 1000 * 1000;  // Первый платёж
    alice.mortgage.rate = 0.14;  // Процентная ставка за ипотеку 14 %
    alice.mortgage.monthly_payments = 165882;  // Ежемесячный платёж https://calculator-credit.ru/?sum=15000000&currency=1&period=30&period_type=1&rate=14&rate_type=1&fee=1000000&fee_type=1&commission_once=0&commission_once_type=2&commission_monthly=0&commission_monthly_type=2&payment_type=1&first_month=10&first_year=2024
    alice.account -= alice.mortgage.first_pay;

}


void bob_init()
{
    bob.account = 1000*1000;  // Изначальный капитал Боба
    bob.salary = 200*1000;  // ЗП Боба
    bob.cost_food = 20*1000;  // Траты на еду в месц
    bob.rent = 30*1000;  // Оплата квартиры за месяц
}


void alice_salary(const int month, const int year)
{
    static Money pre_salary = (Money)(200 * 1000 * 1.5);

    if ((year == 2028) && ((month == 3) || (month == 4))) {  // увольнение 
        alice.salary = 0;
    }

    else {
        alice.account += alice.salary;
    }

    if ((year == 2028) && (month == 5)) {
        alice.salary = pre_salary;  //  повышение зп
    }

    if(month == 1) {
        alice.salary = (Money)(1.07 * alice.salary);
    }
}


void alice_food (const int month)
{
    alice.account -= alice.cost_food;
    if (month == 1) {
        alice.cost_food = (Money)(alice.cost_food * 1.07);  //  Инфляция еды
    }
}


void alice_mortgage()
{
    alice.account -= alice.mortgage.monthly_payments;
}


void alice_house_cost(const int month)
{
 if (month == 1){
    alice.house_cost = (Money)(alice.house_cost * 1.07);  //  Подорожание квартиры на 7 % за год
 }
}


void bob_salary(const int month, const int year)
{
    bob.account += bob.salary;

    if(month == 1) {
        bob.salary = (Money)(1.07 * bob.salary);
    }
}


void bob_rent(const int month)
{
    bob.account -= bob.rent;

    if (month == 1){
        bob.rent = (Money)(bob.rent * 1.07);  //  Индексация зп на 7 %
    }
}


void bob_food (const int month)
{
    bob.account -= bob.cost_food;

    if (month == 1) {
        bob.cost_food = (Money)(bob.cost_food * 1.07);  //  Инфляция еды
    }
}


void alice_print()
{
    printf("Alice accaount : %lld RUB\n", alice.account);
    printf("Alice house cost : %lld RUB\n", alice.house_cost);
    printf("Alice capital : %lld RUB\n", (alice.account + alice.house_cost));
}


void bob_print()
{
    printf("Bob capital = %lld RUB\n", bob.account);
}


void simulation()
{
    int year = 2024;
    int month = 9;

    while (!((year == 2024 + 30) && (month == 9)))
    {
        alice_salary(month,year);
        alice_food(month);
        alice_mortgage();
        alice_house_cost(month);

        bob_salary(month, year);
        bob_food(month);
        bob_rent(month);


        month++;
        if(month == 13){
            month = 1;
            year++;
        }
    }
}


 int main()
{
    bob_init();
    alice_init();
    
    simulation();

    alice_print();
    bob_print();

    return 0;
}
