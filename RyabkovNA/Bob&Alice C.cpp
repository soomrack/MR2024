#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int YEARS = 30; //для удобства использования в симуляции
int MONTHS_IN_YEAR = 12;

typedef double Money;

typedef struct {
	char name[20];
	double savings; //сбережения человека, по этому параметру в будущем можно будет сравнивать
	double salary; //зарплата
	double costs; //расходы установленные, на аренду или ипотеку
	double expenses; //прочие расходы
	double property_value; //стоимость имущества
}Person;

void apply_inflation(Money* value, double inflation_rate) { //функция для рассчёта инфляции
	*value *= inflation_rate + 1;
};

void index_salary(Money* salary, double index_rate) { //функция для рассчёта индексации
	*salary *= index_rate + 1;
};

void apply_interest(Money* savings, double interest_rate) {
	*savings *= interest_rate + 1;
};

void simulate_month(Person* person, double inflation_rate, double interest_rate) { // Функция для симуляции одного месяца
    apply_inflation(&person->expenses, inflation_rate / 12); //учитываем инфляцию
	double income = person->salary; //рассчёт доходов
	double expenses = person->expenses + person->costs; //рассчёт расходов
	person->savings += income - expenses; // Обновляем сбережения
	apply_interest(&person->savings, interest_rate / 12); // Месячный процент
}

void simulate_year(Person* person, int year) { // Функция для симуляции одного года
	double inflation_rate = 0.04 + (rand() % 40) / 1000.0; // 4-8% инфляция
	double interest_rate = 0.20; // 20% годовых на вклад
	double salary_index_rate = 0.02; // Индексация зарплаты

	// Ежегодные операции
	index_salary(&person->salary, salary_index_rate);

	// Увеличиваем стоимость недвижимости
	if (person->property_value > 0) {
		apply_inflation(&person->property_value, inflation_rate);
	}

	for (int month = 0; month < MONTHS_IN_YEAR; month += 1) {
		simulate_month(person, inflation_rate, interest_rate);
	}

	/*
	//debug
	printf("%s's status after year %d:\n", person->name, year + 1);
	printf("Savings: %.2f\n", person->savings);
	printf("Salary: %.2f\n", person->salary);
	printf("Expenses: %.2f\n", person->expenses);
	printf("Costs: %.2f\n", person->costs);
	printf("Property value: %.2f\n\n", person->property_value);
	*/
	}

int main() {
	srand(time(NULL)); // Инициализация генератора случайных чисел

	Person bob = { "Bob", 1000000, 200000, 60000, 30000, 0 }; //сбережения - 1000000, 200000 - зарплата, 30000 - аренда, 30000 - расходы на бытовые вещи, 0 - недвижимость
	Person alice = { "Alice", 0, 200000, 161370, 30000, 13000000 }; //сбережения - 0 (потратила на первый взнос), 200000 - зарплата, 161370 - ипотека, 30000 - расходы на бытовые вещи, 13000000 - недвижимость

	for (int year = 0; year < YEARS; year += 1) {
		simulate_year(&bob, year);
		simulate_year(&alice, year);
	}

	// Вывод итоговых результатов
	printf("Final results after %d years:\n", YEARS);
	printf("Bob's total capital: %.2f\n", bob.savings);
	printf("Alice's total capital: %.2f\n", alice.savings + alice.property_value);

	return 0;
}
