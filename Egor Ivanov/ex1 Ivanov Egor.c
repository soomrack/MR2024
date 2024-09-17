#include <stdio.h>
typedef long long int Money;// kap

struct Mortgage {
	Money homecost;
	Money first_pay;
	Money monthly_payments;
	double rate;
};

struct Person
{
	Money capital;
	Money salary;
	Money eat;
	Money utilities;
	Money rent;
	Money personalpayments;
	Mortgage mortgage;
};
	Person alice;
	Person bob;


	void alice_init()
	{
	alice.capital = 1000*1000*100;
    alice.salary = 200 * 1000 * 100;
    alice.eat = 100 * 100*100;
    alice.utilities = 5 * 1000*100;
    alice.personalpayments = 1000 * 1000; 
    alice.mortgage.homecost = 13 * 1000 * 1000*100;
    alice.mortgage.monthly_payments = 16137084;
	alice.mortgage.rate = 0.17;
	alice.mortgage.first_pay = 1000 * 1000;
	alice.capital -= alice.mortgage.first_pay;
	}


	void bob_init()
	{
    bob.capital = 1000*1000*100; 
    bob.salary = 200 * 1000 * 100;
    bob.eat = 100 * 100 * 100;
    bob.utilities = 5 * 1000 * 100;
    bob.personalpayments = 1000 * 1000;
    bob.rent = 3*1000*1000;
	}


	void alice_salary(const int month, const int year)
	{
		alice.capital += alice.salary;
	
		if (month == 12) {
			alice.salary *= 1.07;
		}
	}


	void alice_expenses(const int month, const int year)
	{
		alice.capital -= alice.eat;
		alice.capital -= alice.utilities;
		alice.capital -= alice.personalpayments;
		alice.capital -= alice.mortgage.monthly_payments;
		if (month == 12) {
			alice.eat *= 1.09;
			alice.utilities *= 1.09;
			alice.personalpayments *= 1.09;
			alice.capital *= 1.02;
			alice.mortgage.homecost *= 1.09;
		}
	}


	void bob_salary(const int month, const int year)
	{
		bob.capital += bob.salary;

		if (month == 12) {
			bob.salary *= 1.07;
		}
	}


	void bob_expenses(const int month, const int year)
	{
		bob.capital -= bob.eat;
		bob.capital -= bob.utilities;
		bob.capital -= bob.personalpayments;
		bob.capital -= bob.rent;
		if (month == 12) {
			bob.eat *= 1.09;
			bob.utilities *= 1.09;
			bob.personalpayments *= 1.09;
			bob.capital *= 1.02;
		}
	}
	/*void alice_disaster(const int month, const int year)
	{
		if (month == 7 && year == 2028) {
			alice.capital -= 50* 100*100;// ctiralka
		}
	}*/

	void alicebob_print()
	{
		printf("Alice capital =%lld kap\n", alice.capital);
		printf("Bob capital =%lld kap\n", bob.capital);
	}


	void simulation()
	{
		int year = 2024;
		int month = 9;
		while (! ((year == 2024+30 )&& (month ==10)))
	{
		alice_salary(month, year);
		alice_expenses(month, year);
		bob_salary(month, year);
		bob_expenses(month, year);
		//alice_disaster(month, year);
		++month;
			if (month == 13) {
				month = 1;
				++year;
			}
		}
	}


	int main()
	{
		alice_init();
		bob_init();
		simulation();
		alicebob_print();
	}