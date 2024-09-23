#include <stdio.h>
typedef long long int Money;


int START_YEAR = 2024;
int START_MONTH = 9;
int PERIOD = 20;


struct Mortgage {
    Money sum;
    Money first_pay;
    Money monthly_payments;
    double rate;
};


struct Person {
    Money salary;
    Money account;
    struct Mortgage mortgage;
    Money house_price;
    Money house_rent;
    Money life_cost;
    double deposit_rate;
    double inflation_index;
};


struct Person alice;
struct Person bob;


void alice_init()
{
    alice.salary = 200 * 1000;
    alice.account = 1000 * 1000;
    alice.life_cost = 50*1000;

    alice.deposit_rate = 0.15;
    alice.inflation_index = 0.07;

    alice.mortgage.sum = 14 * 1000 * 1000;
    alice.mortgage.first_pay = 1000 * 1000;
    alice.mortgage.rate = 0.17;
    alice.mortgage.monthly_payments = 120*1000; 
    alice.account -= alice.mortgage.first_pay;
    alice.house_price = alice.mortgage.sum;
}


void alice_salary(const int month)
{
     {
        alice.salary *= (1 + alice.inflation_index);
    }
    alice.account += alice.salary;
}


void alice_disaster(const int month, const int year)
{
    if(month == 3 && year == 2028){
        alice.account -= 50*1000;
    }
}


void alice_mortgage()
{
    alice.account -= alice.mortgage.monthly_payments;
}
void alice_life_cost(const int month)
{
    if (month == 1) {
        alice.life_cost *= (1 + alice.inflation_index);
    }
    alice.account -= alice.life_cost;
}

void alice_house_price(const int month, const int year)
{
    if(month == 1) alice.house_price *= 1.07;
    if(month == 2 && year == 2025) alice.house_price *= 1.1;

}


void alice_deposit()
{
    alice.account += alice.account * (alice.deposit_rate / 12);
}    


void alice_print()
{
    printf("Alice summary capital: %lld \n", alice.account + alice.house_price);
}


void bob_init()
{
    bob.salary = 200*1000;
    bob.account = 1000*1000;
    bob.house_rent = 50*1000;
    bob.life_cost = 50*1000;

    bob.deposit_rate = 0.15;
    alice.inflation_index = 0.07;
}


void bob_life_cost(const int month)
{
    if (month == 1) {
        bob.life_cost *= (1 + bob.inflation_index);
    }
    bob.account -= bob.life_cost;
}


void bob_rent(const int month)
{
    bob.account -= bob.house_rent;
}


void bob_salary(const int month)
{
    {
        bob.salary *= (1 + bob.inflation_index);
    }
    bob.account += bob.salary;
}


void bob_deposit()
{
    bob.account += bob.account * (bob.deposit_rate / 12);
}


void bob_print()
{
    printf("Bob summary capital: %lld\n", bob.account);
}


void conclusion()
{
    printf("-----------------------------\n");
        
         if ((alice.account + alice.house_price) > bob.account) {
            printf("Alice is winner\n");
    } else {
        if ((alice.account + alice.house_price) == bob.account) {
            printf("Alice and Bob are equal\n");
        } else {
            printf("Bob is winner\n");
        }
    }
}


void simulation ()
{
    int month = 9;
    int year = 2024;

    while( !((year==2024+20)&&(month == 9)) ) {
        alice_salary(month);
        alice_disaster(month, year);
        alice_mortgage();
        alice_house_price(month,year);

        alice_deposit();
        
        bob_salary(month);
        bob_rent(month);
        bob_life_cost(month);

        bob_deposit();

        month++;
        if(month == 13){
            year++;
            month = 1;
        }
    }
}


int main() 
{
    alice_init();
    bob_init();

    simulation();

    printf("Results for %d.%d are:\n",START_MONTH, START_YEAR + PERIOD);
    printf("-----------------------------\n");

    alice_print();
    bob_print();

    conclusion();
    return 0;
}
