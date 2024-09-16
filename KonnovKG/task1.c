#include <stdio.h>
#include <math.h>
#include <locale.h>
#include <string.h>

typedef long long int Money;
const double INFLATION_RATE = 0.09; // Значение годовой инфляции в п.п.
const double DEPOSITE_RATE = 0.2;   // Значение процента по вкладу в п.п.

typedef struct
{
    char name[10];
    Money capital;
    Money salary;
    Money food_cost;
    Money service_cost;
    Money personal_cost;
    Money rent_cost;
    Money deposit;
} Person;

typedef struct
{
    int year_start;
    int month_start;
    int year_end;
    int month_end;
    Money credit_cost;
    double credit_rate;
    Money credit_payment;
} Credit;

Person alice;
Person bob;
Credit alice_apartment_mortgage;

// Функция для расчета платежа по кредиту
double calc_credit_payment(Credit c)
{
    Money cost = c.credit_cost;
    double rate = c.credit_rate;
    int duration = (c.year_end * 12 + c.month_end) - (c.year_start * 12 + c.month_start); // Расчёт срока кредита в месяцах

    double month_rate = rate / 12;                                      // Ежемесячная ставка
    double whole_rate = pow((1 + month_rate), (duration));              // Общая ставка
    double payment = cost * month_rate * whole_rate / (whole_rate - 1); // Ежемесячный платеж
    return payment;
}

void alice_init()
{
    strcpy(alice.name, "Alice");
    alice.capital = 1000 * 1000 * 100;  // коп.
    alice.salary = 200 * 1000 * 100;  // коп.
    alice.food_cost = 15 * 1000 * 100;  // коп.
    alice.service_cost = 8 * 1000 * 100;  // коп.
    alice.personal_cost = 17 * 1000 * 100;  // коп.
    alice.deposit = 0;  // коп.
    alice.rent_cost = 0;  // коп.
};

void bob_init()
{
    strcpy(bob.name, "Bob");
    bob.capital = 1000 * 1000 * 100;  // коп.
    bob.salary = 200 * 1000 * 100;  // коп.
    bob.food_cost = 15 * 1000 * 100;  // коп.
    bob.service_cost = 8 * 1000 * 100;  // коп.
    bob.personal_cost = 17 * 1000 * 100;  // коп.
    bob.deposit = 0;  // коп.
    bob.rent_cost = 30 * 1000 * 100;  // коп.
};

void alice_mortgage_init()
{
    alice_apartment_mortgage.year_start = 2024;
    alice_apartment_mortgage.month_start = 9;
    alice_apartment_mortgage.year_end = 2054;
    alice_apartment_mortgage.month_end = 9;
    alice_apartment_mortgage.credit_cost = 13 * 1000 * 1000 * 100; // коп.
    alice_apartment_mortgage.credit_rate = 0.16; // Значение ставки по кредиту/ипотеке, п.п.
    alice_apartment_mortgage.credit_payment = calc_credit_payment(alice_apartment_mortgage);
}

void manage_salary(Person *p, int month)
{
    if (month == 12) {
        p->salary *= 1 + INFLATION_RATE;
    }
    p->capital += p->salary;
}

void manage_expenses(Person *p, int month)
{
    if (month == 12) {
        p->food_cost *= 1 + INFLATION_RATE;
        p->service_cost *= 1 + INFLATION_RATE;
        p->personal_cost *= 1 + INFLATION_RATE;
    }
    p->capital -= (p->food_cost + p->service_cost + p->personal_cost);
}

void manage_rent(Person *p, int month)
{
    if (month == 12) {
        p->rent_cost *= 1 + INFLATION_RATE;
    }

    p->capital -= p->rent_cost;
}

void manage_deposit(Person *p)
{
    p->deposit += p->capital;
    p->deposit *= 1 + DEPOSITE_RATE / 12;
    p->capital = 0;
}

void manage_credit(Person *p, Credit *c, int current_month, int current_year)
{
    if (c->year_start <= current_year <= c->year_end && c->month_start <= current_month <= c->month_end) {
        p->capital -= c->credit_payment;
    }
}

void print_capital(Person *p) 
{
    setlocale(LC_NUMERIC, "");  // Для вывода чисел в американском формате (1,234,567,890) 

    printf("%s\n", p->name);
    printf("Final capital is %'.lld Rub\n", p->deposit / 100);  // Вывод финального капитала, руб.
    printf("\n");
}

void simulation(int month, int year)
{
    int current_year = year;
    int current_month = month;

    while (!(year == (current_year + 30) && month == current_month)) {
        manage_salary(&alice, month);
        manage_salary(&bob, month);

        manage_expenses(&alice, month);
        manage_expenses(&bob, month);

        manage_credit(&alice, &alice_apartment_mortgage, current_month, current_year);

        manage_rent(&bob, month);

        manage_deposit(&alice);
        manage_deposit(&bob);

        ++month;

        if (month == 13) {
            month = 1;
            ++year;
        }
    }
};

int main()
{
    alice_init();
    bob_init();
    alice_mortgage_init();

    simulation(9, 2024);

    print_capital(&alice);
    print_capital(&bob);
    
    return 0;
}