#include <stdio.h>
#include <math.h>
#include <locale.h>

#define YEARS 30
#define MONTHS_IN_YEAR 12
double HOUSE_COST = 13000000;

typedef struct {
    double capital;           // Капитал
    double salary;            // Зарплата
    double monthlyExpenses;   // Месячные расходы на еду, коммунальные и т.д.
    double rentOrMortgage;    // Аренда или ипотека (фиксированная)
    double inflationRate;     // Инфляция
    double depositRate;       // Процент на депозит (накопления)
    double monthlyRent;       // Месячная аренда (для Боба)
    double mortgageRate;      // Ипотечная ставка
    int hasHouse;             // Флаг, если купил квартиру
} Person;

// Функция для вычисления суммы после начисления процентов
double applyInterest(double amount, double rate) {
    return amount * (1 + rate);
}

// Моделирование каждого месяца
void simulateMonth(Person* person, int isAlice, int month) {
    // Месячные расходы увеличиваются с инфляцией (индексация происходит каждый месяц)
    if (!isAlice || month > 0) {
        person->monthlyExpenses = applyInterest(person->monthlyExpenses, person->inflationRate / 12);
        if (!person->hasHouse) {
            person->monthlyRent = applyInterest(person->monthlyRent, person->inflationRate / 12);
        }
    }

    // Уменьшение капитала за счет аренды или ипотеки и других расходов
    if (isAlice) {
        person->capital -= person->rentOrMortgage + person->monthlyExpenses;
    }
    else {
        // Если у Боба уже есть квартира, он больше не платит аренду
        if (person->hasHouse) {
            person->capital -= person->monthlyExpenses;
        }
        else {
            person->capital -= person->monthlyRent + person->monthlyExpenses;
        }
    }

    // Добавление зарплаты (индексация зарплаты происходит один раз в год)
    if ((month-1)%12==0) {
        person->salary = applyInterest(person->salary, person->inflationRate);
    }

    person->capital += person->salary;

    // Начисление процентов на остаток капитала
    person->capital = applyInterest(person->capital, person->depositRate / 12);
}

// Моделирование на несколько лет
void simulateYears(Person* alice, Person* bob, int years) {
    for (int year = 1; year <= years; year++) {
        for (int month = 0; month < MONTHS_IN_YEAR; month++) {
            simulateMonth(alice, 1, month);  // Симуляция для Алисы
            simulateMonth(bob, 0, month);    // Симуляция для Боба
            HOUSE_COST = applyInterest(HOUSE_COST, bob->inflationRate / 12);

            // Проверка, накопил ли Боб на квартиру
            if (!bob->hasHouse && bob->capital >= HOUSE_COST) {
                bob->capital -= HOUSE_COST;  // Боб покупает квартиру
                bob->hasHouse = 1;           // Теперь у Боба есть квартира
            }
        }

        // Вывод информации по итогам каждого года
        printf("Год %d-й:\n", year);
        printf("Капитал Элис: %.2f\n", alice->capital);
        if (bob->hasHouse) {
            printf("Капитал Боба (после покупки квартиры): %.2f\n", bob->capital);
        }
        else {
            printf("Капитал Боба: %.2f\n", bob->capital);
        }
        printf("Стоимость квартиры: %.2f\n", HOUSE_COST);
        printf("\n");
    }
}

// Функция для сравнения капиталов и вывод эффективной стратегии
void compareStrategies(Person alice, Person bob) {
    // Итоговый капитал Алисы — это остаток на депозите плюс стоимость квартиры
    double aliceFinalCapital = alice.capital + HOUSE_COST;

    // Итоговый капитал Боба — это остаток на депозите плюс стоимость квартиры, если он её купил
    double bobFinalCapital = bob.capital;
    if (bob.hasHouse) {
        bobFinalCapital += HOUSE_COST;
    }

    printf("После %d лет:\n", YEARS);
    printf("Конечный капитан Элис: %.2f\n", aliceFinalCapital);
    printf("Конечный капитал Боба: %.2f\n", bobFinalCapital);

    if (aliceFinalCapital > bobFinalCapital) {
        printf("Стратегия Элис более эффективна.\n");
    }
    else if (aliceFinalCapital < bobFinalCapital) {
        printf("Стратегия Боба более эффективна.\n");
    }
    else {
        printf("Стратегии равнозначны.\n");
    }
}

int main() {
    setlocale(LC_ALL, "Rus");
    // Инициализация начальных данных для Алисы (покупка в ипотеку)
    Person alice = {
        1000000,               // capital (начальный капитал)
        200000,                // salary (зарплата)
        15000,                 // monthlyExpenses (месячные расходы)
        13000000 * 0.16 / 12,  // rentOrMortgage (ежемесячная оплата ипотеки)
        0.09,                  // inflationRate (инфляция 9%)
        0.20,                  // depositRate (ставка по депозиту 20%)
        0,                     // monthlyRent (нет аренды для Алисы)
        0.16,                  // mortgageRate (ипотечная ставка 16%)
        1                      // hasHouse (Алиса сразу владеет домом через ипотеку)
    };

    // Инициализация начальных данных для Боба (снимает жильё и копит на покупку)
    Person bob = {
        1000000,   // capital (начальный капитал)
        200000,    // salary (зарплата)
        15000,     // monthlyExpenses (месячные расходы)
        0,         // rentOrMortgage (нет ипотеки для Боба)
        0.09,      // inflationRate (инфляция 9%)
        0.20,      // depositRate (ставка по депозиту 20%)
        30000,     // monthlyRent (аренда 30,000 в месяц)
        0,         // mortgageRate (нет ипотеки для Боба)
        0          // hasHouse (пока нет недвижимости, Боб копит)
    };

    simulateYears(&alice, &bob, YEARS);
    compareStrategies(alice, bob);

    return 0;
}
