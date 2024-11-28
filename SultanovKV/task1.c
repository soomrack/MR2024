#include<stdio.h>
#include <math.h>

typedef long long  Money;
const double INFLATION = 0.09;                   // Инфляция
const double INDEXSATION = 0.04;                 // Индексация
const double DEPOSITE = 0.5;                     // Ставка депозита 

struct Cat
{
	Money buy;
	Money cost;
	Money funeral;
};


struct Person
{
	Money salary;                                     // Зарплата
	Money deposit;                                    // Депозит
	Money food;                                       // Еда
	Money transport;                                  // Транспорт                             
	Money expense;                                    // Вб, Озон, Различные покупки
	Money communal_bills;                             // Комунальные услуги
	Money monthly_mortgage_pay;                       // Месячная Оплата Ипотеки
	Money loan_amount;                                // Cумма Кредита
	int annual_rate;                                  // Годовая ставка
	int term;                                         // Срок аренды 
	struct Cat cat;
};


 

Person bob;
Person alice;

void bob_init()
{

	bob.salary = 10 * 10;
	bob.deposit = 1000 * 1000;
	bob.food = 20 * 1000;
	bob.transport = 6 * 1000;
	bob.expense = bob.food + bob.transport;
	bob.communal_bills= 5 * 1000;
}


void alice_init()
{

	alice.salary = 450 * 1000;
	alice.deposit = 1000 * 1000;
	alice.food = 25 * 1000;
	alice.transport = 10 * 1000;
	alice.expense = alice.food + alice.transport;
	alice.communal_bills = 4 * 1000;
	alice.monthly_mortgage_pay;
	alice.loan_amount = 15 * 1000 * 1000;
	alice.annual_rate = 16;
	alice.term = 30;
	alice.cat.buy = 30 * 1000;
	alice.cat.cost = 12 * 1000;
	alice.cat.funeral = 45 * 1000;
}


void alice_cat(const int month, const int year)
{
	static int is_cat = 0;

	if ((month == 2) && (year == 2026)) {
		alice.salary -= alice.cat.buy;
		is_cat = 1;
	}

	if (is_cat == 1) {
		if (month == 1) {
			alice.cat.cost *= (1. + INFLATION);
		}

		alice.salary -= alice.cat.cost;
	}

	if ((month == 7) && (year == 2038)) {
		alice.salary -= alice.cat.funeral;
		is_cat = 0;
	}
}


void bob_money(const int month)
{
	bob.deposit += bob.salary;

	if (month == 12)
		bob.salary *= 1. + INDEXSATION;
}


void bob_flat(const int month)
{
	bob.deposit -= bob.monthly_mortgage_pay;

	if (month == 12) {
		bob.monthly_mortgage_pay *= 1. + INFLATION;
	}
}


void bob_deposit(const int month)
{
	if (month == 12)
	{
		bob.deposit *= 1. + DEPOSITE;
	}
}


void bob_expense(const int month)
{

	bob.deposit -= bob.expense;
	if (month == 12) {
		bob.expense *= 1. + INFLATION;
	}
}


void alice_communal_bills(const int month)
{
	alice.deposit -= alice.communal_bills;

	if (month == 12) {
		alice.communal_bills *= 1. + INFLATION;
	}
}

void alice_money(const int month)
{
	alice.deposit += alice.salary;

	if (month == 12)
		alice.salary *= 1. + INDEXSATION;
}

void alice_mortgage_monthly_pay()
{
	double monthly_rate = alice.annual_rate / 12.0 / 100.0;
	int total_month = alice.term * 12;

	alice.monthly_mortgage_pay = alice.loan_amount *
		(monthly_rate * pow(1 + monthly_rate, total_month))
		/ (pow(1 + monthly_rate, total_month) - 1);

	alice.deposit -= alice.monthly_mortgage_pay;

}


void alice_expense(const int month)
{
	alice.deposit -= alice.expense;
	if (month == 12) {
		alice.expense *= 1. + INFLATION;
	}
}


void alice_deposit(const int month)
{
	if (month == 12)
	{
		alice.deposit *= 1. + DEPOSITE;
	}
}


void simulation(int month, int year)
{
	int year_now = year;
	int month_now = month;

	while (!(year == (year_now + 30) && month == month_now))
	{

		bob_money(month);
		bob_expense(month);
		bob_flat(month);
		bob_deposit(month);

		alice_money(month);
		alice_expense(month);
		alice_communal_bills(month);
		alice_deposit(month);
		alice_mortgage_monthly_pay();
		alice_cat(month, year);

		month++;

		if (month == 13) {
			month = 1;
			++year;
		}

	}
}


void print_result()
{
	printf("Bob capital = % lld \n", (Money)(bob.deposit));
	printf("Alice capital = % lld \n", (Money)(alice.deposit));
}


int main()
{
	alice_init();
	bob_init();

	simulation(9, 2024);

	print_result();

	return 0;

}
