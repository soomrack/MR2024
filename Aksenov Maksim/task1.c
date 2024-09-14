#include <stdio.h>


typedef long long int Money;    // увеличиваем диапазон счёта, который будем вести в копейках


struct Human                  // структура для Марка и Павла
{
    Money capital;             // накопленный начальный капитал
    Money income;              // доходы
    Money life_spendings;      // траты на жизнь
    Money monthly_payment;     // ежемесячный платёж по ипотеке или аренда квартиры
};                             


struct Human Mark;
struct Human Pavel;


const int START_MONTH = 9;                      // начальный месяц
const int START_YEAR = 2024;                    // начальный год
const int LAST_MONTH = 8;                       // конечный месяц
const Money PRICE = 15 * 1000 * 1000 * 100;     // стоимость квартиры 
const Money SALARY = 300 * 1000 * 100;          // зарплата Марка и Павла
const Money START_CAPITAL = 1000 * 1000 * 100;  // стартовый капитал Марка и Павла
const int YEARS = 30;                           // количество лет
const double INFLATION = 0.08;                  // инфляция в год 
const double BANK_PERCENT = 0.2;                // годовая ставка вклада в банке
const double MORTGAGE_RATE = 0.17;              // ставка по ипотеке
const Money LIFE_SPENDINGS = 30 * 1000 * 100;   // ежемесячные траты

void Mark_init()                               // инициализация Марка
{
    Mark.income = SALARY;
    Mark.capital = START_CAPITAL;
    Mark.life_spendings = LIFE_SPENDINGS;
    Mark.monthly_payment = 200 * 1000 * 100;   // ежемесячный платёж согласно сайту https://www.banki.ru/services/calculators/hypothec/
}
