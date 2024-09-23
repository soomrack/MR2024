#include <stdio.h>

typedef long long int Money;


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
};


struct Person alice;


void alice_init()
{
    alice.salary = 200 * 1000;
    alice.account = 1000 * 1000;

    alice.mortgage.sum = 14 * 1000 * 1000;
    alice.mortgage.first_pay = 1000 * 1000;
    alice.mortgage.rate = 0.17;
    alice.mortgage.monthly_payments = 170*1000; // source: url...
    alice.account -= alice.mortgage.first_pay;
    alice.house_price = alice.mortgage.sum;
}


void print_alice()
{
    printf("Alice capital = %lld\n",alice.account);
    printf("Alice house price = %lld\n",alice.house_price);
    printf("-----------------------\n");
    printf("Alice capital = %lld\n",alice.account + alice.house_price);
}


void alice_salary(const int month, const int year)
{
    if (year == 2025 && month == 3){
        alice.salary *= 1.5;
    }
    if(month == 1){
        alice.salary*=1.07;
    }
    alice.account += alice.salary;
}


void alice_disaster(const int month, const int year)
{
    if(month == 3 && year == 2028){
        alice.account -= 50000; //lundary machine
    }
}


void alice_mortgage()
{
    alice.account -= alice.mortgage.monthly_payments;
}


void alice_house_price(const int month, const int year)
{
    if(month == 1) alice.house_price *= 1.07;
    if(month == 2 && year == 2025) alice.house_price *= 1.5;

}

void simulation ()
{
    int month = 9;
    int year = 2024;

    while( !((year==2024+30)&&(month == 9)) ) {
        alice_salary(month, year);
        alice_disaster(month, year);
        alice_mortgage();
        alice_house_price(month,year);

        month++;
        if(month == 13){
            month = 1;
            year++;
        }
    }
}


int main() {
    alice_init();

    simulation();

    print_alice();
    return 0;
}
