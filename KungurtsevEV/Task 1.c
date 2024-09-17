#include <stdio.h>
#include <math.h>

typedef long long int Cash;
const float inflation = 1.1;
const float indexation = 1.07;
const float deposite = 1.18;

struct Person
{
	Cash salary;
	Cash check;
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

void bob_set()
{

	bob.salary = 200 * 1000;
	bob.food = 30 * 1000;
	bob.transport = 10 * 1000;
	bob.person_expense = 50 * 1000;
	bob.flat = 30 * 1000;
	bob.check = 1000 * 1000;
	bob.expense = bob.food + bob.person_expense + bob.transport;
}


void alice_set()
{

	alice.salary = 200 * 1000;
	alice.food = 30 * 1000;
	alice.transport = 10 * 1000;
	alice.person_expense = 30 * 1000;
	alice.flat = 5 * 1000;
	alice.check = 1000 * 1000;
	alice.monthly_motgage_pay;
	alice.loan_amount = 13 * 1000 * 1000;
	alice.annual_rate = 16;
	alice.term = 30;
	alice.expense = alice.food + alice.person_expense + alice.transport;
}


void bob_salary(const int month)
{
	bob.check += bob.salary;

	if (month == 12)
		bob.salary *= indexation;
}


void bob_flat(const int month)
{
	bob.check -= bob.flat;

	if (month == 12) {
		bob.flat *= inflation;
	}
}


void bob_expense(const int month)
{

	bob.check -= bob.expense;
	if (month == 12) {
		bob.expense *= inflation;
	}
}


void bob_deposite(const int month)
{
	if (month == 12)
	{
		bob.check *= deposite;
	}
}


void alice_salary(const int month)
{
	alice.check += alice.salary;

	if (month == 12)
		alice.salary *= indexation;
}


void alice_communal_bills(const int month)
{
	alice.check -= alice.communal_bills;

	if (month == 12) {
		alice.communal_bills *= inflation;
	}
}


void alice_mortgage_monthly_pay () 
{	
	double monthly_rate = alice.annual_rate / 12.0 / 100.0; 
	int total_month = alice.term * 12;             

	alice.monthly_motgage_pay = alice.loan_amount * (monthly_rate * pow(1 + monthly_rate, total_month)) / (pow(1 + monthly_rate, total_month) - 1);

	alice.check -= alice.monthly_motgage_pay;
	
}


void alice_expense(const int month)
{
	
	alice.check -= alice.expense;
	if (month == 12) {
		alice.expense *= inflation;
	}
}


void alice_deposite(const int month)
{
	if (month == 12) 
	{
		alice.check *= deposite;
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
	alice.check += alice.loan_amount * inflation * alice.term;
	
}


void print_result()
{
	printf("Bob check = % lli rub\n", (Cash)(bob.check));
	printf("Alice check = % lli rub\n", (Cash)(alice.check));
}


int main()
{
	alice_set();
	bob_set();

	simulation(9, 2024);

	print_result();

	return 0;

}