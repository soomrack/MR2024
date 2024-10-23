#include <stdio.h>
#include <math.h>
#include <locale.h>
#include <stdlib.h>

typedef long long int Kop;

Kop Amortization = 5000 * 100;
Kop Flat_price = 10 * 1000 * 1000 * 100;
double Inflation = 0.01;
int count;

struct Person {
	Kop capital;
	Kop salary;
	Kop expenses;
	Kop rent_price;
	Kop mortgage_price;
	Kop vklad;
} ;


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


void alice_salary_income(const int month) {
	if (month == 12) {
        	Alice.capital += Alice.salary / 2;
	}
	Alice.capital += Alice.salary;
	Alice.salary *= (Inflation / 12 + 1);
}


void bob_salary_income(){
	Bob.capital += Bob.salary;
	Bob.salary *= (Inflation/12 + 1);
}


void alice_deposite(const int month){
if (Alice.capital > 0){
        	Alice.vklad += Alice.capital;
		Alice.capital = 0;
    	}
if (month == 12){
        	Alice.vklad *= 1.02;
    	}
}


void bob_deposite(const int month) {
if (Bob.capital > 0){
        	Bob.vklad += Bob.capital;
		Bob.capital = 0;
    	}
if (month == 12){
        	Bob.vklad *= 1.02;
    	}
}


void alice_expenses() {
	Alice.capital -= Alice.expenses;
	Alice.expenses *= (Inflation / 12 + 1);
}


void bob_expenses(){
	Bob.capital -= Bob.expenses;
	Bob.expenses *= (Inflation / 12 + 1);
}


void alice_rent() {
Alice.capital -= Alice.rent_price;
Alice.rent_price *= (Inflation / 12 + 1);
}


void bob_rent() {
if ((Bob.capital - Flat_price + Bob.vklad) < 0) {
    	Bob.capital -= Bob.mortgage_price;
}
Bob.mortgage_price *= (Inflation / 12 + 1);
}


void alice_extra_spend() {
Alice.capital -= Amortization * (rand()%100 / 100);  
}


void bob_extra_spend() {
Bob.capital -= Amortization * (rand()%100 / 100); 
}


void simulation() {
int month = 9, year = 2023;
while(!((year == 2053) && (month == 8))){
	alice_salary_income(month);
	alice_rent();
	alice_expenses();
	alice_extra_spend();
	alice_deposite(month);

	bob_salary_income();
	bob_rent();
	bob_expenses();
	bob_extra_spend();
	bob_deposite(month);

//printf("|%0.2lld | %0.2lld|\n ", ((Bob.capital - Flat_price + Bob.vklad)/100), ((Alice.capital + Alice.vklad)/100));

	++month;
	if (month > 12){
	       	month = 1;
	       	++year;
	    	}
	}
}


void print_winner() {
	Kop Bob_capital = (Bob.capital - Flat_price + Bob.vklad) / 100;
	Kop Alice_capital = (Alice.capital + Alice.vklad) / 100;
	printf("|%0.2lld | %0.2lld|\n %s", Alice_capital, Bob_capital,  (Alice_capital > Bob_capital) ? "Alice will accumulate more capital":"Bob will accumulate more capital");
}


int main() {
 	alice_init();
	bob_init();
	simulation();
	print_winner();
    	return 0 ;
}
