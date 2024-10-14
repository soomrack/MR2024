#include <stdio.h>

typedef long long int Money;

struct Mortgage {
	Money sum;
	Money first_pay;
	Money monthly_payments;
	double rate;
};

struct Person {
	Money account;
	Money salary;
	struct Mortgage mortgage;
	int car;
	Money rent;
};

struct Person alice;
struct Person bob;

void alice_init() {
	alice.account = 1000 * 1000;
	alice.salary = 250 * 1000;
	
	alice.mortgage.sum = 15 * 1000 * 1000;
	alice.mortgage.first_pay = 1000 * 1000;
	alice.mortgage.rate = 0.06;
	alice.mortgage.monthly_payments = 89932; 	// https://calcus.ru/kalkulyator-ipoteki?input=eyJjdXJyZW5jeSI6IlJVQiIsInR5cGUiOiIxIiwiY29zdCI6IjE1MDAwMDAwIiwic3RhcnRfc3VtIjoiMTAwMDAwMCIsInN0YXJ0X3N1bV90eXBlIjoiMSIsInBlcmlvZCI6IjMwIiwicGVyaW9kX3R5cGUiOiJZIiwicGVyY2VudCI6IjciLCJwYXltZW50X3R5cGUiOiIxIn0=
	alice.account -= alice.mortgage.first_pay;
	
}

void alice_salary(const int month, const int year) {
	if (month == 1) {
		alice.salary *= 1.07;
	}
	alice.account += alice.salary;
}

void alice_mortgage() {
	alice.account -= alice.mortgage.monthly_payments;
}

void alice_monthly_expenses() {
	alice.account -= 30 * 1000;
}

void alice_vacation(const int month, const int year) {
	if (month == 7) {
		alice.account -= 500 * 1000;
	}
}

void alice_utilities(const int month, const int year) {
	if ((month >= 1 && month <= 3) || month == 12) {
		alice.account -= 8000;
	}
	if (month >= 5 && month <= 9) {
		alice.account -= 6000;
	}
	else {
		alice.account -= 7000;
	}
}

void alice_iphone(const int month, const int year) {
	if (month == 9 && year % 3 == 0) {
		alice.account -= 120 * 1000;
	}
}

void alice_cat(const int month, const int year) {
	if ((year >= 2032 && month >= 10) && (year <= 2040 && month <= 6)) {
		alice.account -= 4000;
	}
	if (month == 10 && year == 2033) {
		alice.account -= 10 * 1000;
	}
}

void alice_unexpected_situation(const int month, const int year) {
	if (month == 3 && year == 2047) {
		alice.account -= 200 * 1000;
	}
}

void alice_car(const int month, const int year) {
	int car_year = 0;
	if (alice.account > 6 * 1000 * 1000 && alice.car != 1) {
		alice.account -= 5 * 1000 * 1000;
		alice.car = 1;
		car_year = year;
	}
	if (alice.car == 1) {
		alice.account -= 10 * 1000; // gas
	}
	if (alice.car == 1 && month == 11) {
		alice.account -= 5250; // tax (150 hp)
	}
	if ((alice.car == 1) && ((year - car_year) % 4 == 0) && (month == 10)) {
		alice.account -= 40 * 1000; //winter tires
	}
	if ((car_year >= 4) && (car_year <= 10) && (car_year % 2 == 0) && (month == 1)) {
		alice.account -= 20 * 1000; // TO
	}
	if ((car_year > 10) && (month == 1)) {
		alice.account -= 30 * 1000;	// TO (old car)
	}
	if (car_year == 15) {
		alice.car = 0; // need to buy a new car
	}
}

void alice_print() {
	printf("Alice account = %lld \n", alice.account);
}

void bob_init() {
	bob.account = 1000 * 1000;
	bob.salary = 150 * 1000;
	bob.rent = 30 * 1000;
}

void bob_salary(const int month, const int year) {
	if (month == 1) {
		bob.salary *= 1.07;
	}
	bob.account += bob.salary;
}

void bob_rent(const int month, const int year) {
	if (month == 1) {
		bob.rent *= 1.1;
	}
	bob.account -= bob.rent;
}

void bob_monthly_expenses() {
	bob.account -= 50 * 1000;
}

void bob_vacation(const int month, const int year) {
	if (month == 6) {
		bob.account -= 1000 * 1000;
	}
}

void bob_utilities(const int month, const int year) {
	if ((month >= 1 && month <= 3) || month == 12) {
		bob.account -= 9000;
	}
	if (month >= 5 && month <= 9) {
		bob.account -= 7000;
	}
	else {
		bob.account -= 8000;
	}
}

void bob_samsung(const int month, const int year) {
	if (month == 2 && year % 3 == 0) {
		bob.account -= 120 * 1000;
	}
}

void bob_unexpected_situation(const int month, const int year) {
	if (month == 3 && year == 2050) {
		bob.account -= 500 * 1000;
	}
}

void bob_car(const int month, const int year) {
	int car_year = 0;
	if (bob.account > 11 * 1000 * 1000 && bob.car != 1) {
		bob.account -= 10 * 1000 * 1000;
		bob.car = 1;
		car_year = year;
	}
	if (bob.car == 1) {
		bob.account -= 20 * 1000; // gas
	}
	if (bob.car == 1 && month == 11) {
		bob.account -= 18375; // tax (245 hp)
	}
	if ((bob.car == 1) && ((year - car_year) % 4 == 0) && (month == 10)) {
		bob.account -= 60 * 1000; //winter tires
	}
	if ((car_year >= 4) && (car_year <= 10) && (car_year % 2 == 0) && (month == 1)) {
		bob.account -= 40 * 1000; // TO
	}
	if ((car_year > 10) && (month == 1)) {
		bob.account -= 60 * 1000;	// TO (old car)
	}
	if (car_year == 10) {
		bob.car = 0; // need to buy a new car
	}
}

void bob_print() {
	printf("Bob account = %lld \n", bob.account);
}

void simulation() {
	int month = 9;
	int year = 2024;
	while (!((year == 2024 + 30) && (month == 10))) {
		alice_salary(month, year);
		alice_mortgage();
		alice_monthly_expenses();
		alice_vacation(month, year);
		alice_utilities(month, year);
		alice_iphone(month, year);
		alice_cat(month, year);
		alice_unexpected_situation(month, year);
		alice_car(month, year);

		bob_salary(month, year);
		bob_rent(month, year);
		bob_monthly_expenses();
		bob_vacation(month, year);
		bob_utilities(month, year);
		bob_samsung(month, year);
		bob_unexpected_situation(month, year);
		bob_car(month, year);

		month++;
		if (month == 13) {
			month = 1;
			year++;
		}
	}
}

int main() {
	alice_init();
	bob_init();

	simulation();

	alice_print();
	bob_print();

	return 0;
}