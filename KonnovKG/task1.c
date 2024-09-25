#include <stdio.h>
#include <math.h>
#include <locale.h>
#include <string.h>


typedef long long int Money;        // коп.
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
    Money vacation_cost;
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


// Функция для расчета платежа по кредиту, аналогична сайту calcus.ru
double calc_credit_payment(Credit credit)
{
    Money cost = credit.credit_cost;
    double rate = credit.credit_rate;
    int duration = (credit.year_end * 12 + credit.month_end) - (credit.year_start * 12 + credit.month_start); // Расчёт срока кредита в месяцах

    double month_rate = rate / 12;                                       // Ежемесячная ставка
    double whole_rate = pow((1 + month_rate), (duration));               // Общая ставка
    double payment = cost * month_rate * whole_rate / (whole_rate - 1);  // Ежемесячный платеж
    return payment;
}


void alice_init()
{
    strcpy(alice.name, "Alice");
    alice.capital = 1000 * 1000 * 100;
    alice.salary = 200 * 1000 * 100;
    alice.food_cost = 15 * 1000 * 100;
    alice.service_cost = 8 * 1000 * 100;
    alice.personal_cost = 17 * 1000 * 100; 
    alice.deposit = 0; 
    alice.rent_cost = 0; 
};


void bob_init()
{
    strcpy(bob.name, "Bob");
    bob.capital = 1000 * 1000 * 100;
    bob.salary = 200 * 1000 * 100;
    bob.food_cost = 15 * 1000 * 100;
    bob.service_cost = 8 * 1000 * 100;
    bob.personal_cost = 17 * 1000 * 100;
    bob.vacation_cost = 100 * 1000 * 100;
    bob.deposit = 0;
    bob.rent_cost = 30 * 1000 * 100;
};


void alice_mortgage_init()
{
    alice_apartment_mortgage.year_start = 2024;
    alice_apartment_mortgage.month_start = 9;
    alice_apartment_mortgage.year_end = 2054;
    alice_apartment_mortgage.month_end = 9;
    alice_apartment_mortgage.credit_cost = 13 * 1000 * 1000 * 100;
    alice_apartment_mortgage.credit_rate = 0.16;  // Значение ставки по кредиту/ипотеке, п.п.
    alice_apartment_mortgage.credit_payment = calc_credit_payment(alice_apartment_mortgage);
}


void manage_alice_salary(const int month)
{
    if (month == 12) {
        alice.salary *= 1. + INFLATION_RATE;
    }

    alice.capital += alice.salary;

}


void manage_bob_salary(const int month)
{
    if (month == 12) {
        bob.salary *= 1. + INFLATION_RATE;
    }

    if (month != 8) {
        bob.capital += bob.salary;
    }

}


void manage_alice_expenses(const int month)
{
    if (month == 12) {
        alice.food_cost *= 1. + INFLATION_RATE;
        alice.service_cost *= 1. + INFLATION_RATE;
        alice.personal_cost *= 1. + INFLATION_RATE;
    }
    alice.capital -= (alice.food_cost + alice.service_cost + alice.personal_cost);
}


void manage_bob_expenses(const int month)
{
    if (month == 12) {
        bob.food_cost *= 1. + INFLATION_RATE;
        bob.service_cost *= 1. + INFLATION_RATE;
        bob.personal_cost *= 1. + INFLATION_RATE;
        bob.vacation_cost *= 1. + INFLATION_RATE;
    }

    if (month == 8) {
        bob.capital -= bob.vacation_cost;
    }

    bob.capital -= (bob.food_cost + bob.service_cost + bob.personal_cost);
}


void manage_bob_rent(const int month)
{
    if (month == 12) {
        bob.rent_cost *= 1. + INFLATION_RATE;
    }

    bob.capital -= bob.rent_cost;
}


void manage_alice_deposit()
{
    alice.deposit += alice.capital;
    alice.deposit *= 1. + DEPOSITE_RATE / 12;
    alice.capital = 0;
}


void manage_bob_deposit()
{
    bob.deposit += bob.capital;
    bob.deposit *= 1. + DEPOSITE_RATE / 12;
    bob.capital = 0;
}


void manage_alice_credit(Credit *credit, const int current_month, const int current_year)
{
    if (credit->year_start <= current_year <= credit->year_end && credit->month_start <= current_month <= credit->month_end) {
        alice.capital -= credit->credit_payment;
    }
}


void print_capital(Person *p) 
{
    setlocale(LC_NUMERIC, "");  // Для вывода чисел в американском формате (1,234,567,890) 

    printf("%s\n", p->name);
    printf("Final capital is %'.lld Rub\n", p->deposit / 100);  // Вывод финального капитала, руб.
    printf("\n");
}


void simulation(const int start_month, const int start_year)
{
    int year = start_year;
    int month = start_month;

    while (!(year == (start_year + 30) && month == start_month)) {

        manage_alice_salary(month);
        manage_alice_expenses(month);
        manage_alice_credit(&alice_apartment_mortgage, month, year);
        manage_alice_deposit();

        manage_bob_salary(month);
        manage_bob_expenses(month);
        manage_bob_rent(month);
        manage_bob_deposit();

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