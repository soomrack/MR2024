#include <stdio.h>

typedef struct {
    long long int account;          // Счет
    long long int deposit;          // Депозитный счет
    long long int salary;           // Месячная зарплата
    float indexation;               // Индексация зарплаты
    long long int monthly_expenses; // Месячные расходы
} Person;

typedef struct {
    float mortgage_rate;            // Ипотечная ставка
    float deposit_rate;             // Депозитная ставка
    long long int rent;             // Аренда
    float inflation;                // Инфляция
    long long int house_cost;       // Стоимость квартиры
} Finance;

Person alice, bob;
Finance finance;

void initialize_person(Person* person, long long int account, long long int salary, float indexation, long long int monthly_expenses) {
    person->account = account;
    person->deposit = 0;
    person->salary = salary;
    person->indexation = indexation;
    person->monthly_expenses = monthly_expenses;
}

void initialize_finance(Finance* finance, float mortgage_rate, float deposit_rate, long long int rent, float inflation, long long int house_cost) {
    finance->mortgage_rate = mortgage_rate;
    finance->deposit_rate = deposit_rate;
    finance->rent = rent;
    finance->inflation = inflation;
    finance->house_cost = house_cost;
}

// Функция для начисления зарплаты и индексации
void process_salary(Person* person, int current_month, int indexation_month) {
    person->account += person->salary; // начисление месячной зарплаты
    if (current_month == indexation_month) { // индексация зарплаты в девятый месяц
        person->salary *= person->indexation;
    }
}

// Функция для управления депозитом: остаток на счету переводится на депозит, начисляются проценты
void process_deposit(Person* person, float deposit_rate) {
    person->deposit += person->account; // перевод остатка на депозитный счет
    person->account = 0; // очищаем основной счет
    person->deposit *= deposit_rate; // начисление процентов на депозит
}

// Функция для расходов: снятие денег за еду и жильё
void process_expenses(Person* person, long long int expenses) {
    person->account -= expenses;
}

// Функция для платежей по ипотеке или аренде
void process_housing(Person* person, long long int payment) {
    person->account -= payment;
}

// Ежегодное обновление расходов и стоимости квартиры с учетом инфляции
void update_inflation(Finance* finance) {
    finance->house_cost *= finance->inflation;     // увеличение стоимости квартиры
    finance->rent *= finance->inflation;           // увеличение арендной платы
}

// Проверка, может ли Боб купить квартиру
void check_bob_purchase(Person* bob, Finance* finance) {
    if (bob->deposit >= finance->house_cost) {
        bob->deposit -= finance->house_cost; // покупка квартиры
        finance->rent = 0;                   // Боб больше не платит аренду
    }
}

// Вывод текущих капиталов
void print_capital(int year) {
    printf("Year %d:\n", year);
    printf("Alice account = %lld, Deposit = %lld\n", alice.account, alice.deposit);
    printf("Bob account = %lld, Deposit = %lld\n", bob.account, bob.deposit);
}

// Функция для моделирования одного года
void simulate_year(int year, int indexation_month) {
    for (int month = 1; month <= 12; ++month) {
        // Зарплата и индексация
        process_salary(&alice, month, indexation_month);
        process_salary(&bob, month, indexation_month);

        // Траты на еду и другие расходы
        process_expenses(&alice, alice.monthly_expenses);
        process_expenses(&bob, bob.monthly_expenses);

        // Ипотека для Алисы и аренда для Боба
        process_housing(&alice, finance.house_cost * (finance.mortgage_rate - 1) / 12);
        process_housing(&bob, finance.rent);

        // Перевод остатка на депозит и начисление процентов
        process_deposit(&alice, finance.deposit_rate);
        process_deposit(&bob, finance.deposit_rate);
    }

    // Обновление стоимости квартиры и арендной платы
    update_inflation(&finance);

    // Проверка, может ли Боб купить квартиру
    check_bob_purchase(&bob, &finance);

    // Вывод результатов по окончании года
    // print_capital(year);
}

// Функция для окончательного сравнения капиталов
void compare_final_capital(int years) {
    long long alice_final = alice.deposit + finance.house_cost;
    long long bob_final = bob.deposit + (finance.rent == 0 ? finance.house_cost : 0);

    printf("\n--- After %d years ---\n", years);
    printf("Alice capital: %lld\n", alice_final);
    printf("Bob capital: %lld\n", bob_final);

    if (alice_final > bob_final) {
        printf("Alice strategy is better.\n");
    }
    else if (alice_final < bob_final) {
        printf("Bob strategy is better.\n");
    }
    else {
        printf("Strategies are similar.\n");
    }
}

int main() {
    // Инициализация начальных данных
    initialize_person(&alice, 1200000, 200000, 1.1, 40000); // Алиса
    initialize_person(&bob, 1200000, 200000, 1.07, 40000);  // Боб
    initialize_finance(&finance, 1.16, 1.15, 40000, 1.1, 13000000); // Финансовые параметры

    // Запуск симуляции на 30 лет
    for (int year = 2024; year < 2024 + 30; ++year) {
        simulate_year(year, 9); // Индексация зарплаты в сентябре каждого года (9-й месяц)
    }

    // Сравнение результатов по завершении симуляции
    compare_final_capital(30);

    return 0;
}
