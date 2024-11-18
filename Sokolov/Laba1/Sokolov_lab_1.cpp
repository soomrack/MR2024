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
    bool has_house;                  // Флаг, если купил квартиру
    float indexation_rate;           // Индексация зарплаты
    float mortgage_rate;             // Ипотечная ставка
} Person;

typedef struct {
    float inflation_rate;            // Инфляция
    long long int house_cost;        // Стоимость квартиры с учётом инфляции
    float deposit_rate;              // Процент на депозит
} Money;

Person Alice;
Person Bob;
Money money;

// Функция для расчёта суммы после начисления процента
long long int apply_interest(long long int amount, float rate) {
    return amount * (1 + rate);
}

// Моделирование одного месяца
void simulate_months(Person* person, const Money* money, int month) {
    // Вычитание ежемесячных расходов
    person->capital -= person->monthly_expenses;

    // Если у Боба нет квартиры, он платит аренду
    if (!person->has_house) {
        person->capital -= person->rent;
    }

    // Добавляем месячную зарплату
    person->capital += person->salary / 12;

    // Начисляем проценты на депозит (капитал)
    person->capital = apply_interest(person->capital, money->deposit_rate / 12);
}

// Моделирование одного года
void simulate_years(Person* alice, Person* bob, Money* money) {
    for (int year = 1; year <= YEARS; year++) {
        // Месячное обновление капитала для каждого
        for (int month = 1; month <= MONTHS_IN_YEAR; month++) {
            simulate_months(alice, money, month);
            simulate_months(bob, money, month);
        }

        // Ежегодное увеличение зарплаты и расходов на инфляцию
        alice->monthly_expenses = apply_interest(alice->monthly_expenses, money->inflation_rate);
        bob->monthly_expenses = apply_interest(bob->monthly_expenses, money->inflation_rate);
        bob->rent = apply_interest(bob->rent, money->inflation_rate);

        // Увеличение стоимости квартиры на инфляцию
        money->house_cost = apply_interest(money->house_cost, money->inflation_rate);

        // Индексация зарплаты немного выше инфляции
        alice->salary = apply_interest(alice->salary, alice->indexation_rate);
        bob->salary = apply_interest(bob->salary, bob->indexation_rate);

        // Проверка, может ли Боб купить квартиру
        if (!bob->has_house && bob->capital >= money->house_cost) {
            bob->capital -= money->house_cost;  // Боб покупает квартиру
            bob->has_house = true;  // Теперь у Боба есть квартира
        }

        // Вывод информации о капитале и статусе квартиры по итогам года
        printf("Year %d:\n", year);
        printf("Alice's capital: %lld\n", alice->capital);
        printf("Bob's capital: %lld (Has house: %s)\n\n", bob->capital, bob->has_house ? "Yes" : "No");
    }
}

// Функция для сравнения стратегий и вывода финальных результатов
void compare_strategies(Person* alice, Person* bob, Money* money) {
    // Расчёт конечной стоимости квартиры с учётом инфляции
    long long int final_house_cost = money->house_cost;

    // Итоговый капитал Алисы включает стоимость квартиры
    long long int alice_final_capital = alice->capital + final_house_cost;

    // Итоговый капитал Боба включает стоимость квартиры, если он её купил
    long long int bob_final_capital = bob->capital;
    if (bob->has_house) {
        bob_final_capital += final_house_cost;
    }

    printf("После %d лет:\n", YEARS);
    printf("Конечный капитал Алисы (включая стоимость квартиры): %lld\n", alice_final_capital);
    printf("Конечный капитал Боба (включая стоимость квартиры): %lld\n", bob_final_capital);

    // Вывод результата, чья стратегия была более эффективной
    if (alice_final_capital > bob_final_capital) {
        printf("Стратегия Алисы (покупка в ипотеку) оказалась более эффективной.\n");
    }
    else if (alice_final_capital < bob_final_capital) {
        printf("Стратегия Боба (накопление и покупка) оказалась более эффективной.\n");
    }
    else {
        printf("Обе стратегии оказались равнозначными.\n");
    }
}

// Инициализация начальных данных для Алисы, Боба и параметров экономики
void initialization() {
    money = {
        0.05,               // Инфляция 5% (снижение для большей реалистичности)
        13 * 1000000,       // Стоимость квартиры 13 млн
        0.20                // Депозит 20%
    };

    // Алиса покупает в ипотеку
    Alice = {
        1000000,            // Начальный капитал
        200000,             // Зарплата
        20000,              // Месячные расходы
        0,                  // Аренда отсутствует
        true,               // Изначально есть квартира (ипотека)
        0.06,               // Индексация зарплаты 6%
        0.16                // Ипотечная ставка 16%
    };

    // Боб арендует и копит
    Bob = {
        1000000,            // Начальный капитал
        200000,             // Зарплата
        20000,              // Месячные расходы
        10000,              // Месячная аренда
        false,              // Нет квартиры
        0.06,               // Индексация зарплаты 6%
        0                   // Ипотека отсутствует
    };
}

int main() {
    setlocale(LC_ALL, "Rus");

    initialization();

    simulate_years(&Alice, &Bob, &money);
    compare_strategies(&Alice, &Bob, &money);

    return 0;
}
