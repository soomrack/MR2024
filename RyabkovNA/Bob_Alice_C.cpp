#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef long long int Money; // RUB


typedef struct {
	Money savings;
	Money salary;
	Money monthly_fee; 
	Money expenses; 
	Money property_value; 
}Person;

Person alice;
Person bob;

static void alice_init()
{
	alice.savings = 0;
	alice.salary = 200000;
	alice.monthly_fee = 161370;
	alice.expenses = 0;
	alice.property_value = 13000000;
}

static void bob_init()
{
	bob.savings = 1000000;
	bob.salary = 200000;
	bob.monthly_fee = 60000;
	bob.expenses = 0;
}

static void salary(Person* person, const int month)
{
	person->savings += person->salary;
		if (month == 12) {
			person->salary *= 1.0 + (rand() % 51) / 1000.0;
		}
}

static void expenses(Person* person, const int month)
{
	person->expenses = 0;
	Money utility_expenses = 10000;
	Money food_expenses = person->salary * 0.12;
	Money other_expenses = person->salary * 0.05;
	person->expenses += utility_expenses + food_expenses + other_expenses + person->monthly_fee;

	if (month == 12) {
		utility_expenses *= 1.02 + (rand() % 100) / 1000.0;
		food_expenses *= 1.02 + (rand() % 100) / 1000.0;
		other_expenses *= 1.02 + (rand() % 100) / 1000.0;
		person->expenses += utility_expenses + food_expenses + other_expenses + person->monthly_fee;
	}
}

void carier(Person* person, const int month)
{ 
	if (month == 12) {
	int chance_carier = (rand() % 15);
	if (chance_carier == 7)
	{
		person->salary *= 1.2;
	}
	if (chance_carier == 14)
	{
		person->salary *= 0.8;
	}
	}
}

void random_expenses(Person* person)
{
	int chance_random_expenses = (rand() % 10);
	int sum_random_expenses = (rand() % 100);
	if (chance_random_expenses == 8) {
		Money expense = sum_random_expenses * 1000;
		if (person->savings >= expense) {
			person->savings -= expense;
		}
		else {
			person->savings = 0;
		}
	}
}

void savings(Person* person, const int month)
{
	person->savings -= person->expenses;
	if (month == 12) {
		person->savings *= 1.2;
	}
}

void print_info_alice() {
	printf("Alice savings = %lld RUB\n", alice.savings + alice.property_value);
}

void print_info_bob() {
	printf("Bob savings = %lld RUB\n", bob.savings);
}

void simulation(Person* person) 
{
	int year = 2024;
	int month = 9;

	while (!((year == 2024 + 30) && (month == 10))) {
		salary(person, month);
		expenses(person, month);
		random_expenses(person);
		savings(person, month);
		carier(person, month);

		++month;
		if (month == 13)
		{
			month = 1;
			++year;
		}
	}
}

int main() {
	srand(time(NULL));

	alice_init();
	bob_init();

	simulation(&alice);
	simulation(&bob);

	print_info_alice();
	print_info_bob();

	return 0;
}
