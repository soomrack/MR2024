#include <stdio.h>
#include <math.h>

const long start_capital = 1000 * 1000;       // Стартовый капитал, руб
const float inflation_rate = 0.08;            // Годовая инфляция 8% 
const float deposit_rate = 0.2;               // Проценты на банковском вкладе
const long number_of_years = 30;              // Количество лет, в течение которых проводится симуляция
const float interest_rate = 0.17;             // Ставка по ипотеке 17%
long flat_cost = 15 * 1000 * 1000;            // Стоимость квартиры, руб
long salary = 250 * 1000;                     // Зарплата, руб
long flat_rent_cost = 34000;                  // Стоимость аренды квартиры, руб

long food_cost = 15000;                        // Доп. траты
long public_utility_cost = 4000;
long personal_expenses = 10000;

const long start_year = 2024;
const long start_month = 9;                    // Начинаем симуляцию с сентября 2024 года
long end_year;
long end_month;
int current_year;
int current_month;

int month_pay;
float monthly_deposit_rate;                    // Ежемесячный процент по вкладу

long bob_capital = 1000 * 1000;
long alice_capital = 0;


int count_mortgage_pay()
{
    int number_of_months = number_of_years * 12;
    float month_rate = interest_rate / 12;
    month_pay = (flat_cost - start_capital) * (month_rate * pow(1 + month_rate, number_of_months)) / (pow(1 + month_rate, number_of_months) - 1);
}


int new_month()
{
    int money_for_month = salary - (food_cost + public_utility_cost + personal_expenses);
    bob_capital += money_for_month - flat_rent_cost;
    alice_capital += money_for_month - month_pay;
    bob_capital *= (1 + monthly_deposit_rate);
    alice_capital *= (1 + monthly_deposit_rate);
    current_month += 1;
    money_for_month = 0;
    printf("%.ld, %.ld\n", bob_capital, alice_capital);
}
 
int new_year()
{
    current_month = 1;
    flat_cost *= (1 + inflation_rate);
    flat_rent_cost *= (1 + inflation_rate);
    food_cost *= (1 + inflation_rate);
    public_utility_cost *= (1 + inflation_rate);
    personal_expenses *= (1 + inflation_rate);
    salary *= (1 + inflation_rate);
    current_year += 1;
}

int simulation()
{
    end_year = start_year + number_of_years;
    end_month = start_month - 1;
    current_year = start_year;
    current_month = start_month;
    monthly_deposit_rate = deposit_rate / 12; // Ежемесячный процент по вкладу

    count_mortgage_pay();
    
    while (!(current_year == start_year + number_of_years + 1))
    {
        while (!(current_month == 12))
        {
            new_month();
            if (current_month == start_month && current_year == start_year + number_of_years)
            {
                break;
            }
            
        }
        new_year();
    }
}

void main()
{
    simulation();
    printf("Капитал Боба: %.ld руб, капитал Элис: %.ld руб + квартира за %ld руб, %d, %d", bob_capital, alice_capital, 
    flat_cost, current_year, current_month); 
}