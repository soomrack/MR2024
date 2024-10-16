#include <stdio.h>
#include <math.h>

typedef long long int Coins; // рассчет в копейках

const double inflation = 1.09;
const double deposit_percent = 1.0169; // при процентой ставке 20% годовых примерный процент в месяц 1.69%

int month, year;

int start_month = 10;
int start_year = 2024;

typedef struct  Mortgage {
	Coins house_cost;
	Coins sum;
	Coins first_payment;   //ипотека
	Coins monthly_payment;
	double mortgage_percent;
}Mortgage;
   
typedef struct Cintribution {
	Coins deposit;          //вклад
	double deposit_percent; 
}Contribution;

typedef struct Household{
	Coins food;
	Coins person_expens; //бытовые расходы
}Household;
typedef struct Person {
	Coins salary;
	Mortgage mortgage;
	Contribution contribution;
	Coins house_timeprice;
	Household household;
	Coins house_bills;
	Coins rent;
	Coins status;
}Person;

Person alice;
Person bob;

void alice_start_parametrs(){
	alice.salary = 200 * 1000 * 100;
	alice.status = 1000 * 1000 * 100;

	alice.mortgage.house_cost = 5600 * 1000 * 100; //ñ ñàéòà
	alice.mortgage.first_payment = 1000 * 1000 * 100;
	alice.mortgage.sum = alice.mortgage.house_cost - alice.mortgage.first_payment;
	alice.mortgage.monthly_payment = 24694 * 100;						//ëüãîòíàÿ èïîòåêà 5%
	alice.house_bills =  2500 * 100; //ñ ñàéòà, ïî êàäàñòðó
	alice.house_timeprice = alice.mortgage.house_cost = 5600 * 1000 * 100;
	
	alice.status -= alice.mortgage.first_payment;

	alice.household.food = 10 * 000 * 100;
	alice.household.person_expens = 25 * 1000 * 100;
	alice.contribution.deposit = 0;
}

void bob_start_parametrs() {
	bob.salary = 200 * 1000 * 100;
	bob.status = 1000 * 1000 * 100;

	bob.house_bills = 2500 * 100;
	bob.rent = 40*1000 * 100; 

	bob.household.food = 10 * 1000 * 100;
	bob.household.person_expens = 25 * 1000 * 100;
	bob.contribution.deposit = 0;
}

void alice_life_for_month(const int month) {

	alice.contribution.deposit *= deposit_percent;

	alice.status += (alice.salary - alice.mortgage.monthly_payment - alice.house_bills - alice.household.food - alice.household.person_expens);
	alice.contribution.deposit += alice.status = 1000 * 1000 * 100;
	alice.status = 0;

}

void bob_life_for_month(const int month) {

	bob.contribution.deposit *= deposit_percent;

	bob.status += (bob.salary - bob.rent - bob.house_bills - bob.household.food - bob.household.person_expens);
	bob.contribution.deposit += bob.status = 1000 * 1000 * 100;
	bob.status = 0;
	
}

void alice_life_for_year(const int month) {

	if (month == 12) {
		alice.salary *= inflation;
		alice.household.food *= inflation;
		alice.household.person_expens *= inflation;
		alice.mortgage.house_cost *= inflation;
	}
}

void bob_life_for_year(const int month) {
	if (month == 12) {
		bob.salary *= inflation;
		bob.household.food *= inflation;
		bob.household.person_expens *= inflation;
		bob.rent *= inflation;
	}
}

void life(){

	int month = start_month;
	int year = start_year;
	while (!((year == start_year + 30) && (month == start_month))){
		alice_life_for_month(month);
		alice_life_for_year(month);
		bob_life_for_month(month);
		bob_life_for_year(month);
		month++;
		if (month == 13) {
			month = 1;
			year++;
		}
	}
}
int main() {
	bob_start_parametrs();
	alice_start_parametrs();

	life();
	
	printf("Alce status: %lld rubles\n", (alice.contribution.deposit + alice.mortgage.house_cost) / 100);
	printf("Bob status: %lld rubles\n", bob.contribution.deposit / 100);

		return 0;
}
