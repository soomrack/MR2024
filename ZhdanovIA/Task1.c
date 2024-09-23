#include <stdio.h>
#include <math.h>

const int base_capital = 1000 * 1000;       
const float inflation_rate = 0.09;            // Годовая инфляция 9% 
const float deposit_rate = 0.2;               // Проценты на банковском вкладе
const int number_of_years = 30;              // Количество лет, в течение которых проводится симуляция
const float mortage_rate = 0.16;             // Ставка по ипотеке 16%
int flat_cost = 13 * 1000 * 1000;            // Стоимость квартиры, руб
int salary = 200 * 1000;                     
int flat_rent_cost = 30 * 1000;                  // Стоимость аренды квартиры, руб
double salary_index_rate = 0.02;              // Индексация зарплаты

long food_cost = 20 * 1000;                        // Доп. траты
long public_utility_cost = 4 * 1000;
long personal_cost = 10 * 1000;

const long start_year = 2024;
const long start_month = 9;          // начинаем симуляцию с сентября 2024           
long end_year;
long end_month;
int current_year;
int current_month;

int number_of_months = number_of_years * 12; 

int month_pay;                 
float monthly_deposit_rate = deposit_rate / 12;          // Ежемесячный процент по вкладу

long bob_capital = 1000 * 1000;
long alice_capital = 0;


int count_mortgage_pay()                 // выплаты по ипотеке
{
    float month_rate = mortage_rate / 12;
    month_pay = (flat_cost - base_capital) * (month_rate * pow(1 + month_rate, number_of_months)) / (pow(1 + month_rate, number_of_months) - 1); // ежемесячный платёж
}


int new_month()   
{
    int money_for_month = salary - (food_cost + public_utility_cost + personal_cost);   // остаток средств после прочих расходов без включения жилья
    bob_capital += money_for_month - flat_rent_cost;                //  капитал Боба после оплаты аренды
    alice_capital += money_for_month - month_pay;                  //  капитал Элис после месячного платежа 
    bob_capital *= (1 + monthly_deposit_rate);                    //  начисление процента на депозитном счёте Боба
    alice_capital *= (1 + monthly_deposit_rate);                 //  начисление процента на депозитном счёте Элис
    current_month += 1;
    money_for_month = 0;
}
 
int new_year()             // индексация уровня жизни 
{
    current_month = 1;
    flat_cost *= (1 + inflation_rate);
    flat_rent_cost *= (1 + inflation_rate);
    food_cost *= (1 + inflation_rate);
    public_utility_cost *= (1 + inflation_rate);
    personal_cost *= (1 + inflation_rate);
    salary *= (1 + salary_index_rate);
    current_year += 1;
}

int simulation()
{
    end_year = start_year + number_of_years;
    end_month = number_of_months - 1;
    current_year = start_year;
    current_month = start_month;
    monthly_deposit_rate = deposit_rate / 12;         

    count_mortgage_pay(); 
    
     while ((current_month < start_month) || (current_year < start_year + number_of_years ))
      {    
        if (current_month < 12)
        {
            new_month();
     }  else{
        new_year();
     }
      }
}
void main()
{
    simulation();
    printf(" капитал Элис: %.ld руб + квартира за %ld руб, Общая сумма ипотеки %ld руб, Капитал Боба: %.ld руб,  %d, %d", alice_capital, 
    flat_cost, month_pay * 30 * 12, bob_capital, current_year, current_month); 
} 