#include <stdio.h>
#include <math.h>
#include <string.h>

typedef long long int Money;  // Рубли 

const int DURATION_OF_SIMULATION = 30;

const double INFLATION_RATE = 0.12;  
const double DEPOSITE_RATE = 0.1;   

const Money INITIAL_CAPITAL = 0;                
const Money SALARY = 220 * 1000;                
const Money NECESSARY_COST = 30 * 1000;         
const Money OTHER_COST = 30 * 1000;             
const Money RENT_COST = 100 * 1000;              
const Money APPARTMENT_COST = 12 * 1000000; 

const int CREDIT_START_YEAR = 2024;
const int CREDIT_START_MONTH = 1;
const int CREDIT_END_YEAR = CREDIT_START_YEAR + DURATION_OF_SIMULATION;
const int CREDIT_END_MONTH = 1;
const double CREDIT_RATE = 0.15;  

typedef struct {
    char name[10];
    Money capital;          // Текущий капитал
    Money salary;           // Зарплата
    Money necessary_cost;   // Необходимые для жизни траны (еда, комунальные расходы и т.д.)
    Money other_cost;       // Другие траты (спорт, одежда и т.д.)
    Money rent_cost;        // Стоимость аренды (если арендует жильё)
    Money appartment_cost;  // Стоимость квартиры (если есть жильё)
    Money deposit;          // Вклад в банке
} Person;

typedef struct {
    int year_start;         // Начало кредита (год)
    int month_start;        // Начало кредита (месяц)
    int year_end;           // Конец кредита (год)
    int month_end;          // Конец кредита (месяц)
    Money credit_cost;      // Общая стоимость кредита
    double credit_rate;     // Процентная ставка по кредиту
    Money credit_payment;   // Ежемесячный платеж по кредиту
} Credit;


Credit alice_credit;
Person alice;
Person bob;


Money calculate_credit_payment(Credit* credit) 
{
    int months = (credit->year_end - credit->year_start) * 12 + (credit->month_end - credit->month_start);
    double month_rate  = credit->credit_rate / (double)12;
    double pow_rate = pow(1 + month_rate, (double)months);
    double payment_double = (credit->credit_cost * month_rate * pow_rate) / (pow_rate - 1);
    Money payment = (Money)round(payment_double);
    return payment;
}


void alice_credit_init(Credit* credit, int start_year, int start_month, int end_year, int end_month, Money appartment_cost, double rate) 
{
    credit->year_start = start_year;
    credit->month_start = start_month;
    credit->year_end = end_year;
    credit->month_end = end_month;
    credit->credit_cost = appartment_cost;
    credit->credit_rate = rate;
    credit->credit_payment = calculate_credit_payment(credit);
}


void alice_init(Person* alice) 
{
    strcpy(alice->name, "Alice");
    alice->capital = INITIAL_CAPITAL;
    alice->salary = SALARY;
    alice->necessary_cost = NECESSARY_COST;
    alice->other_cost = OTHER_COST;
    alice->rent_cost = 0;                      // Alice не арендует квартиру
    alice->appartment_cost = APPARTMENT_COST;  // Стоимость квартиры 
    alice->deposit = 0;
    alice_credit_init(&alice_credit, CREDIT_START_YEAR, CREDIT_START_MONTH, CREDIT_END_YEAR, CREDIT_END_MONTH, APPARTMENT_COST, CREDIT_RATE);

}


void bob_init(Person* bob) 
{
    strcpy(bob->name, "Bob");
    bob->capital = INITIAL_CAPITAL;
    bob->salary = SALARY;
    bob->necessary_cost = NECESSARY_COST;
    bob->other_cost = OTHER_COST;
    bob->rent_cost = RENT_COST;              // Bob арендует квартиру
    bob->appartment_cost = 0;                // Bob не покупает квартиру
    bob->deposit = 0;
}


void add_salary_alice(Person* alice, int month) 
{
    if (month == 5 || month == 6) {
        return;                                // alice теряет работу на 2 месяца 
    }
    alice->capital += alice->salary;
}


void add_salary_bob(Person* bob, int month) 
{ 
    int years = month / 12;
    if (years == 15) {
        bob->salary = (Money)round((double)bob->salary * 1.25);   // Увеличение ЗП на 25%
    }
    bob->capital += bob->salary;
}


void apply_expenses_alice(Person* alice) 
{ 
    alice->capital -= (alice->necessary_cost + alice->other_cost);
}



void apply_expenses_bob(Person* bob) 
{ 
    bob->capital -= (bob->necessary_cost + bob->other_cost);
    bob->capital -= bob->rent_cost;
}


void apply_credit_payment(Person* person, Credit* credit) 
{
    person->capital -= credit->credit_payment;
}


void capital_to_deposit_alice(Person* alice) 
{
    if (alice->capital > 0) {
        alice->deposit += alice->capital;
        alice->capital = 0;
    }
}


void capital_to_deposit_bob(Person* bob) 
{
    if (bob->capital > 0) {
        bob->deposit += bob->capital;
        bob->capital = 0;
    }
}


void apply_percent_on_deposit_alice(Person* alice) 
{
    double deposit_double = (double)alice->deposit * (DEPOSITE_RATE / (double)12);
    Money deposit = (Money)round(deposit_double);
    alice->deposit += deposit;
}


void apply_percent_on_deposit_bob(Person* bob) 
{
    double deposit_double = (double)bob->deposit * (DEPOSITE_RATE / (double)12);
    Money deposit = (Money)round(deposit_double);
    bob->deposit += deposit;
}


void apply_inflation_alice(Person* alice, int month) 
{
    if (month % 12 == 0) { 
        alice->salary = (Money)round((double)alice->salary * (1.0 + INFLATION_RATE));
        alice->necessary_cost = (Money)round((double)alice->necessary_cost * (1.0 + INFLATION_RATE));
        alice->other_cost = (Money)round((double)alice->other_cost * (1.0 + INFLATION_RATE));
        alice->appartment_cost = (Money)round((double)alice->appartment_cost * (1.0 + INFLATION_RATE));
    }
}


void apply_inflation_bob(Person* bob, int month) 
{
    if (month % 12 == 0) { 
        bob->salary = (Money)round((double)bob->salary * (1.0 + INFLATION_RATE));
        bob->necessary_cost = (Money)round((double)bob->necessary_cost * (1.0 + INFLATION_RATE));
        bob->other_cost = (Money)round((double)bob->other_cost * (1.0 + INFLATION_RATE));
        bob->rent_cost = (Money)round((double)bob->rent_cost * (1.0 + INFLATION_RATE));
    }
}


void printing_deposit(Person* person) 
{ 
    printf("Финальный депозит %s: %lld руб.\n", person->name, person->deposit);
}


void simulation() 
{
    int months = (CREDIT_END_YEAR - CREDIT_START_YEAR) * 12 + (CREDIT_END_MONTH - CREDIT_START_MONTH);
    
    for (int month = 1; month <= months; month++) {

        add_salary_alice(&alice, month);
        add_salary_bob(&bob, month);

        apply_expenses_alice(&alice);
        apply_expenses_bob(&bob);
        
        apply_credit_payment(&alice, &alice_credit);

        capital_to_deposit_alice(&alice);
        capital_to_deposit_bob(&bob);

        apply_percent_on_deposit_alice(&alice);
        apply_percent_on_deposit_bob(&bob);

        apply_inflation_alice(&alice, month);
        apply_inflation_bob(&bob, month);
    }

}


int main() 
{
    alice_init(&alice);
    bob_init(&bob);

    simulation();
    
    printing_deposit(&alice);
    printing_deposit(&bob);

    return 0;
}
