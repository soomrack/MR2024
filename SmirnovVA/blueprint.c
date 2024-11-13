#include<stdio.h>

float INFL = 1.13;

typedef long long int Money;

typedef struct {
    int deposit_perc;
    Money account;
} Deposit;

typedef struct {
    char* name;
    Money account;
    Money salary;
    Money mortgage_sum;
    Money bytovuha;
    Money monthly_pay;
    Money downplayment;
    Deposit deposit;
} Person;

void print_person(Person* pers)
{
    printf("%s account = %d RUB\n", pers->name, pers->account);
}

void alice_init(Person* pers)
{
    pers->name = "Alice";
    pers->account = 100000;
    pers->salary = 200.0 * 1000.0;
    pers->mortgage_sum = 13*1000*1000;
    pers->downplayment = 900*1000;
    pers->bytovuha = 30000;
    pers->monthly_pay = 162715;
    pers->deposit.deposit_perc = 30;
}

void bob_init(Person* pers)
{
    pers->name = "Bob";
    pers->account = 1000000;
    pers->salary = 200 * 1000;
    pers->bytovuha = 30000;
    pers->monthly_pay = 30000;
    pers->deposit.deposit_perc = 20;
    pers->deposit.account = 0;
}

int deposit_inflation(int perc, int month)
{
    if (month==12)
    {
        perc*=INFL; 
    }
    return perc;
}

void salary(Person* pers, const int month)
{
    pers->account += pers->salary;
    if(month == 12) {
        pers->salary *= 1.07;
    }
}

void traty(Person* pers, const int month)
{
    pers->account-= pers->bytovuha;
    if (month == 12)
    {
        pers->bytovuha*=INFL;
    }
}

void mortgage_payment(Person* pers)
{
    pers->mortgage_sum -= pers->monthly_pay;
    pers->account -= pers->monthly_pay;
}

void pay_deposit(Person* pers) {
    pers->account -= pers->salary * pers->deposit.deposit_perc/100;
}

void simulation(Person* alice, Person* bob)
{
    int year = 2024;
    int month = 9;
    while( !((year == 2024 + 30) && (month == 10)) ) 
    {
        salary(alice, month);
        mortgage_payment(alice);
        traty(alice, month);

        // bob_deposit
        salary(bob, month);
        traty(bob, month);
        pay_deposit(bob);

        ++month;
        if(month == 13) {
            month = 1;
            ++year;
        }
        
        print_person(alice);
        print_person(bob);
    }
}

int main()
{
    Person alice;
    Person bob;

    alice_init(&alice);
    bob_init(&bob);
    
    print_person(&alice);
    print_person(&bob);

    simulation(&alice, &bob);

    print_person(&alice);
    print_person(&bob);
}
