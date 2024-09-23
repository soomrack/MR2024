#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct input_data {
  int years;
  int now_year;
  int total_expenses;
} input_data;

typedef struct mortgage {
	int initial_payment;
	double wages;
	double interest_rate;
	double apartment_price;
	double pay;
} mortgage;

typedef struct deposit {
	double wages;
	double deposit;
	double rent;
	double deposit_percentage;
} deposit;

double payment(const mortgage Alice, int years) {

	double monthly_rate = Alice.interest_rate / 12;
	double total_rate = pow((1 + monthly_rate), 12 * years);
	double pay = (Alice.apartment_price - Alice.initial_payment) * monthly_rate * total_rate / (total_rate - 1);

	return pay;
}

input_data Data_inite(input_data Data) {
	
	Data.years = 30;
	Data.now_year = 2024;
	Data.total_expenses = 20*1000 /*food*/ + 15*1000 /*public utilities*/ + 15*1000 /*personal expenses*/;
	
	return Data;
}

mortgage mortgage_inite(mortgage Alice, input_data Data) {
	
	Alice.wages = 200*1000;
	Alice.initial_payment = 1000*1000;
	Alice.interest_rate = 0.17;
	Alice.apartment_price = 15*1000*1000;
	Alice.pay = payment(Alice, Data.years);
	
	return Alice;
}

deposit deposit_inite(deposit Bob) {
	
	Bob.wages = 200*1000;
	Bob.deposit = 1000*1000;
	Bob.rent = 30*1000;
	Bob.deposit_percentage = 0.2;
	
	return Bob;
}

int check_conditions_mortgage(mortgage Alice, double exp, int year) {

	if((Alice.pay + exp) > Alice.wages) {
		
		printf("You will not have monthly salary for mortgage in %d year\n", year);
		printf("Your wages = %.0lf, but payment = %.0lf and expenses = %.0lf\n", Alice.wages, Alice.pay, exp);
		
		return 0;
	}

	return 1;
}

int check_conditions_deposit(deposit Bob, double exp, int year) {

	if((Bob.rent + exp) > Bob.wages) {

		printf("You will not have monthly salary for rent in %d year\n", year);
		printf("Your wages = %.0lf, but payment = %.0lf and expenses = %.0lf\n", Bob.wages, Bob.rent, exp);
		
		return 0;		
	}

	return 1;
}

double Capital_of_mortgage(mortgage Alice, input_data Data) {
	
	double capital = 0;
	double exp = Data.total_expenses;

	for(int i = Data.now_year; i < Data.now_year + Data.years; ++i) {
		
		capital += 12 * (Alice.wages - (Alice.pay + exp));

		if(check_conditions_mortgage(Alice, exp, i) == 0)
			return 0;

		Alice.wages = Alice.wages * (1 + 0.08);
		exp = exp * (1 + 0.08);
		Alice.apartment_price = Alice.apartment_price * (1 + 0.08);
	}

	return round(capital + Alice.apartment_price);
}

double Capital_of_deposit(deposit Bob, input_data Data) {
	
	double capital = 0;
	double exp = Data.total_expenses;
	
	for(int i = Data.now_year; i < Data.now_year + Data.years; ++i) {
	
		capital += Bob.wages - (Bob.rent + exp);
		
		if(check_conditions_deposit(Bob, exp, i) == 0)
				return 0;
			
		Bob.deposit = Bob.deposit * (1 + Bob.deposit_percentage);
		Bob.rent = Bob.rent * (1 + 0.08);
		Bob.wages = Bob.wages * (1 + 0.08);
		exp = exp * (1 + 0.08);
	}

	return round(capital + Bob.deposit);
}

void result(double Alice_capital, double Bob_capital) {
	
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
	
	input_data Data = Data_inite(Data);
	mortgage Alice = mortgage_inite(Alice, Data);
	deposit Bob = deposit_inite(Bob);

	double Alice_capital = Capital_of_mortgage(Alice, Data);
	double Bob_capital = Capital_of_deposit(Bob, Data);

	result(Alice_capital, Bob_capital);
	
	return 0;
}
