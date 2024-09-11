#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//data of economycs
#define WAGES 200*1000
#define INFLATION 0.08
#define INDEXCATION 0.08

//time
#define MONTHS 12
#define YEARS 30
#define NOW_YEAR 2024

//data of mortgage
#define INITIAL_PAYMENT 1000*1000
#define INTEREST_RATE 0.17
#define SUM_OF_MORTGAGE 15*1000*1000

//data of deposit and rent
#define DEPOSIT_AMOUNT 1000*1000
#define RENT 30*1000
#define DEPOSIT_PERCENTAGE 0.2

//data of monthly expenses
#define FOOD 20*1000
#define PUBLIC_UTILITIES 15*1000 
#define PERSONAL_EXPENSES 15*1000
#define TOTAL_EXPENSES (FOOD+PUBLIC_UTILITIES+PERSONAL_EXPENSES)

typedef struct mortgage {
	int initial_payment;
	float interest_rate;
	float sum_of_mortgage;
} mortgage;

typedef struct deposit_and_rent {
	float deposit_amount;
	float rent;
	float deposit_percentage;
} deposit_and_rent;

float payment(mortgage Alice, int years) {
	float monthly_rate = Alice.interest_rate / MONTHS;
	float total_rate = pow((1 + monthly_rate), MONTHS*years);
	float pay = (Alice.sum_of_mortgage - Alice.initial_payment) * monthly_rate * total_rate / (total_rate - 1);
	return pay;
}

int check_conditions(float wages, float pay, float exp) {
	if((pay + exp) > wages) {
		return 0;
	}
	return 1;
}

float Capital_of_mortgage(mortgage Alice, float wages, float exp) {
	float capital = 0, pay = payment(Alice, YEARS), apartment_price = Alice.sum_of_mortgage;	
	for(int i=0; i<YEARS*MONTHS; ++i) {
		capital += wages - (pay + exp);
		if(i%12 == 0) {
			if(check_conditions(wages, pay, exp) == 0) {
				printf("You will not have monthly salary for mortgage in %d year\n", i+NOW_YEAR);
				printf("Your wages = %f, but mortgage payment = %f and expenses = %f\n", wages, pay, exp);
				return 0;
			}
			wages = wages * (1 + INDEXCATION);
			exp = exp * (1 + INFLATION);
			apartment_price = apartment_price * (1 + INFLATION);
		}
	}
	return capital + apartment_price;
}

float Capital_of_deposit_and_rent(deposit_and_rent Bob, float wages, float exp) {
	float capital = 0, pay = RENT, deposit = Bob.deposit_amount;
	for(int i=0; i<YEARS*MONTHS; ++i) {
		capital += wages - (pay + exp);
		if(i%12 == 0) {
			if(check_conditions(wages, pay, exp) == 0) {
				printf("You will not have enough monthly salary for rent in %d year\n", i+NOW_YEAR);
				printf("Your wages = %f, but rent = %f and expenses = %f\n", wages, pay, exp);
				return 0;
			}
			deposit = deposit * (1 + Bob.deposit_percentage);
			pay = pay * (1 + INFLATION);
			wages = wages * (1 + INDEXCATION);
			exp = exp * (1 + INFLATION);
		}
	}
	return capital + deposit;
}

void result(float Alice_capital, float Bob_capital) {
	if(!Alice_capital) {
		printf("Bob`s strategy is the only option under these conditions\n");
		return;
	}
	if(!Bob_capital) {
		printf("Alice`s strategy is the only option under these conditions\n");
		return;
	}
	if(Alice_capital > Bob_capital) {
		printf("Alice`s strategy is more profitable\n");
		return;
	} else if(Bob_capital > Alice_capital) {
		printf("Bob`s strategy is more profitable\n");
		return;
	} else {
		printf("Both strategies are equivalent\n");
		return;
	}

}

int main() {
	mortgage Alice = {
		.initial_payment = INITIAL_PAYMENT,
		.interest_rate = INTEREST_RATE,
	  	.sum_of_mortgage = SUM_OF_MORTGAGE		  
	};
	deposit_and_rent Bob = {
		.deposit_amount = DEPOSIT_AMOUNT,
		.rent = RENT,
		.deposit_percentage = DEPOSIT_PERCENTAGE	
	};
	float Alice_capital = Capital_of_mortgage(Alice, WAGES, TOTAL_EXPENSES);
	float Bob_capital = Capital_of_deposit_and_rent(Bob, WAGES, TOTAL_EXPENSES);
	result(Alice_capital, Bob_capital);
	return 0;
}
