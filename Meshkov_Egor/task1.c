#include <stdio.h>
#include <math.h>
#include <stdlib.h>

typedef long long int Money;  // RUB


typedef struct {
    int month;
    int year;
} Date;


typedef struct {
    Date current_simulation_date;
    Date end_simulation_date;
    Date purchase_car_date;
} Important_dates;



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
    char name[5];
    Money salary;
    Money expenses;
    Money rent;
    Money money_cash;

    Money deposit;
    double deposit_percentage;

    Car car;
    Mortgage mortgage;
} Person;


const double INFLATION = 0.08 / 12;
const double INDEXCATION = 0.08 / 12; 
const Money STARTING_CAPITAL = 1000 * 1000;
const int DURATION = 30;  // years


void Alice_init(Person *Alice) {
    *Alice = (Person){
        .name = "Alice",
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
        .name = "Bob",
        .salary = 200 * 1000,
        .money_cash = 0,
        .expenses = 20 * 1000 /*food*/ + 15 * 1000 /*public utilities*/ + 15 * 1000 /*personal expenses*/,
        .rent = 30 * 1000,
        .deposit = STARTING_CAPITAL,
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
    double total_rate = pow((1 + monthly_rate), 12 * DURATION);
    double pay = (Alice->mortgage.flat_cost - STARTING_CAPITAL) * monthly_rate * total_rate / (total_rate - 1);

    return pay;
}


int is_date_equal(const Date *date_current, const Date *date_end) {

    return (date_current->year == date_end->year) && (date_current->month == date_end->month);
}


void money_cash(Person *person, const Money *payment) {
    person->money_cash = person->salary - (person->expenses + *payment);
}


int person_is_bankrupt(const Date *date_current, char name[5]) {
    printf("%s became bankrupt in the %dth month of %d year\n", name, date_current->month, date_current->year);
    return 0;
}


int check_payment_by_deposit(Person *person, const Date *date_current) {
    if(person->deposit - abs(person->money_cash) > 0) {
        person->deposit -= person->money_cash;
    } else {
        return person_is_bankrupt(date_current, person->name);
    }

    return 1;
}


int check_bankruptcy(Person *person, const Date *date_current) {
    if(person->money_cash < 0) {
        return check_payment_by_deposit(person, date_current);
    }

    return 1;
} 


void update_deposit(Person *person) {
    person->deposit = (Money)round(person->deposit * (1 + person->deposit_percentage));
    person->deposit += person->money_cash;
}


void result(const Money Alice_capital, const Money Bob_capital) {
    if(Alice_capital > Bob_capital) {
        printf("Alice`s strategy is more profitable\n");
        return;
    } else if(Bob_capital > Alice_capital) {
        printf("Bob`s strategy is more profitable\n");
        return;
    } else if(!Alice_capital && !Bob_capital){
        printf("Both strategies are equivalent\n");
        return;
    } else if(!Alice_capital) {
        printf("Bob`s strategy is the only option under these conditions\n");
        return;
    } else if(!Bob_capital) {
        printf("Alice`s strategy is the only option under these conditions\n");
        return;
    } 
}


void indexcation_salary(Person *person) {
    person->salary = person->salary * (1 + INDEXCATION);
}


void inflation_expenses_for_Alice(Person *Alice) {
    Alice->expenses = Alice->expenses * (1 + INFLATION);
}


void inflation_flat_cost(Person *person) {
    person->mortgage.flat_cost = person->mortgage.flat_cost * (1 + INFLATION);
}


void inflation_expenses_for_Bob(Person *Bob) {
    Bob->rent = Bob->rent * (1 + INFLATION);
    Bob->expenses = Bob->expenses * (1 + INFLATION);
}


void time_update(Date *date_current) {
    if(++date_current->month == 13) {
        ++date_current->year;
        date_current->month = 1;
    }
}


void inflation_car(Person *person) {
    person->car.car_price = person->car.car_price * (1 + INFLATION);
    person->car.maintenance = person->car.maintenance * (1 + INFLATION + 0.05 /*increase transport tax*/);
}


int is_date_buy_car(Date *date_current, Date *purchase_car_date_car) {
    return (date_current->year == purchase_car_date_car->year) && (date_current->month == purchase_car_date_car->month);
}


void error_to_buy_car(Date *purchase_car_date, char name[5]) {
    printf("%s couldn`t have a car in the %d month of %d year\n", name, purchase_car_date->month, purchase_car_date->year);
}


void try_to_buy_car(Person *person, Date *purchase_car_date_car) {
    if(person->deposit > person->car.car_price) {
        person->deposit -= person->car.car_price;
        ++person->car.count_of_car;
    } else {
        error_to_buy_car(purchase_car_date_car, person->name);
    }
}


void money_cash_after_servicing_car(Person *person) {
    person->money_cash -= person->car.maintenance;
}


void salary_Alice(Person *Alice, Money *Alice_payment) {
    money_cash(Alice, Alice_payment);

    indexcation_salary(Alice);
}


void salary_Bob(Person *Bob) {
    money_cash(Bob, &Bob->rent);

    indexcation_salary(Bob);
}


void car(Person *person, Date *date_current, Date *purchase_car_date) {
    if(is_date_buy_car(date_current, purchase_car_date)) {
        try_to_buy_car(person, purchase_car_date);
    }

    if(person->car.count_of_car == 1) {
        money_cash_after_servicing_car(person);
    }

    inflation_car(person);
}


void important_dates_init(Important_dates *dates) {
    *dates = (Important_dates){
        .current_simulation_date = {.month = 9, .year = 2024},
        .end_simulation_date = {.month = 9, .year = 2024 + DURATION},
        .purchase_car_date = {.month = 7, .year = 2030},
    };
}


Money simulation_Alice(Person *Alice) {

    Important_dates dates;
    important_dates_init(&dates);

    Money Alice_payment = mortgage_payment(Alice);

    while(!is_date_equal(&dates.current_simulation_date, &dates.end_simulation_date)) {
        salary_Alice(Alice, &Alice_payment);
        car(Alice, &dates.current_simulation_date, &dates.purchase_car_date);
        update_deposit(Alice);
        
        if(!check_bankruptcy(Alice, &dates.current_simulation_date)) {
            return 0;
        }

        inflation_expenses_for_Alice(Alice);
        inflation_flat_cost(Alice);

        time_update(&dates.current_simulation_date);
    }

return (Money)round(Alice->mortgage.flat_cost + Alice->deposit + Alice->car.car_price);
}

Money simulation_Bob(Person *Bob) {

    Important_dates dates;
    important_dates_init(&dates);

    while(!is_date_equal(&dates.current_simulation_date, &dates.end_simulation_date)) {
        salary_Bob(Bob);
        car(Bob, &dates.current_simulation_date, &dates.purchase_car_date);
        update_deposit(Bob);

        if(!check_bankruptcy(Bob, &dates.current_simulation_date)) {
            return 0;
        }
        
        inflation_expenses_for_Bob(Bob);

        time_update(&dates.current_simulation_date);
    }

    return (Money)round(Bob->deposit + Bob->car.car_price);         
}


int main() {
    Person Alice, Bob;
    
    Alice_init(&Alice);
    Bob_init(&Bob);

    result(simulation_Alice(&Alice), simulation_Bob(&Bob));

    return 1;
}
