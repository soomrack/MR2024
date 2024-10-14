#include <stdio.h>
#include <math.h>


typedef unsigned long long int Money; // in kopecks


const double INFLATION_PERCENT = 9.;
const double INDEXATION_PERCENT = 8.;
const double DEPOSIT_PERCENT = 20.;
const double MORTGAGE_PERCENT = 17.;
const int SIMULATION_TIME = 30; 

const Money FOOD_COST = 15 * 1000 * 100;
const Money TRANSPORT_COST = 3 * 1000 * 100;
const Money HOUSE_BILLS = 7 * 1000 * 100;
const Money PERSONAL = 10 * 1000 * 100;


struct Mortgage {
    Money sum;
    Money first_pay;
    Money month_pay;
};

struct Rent {
    Money month_pay;
};

struct Car {
    int year_of_purchase;
    int month_of_purchase;
    Money cost;
    Money petrol_cost;
    Money maintenance_cost;
    Money transport_tax;
    int count_of_car;
};

typedef struct {
    Money bank_account;
    Money deposit;
    Money salary;
    Money month_expences;

    struct Mortgage mortgage;
    struct Rent rent;
    struct Car car;
} Person;


Person alice;
Person bob;


void alice_data_init() {
    alice.bank_account = (1000 * 1000) * 100;
    alice.deposit = 0;
    alice.salary = (200 * 1000) * 100;
    alice.month_expences = 0;
    alice.mortgage.sum = (13 * 1000 * 1000) * 100;
    alice.mortgage.first_pay = (1000 * 1000) * 100;


    alice.car.year_of_purchase = 2030;
    alice.car.month_of_purchase = 9;
    alice.car.cost = (2 * 1000 * 1000) * 100;
    alice.car.petrol_cost = (6 * 1000) * 100;
    alice.car.maintenance_cost = (10 * 1000) * 100;
    alice.car.transport_tax = (8 * 1000) * 100;
    alice.car.count_of_car = 0;
}


void bob_data_init() {
    bob.bank_account = (1000 * 1000) * 100;
    bob.deposit = 0;
    bob.salary = (200 * 1000) * 100;
    bob.rent.month_pay = (70 * 1000) * 100;
    bob.month_expences = 0;

    bob.car.year_of_purchase = 2030;
    bob.car.month_of_purchase = 9;
    bob.car.cost = (2 * 1000 * 1000) * 100;
    bob.car.petrol_cost = (6 * 1000) * 100;
    bob.car.maintenance_cost = (10 * 1000) * 100;
    bob.car.transport_tax = (8 * 1000) * 100;
    bob.car.count_of_car = 0;
}


void mortgage_month_pay() {
    double month_percentage = MORTGAGE_PERCENT / (100 * 12);

    alice.bank_account -= alice.mortgage.first_pay;
    
    alice.mortgage.month_pay = (alice.mortgage.sum - alice.mortgage.first_pay) \
        * month_percentage / (1 - (1 / pow((1 + month_percentage), SIMULATION_TIME * 12)));

    alice.mortgage.month_pay = (Money)round(alice.mortgage.month_pay);
}


void alice_car_expences(const int year, const int month) {
    if (alice.car.count_of_car == 0) {
        if ((year >= alice.car.year_of_purchase) && (month >= alice.car.month_of_purchase)) {
            if (alice.deposit > alice.car.cost) {
                alice.deposit -= alice.car.cost;
                alice.car.count_of_car = 1;
            }
        } 
    } 
    else {
        alice.bank_account -= alice.car.petrol_cost + alice.car.maintenance_cost + alice.car.transport_tax;
    }

    if (month == 12) {
        alice.car.cost *= (1. + INFLATION_PERCENT / (100 * 12));
        alice.car.petrol_cost *= (1. + INFLATION_PERCENT / (100 * 12));
        alice.car.maintenance_cost = (1. + INFLATION_PERCENT / (100 * 12));
        alice.car.transport_tax = (1. + INFLATION_PERCENT / (100 * 12));
    }
}


void bob_car_expences(const int year, const int month) {
    if (bob.car.count_of_car == 0) {
        if ((year >= bob.car.year_of_purchase) && (month >= bob.car.month_of_purchase)) {
            if (bob.deposit > bob.car.cost) {
                bob.deposit -= bob.car.cost;
                bob.car.count_of_car = 1;
            }
        } 
    }
    else {
        bob.bank_account -= bob.car.petrol_cost + bob.car.maintenance_cost + bob.car.transport_tax;
    }

    if (month == 12) {
        bob.car.cost *= (1. + INFLATION_PERCENT / (100 * 12));
        bob.car.petrol_cost *= (1. + INFLATION_PERCENT / (100 * 12));
        bob.car.maintenance_cost = (1. + INFLATION_PERCENT / (100 * 12));
        bob.car.transport_tax = (1. + INFLATION_PERCENT / (100 * 12));
    }
}


void alice_salary(const int year, const int month) {
    alice.bank_account += alice.salary;
    if (month == 12) {
        alice.salary *= (1. + INDEXATION_PERCENT / (100 * 12));
    }
}


void bob_salary(const int year, const int month) {
    bob.bank_account += bob.salary;
    if (month == 12) {
        bob.salary *= (1. + INDEXATION_PERCENT / (100 * 12));
    }
}


void alice_personal_expences(const int year, const int month, const int start_year, const int start_month) {
    if ((year == start_year) && (month == start_month)) {
        alice.month_expences = FOOD_COST + TRANSPORT_COST + HOUSE_BILLS + PERSONAL;
    }

    alice.bank_account -= alice.month_expences;

    if (month == 12) {
            alice.month_expences *= (1. + INFLATION_PERCENT / (100 * 12));
    }
}


void alice_mortgage_expences(const int year, const int month) {
    alice.bank_account -= alice.mortgage.month_pay;
}


void bob_personal_expences(const int year, const int month, const int start_year, const int start_month) {
    if ((year == start_year) && (month == start_month)) {
        bob.month_expences = FOOD_COST + TRANSPORT_COST + HOUSE_BILLS + PERSONAL;
    }

    bob.bank_account -= bob.month_expences;

    if (month == 12) {
            bob.month_expences *= (1. + INFLATION_PERCENT / (100 * 12));
    }
}


void bob_rent_expences(const int year, const int month) {
    bob.bank_account -= bob.rent.month_pay;

    if (month == 12) {
        bob.rent.month_pay *= (1. + INFLATION_PERCENT / (100 * 12));
    }
}


void alice_deposit() {
    alice.deposit *= (1. + DEPOSIT_PERCENT / (100 * 12));
    alice.deposit += alice.bank_account;
    alice.bank_account = 0;
}


void bob_deposit() {
    bob.deposit *= (1. + DEPOSIT_PERCENT / (100 * 12));
    bob.deposit += bob.bank_account;
    bob.bank_account = 0;
}


void print_simulation_result() {
    alice.deposit += alice.mortgage.sum;

    if(alice.deposit < 0) {
        printf("Alice will eventually go bankrupt\n");
    } else {
        printf("Alice capital = %lld\n", alice.deposit);
        
        if(alice.car.count_of_car == 0) {
            printf("Alice couldn't buy a car\n");
        }
    }

    if(bob.deposit > 0) {
        printf("Bob will eventually go bankrupt\n");
    } else {
        printf("Bob capital = %lld\n", bob.deposit); 

        if(bob.car.count_of_car == 0) {
            printf("Bob couldn't buy a car\n");
        }
    }
}


void time_update(int *year, int *month) {
    ++*month;
    if(*month == 13) {
        *month = 1;
        ++*year;
    }
}

   printMatrix(TMPmatrix, 3);

void simulation() {
    mortgage_month_pay();
    
    const int start_year = 2024;
    const int start_month = 9;

    int year = start_year;
    int month = start_month;

    int end_year = start_year + SIMULATION_TIME;

    int end_month = (start_month == 12) ? (end_year++, 1) : (start_month + 1);

    while( !((year == end_year) && (month == end_month)) ) {

        alice_salary(year, month);  
        alice_personal_expences(year, month, start_year, start_month);  
        alice_mortgage_expences(year, month);
        alice_car_expences(year, month);
        alice_deposit(); 
        
        bob_salary(year, month); 
        bob_personal_expences(year, month, start_year, start_month);
        bob_rent_expences(year, month);
        bob_car_expences(year, month);
        bob_deposit();
        
        time_update(&year, &month);
    }
}


int main()
{
    alice_data_init();
    bob_data_init();

    simulation();

    print_simulation_result();
    
    return 0;
}
