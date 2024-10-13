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

const Money BOB_CAR_COST = 4 * 1000 * 1000;
const Money BOB_CAR_GAS_COST = 15 * 1000;
const Money BOB_CAR_SERVICE_COST = 20 * 1000;
const Money BOB_CAR_TAX_COSTS = 30 * 1000; 

const Money BOB_SALARY_BONUS_1 = 1 * 1000 * 1000;

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

typedef struct
{
    Money cost;
    Money gas_cost;
    Money service_cost;     //  ТО один раз в год
    Money tax_cost;
    int year_of_purchasing;
    int month_of_purchasing;
} Car;

Credit alice_credit;
Person alice;
Person bob;
Car bob_car;

Money calculate_credit_payment(Credit* credit) 
{
    int months = (credit->year_end - credit->year_start) * 12 + (credit->month_end - credit->month_start);
    double month_rate  = credit->credit_rate / (double)12;
    double pow_rate = pow(1 + month_rate, (double)months);
    double payment_double = (credit->credit_cost * month_rate * pow_rate) / (pow_rate - 1);
    Money payment = (Money)round(payment_double);
    return payment;
}


void alice_credit_init(Credit* credit) 
{
    credit->year_start = CREDIT_START_YEAR;
    credit->month_start = CREDIT_START_MONTH;
    credit->year_end = CREDIT_END_YEAR;
    credit->month_end = CREDIT_END_MONTH;
    credit->credit_cost = APPARTMENT_COST;
    credit->credit_rate = CREDIT_RATE;
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

void bob_car_init(Car* bob_car, Person* bob, const int year_of_purchasing, const int month_of_purchasing)
{
    bob_car->cost = BOB_CAR_COST;
    bob_car->gas_cost = BOB_CAR_GAS_COST;
    bob_car->service_cost = BOB_CAR_SERVICE_COST;
    bob_car->tax_cost = BOB_CAR_TAX_COSTS;
    bob_car->month_of_purchasing = month_of_purchasing;
    bob_car->year_of_purchasing = year_of_purchasing;

    bob->deposit -= bob_car->cost;
}


void alice_salary(Person* alice, const int year, const int month) 
{
    if (month == 12) {
        alice->salary = (Money)(double)alice->salary * (1.0 + INFLATION_RATE);
    }

    if ((year == 2029) && (month == 5 || month == 6)) {  // alice теряет работу на 2 месяца
        return;                                 
    }

    alice->capital += alice->salary;
}


void bob_salary(Person* bob, const int year, const int month) 
{ 
    if (month == 12) {
        bob->salary = (Money)(double)bob->salary * (1.0 + INFLATION_RATE);
    }

    if ((year == 2035) && (month == 5)) {  // карьерный рост bob
        bob->salary *= 1.4;
    }

    if ((year == 2045) && (month == 11)) {
        bob->salary += BOB_SALARY_BONUS_1;
    }

    bob->capital += bob->salary;
}


void alice_apply_expenses(Person* alice, const int year, const int month) 
{ 
    alice->capital -= (alice->necessary_cost + alice->other_cost);

    if (month == 12) {
        alice->necessary_cost = (Money)(double)alice->necessary_cost * (1.0 + INFLATION_RATE);
        alice->other_cost = (Money)(double)alice->other_cost * (1.0 + INFLATION_RATE);
        alice->appartment_cost = (Money)(double)alice->appartment_cost * (1.0 + INFLATION_RATE);
    }
}



void bob_apply_expenses(Person* bob, Car* bob_car, const int year, const int month) 
{ 
    bob->capital -= (bob->necessary_cost + bob->other_cost);
    bob->capital -= bob->rent_cost;

    if (month == 12) {
        bob->necessary_cost = (Money)(double)bob->necessary_cost * (1.0 + INFLATION_RATE);
        bob->other_cost = (Money)(double)bob->other_cost * (1.0 + INFLATION_RATE);
        bob->rent_cost = (Money)(double)bob->rent_cost * (1.0 + INFLATION_RATE);
    }
}


void alice_apply_credit_payment(Person* person, Credit* credit) 
{
    person->capital -= credit->credit_payment;
}


void alice_deposit(Person* alice) 
{
    if (alice->capital > 0) {
        alice->deposit += alice->capital;
        alice->capital = 0;
    }
}


void bob_deposit(Person* bob) 
{
    if (bob->capital > 0) {
        bob->deposit += bob->capital;
        bob->capital = 0;
    }
}


void alice_apply_percent_on_deposit(Person* alice) 
{
    double deposit_double = (double)alice->deposit * (DEPOSITE_RATE / (double)12);
    alice->deposit += (Money)deposit_double;
}


void bob_apply_percent_on_deposit(Person* bob) 
{
    double deposit_double = (double)bob->deposit * (DEPOSITE_RATE / (double)12);
    bob->deposit += (Money)deposit_double;
}


void bob_apply_car_expenses(Person* bob, Car* bob_car, const int year, const int month)
{
    static int is_car = 0;
    
    if (year == 2038 && month == 3) {
        bob_car_init(bob_car, bob, year, month);
        is_car = 1;
    }
    
    if (is_car) {
        bob->capital -= bob_car->gas_cost;

        if (month == 12) {
            bob->capital -= (bob_car->service_cost + bob_car->tax_cost);

            bob_car->gas_cost = (Money)(double)bob_car->gas_cost * (1.0 + INFLATION_RATE);
            bob_car->tax_cost = (Money)(double)bob_car->tax_cost * (1.0 + INFLATION_RATE);
            bob_car->service_cost = (Money)(double)bob_car->service_cost * (1.0 + INFLATION_RATE);

        }   
    }   
}


void print_capital(Person* person) 
{ 
    printf("Финальный депозит %s: %lld руб.\n", person->name, person->deposit + person->appartment_cost);
}


void simulation() 
{
    int current_year = CREDIT_START_YEAR;
    int current_month = CREDIT_START_MONTH;

    while (!(current_year == CREDIT_START_YEAR + DURATION_OF_SIMULATION
                && current_month == CREDIT_END_MONTH)) {

        alice_salary(&alice, current_year, current_month);
        alice_apply_expenses(&alice, current_year, current_month);
        alice_deposit(&alice);
        alice_apply_percent_on_deposit(&alice);
        alice_apply_credit_payment(&alice, &alice_credit);
                

        bob_salary(&bob, current_year, current_month);
        bob_apply_expenses(&bob, &bob_car, current_year, current_month);
        bob_apply_car_expenses(&bob, &bob_car, current_year, current_month);
        bob_deposit(&bob);
        bob_apply_percent_on_deposit(&bob);
        
        current_month += 1;

        if (current_month == 13) {
            current_year++;
            current_month = 1;
        }
    }

}


int main() 
{
    alice_init(&alice);
    alice_credit_init(&alice_credit);
    bob_init(&bob);

    simulation();
    
    print_capital(&alice);
    print_capital(&bob);

    return 0;
}
