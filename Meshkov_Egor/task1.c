#include <stdio.h>
#include <stdlib.h>
#include <math.h>


typedef struct Input_Data {
  int years;
  int initial_month;
  int initial_year;
  int total_expenses;
  double deposit_percentage;
} Input_Data;


//An additional goal is to buy a car
typedef struct Data_of_Car {
	double car_price; 
	int maintenance;
	int max_count_of_car;
	int real_count_of_car;
} Data_of_Car;


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


void data_init(Input_Data *Data) {
	
	Data->initial_year = 2024;
	Data->initial_month = 9;
	Data->years = 30;
	Data->total_expenses = 20 * 1000 /*food*/ + 15 * 1000 /*public utilities*/ + 15 * 1000 /*personal expenses*/;
	Data->deposit_percentage = 0.2 / 12;

	return;
}

void car_init(Data_of_Car *Car) {

	Car->car_price = 1.5 * 1000 * 1000; 
	Car->max_count_of_car = 1;
	Car->real_count_of_car = 0;
	Car->maintenance = 25 * 1000;

	return;
}

void mortgage_init(Mortgage *Alice, Input_Data *Data) {

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


void dream_purchase_car_for_Alice(Input_Data *Data, Mortgage *Alice, Data_of_Car *Car) {
	if((Car->real_count_of_car < Car->max_count_of_car) && check_capital_for_car(Alice->deposit, Car->car_price)) {
		Alice->deposit -= Car->car_price;
		Data->total_expenses += Car->maintenance;
		Car->real_count_of_car++;
	} 

	return;
}


void dream_purchase_car_for_Bob(Input_Data *Data, Deposit *Bob, Data_of_Car *Car) {
	if((Car->real_count_of_car < Car->max_count_of_car) && check_capital_for_car(Bob->deposit, Car->car_price)) {
		Bob->deposit -= Car->car_price;
		Data->total_expenses += Car->maintenance;
		Car->real_count_of_car++;
	} 

	return;
}


void alice_increase(Input_Data *Data, Mortgage *Alice, int *now_year, const int month) {
	
	if((month - Data->initial_month) % 12 == 0) {
		++*now_year;
		Alice->wages = Alice->wages * (1 + 0.08);
		Data->total_expenses = Data->total_expenses * (1 + 0.08);
		Alice->apartment_price = Alice->apartment_price * (1 + 0.08);
	}

	return;
}


void deposit_Alice(Mortgage *Alice, const Input_Data *Data) {

	Alice->deposit = Alice->deposit * (1 + Data->deposit_percentage);
	Alice->deposit += Alice->wages - (Alice->pay + Data->total_expenses);
	
	return;
}


void buy_car_Alice(Input_Data *Data, Mortgage *Alice, Data_of_Car *Car, const int month) {
	dream_purchase_car_for_Alice(Data, Alice, Car);
	
	if((month - Data->initial_month) % 12 == 0)
		Car->car_price = Car->car_price * (1 + 0.08);

	return;
}


double capital_of_Alice(Mortgage *Alice, Input_Data *Data, Data_of_Car *Car) {
	int now_year = Data->initial_year;

	for(int month = Data->initial_month; month < Data->years * 12 + Data->initial_month; ++month) {

		if(check_conditions_mortgage(Alice, Data->total_expenses, now_year, month) == 0)
			return 0;

		deposit_Alice(Alice, Data);
		
		//It`s more profitable to buy a car after the interest on the deposit
		buy_car_Alice(Data, Alice, Car, month);

		alice_increase(Data, Alice, &now_year, month);
	}

	return round(Alice->deposit + Alice->apartment_price + Car->car_price);
}


void Bob_increase(Input_Data *Data, Deposit *Bob, int *now_year, const int month) {
	
	if((month - Data->initial_month) % 12 == 0) {
		++*now_year;
		Bob->wages = Bob->wages * (1 + 0.08);
		Bob->rent = Bob->rent * (1 + 0.08);
		Data->total_expenses = Data->total_expenses * (1 + 0.08);
	}

	return;
}


void deposit_Bob(Deposit *Bob, const Input_Data *Data) {

	Bob->deposit = Bob->deposit * (1 + Data->deposit_percentage);
	Bob->deposit += Bob->wages - (Bob->rent + Data->total_expenses);
	
	return;
}


void buy_car_Bob(Input_Data *Data, Deposit *Bob, Data_of_Car *Car, const int month) {
	dream_purchase_car_for_Bob(Data, Bob, Car);
	
	if((month - Data->initial_month) % 12 == 0)
		Car->car_price = Car->car_price * (1 + 0.08);

	return;
}


double capital_of_Bob(Deposit *Bob, Input_Data *Data, Data_of_Car *Car) {
	int now_year = Data->initial_year;

	for(int month = Data->initial_month; month < Data->initial_month + Data->years * 12; ++month) {
		
		if(check_conditions_deposit(Bob, Data->total_expenses, now_year, month) == 0)
			return 0;
		
		deposit_Bob(Bob, Data);

		//It`s more profitable to buy a car after the interest on the deposit
		buy_car_Bob(Data, Bob, Car, month);

		Bob_increase(Data, Bob, &now_year, month);
	}

	return round(Bob->deposit+Car->car_price);
}


void result(const int Alice_capital, const int Bob_capital) {

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
	
	Input_Data Data;
	Mortgage Alice;
	Deposit Bob;
	Data_of_Car Car;

	data_init(&Data);
	mortgage_init(&Alice, &Data);
	deposit_init(&Bob);
	car_init(&Car);

	int Alice_capital = (int)capital_of_Alice(&Alice, &Data, &Car);
	car_init(&Car); //Set initial values of Car
	data_init(&Data); //Set initial values fo Data
	int Bob_capital = (int)capital_of_Bob(&Bob, &Data, &Car);

	result(Alice_capital, Bob_capital);
	
	return 0;
}
