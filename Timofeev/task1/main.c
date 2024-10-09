#include <stdio.h>
#include <math.h>


typedef long long int Money;


//all money are in kopeiki
const Money food_cost = 16 * 1000 * 100;
const Money transport_cost = 2 * 1000 * 100;
const Money bills = 5 * 1000 * 100;
const Money personal_spends = 12 * 1000 * 100;

const Money standart_start_balance = (1000 * 1000) * 100;
const Money standart_salary = (200 * 1000) * 100;

const double year_inflation_per = 9.;
const double year_indexation_per = 7.;
const double deposit_per = 20.;
const double mortage_per = 16.;
const int years_of_simulation = 30;


typedef struct {
    Money deposit_sum;
    double deposit_per;
} Deposit;

typedef struct {
    int year_of_appearance;
    int month_of_payment;
    Money cost;
    Money food_cost;
    Money medical_cost;
    Money funeral_cost;
    int duration_of_life;
} Pet;

typedef struct {
    Money month_pay;
} Rent;

typedef struct {
    Money sum;
    Money first_pay;
    Money month_pay;
} Mortgage;

typedef struct {
    Money balance;
    Money salary;
    Money month_expences;
    double deposit_percent;

    Pet pet;
    Rent rent;
    Mortgage mortgage;
    Deposit deposit;
} Person;


Person alice;
Person bob;


void deposit_init(Person *person, Money deposit, const double deposit_per)
{
    person->deposit.deposit_sum = deposit;
    person->deposit.deposit_per = deposit_per;
}

void person_init(Person *person, 
                    Money deposit, 
                    Money balance, 
                    Money salary, 
                    Money month_expences, 
                    const double deposit_percent) 
{
    person->balance = balance;
    person->deposit.deposit_sum = deposit;
    person->salary = salary;
    person->month_expences = month_expences;
    person->deposit.deposit_per = deposit_percent;
}

void mortgage_init(Person *person, Money sum, Money first_pay, const double mortage_per) 
{
    person->mortgage.sum = sum;
    person->mortgage.first_pay = first_pay;

    double month_percentage = mortage_per / (100 * 12);
    person->balance -= person->mortgage.first_pay;
    person->mortgage.sum -= person->mortgage.first_pay;

    person->mortgage.month_pay = (person->mortgage.sum) \
    * month_percentage / (1 - (1 / pow((1 + month_percentage), years_of_simulation * 12)));
}

void rent_init(Person *person, Money month_pay) 
{
    person->rent.month_pay;
}

void pet_init(Person *person, 
                int year_of_appearance,
                int month_of_payment,
                Money cost,
                Money food_cost,
                Money medical_cost,
                Money funeral_cost,
                int duration_of_life) 
{

    person->pet.year_of_appearance = year_of_appearance;
    person->pet.month_of_payment = month_of_payment;
    person->pet.cost = cost;
    person->pet.food_cost = food_cost;
    person->pet.medical_cost = medical_cost;
    person->pet.funeral_cost = funeral_cost;
    person->pet.duration_of_life = duration_of_life;
}

void pet_expances(Person *person, const int year, const int month, const double year_inflation_per) 
{

    if ((year == alice.pet.year_of_appearance) && (month == alice.pet.month_of_payment))
    {
        person->balance -= person->pet.cost;
    }

    if ((year == person->pet.year_of_appearance + person->pet.duration_of_life) && (month == person->pet.month_of_payment))
    {
        person->balance -= person->pet.funeral_cost;
    }
        
    if ((year >= person->pet.year_of_appearance + person->pet.duration_of_life) && (month >= person->pet.month_of_payment))
    {
        person->balance -= person->pet.food_cost;
        person->balance -= person->pet.medical_cost;
    }

    if (month == 12) 
    {
        person->pet.cost *= (1. + year_inflation_per / (100*12));
        person->pet.funeral_cost *= (1. + year_inflation_per / (100*12));
        person->pet.food_cost *= (1. + year_inflation_per / (100*12));
        person->pet.medical_cost *= (1. + year_inflation_per / (100*12));
    }
}

void mortgage_expences(Person *person, const int year, const int month) 
{
    person->balance -= person->mortgage.month_pay;
}

void monthly_personal_expences(Person *person, 
                                const int year, 
                                const int month, 
                                const int start_year, 
                                const int start_month, 
                                const double year_inflation_per) 
{
    if ((year == start_year) && (month == start_month)) 
    {
        person->month_expences = food_cost + transport_cost + bills + personal_spends;
    }

    person->balance -= person->month_expences;

    if (month == 12) 
    {
        person->month_expences *= (1. + year_inflation_per / (100*12));
    }
}

void salary(Person *person, const int month, const double year_inflation_per) 
{
    person->balance += person->salary;
    if(month == 12) 
    {
        person->salary *= (1. + year_inflation_per / (100*12));
    }
}

void rent_expences(Person *person, const int month, const double year_inflation_per) 
{
    person->balance -= person->rent.month_pay;

    if (month == 12) 
    {
        person->rent.month_pay *= (1. + year_inflation_per / (100*12));
    }
}

void deposit_change(Person *person) 
{
    person->deposit.deposit_sum *= (1. + person->deposit.deposit_per / (100*12));
    person->deposit.deposit_sum += person->balance;
    person->balance = 0;
}

void simulation(const int start_year, const int start_month, const int simulation_duration) 
{

    int year = start_year;
    int month = start_month;

    int end_year = start_year + simulation_duration;
    int end_month = (start_month == 12) ? (end_year++, 1) : (start_month + 1);

    while( !((year == end_year) && (month == end_month)) ) 
    {

        salary(&alice, month, year_indexation_per);
        monthly_personal_expences(&alice, year, month, start_year, start_month, year_inflation_per);
        pet_expances(&alice, year, month, year_inflation_per);
        mortgage_expences(&alice, year, month);
        deposit_change(&alice);

        salary(&bob, month, year_indexation_per);
        monthly_personal_expences(&bob, year, month, start_year, start_month, year_inflation_per);
        rent_expences(&bob, month, year_inflation_per);
        deposit_change(&bob);
        
        ++month;
        if(month == 13) 
        {
            month = 1;
            ++year;
        }
    }
}

void alice_pet_inint() 
{
    pet_init(&alice, 
            2030, 
            8, 
            (20 * 1000) * 100,
            (2 * 1000) * 100,
            (2 * 1000) * 100,
            (25 * 1000) * 100,
            12);
}

void print_result() 
{
    alice.deposit.deposit_sum += alice.mortgage.sum;

    printf("Alice capital = %lld\nBob capital   = %lld\n", alice.deposit.deposit_sum/100, bob.deposit.deposit_sum/100);
}


int main() {
    person_init(&bob, 0, standart_start_balance, standart_salary, 0, deposit_per);
    rent_init(&bob, (70 * 1000) * 100);

    person_init(&alice, 0, standart_start_balance, standart_salary, 0, deposit_per);
    alice_pet_inint();
    mortgage_init(&alice, (13 * 1000 * 1000) * 100, (1000 * 1000) * 100, mortage_per);
    
    simulation(2024, 9, years_of_simulation);

    print_result();

    return 0;
}
