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


void indexcation_salary_for_Alice(Person *Alice) {
    Alice->salary = Alice->salary * (1 + INDEXCATION);
}


void indexcation_salary_for_Bob(Person *Bob) {
    Bob->salary = Bob->salary * (1 + INDEXCATION);
}


void inflation_expenses_for_Alice(Person *Alice) {
    Alice->expenses = Alice->expenses * (1 + INFLATION);
}


void inflation_flat_cost(Person *Alice) {
    Alice->mortgage.flat_cost = Alice->mortgage.flat_cost * (1 + INFLATION);
}


void inflation_expenses_for_Bob(Person *Bob) {
    Bob->rent = Bob->rent * (1 + INFLATION);
    Bob->expenses = Bob->expenses * (1 + INFLATION);
}


void time_update(Date *date_current) {
    if(date_current->month++ == 13) {
        ++date_current->year;
        date_current->month = 1;
    }
}


void inflation_car_for_Alice(Person *Alice) {
    Alice->car.car_price = Alice->car.car_price * (1 + INFLATION);
    Alice->car.maintenance = Alice->car.maintenance * (1 + INFLATION + 0.05 /*increase transport tax*/);
}


void inflation_car_for_Bob(Person *Bob) {
    Bob->car.car_price = Bob->car.car_price * (1 + INFLATION);
    Bob->car.maintenance = Bob->car.maintenance * (1 + INFLATION + 0.05 /*increase transport tax*/);
}


int is_date_buy_car(Date *date_current, Date *date_purchase_car_car) {
    return (date_current->year == date_purchase_car_car->year) && (date_current->month == date_purchase_car_car->month);
}


void error_to_buy_car_for_Alice(Date *date_purchase_car_car) {
    printf("Alice couldn`t have a car in the %d month of %d year\n", date_purchase_car_car->month, date_purchase_car_car->year);
}


void try_to_buy_car_for_Alice(Person *Alice, Date *date_purchase_car_car) {
    if(Alice->deposit > Alice->car.car_price) {
        Alice->deposit -= Alice->car.car_price;
        ++Alice->car.count_of_car;
    } else {
        error_to_buy_car_for_Alice(date_purchase_car_car);
    }
}


void error_to_buy_car_for_Bob(Date *date_purchase_car) {
    printf("Bob couldn`t have a car in the %d month of %d year\n", date_purchase_car->month, date_purchase_car->year);
}


void try_to_buy_car_for_Bob(Person *Bob, Date *date_purchase_car) {
    if(Bob->deposit > Bob->car.car_price) {
        Bob->deposit -= Bob->car.car_price;
        ++Bob->car.count_of_car;
    } else {
        error_to_buy_car_for_Bob(date_purchase_car);
    }
}


void money_cash_after_servicing_Alice_car(Person *Alice) {
    Alice->money_cash -= Alice->car.maintenance;
}


void money_cash_after_servicing_Bob_car(Person *Bob) {
    Bob->money_cash -= Bob->car.maintenance;
}


void salary_Alice(Person *Alice, Money *Alice_payment) {
    money_cash_for_Alice(Alice, Alice_payment);

    indexcation_salary_for_Alice(Alice);

    inflation_expenses_for_Alice(Alice);
}


void car_Alice(Person *Alice, Date *date_current, Date *date_purchase_car) {

    if(is_date_buy_car(date_current, date_purchase_car)) {
        try_to_buy_car_for_Alice(Alice, date_purchase_car);
    }

    if(Alice->car.count_of_car == 1) {
        money_cash_after_servicing_Alice_car(Alice);
    }

    inflation_car_for_Alice(Alice);
}


void salary_Bob(Person *Bob) {
    money_cash_for_Bob(Bob);

    indexcation_salary_for_Bob(Bob);
}


void car_Bob(Person *Bob, Date *date_current, Date *date_purchase_car) {
    if(is_date_buy_car(date_current, date_purchase_car)) {
        try_to_buy_car_for_Bob(Bob, date_purchase_car);
    }

    if(Bob->car.count_of_car == 1) {
        money_cash_after_servicing_Bob_car(Bob);
    }

    inflation_car_for_Bob(Bob);
}


void important_dates_init(Important_dates *dates) {
    Date date_current = {.month = 9, .year = 2024};
    Date date_end = date_current;
    date_end.year += DURATION;
    Date date_purchase_car = {.month = 7, .year = 2030};
}


Money simulation_Alice(Person *Alice) {

    Important_dates dates;
    important_dates_init(&dates);

    Money Alice_payment = mortgage_payment(Alice);

    while(!is_date_equal(&dates.current_simulation_date, &dates.end_simulation_date)) {
        salary_Alice(Alice, &Alice_payment);
        car_Alice(Alice, &dates.current_simulation_date, &dates.purchase_car_date);
        update_deposit_for_Alice(Alice);
        
        if(!check_bankruptcy_Alice(Alice, &dates.current_simulation_date)) {
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
        update_deposit_for_Bob(Bob);
        car_Bob(Bob, &dates.current_simulation_date, &dates.purchase_car_date);

        if(!check_bankruptcy_Bob(Bob, &dates.current_simulation_date)) {
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
