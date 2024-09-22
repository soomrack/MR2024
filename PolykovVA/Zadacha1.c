#include <stdio.h>
#include <math.h>

typedef long long  Cash;
const double INFLATION_RATE = 0.09;
const double INDEXSATION_RATE = 0.07;
const double DEPOSITE_RATE = 0.2;

struct Person
{
	Cash salary;
	Cash deposit;
	Cash food;
	Cash transport;
	Cash person_expense;
	Cash expense;
	Cash flat;
	Cash communal_bills;
	Cash monthly_motgage_pay;
	Cash loan_amount;
	int annual_rate;
	int term;

};

Person bob;
Person alice;

void bob_init()
{

	bob.salary = 200 * 1000;
	bob.food = 30 * 1000;
	bob.transport = 10 * 1000;
	bob.person_expense = 50 * 1000;
	bob.flat = 30 * 1000;
	bob.deposit = 1000 * 1000;
	bob.expense = bob.food + bob.person_expense + bob.transport;
}


void alice_init()
{

	alice.salary = 200 * 1000;
	alice.food = 30 * 1000;
	alice.transport = 10 * 1000;
	alice.person_expense = 30 * 1000;
	alice.flat = 5 * 1000;
	alice.deposit = 1000 * 1000;
	alice.monthly_motgage_pay;
	alice.loan_amount = 13 * 1000 * 1000;
	alice.annual_rate = 16;
	alice.term = 30;
	alice.expense = alice.food + alice.person_expense + alice.transport;
}


void bob_salary(const int month)
{
	bob.deposit += bob.salary;

	if (month == 12)
		bob.salary *= 1. + INDEXSATION_RATE;
}


void bob_flat(const int month)
{
	bob.deposit -= bob.flat;

	if (month == 12) {
		bob.flat *= 1. + INFLATION_RATE;
	}
}


void bob_expense(const int month)
{

	bob.deposit -= bob.expense;
	if (month == 12) {
		bob.expense *= 1. + INFLATION_RATE;
	}
}


void bob_deposite(const int month)
{
	if (month == 12)
	{
		bob.deposit *= 1. + DEPOSITE_RATE;
	}
}


void alice_salary(const int month)
{
	alice.deposit += alice.salary;

	if (month == 12)
		alice.salary *= 1. + INDEXSATION_RATE;
}


void alice_communal_bills(const int month)
{
	alice.deposit -= alice.communal_bills;

	if (month == 12) {
		alice.communal_bills *= 1. + INFLATION_RATE;
	}
}


void alice_mortgage_monthly_pay()
{
	double monthly_rate = alice.annual_rate / 12.0 / 100.0;
	int total_month = alice.term * 12;

	alice.monthly_motgage_pay = alice.loan_amount * (monthly_rate * pow(1 + monthly_rate, total_month)) / (pow(1 + monthly_rate, total_month) - 1);

	alice.deposit -= alice.monthly_motgage_pay;

}


void alice_expense(const int month)
{

	alice.deposit -= alice.expense;
	if (month == 12) {
		alice.expense *= 1. + INFLATION_RATE;
	}
}


void alice_deposite(const int month)
{
	if (month == 12)
	{
		alice.deposit *= 1. + DEPOSITE_RATE;
	}
}


void simulation(int month, int year)
{
	int year_now = year;
	int month_now = month;

	while (!(year == (year_now + 30) && month == month_now))
	{

		bob_salary(month);
		bob_expense(month);
		bob_flat(month);
		bob_deposite(month);

		alice_salary(month);
		alice_expense(month);
		alice_communal_bills(month);
		alice_deposite(month);
		alice_mortgage_monthly_pay();

		month++;

		if (month == 13) {
			month = 1;
			++year;
		}

	}
	alice.deposit += alice.loan_amount * (1. + INFLATION_RATE) * alice.term;

}


void print_result()
{
	printf("Bob capital = % lli Rub\n", (Cash)(bob.deposit));
	printf("Alice capital = % lli Rub\n", (Cash)(alice.deposit));
}


int main()
{
	alice_init();
	bob_init();

	simulation(9, 2024);

	print_result();

	return 0;

}
