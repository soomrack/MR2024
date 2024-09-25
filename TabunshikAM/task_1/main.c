#include <stdio.h>
#include <math.h>


typedef unsigned long long int Money; // в копейках


const double INFLATION_PERCENT = 9.;
const double IDEXATION_PERCENT = 7.;
const double DEPOSIT_PERCENT = 20;
const double MORTGAGE_PERCENT = 16;
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

struct Cat {
    int year_of_purchase;
    int month_of_purchase;
    Money cost;
    Money food_cost;
    Money medical_cost;
    Money funeral_cost;
    int duration_of_life;
};

typedef struct {
    Money bank_account;
    Money deposit;
    Money earns;
    Money month_expences;

    struct Mortgage mortgage;
    struct Rent rent;
    struct Cat cat;
} Person;


Person alice;
Person bob;


void alice_data_init() {
    alice.bank_account = (1000 * 1000) * 100;
    alice.deposit = 0;
    alice.earns = (200 * 1000) * 100;
    alice.month_expences = FOOD_COST + TRANSPORT_COST + HOUSE_BILLS + PERSONAL + alice.cat.food_cost + alice.cat.medical_cost;
    alice.mortgage.sum = (13 * 1000 * 1000) * 100;
    alice.mortgage.first_pay = (1000 * 1000) * 100;


    alice.cat.year_of_purchase = 2030;
    alice.cat.month_of_purchase = 9;
    alice.cat.cost = (50 * 1000) * 100;
    alice.cat.food_cost = (3 * 1000) * 100;
    alice.cat.medical_cost = (2 * 1000) * 100;
    alice.cat.funeral_cost = (30 * 1000) * 100;
    alice.cat.duration_of_life = 12;
}


void bob_data_init() {
    bob.bank_account = (1000 * 1000) * 100;
    bob.deposit = 0;
    bob.earns = (200 * 1000) * 100;
    bob.rent.month_pay = (70 * 1000) * 100;
    bob.month_expences = FOOD_COST + TRANSPORT_COST + HOUSE_BILLS + PERSONAL;
}


void mortgage_month_pay() {
    double month_percentage = MORTGAGE_PERCENT / (100 * 12);

    alice.bank_account -= alice.mortgage.first_pay;
    
    alice.mortgage.month_pay = (alice.mortgage.sum - alice.mortgage.first_pay) \
    * month_percentage / (1 - ( 1/ pow((1 + month_percentage), SIMULATION_TIME*12)));

    alice.mortgage.month_pay = round (alice.mortgage.month_pay);
    (Money)alice.mortgage.month_pay;
}


void cat_keeping(int year, int month) {
    if ((year == alice.cat.year_of_purchase) && (month == alice.cat.month_of_purchase));
        alice.bank_account -= alice.cat.cost;
    if ((year == alice.cat.year_of_purchase + alice.cat.duration_of_life) && (month == alice.cat.month_of_purchase));
        alice.bank_account -= alice.cat.funeral_cost;
}


void alice_earns(int year, int month) {
    
    alice.bank_account += alice.earns;
    if (month == 12) {
            alice.earns *= (1 + IDEXATION_PERCENT / 100);;
        }
}


void bob_earns(int year, int month) {
    bob.bank_account += bob.earns;
    if (month == 12) {
            bob.earns *= (1 + IDEXATION_PERCENT / 100);;
        }
}


void alice_expences(int year, int month) {
    alice.bank_account -= alice.month_expences;
    alice.bank_account -= alice.mortgage.month_pay;
    cat_keeping(year, month);

    if (month == 12) {
            alice.month_expences *= (1 + INFLATION_PERCENT / 100);
        }
}


void bob_expences(int year, int month) {
    bob.bank_account -= bob.month_expences;
    bob.bank_account -= bob.rent.month_pay;

    if (month == 12) {
            bob.month_expences *= (1 + INFLATION_PERCENT / 100);
            bob.rent.month_pay *= (1 + INFLATION_PERCENT / 100);
        }
}


void alice_deposit() {
    alice.deposit *= (1 + DEPOSIT_PERCENT / (100*12));
    alice.deposit += alice.bank_account;
    alice.bank_account = 0;
}


void bob_deposit() {
    bob.deposit *= (1 + DEPOSIT_PERCENT / (100*12));
    bob.deposit += bob.bank_account;
    bob.bank_account = 0;
}


void print() {
    alice.deposit += alice.mortgage.sum;

    printf("Alice capital = %lld\nBob capital   = %lld\n", alice.deposit, bob.deposit);
}


void simulation() {
    mortgage_month_pay();
    
    const int start_year = 2024;
    const int start_month = 9;
    int year = start_year;
    int month = start_month;
    
    while( !((year == start_year + SIMULATION_TIME) && (month == start_month + 1)) ) {

        alice_earns(year, month);  // операции с зарплатой участников
        bob_earns(year, month); 

        alice_expences(year, month);  // операции с тратами участников
        bob_expences(year, month);

        alice_deposit();  // операции с депозитами участников
        bob_deposit();
        
        ++month;
        if(month == 13) {
            
            month = 1;
            ++year;
        }
    }
}


int main()
{
    alice_data_init();
    bob_data_init();

    simulation();

    print();
    
    return 0;
}
