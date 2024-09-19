#include <stdio.h>
#include <math.h>
#include <locale.h>

typedef long long int Money;      //  В рублях
typedef double PercentagePt;

const PercentagePt INFLATION_RATE = 0.09;


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
    Money property;          // Стоимость имущества
} Person;


typedef struct
{
    PercentagePt mortgage_rate;
    Money sum_of_mortgage;
    Money first_pay;
    Money monthly_pay;
    int duration_years;
} Mortgage;


Person bob;      // Снимает квартиру
Person alice;    // Выплачивает ипотеку за квартиру
Mortgage alice_mortgage;


Money calculate_mortgage_pay(Mortgage mortgage)
{
    int number_of_months = mortgage.duration_years * 12;
    PercentagePt monthly_percent = mortgage.mortgage_rate / 12;
    Money payment = (mortgage.sum_of_mortgage - mortgage.first_pay) * (monthly_percent * pow((1. + monthly_percent), number_of_months) / (pow((1. + monthly_percent), number_of_months) - 1));

    return payment;
}


void alice_mortgage_init()
{
    alice_mortgage.duration_years = 30;
    alice_mortgage.first_pay = 1000 * 1000;
    alice_mortgage.mortgage_rate = 0.17;
    alice_mortgage.sum_of_mortgage = 15 * 1000 * 1000;
    alice_mortgage.monthly_pay = calculate_mortgage_pay(alice_mortgage);
}


void alice_init()
{
    alice.money_on_bank_account = 0;
    alice.deposit = 1000 * 1000;
    alice.deposit_rate = 0.2;
    alice.salary = 250 * 1000;

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
    bob.salary = 250 * 1000;

    bob.food_expenses = 15000;
    bob.personal_expenses = 10000;
    bob.utility_expenses = 5000;
    bob.pay_for_flat = 34000;  // Стоимость аренды квартиры
    
    bob.property = 0;
}


void alice_buying_flat()
{
    alice.deposit -= alice_mortgage.first_pay;
    alice.property += alice_mortgage.sum_of_mortgage; 
}

void salary()
{
    bob.money_on_bank_account += bob.salary;
    alice.money_on_bank_account += alice.salary;
}


void paying_expenses()
{
    alice.money_on_bank_account -= (alice.food_expenses + alice.personal_expenses + alice.utility_expenses + alice.pay_for_flat);
    bob.money_on_bank_account -= (bob.food_expenses + bob.personal_expenses + bob.utility_expenses + bob.pay_for_flat);
}


void end_of_month()  // Выплата процентов по вкладам (на минимальный остаток) и перевод остатка зарплаты на вклад
{   
    alice.deposit *= (1 + alice.deposit_rate / 12);
    alice.deposit += alice.money_on_bank_account;
    alice.money_on_bank_account = 0;

    bob.deposit *= (1 + bob.deposit_rate / 12);
    bob.deposit += bob.money_on_bank_account;
    bob.money_on_bank_account = 0;
}


void inflation()
{
    alice.food_expenses *= (1 + INFLATION_RATE);
    alice.personal_expenses *= (1 + INFLATION_RATE);
    alice.utility_expenses *= (1 + INFLATION_RATE);
    alice.property *= (1 + INFLATION_RATE);

    bob.food_expenses *= (1 + INFLATION_RATE);
    bob.personal_expenses *= (1 + INFLATION_RATE);
    bob.utility_expenses *= (1 + INFLATION_RATE);
    bob.property *= (1 + INFLATION_RATE);
    bob.pay_for_flat *= (1 + INFLATION_RATE);
}


void salary_increasing()
{
    alice.salary *= (1 + INFLATION_RATE);
    bob.salary *= (1 + INFLATION_RATE);
}


void simulation(int start_year, int start_month, int years_to_simulate)
{
    int current_year = start_year;
    int current_month = start_month;

    while (!(current_year == start_year + years_to_simulate))
    {
        
        salary();
        paying_expenses();
        end_of_month();
        
        current_month += 1;

        if (current_month == 13)
        {
            current_year++;
            current_month = 1;

            inflation();
            salary_increasing();
        }
    }
}


void printing_results()
{
    printf("Капитал Боба: %lld руб и имущество на %lld руб, капитал Элис: %lld руб и имущество на %lld руб", bob.deposit, bob.property, alice.deposit, alice.property);
}


int main()
{
    alice_mortgage_init();
    bob_init();
    alice_init();
    alice_buying_flat();

    simulation(2024, 9, alice_mortgage.duration_years);
    
    printing_results(); 

    return 0;
}