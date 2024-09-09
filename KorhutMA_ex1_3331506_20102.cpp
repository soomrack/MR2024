#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <math.h>

typedef long long int Lli;
const double inflation = 9.0;

typedef struct Person {
	Lli bank;
	Lli salary;
	Lli home;
	Lli downplayment;
	Lli	mortgage_pay; //ipoteka
	Lli food_expenses;
	Lli utility_expenses;
	Lli other_expenses;
	Lli rent;
	int duration;
	double deposit;
	char name[5];

}Person;
Person alice;
Person bob;

void alice_init() {
	alice.bank = 0;
	alice.salary = 200 * 1000 * 100;
	alice.home = 13 * 1000 * 1000 * 100;
	alice.downplayment = 1000 * 1000 * 100;
	alice.mortgage_pay = 16137084;
	alice.food_expenses = 15 * 1000 * 100;
	alice.utility_expenses = 8000 * 100;
	alice.other_expenses = 17000 * 100;
	alice.deposit = 20;
	alice.rent = 0;
	strcpy(alice.name, "Alice");
};

void bob_init() {
	bob.bank = 1000 * 1000 * 100;
	bob.salary = 200 * 1000 * 100;
	bob.home = 0;
	bob.downplayment = 0;
	bob.mortgage_pay = 0;
	bob.food_expenses = 15000 * 100;
	bob.utility_expenses = 8000 * 100;
	bob.other_expenses = 17000 * 100;
	bob.deposit = 20;
	bob.rent = 30 * 1000 * 100;
	strcpy(bob.name, "Bob");
};

void alice_income() {
	alice.bank += (Lli)(alice.bank * alice.deposit / 100.0 / 12.0);
};

void alise_salary(const int year, const int month) {
	if (month == 12)
		alice.salary += (Lli)(alice.salary * inflation / 100.0);
	if (month != 3)
		alice.bank += alice.salary;
};

void alice_home() {
	alice.bank -= alice.mortgage_pay;
	alice.mortgage_pay += (Lli)(alice.mortgage_pay * inflation / 100.0 / 12.0);
};

void alice_expenses() {
	alice.bank -=
		(alice.food_expenses + alice.utility_expenses + alice.other_expenses);
	alice.food_expenses += (Lli)(alice.food_expenses * inflation / 100.0 / 12.0);
	alice.utility_expenses += (Lli)(alice.utility_expenses * inflation / 100.0);
	alice.other_expenses += (Lli)(alice.other_expenses * inflation / 100.0);
};

void alice_home_index(int month) {
	if (month == 12)
		alice.home += (Lli)(alice.home * (inflation / 100.0));
};

void bob_income() {
	bob.bank += (Lli)(bob.bank * bob.deposit / 100.0 / 12.0);
}

void bob_salary(const int year, const int month) {
	if (month == 12)
		bob.salary += (Lli)(bob.salary * inflation / 100.0);
	bob.bank += bob.salary;
}

void bob_rent() {
	bob.bank -= bob.rent;
}

void bob_expenses() {
	bob.bank -=
		(bob.food_expenses + bob.utility_expenses + bob.other_expenses);
	bob.food_expenses += (Lli)(bob.food_expenses * inflation / 100.0 / 12.0);
	bob.utility_expenses += (Lli)(bob.utility_expenses * inflation / 100.0);
	bob.other_expenses += (Lli)(bob.other_expenses * inflation / 100.0);
};

void simulation(int month, int year) {
	int temp_year = year;
	int temp_month = month;
	alice.bank = 0;

	while (!(year == (temp_year + 30) && month == temp_month)) {
		alise_salary(year, month);
		alice_home();
		alice_expenses();
		alice_income();
		alice_home_index(month);

		bob_salary(year, month);
		bob_rent();
		bob_expenses();
		bob_rent();
		bob_income();
		++month;

		if (month == 13) {
			month = 1;
			++year;
		}
	}
	alice.bank += alice.home;
};

void print_person(const Person person)
{
	printf("%s:\n", person.name);
	printf("  bank = %lld rub\n", (Lli)(person.bank / 100));
};

int main()
{
	alice_init();
	bob_init();

	simulation(9, 2024);

	print_person(bob);
	print_person(alice);

	return 0;
}
