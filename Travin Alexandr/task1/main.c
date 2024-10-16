#include <stdio.h>
#include <math.h>
#include <locale.h>
#include <stdlib.h>

typedef long long int kop;

kop amortization = 5000 * 100;
kop flat_price = 10 * 1000 * 1000 * 100;
double inflation = 0.97;
double vklad_prots = 1.1;
int count;

struct Person {
	kop capital;
	kop salary;
	kop expenses;
	kop rent_price;
	kop mortgage_price;
	kop vklad
};


struct Person Alice, Bob;


void alice_init(){
	Alice.capital = 1000 * 1000 * 100;
	Alice.salary = 200 * 1000 * 100;
	Alice.expenses = 50 * 1000 * 100;
	Alice.rent_price = 40 * 1000 * 100;
    	Alice.vklad = 0 * 100;
}


void bob_init(){
	Bob.capital = 0 * 100;
	Bob.salary = 200 * 1000 * 100;
	Bob.expenses = 50 * 1000 * 100;
	Bob.mortgage_price = 100 * 1000 * 100;
	Bob.vklad = 0 * 100;
}


void alice_salary_income(const int mounth) {
	if (mounth == 12) {
        	Alice.capital += Alice.salary;
	}
	Alice.capital += Alice.salary;
	Alice.salary /= (inflation / 12 + 1);

}


void bob_salary_income(){
	Bob.capital += Bob.salary;
	Bob.salary /= (inflation/12 + 1);

}


void alice_deposite(const int mounth){
if (Alice.capital > 0){
        	Alice.vklad += Alice.capital;
		Alice.capital = 0;
    	}
if (mounth == 12){
        	Alice.vklad *= vklad_prots;
    	}
}


void bob_deposite(const int mounth) {
if (Bob.capital > 0){
        	Bob.vklad += Bob.capital;
		Bob.capital = 0;
    	}
if (mounth == 12){
        	Bob.vklad *= vklad_prots;
    	}
}


void alice_expenses() {
	Alice.capital -= Alice.expenses;
	Alice.expenses *= (inflation / 12 + 1);
}


void bob_expenses(){
	Bob.capital -= Bob.expenses;
	Bob.expenses *= (inflation / 12 + 1);
}

void alice_inflation(const int mounth) {
if (mounth == 12){
        	Alice.capital *= inflation;
    	}
}

void bob_inflation(const int mounth) {
if (mounth == 12){
        	Bob.capital *= inflation;
    	}
}

void alice_rent() {
Alice.capital -= Alice.rent_price;
Alice.rent_price *= (inflation / 12 + 1);
}

void bob_rent() {
Bob.capital -= Bob.mortgage_price;
Bob.mortgage_price *= (inflation / 12 + 1);
}


void alice_extra_spend() {
Alice.capital -= amortization * rand()%100 / 100;  
}


void bob_extra_spend() {
Bob.capital -= amortization * rand()%100 / 100; 
}


void simulation() {
int mounth = 9, year = 2023;
while(year < 2053 && mounth !=8){
	alice_salary_income(mounth);
	alice_deposite(mounth);
	alice_rent();
	alice_expenses();
	alice_extra_spend();
	alice_inflation(mounth);

	bob_salary_income();
	bob_deposite(mounth);
	bob_rent();
	bob_expenses();
	bob_extra_spend();
	bob_inflation(mounth);

	++mounth;
	if (mounth > 12){
	       	mounth = 1;
	       	++year;
	    	}
	}
}


void print_winner() {
	kop Bob_capital = (Bob.capital - flat_price + Bob.vklad) / 100;
	kop Alice_capital = (Alice.capital + Alice.vklad) / 100;
	printf("|%0.2lld | %0.2lld|\n %s", Alice_capital, Bob_capital,  (Alice_capital > Bob_capital) ? "Alice will accumulate more capital":"Bob will accumulate more capital");
}

int main() {
 	alice_init();
	bob_init();
	simulation();
	print_winner();
    	return 0 ;
}
