#include <stdio.h>
#include <math.h>
#include <stdlib.h>

typedef long long int Money; //RUB


typedef struct {
	int month;
	int year;
} Date;


typedef struct {
	Money flat_cost;
	double rate; 
} Mortgage;


typedef struct {
	Money car_price;
	Money maintenance;
	int count_of_car;
} Car;


typedef struct {
	Money salary;
	Money expenses;
	Money rent;
	Money money_cash;

	Money deposit;
	double deposit_percentage;

	Car car;
	Mortgage mortgage;
} Person;


const double inflation = 0.08 / 12;
const double indexcation = 0.08 / 12; 
const Money starting_capital = 1000 * 1000;
const int duration = 30; // years


void Alice_init(Person *Alice) {
	*Alice = (Person){
		.salary = 200 * 1000,
		.money_cash = 0,
		.expenses = 20 * 1000 /*food*/ + 15 * 1000 /*public utilities*/ + 15 * 1000 /*personal expenses*/,
		.rent = 0,
		.deposit = 0,
		.deposit_percentage = 0.2 / 12,

		.mortgage = {
			.flat_cost = 15 * 1000 * 1000,
			.rate = 0.17,
		},

		.car = {
			.car_price = 2 * 1000 * 1000,
			.maintenance = 25 * 1000,
			.count_of_car = 0,
		}
	};
}


void Bob_init(Person *Bob) {
	*Bob = (Person){
		.salary = 200 * 1000,
		.money_cash = 0,
		.expenses = 20 * 1000 /*food*/ + 15 * 1000 /*public utilities*/ + 15 * 1000 /*personal expenses*/,
		.rent = 30 * 1000,
		.deposit = starting_capital,
		.deposit_percentage = 0.2 / 12,

		.mortgage = {
			.flat_cost = 0,
			.rate = 0.17,
		},

		.car = {
			.car_price = 2 * 1000 * 1000,
			.maintenance = 25 * 1000,
			.count_of_car = 0,
		}
	};
}


double mortgage_payment(const Person *Alice) {

	double monthly_rate = Alice->mortgage.rate / 12;
	double total_rate = pow((1 + monthly_rate), 12 * duration);
	double pay = (Alice->mortgage.flat_cost - starting_capital) * monthly_rate * total_rate / (total_rate - 1);

	return pay;
}


int is_date_equal(const Date *date_current, const Date *date_end) {

	return (date_current->year == date_end->year) && (date_current->month == date_end->month);
}


void money_cash_for_Alice(Person *Alice, const Money *Alice_payment) {
	Alice->money_cash = Alice->salary - (Alice->expenses + *Alice_payment);
}


int Alice_is_bankrupt(const Date *date_current) {
	printf("Alice became bankrupt in the %dth month of %d year\n", date_current->month, date_current->year);
	return 0;
}


int check_payment_by_Alice_deposit(Person *Alice, const Date *date_current) {
	if(Alice->deposit - abs(Alice->money_cash) > 0) {
		Alice->deposit -= Alice->money_cash;
	} else {
		return Alice_is_bankrupt(date_current);
	}

	return 1;
}


int check_bankruptcy_Alice(Person *Alice, const Date *date_current) {
	if(Alice->money_cash < 0) {
		return check_payment_by_Alice_deposit(Alice, date_current);
	}

	return 1;
} 


int Bob_is_bankrupt(const Date *date_current) {
	printf("Bob became bankrupt in the %dth month of %d year\n", date_current->month, date_current->year);
	return 0;
}


int check_payment_by_Bob_deposit(Person *Bob, const Date *date_current) {
	if(Bob->deposit - abs(Bob->money_cash) > 0) {
		Bob->deposit -= Bob->money_cash;
	} else {
		return Bob_is_bankrupt(date_current);
	}

	return 1;
}


int check_bankruptcy_Bob(Person *Bob, const Date *date_current) {
	if(Bob->money_cash < 0) {
		return check_payment_by_Bob_deposit(Bob, date_current);
	}

	return 1;
} 


void money_cash_for_Bob(Person *Bob) {
	Bob->money_cash = Bob->salary - (Bob->expenses + Bob->rent);
}


void update_deposit_for_Alice(Person *Alice) {
	Alice->deposit = (Money)round(Alice->deposit * (1 + Alice->deposit_percentage));
	Alice->deposit += Alice->money_cash;
}


void update_deposit_for_Bob(Person *Bob) {
	Bob->deposit = (Money)round(Bob->deposit * (1 + Bob->deposit_percentage));
	Bob->deposit += Bob->money_cash; 
}


void result(const Money Alice_capital, const Money Bob_capital) {

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


void indexcation_salary_for_Alice(Person *Alice) {
	Alice->salary = Alice->salary * (1 + indexcation);
}


void indexcation_salary_for_Bob(Person *Bob) {
	Bob->salary = Bob->salary * (1 + indexcation);
}


void inflation_for_Alice(Person *Alice) {
	Alice->mortgage.flat_cost = Alice->mortgage.flat_cost * (1 + inflation);
	Alice->expenses = Alice->expenses * (1 + inflation);
}


void inflation_for_Bob(Person *Bob) {
	Bob->rent = Bob->rent * (1 + inflation);
	Bob->expenses = Bob->expenses * (1 + inflation);
}


void time_update(Date *date_current) {
	if(date_current->month++ == 13) {
		++date_current->year;
		date_current->month = 1;
	}
}


void inflation_car_for_Alice(Person *Alice) {
	Alice->car.car_price = Alice->car.car_price * (1 + inflation);
	Alice->car.maintenance = Alice->car.maintenance * (1 + inflation + 0.05 /*increase transport tax*/);
}


void inflation_car_for_Bob(Person *Bob) {
	Bob->car.car_price = Bob->car.car_price * (1 + inflation);
	Bob->car.maintenance = Bob->car.maintenance * (1 + inflation + 0.05 /*increase transport tax*/);
}


int is_have_car(const Car *car) {
	return (car->count_of_car == 1) ? 1 : 0;
}


int is_date_buy_car(Date *date_current, Date *date_purchase) {
	return (date_current->year == date_purchase->year) && (date_current->month == date_purchase->month);
}


void error_to_buy_car_for_Alice(Date *date_purchase) {
	printf("Alice couldn`t have a car in the %d month of %d year\n", date_purchase->month, date_purchase->year);
}


void try_to_buy_car_for_Alice(Person *Alice, Date *date_purchase) {
	if(Alice->deposit > Alice->car.car_price) {
		Alice->deposit -= Alice->car.car_price;
		++Alice->car.count_of_car;
	} else {
		error_to_buy_car_for_Alice(date_purchase);
	}
}


void error_to_buy_car_for_Bob(Date *date_purchase) {
	printf("Bob couldn`t have a car in the %d month of %d year\n", date_purchase->month, date_purchase->year);
}


void try_to_buy_car_for_Bob(Person *Bob, Date *date_purchase) {
	if(Bob->deposit > Bob->car.car_price) {
		Bob->deposit -= Bob->car.car_price;
		++Bob->car.count_of_car;
	} else {
		error_to_buy_car_for_Bob(date_purchase);
	}
}


void money_cash_after_servicing_Alice_car(Person *Alice) {
	Alice->money_cash -= Alice->car.maintenance;
}


void money_cash_after_servicing_Bob_car(Person *Bob) {
	Bob->money_cash -= Bob->car.maintenance;
}


Money simulation_Alice(Person *Alice) {

	Date date_current = {.month = 9, .year = 2024}; //starting date
	Date date_end = date_current;
	date_end.year += duration;
	Date date_purchase = {.month = 7, .year = 2030}; //date of car purchase

	Money Alice_payment = mortgage_payment(Alice);

	while(!is_date_equal(&date_current, &date_end)) {
		money_cash_for_Alice(Alice, &Alice_payment);
		/*if(is_have_car(&Alice->car))
			money_cash_after_servicing_Alice_car(Alice);*/

		if(!check_bankruptcy_Alice(Alice, &date_current)) 
			return 0;

		update_deposit_for_Alice(Alice);
		/*if(is_date_buy_car(&date_current, &date_purchase))
			try_to_buy_car_for_Alice(Alice, &date_purchase);*/

		inflation_for_Alice(Alice);
		/*inflation_car_for_Alice(Alice);*/
		indexcation_salary_for_Alice(Alice);

		time_update(&date_current);
	}

return (Money)round(Alice->mortgage.flat_cost + Alice->deposit + Alice->car.car_price);
}

Money simulation_Bob(Person *Bob) {

	Date date_current = {.month = 9, .year = 2024}; //starting date
	Date date_end = date_current;
	date_end.year += duration;
	Date date_purchase = {.month = 7, .year = 2030}; //date of car purchase

	while(!is_date_equal(&date_current, &date_end)) {
		money_cash_for_Bob(Bob);
		/*if(is_have_car(&Bob->car))
			money_cash_after_servicing_Bob_car(Bob);*/

		if(!check_bankruptcy_Bob(Bob, &date_current)) 
			return 0;

		update_deposit_for_Bob(Bob);
		/*if(is_date_buy_car(&date_current, &date_purchase)) 
			try_to_buy_car_for_Bob(Bob, &date_purchase);*/

		inflation_for_Bob(Bob);
		//inflation_car_for_Bob(Bob);
		indexcation_salary_for_Bob(Bob);

		time_update(&date_current);
	}

	return (Money)round(Bob->deposit + Bob->car.car_price);
}


int main() {
	Person Alice, Bob;
	
	Alice_init(&Alice);
	Bob_init(&Bob);

	result(simulation_Alice(&Alice), simulation_Bob(&Bob));

	printf("%lld %lld\n", Bob.car.car_price, Bob.deposit);

	return 1;
}
