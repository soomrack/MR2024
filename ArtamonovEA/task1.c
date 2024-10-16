#include <stdio.h>
#include <math.h>
#include <locale.h>

typedef long long int Money;
typedef double PercentagePt;

const PercentagePt INFLATION_RATE = 0.09;
const PercentagePt SALARY_INDEX_RATE = 0.03;


typedef struct
{
    Money deposit;
    PercentagePt deposit_rate;
    Money money_on_bank_account;
    Money salary;
    Money food_expenses;
    Money personal_expenses;
    Money utility_expenses;
    Money pay_for_flat;
    Money property;
    Money pet_expenses;
} Person;


typedef struct
{
    PercentagePt mortgage_rate;
    Money sum_of_mortgage;
    Money first_pay;
    Money monthly_pay;
    int duration_years;
    int start_year;
    int start_month;
} Mortgage;


typedef struct
{
    char name[50];
    Money cost;
    Money food_cost;
    Money wash_cost;
    Money generated_income;
    int year_of_buy;
    int month_of_buy;
    int year_of_death;
    int month_of_death;
} Pet;


Person bob;
Person alice;
Mortgage alice_mortgage;
Pet bob_cat;
Pet bob_dog;


Money calculate_mortgage_pay(Mortgage mortgage)
{
    int number_of_months = mortgage.duration_years * 12;
    PercentagePt monthly_percent = mortgage.mortgage_rate / 12;
    Money payment = (mortgage.sum_of_mortgage - mortgage.first_pay) * 
        (monthly_percent * pow((1. + monthly_percent), number_of_months) / 
            (pow((1. + monthly_percent), number_of_months) - 1));

    return payment;
}


void alice_mortgage_init(const int year, const int month)
{
    alice_mortgage.duration_years = 30;
    alice_mortgage.first_pay = 1000 * 1000;
    alice_mortgage.mortgage_rate = 0.17;
    alice_mortgage.sum_of_mortgage = 13 * 1000 * 1000;
    alice_mortgage.monthly_pay = calculate_mortgage_pay(alice_mortgage);
    alice_mortgage.start_year = year;
    alice_mortgage.start_month = month;
}


void alice_init()
{
    alice.money_on_bank_account = 0;
    alice.deposit = 1000 * 1000;
    alice.deposit_rate = 0.2;
    alice.salary = 200 * 1000;

    alice.food_expenses = 15000;
    alice.personal_expenses = 10000;
    alice.utility_expenses = 5000;
    alice.pay_for_flat = alice_mortgage.monthly_pay;
    alice.property = 0;
}


void bob_init()
{
    bob.money_on_bank_account = 0;
    bob.deposit = 1000 * 1000;
    bob.deposit_rate = 0.2;
    bob.salary = 200 * 1000;

    bob.food_expenses = 15000;
    bob.personal_expenses = 10000;
    bob.utility_expenses = 5000;
    bob.pay_for_flat = 30000;
    bob.property = 0;
}


void alice_buying_flat()
{
    alice.deposit -= alice_mortgage.first_pay;
    alice.property += alice_mortgage.sum_of_mortgage; 
}


void bob_cat_init(const int year_of_buy, const  int month_of_buy)
{
    bob_cat.cost = 10 * 1000;
    bob_cat.food_cost = 5 * 1000;
    bob_cat.wash_cost = 1000;
    bob_cat.generated_income = 0;
    bob_cat.month_of_buy = month_of_buy;
    bob_cat.year_of_buy = year_of_buy;
    bob_cat.year_of_death = year_of_buy + 8;
    bob_cat.month_of_death = month_of_buy + 9;
}

void bob_dog_init(const int year_of_buy, const  int month_of_buy)
{
    bob_dog.cost = 100 * 1000;
    bob_dog.food_cost = 10 * 1000;
    bob_dog.wash_cost = 3 * 1000;
    bob_dog.generated_income = 50 * 1000;
    bob_dog.month_of_buy = month_of_buy;
    bob_dog.year_of_buy = year_of_buy;
    bob_dog.year_of_death = year_of_buy + 15;
    bob_dog.month_of_death = month_of_buy + 3;
}


void bob_salary(const int month)
{
    bob.money_on_bank_account += bob.salary;

    if (month == 12) {
        bob.salary *= (1. + SALARY_INDEX_RATE);
    }
}


void alice_salary(const int month, const int year)
{
    alice.money_on_bank_account += alice.salary;
     if (year == 2027 && month == 6){
        alice.salary *= 2;
     }
    if (month == 12) {
        alice.salary *= (1. + SALARY_INDEX_RATE);
    }
}


void bob_paying_expenses(const int month)
{
    bob.money_on_bank_account -= 
        (bob.food_expenses + bob.personal_expenses + bob.utility_expenses + bob.pay_for_flat);

    if (month == 12) {
        bob.food_expenses *= (1. + INFLATION_RATE);
        bob.personal_expenses *= (1. + INFLATION_RATE);
        bob.utility_expenses *= (1. + INFLATION_RATE);
        bob.property *= (1. + INFLATION_RATE);
        bob.pay_for_flat *= (1. + INFLATION_RATE);
    }
}


void alice_paying_expences(const int month)
{
    alice.money_on_bank_account -= 
        (alice.food_expenses + alice.personal_expenses + alice.utility_expenses);

    if (month == 12) {
        alice.food_expenses *= (1. + INFLATION_RATE);
        alice.personal_expenses *= (1. + INFLATION_RATE);
        alice.utility_expenses *= (1. + INFLATION_RATE);
        alice.property *= (1. + INFLATION_RATE);
    }
}


void alice_paying_mortgage(const int current_year, const int current_month)
{
    if ((current_year <= alice_mortgage.start_year + alice_mortgage.duration_years) 
            || (current_month <= alice_mortgage.start_month)) {
        alice.money_on_bank_account -= alice.pay_for_flat;
    }
    else {
        alice.pay_for_flat = 0;
    } 
}


void bob_deposit_increasing()
{
    bob.deposit *= (1 + bob.deposit_rate / 12);
    bob.deposit += bob.money_on_bank_account;
    bob.money_on_bank_account = 0;
}


void alice_deposit_increasing()
{   
    alice.deposit *= (1 + alice.deposit_rate / 12);
    alice.deposit += alice.money_on_bank_account;
    alice.money_on_bank_account = 0;
}


void bob_pet_expenses(const int current_year, const int current_month)
{
    static int has_cat = 0;
    static int has_dog = 0;
    
    if (current_year == 2029 && current_month == 7) {
        bob_cat_init(current_year, current_month);
        bob.deposit -= bob_cat.cost;
        has_cat = 1;
    }

    if (current_year == 2035 && current_month == 5) {
        bob_dog_init(current_year, current_month);
        bob.deposit -= bob_dog.cost;
        has_dog = 1;
    }

    if (has_cat && bob_cat.year_of_death == current_year && bob_cat.month_of_death == current_month) {
        has_cat = 0;
    }

    if (has_dog && bob_dog.year_of_death == current_year && bob_dog.month_of_death == current_month) {
        has_dog = 0;
    }
    
    if (has_cat) {
        bob.money_on_bank_account += (bob_cat.generated_income - bob_cat.food_cost + bob_cat.wash_cost);

        if (current_month == 12) {
            bob_cat.food_cost *= (1. + INFLATION_RATE);
            bob_cat.wash_cost *= (1. + INFLATION_RATE);
            bob_cat.cost *= (1. + INFLATION_RATE);
            bob_cat.generated_income *= (1. + INFLATION_RATE);
        }
    }

    if (has_dog) {
        bob.money_on_bank_account += (bob_dog.generated_income - bob_dog.food_cost + bob_dog.wash_cost);

        if (current_month == 12) {
            bob_dog.food_cost *= (1. + INFLATION_RATE);
            bob_dog.wash_cost *= (1. + INFLATION_RATE);
            bob_dog.cost *= (1. + INFLATION_RATE);
            bob_dog.generated_income *= (1. + INFLATION_RATE);
        }
    }
}


void calculate_bob_property_cost(const int current_year, const int current_month)
{
    if (bob_cat.year_of_death >= current_year && bob_cat.month_of_death > current_month){
        bob.property += bob_cat.cost;
    }

    if (bob_dog.year_of_death >= current_year && bob_dog.month_of_death > current_month){
        bob.property += bob_dog.cost;
    }
}


void simulation(int start_year, int start_month, int years_to_simulate)
{
    int current_year = start_year;
    int current_month = start_month;

    while ((current_year <= start_year + years_to_simulate) 
                || (current_month <= start_month)) {
    
        bob_salary(current_month);
        bob_paying_expenses(current_month);
        bob_pet_expenses(current_year, current_month);
        bob_deposit_increasing();
        
        alice_salary(current_month, current_year);
        alice_paying_expences(current_month);
        alice_paying_mortgage(current_year, current_month);
        alice_deposit_increasing();
        
        current_month ++;
        if (current_month == 13) {
            current_year++;
            current_month = 1;
        }
    }
    calculate_bob_property_cost(current_year, current_month);
}


void printing_results()
{
    printf("Bob's capital: %lld rub & property value %lld rub, Alice's capital: %lld rub & property value %lld rub", 
            bob.deposit, bob.property, alice.deposit, alice.property);
}


int main()
{
    alice_mortgage_init(2024, 9);
    bob_init();
    alice_init();
    alice_buying_flat();

    simulation(2024, 9, alice_mortgage.duration_years);

    printing_results(); 

    return 0;
}
