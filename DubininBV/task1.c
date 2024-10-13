#include <stdio.h>
#include <math.h>
#include <stdlib.h>

typedef long long int Cash; // в копейках

const float INFL = 0.08;
const short PERIOD = 30;
const short FIRST_YEAR = 2024;
const short FIRST_MONTH = 9;
const float MORTGAGE_RATE = 0.17;


typedef struct
{
    short month;
    short year;
} Date;


typedef struct
{
    Cash bank_account;
    Cash salary;
    Cash flat_payment;
    Cash month_spending;
    Cash flat_cost;
    short unexpected_spending_pp;
} Person;


typedef struct
{
    float rate;
    Cash account;
} Deposit;



Person alice;
Deposit alice_deposit;

Person bob;
Deposit bob_deposit;




Cash mortgage_calculation(const Cash credit_summ, const float rate, const short duration)
{
    float month_rate = rate / 12;
    float final_rate = pow(1 + month_rate, duration * 12);
    return (Cash) (credit_summ * month_rate * final_rate / (final_rate - 1));
}




void alice_init()
{
    alice.salary = 300 * 1000 * 100;
    alice.bank_account = 0;
    alice.flat_cost = 14 * 1000 * 1000 * 100;
    alice.flat_payment = mortgage_calculation(14 * 1000 * 1000 * 100, MORTGAGE_RATE, PERIOD);
    alice.month_spending = 70 * 1000 * 100;
    alice.unexpected_spending_pp = 20;

    alice_deposit.rate = 0.2;
    alice_deposit.account = 0;
}


void alice_salary_income(const Date date)
{
    alice.bank_account += alice.salary;
    if (((date.year - FIRST_YEAR) % 9)  == 0) alice.salary *= 1.08;  // условие, что алиса раз в 9 лет находит новую работу
}                                                                   // с зп на 8% больше


void alice_month_spending(const Date date)
{        
    alice.bank_account -= (Cash) alice.month_spending*(1 + (rand() % alice.unexpected_spending_pp) / 100);
}


void alice_flat_payment(const Date date)
{
    alice.bank_account -= alice.flat_payment;
    if ((date.year == 2031) && ((date.month == 3) || (date.month == 4))) alice.bank_account -= 20 * 1000 * 100;
    // условие, что у Алисы затопило кв и она 2 месяца делала ремонт
}



void alice_inflation(const Date date)
{
    if (date.month == 1){
        alice.salary *= (1 + INFL);
        alice.flat_payment *= (1 + INFL);
        alice.month_spending *= (1 + INFL);
        alice.flat_cost *= (1 + INFL);
    }
}


void alice_deposit_income()
{
    alice_deposit.account += alice.bank_account;
    alice.bank_account = 0;
    alice_deposit.account *= (1 + alice_deposit.rate / 12);
}


void bob_init()
{
    bob.salary = 300 * 1000 * 100;
    bob.bank_account = 0;
    bob.flat_payment = 30 * 1000 * 100;
    bob.flat_cost = 0;
    bob.month_spending = 70 * 1000 * 100;
    bob.unexpected_spending_pp = 20;

    bob_deposit.rate = 0.2;
    bob_deposit.account = 1000 * 1000 * 100;
}


void bob_salary_income(const Date date)
{
    bob.bank_account += bob.salary;
    if ((date.year == 2038) && (date.month == 3)) bob.salary = 0;                // условие, что боб в 2038 году в марте потерял работу
    if ((date.year == 2038) && (date.month == 5)) bob.salary = 320 * 1000 * 100; // на 2 месяц, но потом нашел новую с зп 320 тыс
}


void bob_flat_payment(const Date date)
{
    bob.bank_account -= bob.flat_payment;
}


void bob_month_spending(const Date date)
{
    bob.bank_account -= (Cash) (bob.month_spending*(1 + (rand() % bob.unexpected_spending_pp) / 100));
}


void bob_inflation(const Date date)
{
    if (date.month == 1){
        bob.salary *= (1 + INFL);
        bob.flat_payment *= (1 + INFL);
        bob.month_spending *= (1 + INFL);
    }
}


void bob_deposit_income()
{
    bob_deposit.account += bob.bank_account;
    bob.bank_account = 0;
    bob_deposit.account *= (1 + bob_deposit.rate / 12);
}




void print_results()
{
    printf("Alice result: %lld \n", (alice.bank_account + alice.flat_cost + alice_deposit.account));
    printf("Bob result: %lld \n", (bob.bank_account + bob.flat_cost + bob_deposit.account));
    printf("\n");
}


void simulation()
{
    short lastYear = FIRST_YEAR+PERIOD;
    Date date;
    date.year = FIRST_YEAR;
    date.month = FIRST_MONTH;

    while (!((date.year == lastYear) && (date.month == FIRST_MONTH))) {     
        alice_salary_income(date);
        alice_flat_payment(date);
        alice_month_spending(date);
        alice_inflation(date);
        alice_deposit_income();

        bob_salary_income(date);        
        bob_flat_payment(date);        
        bob_month_spending(date);        
        bob_inflation(date);
        bob_deposit_income();                 

        date.month = date.month % 12 + 1;
        if (date.month == 1) ++date.year;           
    }
}


int main()
{
    alice_init();

    bob_init();

    simulation();

    print_results();

    return 1;
}
