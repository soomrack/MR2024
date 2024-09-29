#include <stdio.h>
#include <stdlib.h>
#include <math.h>


typedef struct Input_data {
  int years;
  //now_month is the count of months since 0 year
  int now_month;
  int total_expenses;
  double deposit_percentage;
} Input_data;


//An additional goal is to buy a car
typedef struct Data_of_car {
	double car_price; 
	int maintenance;
	int max_count_of_car;
	int real_count_of_car;
} Data_of_car;


typedef struct Mortgage {
	int initial_payment;
	double apartment_price;
	double interest_rate;
	double pay;
	double deposit;
	double wages;
} Mortgage;


typedef struct Deposit {
	double wages;
	double deposit;
	double rent;
} Deposit;


double payment(const Mortgage *Alice, const int years) {

	double monthly_rate = Alice->interest_rate / 12;
	double total_rate = pow((1 + monthly_rate), 12 * years);
	double pay = (Alice->apartment_price - Alice->initial_payment) * monthly_rate * total_rate / (total_rate - 1);

	return pay;
}


void data_init(Input_data *Data) {
	
	Data->years = 30;
	Data->now_month = 2024 * 12 /*initial year is 2024*/ + 9 /*initial year is Sentember*/;
	Data->total_expenses = 20 * 1000 /*food*/ + 15 * 1000 /*public utilities*/ + 15 * 1000 /*personal expenses*/;
	Data->deposit_percentage = 0.2 / 12;

	return;
}

void car_init(Data_of_car *Car) {

	Car->car_price = 1.5 * 1000 * 1000; 
	Car->max_count_of_car = 1;
	Car->real_count_of_car = 0;
	Car->maintenance = 25 * 1000;

	return;
}

void mortgage_init(Mortgage *Alice, Input_data *Data) {

	Alice->wages = 200 * 1000;
	Alice->initial_payment = 1000 * 1000;
	Alice->interest_rate = 0.17;
	Alice->apartment_price = 15 * 1000 * 1000;
	Alice->pay = payment(Alice, Data->years);
	Alice->deposit = 0;

	return;
}


void deposit_init(Deposit *Bob) {

	Bob->wages = 200 * 1000;
	Bob->deposit = 1000 * 1000;
	Bob->rent = 30 * 1000;
	
	return;
}


int check_conditions_mortgage(const Mortgage *Alice, const double exp, const int year, const int month) {

	if((Alice->pay + exp) > Alice->wages) {
		
		printf("You will not have monthly salary for mortgage in the %dth month of %d year\n", month, year);
		printf("Your wages = %.0lf, but payment = %.0lf and expenses = %.0lf\n", Alice->wages, Alice->pay, exp);
		
		return 0;
	}

	return 1;
}


int check_conditions_deposit(const Deposit *Bob, const double exp, const int year, const int month) {

	if((Bob->rent + exp) > Bob->wages) {
		printf("You will not have monthly salary for rent in the %dth month of %d year\n", month, year);
		printf("Your wages = %.0lf, but payment = %.0lf and expenses = %.0lf\n", Bob->wages, Bob->rent, exp);
		
		return 0;		
	}

	return 1;
} 


int check_capital_for_car(const double deposit, const double car_price ) {
	
	if(deposit - car_price > 0) {
		return 1;
	}

	return 0;
}


void dream_purchase_car_for_Alice(Input_data *Data, Mortgage *Alice, Data_of_car *Car) {
	if((Car->real_count_of_car < Car->max_count_of_car) && check_capital_for_car(Alice->deposit, Car->car_price)) {
		Alice->deposit -= Car->car_price;
		Data->total_expenses += Car->maintenance;
		Car->real_count_of_car++;
	} 

	return;
}


void dream_purchase_car_for_Bob(Input_data *Data, Deposit *Bob, Data_of_car *Car) {
	if((Car->real_count_of_car < Car->max_count_of_car) && check_capital_for_car(Bob->deposit, Car->car_price)) {
		Bob->deposit -= Car->car_price;
		Data->total_expenses += Car->maintenance;
		Car->real_count_of_car++;
	} 

	return;
}


double capital_of_mortgage(Mortgage *Alice, Input_data *Data, Data_of_car *Car) {

	for(int i = Data->now_month; i < Data->now_month + Data->years * 12; ++i) {

		if(check_conditions_mortgage(Alice, Data->total_expenses, i / 12, i % 12) == 0)
			return 0;

		Alice->deposit = Alice->deposit * (1 + Data->deposit_percentage);
		Alice->deposit += Alice->wages - (Alice->pay + Data->total_expenses);
		
		//It`s more profitable to buy a car after the interest on the deposit
		dream_purchase_car_for_Alice(Data, Alice, Car);

		if((i - Data->now_month) % 12 == 0) {
		
			Alice->wages = Alice->wages * (1 + 0.08);
			Data->total_expenses = Data->total_expenses * (1 + 0.08);
			Alice->apartment_price = Alice->apartment_price * (1 + 0.08);
			//The price of car is subject to inflation
			Car->car_price = Car->car_price * (1 + 0.08);
		}
	}

	return round(Alice->deposit + Alice->apartment_price + Car->car_price);
}


double capital_of_deposit(Deposit *Bob, Input_data *Data, Data_of_car *Car) {
	
	for(int i = Data->now_month; i < Data->now_month + Data->years * 12; ++i) {
		
		if(check_conditions_deposit(Bob, Data->total_expenses, i / 12, i % 12) == 0)
			return 0;
		
		Bob->deposit = Bob->deposit * (1 + Data->deposit_percentage);
		Bob->deposit += Bob->wages - (Bob->rent + Data->total_expenses);

		//It`s more profitable to buy a car after the interest on the deposit
		dream_purchase_car_for_Bob(Data, Bob, Car);

		if((i - Data->now_month) % 12 == 0) {
			
			Bob->wages = Bob->wages * (1 + 0.08);
			Bob->rent = Bob->rent * (1 + 0.08);
			Data->total_expenses = Data->total_expenses * (1 + 0.08);
			//The price of car is subject to inflation
			Car->car_price = Car->car_price * (1 + 0.08);
		}
	}

	return round(Bob->deposit+Car->car_price);
}


void result(const double Alice_capital, const double Bob_capital) {

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
	
	Input_data Data;
	Mortgage Alice;
	Deposit Bob;
	Data_of_car Car;

	data_init(&Data);
	mortgage_init(&Alice, &Data);
	deposit_init(&Bob);
	car_init(&Car);

	double Alice_capital = capital_of_mortgage(&Alice, &Data, &Car);
	double Bob_capital = capital_of_deposit(&Bob, &Data, &Car);

	result(Alice_capital, Bob_capital);
	
	return 0;
}
