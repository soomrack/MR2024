#include <stdio.h>
#include <stdlib.h>
#include <math.h>


typedef struct Input_Data {
  int years;
  int initial_month;
  int initial_year;
  double deposit_percentage;
} Input_Data;


//An additional goal is to buy a car
typedef struct Data_of_Car {
	double car_price; 
	int maintenance;
	int year_of_purchase;
} Data_of_Car;


typedef struct Mortgage {
	int initial_payment;
	double apartment_price;
	double interest_rate;
	double pay;
	double deposit;
	double wages;
	int total_expenses;
} Mortgage;


typedef struct Deposit {
	double wages;
	double deposit;
	double rent;
	int total_expenses;
} Deposit;


double payment(const Mortgage *Alice, const int years) {

	double monthly_rate = Alice->interest_rate / 12;
	double total_rate = pow((1 + monthly_rate), 12 * years);
	double pay = (Alice->apartment_price - Alice->initial_payment) * monthly_rate * total_rate / (total_rate - 1);

	return pay;
}


void common_data_init(Input_Data *common_data) {
	
	common_data->initial_year = 2024;
	common_data->initial_month = 9;
	common_data->years = 30;
	common_data->deposit_percentage = 0.2 / 12;

	return;
}

void car_init(Data_of_Car *car) {

	car->car_price = 1.5 * 1000 * 1000; 
	car->year_of_purchase = 2030;
	car->maintenance = 25 * 1000;

	return;
}

void mortgage_init(Mortgage *Alice, Input_Data *common_data) {

	Alice->wages = 200 * 1000;
	Alice->initial_payment = 1000 * 1000;
	Alice->interest_rate = 0.17;
	Alice->apartment_price = 15 * 1000 * 1000;
	Alice->pay = payment(Alice, common_data->years);
	Alice->deposit = 0;
	Alice->total_expenses = 20 * 1000 /*food*/ + 15 * 1000 /*public utilities*/ + 15 * 1000 /*personal expenses*/;

	return;
}


void deposit_init(Deposit *Bob) {

	Bob->wages = 200 * 1000;
	Bob->deposit = 1000 * 1000;
	Bob->rent = 30 * 1000;
	Bob->total_expenses = 20 * 1000 /*food*/ + 15 * 1000 /*public utilities*/ + 15 * 1000 /*personal expenses*/;
	
	return;
}


int bankruptcy_check_for_Alice(Mortgage *Alice, const double exp, const int year, const int month) {
	double remaining_money = Alice->wages - (Alice->pay + exp); 

	if(remaining_money < 0) {
		if(Alice->deposit > 0) {
			Alice->deposit -= remaining_money;

		} else {
			printf("Alice became bankrupt in the %dth month of %d year\n", month, year);
			return 0;
		}
	}

	return 1;
}


int bankruptcy_check_for_Bob(Deposit *Bob, const double exp, const int year, const int now_month) {
	double remaining_money = Bob->wages - (Bob->rent + exp);

	if(remaining_money < 0) {
		if(Bob->deposit > 0) {
			Bob->deposit -= remaining_money;

		} else {
			printf("Bob became bankrupt in the %dth month of %d year\n", now_month, year);
			return 0;
		}
	}

	return 1;
} 


int dream_purchase_car_for_Alice(const Input_Data *common_data, Mortgage *Alice, Data_of_Car *car, const int now_year) {
	
	if((now_year == car->year_of_purchase) && (Alice->deposit > car->car_price)) {
		Alice->deposit -= car->car_price;
		
		return 1;
	} 

	return 0;
}


int dream_purchase_car_for_Bob(const Input_Data *common_data, Deposit *Bob, Data_of_Car *car, const int now_year) {
	
	if((now_year == car->year_of_purchase) && (Bob->deposit > car->car_price)) {
		Bob->deposit -= car->car_price;
		
		return 1;
	} 

	return 0;
}


void accounting_inflation_for_alice(const Input_Data *common_data, Mortgage *Alice, int *now_year, const int now_month) {
	
	if((now_month - common_data->initial_month) % 12 == 0) {
		++*now_year;
		Alice->wages = Alice->wages * (1 + 0.08);
		Alice->total_expenses = Alice->total_expenses * (1 + 0.08);
		Alice->apartment_price = Alice->apartment_price * (1 + 0.08);
	}

	return;
}


void increase_deposit_Alice(Mortgage *Alice, const Input_Data *common_data) {

	Alice->deposit = Alice->deposit * (1 + common_data->deposit_percentage);
	Alice->deposit += Alice->wages - (Alice->pay + Alice->total_expenses);
	
	return;
}


void inflation_of_car_Alice(Input_Data *common_data, Mortgage *Alice, Data_of_Car *car, const int now_month) {

	car->car_price = car->car_price * (1 + 0.08); //accounting inflation of car

	if(now_month % 12 == 0) 
		car->maintenance = car->maintenance * (1 + 0.1); //maintenance is also on the rise

	return;
}


double capital_of_Alice(Mortgage *Alice, Input_Data *common_data, Data_of_Car *car) {
	int now_year = common_data->initial_year;
	int count_of_car = 0;

	for(int now_month = common_data->initial_month; now_month < common_data->years * 12 + common_data->initial_month; ++now_month) {

		if(bankruptcy_check_for_Alice(Alice, Alice->total_expenses + car->maintenance * count_of_car, now_year, now_month) == 0)
			return 0;

		increase_deposit_Alice(Alice, common_data);
		
		//It`s more profitable to buy a car after the increasing on the deposit
		if(count_of_car != 1) {
			dream_purchase_car_for_Alice(common_data, Alice, car, now_year);
			++count_of_car;
		}

		inflation_of_car_Alice(common_data, Alice, car, now_month);

		accounting_inflation_for_alice(common_data, Alice, &now_year, now_month);
	}

	return round(Alice->deposit + Alice->apartment_price + car->car_price);
}


void accounting_inflation_for_Bob(Input_Data *common_data, Deposit *Bob, int *now_year, const int now_month) {
	
	if((now_month - common_data->initial_month) % 12 == 0) {
		++*now_year;
		Bob->wages = Bob->wages * (1 + 0.08);
		Bob->rent = Bob->rent * (1 + 0.08);
		Bob->total_expenses = Bob->total_expenses * (1 + 0.08);
	}

	return;
}


void increase_deposit_Bob(Deposit *Bob, const Input_Data *common_data) {

	Bob->deposit = Bob->deposit * (1 + common_data->deposit_percentage);
	Bob->deposit += Bob->wages - (Bob->rent + Bob->total_expenses);
	
	return;
}


void inflation_of_car_Bob(Input_Data *common_data, Deposit *Bob, Data_of_Car *car, const int now_month) {

	car->car_price = car->car_price * (1 + 0.08); //accounting inflation of car

	if(now_month % 12 == 0) 
		car->maintenance = car->maintenance * (1 + 0.1); //maintenance is also on the rise

	return;
}


double capital_of_Bob(Deposit *Bob, Input_Data *common_data, Data_of_Car *car) {
	int now_year = common_data->initial_year;
	int count_of_car = 0;

	for(int now_month = common_data->initial_month; now_month < common_data->years * 12 + common_data->initial_month; ++now_month) {
		
		if(bankruptcy_check_for_Bob(Bob, Bob->total_expenses + car->maintenance * count_of_car, now_year, now_month) == 0)
			return 0;
		
		increase_deposit_Bob(Bob, common_data);

		//It`s more profitable to buy a car after the increasing on the deposit
		if(count_of_car != 1) {
			dream_purchase_car_for_Bob(common_data, Bob, car, now_year);
			++count_of_car;
		}

		inflation_of_car_Bob(common_data, Bob, car, now_month);

		accounting_inflation_for_Bob(common_data, Bob, &now_year, now_month);
	}

	return round(Bob->deposit + car->car_price);
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
	
	Input_Data common_data;
	Mortgage Alice;
	Deposit Bob;
	Data_of_Car car;

	common_data_init(&common_data);
	mortgage_init(&Alice, &common_data);
	deposit_init(&Bob);
	car_init(&car);

	int Alice_capital = (int)capital_of_Alice(&Alice, &common_data, &car);
	int Bob_capital = (int)capital_of_Bob(&Bob, &common_data, &car);

	result(Alice_capital, Bob_capital);
	
	return 0;
}
