#include <stdio.h>
#include <math.h>
#include <locale.h>

#define YEARS 30
#define MONTHS_IN_YEAR 12

typedef struct {
    long long int capital;           // Капитал
    long long int salary;            // Зарплата
    long long int monthly_expenses;  // Месячные расходы на еду, коммунальные и т.д.
    long long int rent;              // Аренда
    float deposit_rate;              // Процент на депозит
    bool has_house;                  // Флаг, если купил квартиру
} Person;


typedef struct {
    float inflation_rate;            // Инфляция
    float indexation_rate;           // Индексация
    long long int house_cost;        // Стоимость квартиры
    float mortgage_rate;             // Ипотечная ставка
} Money;


Person Alice;
Person Bob;

long long int apply_interest(long long int amount, long long int rate)
{
    return amount * (1 + rate);
}


void simulate_months(Person* person, Money* money, int month)
{
    person->capital -= person->monthly_expenses;
    person->capital += person->salary;

    if (!person->has_house)
        person->capital -= person->rent;
    else
        person->capital -= apply_interest(money->house_cost, money->mortgage_rate / 12);
}


void simulate_years(Person* person, Money* money)
{
    for (int year = 1; year <= YEARS; year++)
        for (int month = 0; month < MONTHS_IN_YEAR; month++)
            simulate_months(person, money, month);

    person->monthly_expenses = apply_interest(person->monthly_expenses, money->inflation_rate);
    person->rent = apply_interest(person->rent, money->inflation_rate);
    money->house_cost = apply_interest(money->house_cost, money->inflation_rate);

    person->salary = apply_interest(person->salary, money->indexation_rate);
    person->capital = apply_interest(person->capital, person->deposit_rate);
}


void compare_strategies(Person* alice, Money* m_alice, Person* bob, Money* m_bob)
{
    double alice_final_capital = alice->capital + m_alice->house_cost + YEARS * m_alice->house_cost * m_alice->mortgage_rate * YEARS;

    double bob_final_capital = bob->capital;
    if (bob->has_house)
        bob_final_capital += m_bob->house_cost;

    printf("После %d лет:\n", YEARS);
    printf("Конечный капитан Элис: %.2f\n", alice_final_capital);
    printf("Конечный капитал Боба: %.2f\n", bob_final_capital);

    if (alice_final_capital > bob_final_capital)
        printf("Стратегия Элис более эффективна.\n");
    else if (alice_final_capital < bob_final_capital)
        printf("Стратегия Боба более эффективна.\n");
    else
        printf("Стратегии равнозначны.\n");
}


int main() {
    setlocale(LC_ALL, "Rus");

    Money m_alice = {
    0.1,                             // Инфляция
    0.1,                             // Индексация
    13 * 1000000,                    // Стоимость квартиры
    0.16                             // Ипотечная ставка
    };

    Money m_bob = {
    0.1,                             // Инфляция
    0.1,                             // Индексация
    13 * 1000000,                    // Стоимость квартиры
    0                                // Ипотечная ставка
    };

    // Инициализация начальных данных для Алисы (покупка в ипотеку)
    Alice = {
    1200000,                         // Капитал
    200 * 1000,                      // Зарплата
    20 * 1000,                       // Месячные расходы
    0,                               // Аренда
    0.2,                             // Процент на депозит
    true                             // Флаг, если купил квартиру
    };

    // Инициализация начальных данных для Боба (снимает жильё и копит на покупку)
    Bob = {
    1000000,                         // Капитал
    200 * 1000,                      // Зарплата
    20 * 1000,                       // Месячные расходы
    0,                               // Аренда
    0.2,                             // Процент на депозит
    false                            // Флаг, если купил квартиру
    };

    simulate_years(&Alice, &m_alice);
    simulate_years(&Bob, &m_bob);

    compare_strategies(&Alice, &m_alice, &Bob, &m_bob);

    return 0;
}