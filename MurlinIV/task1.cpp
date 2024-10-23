#include <iostream>


typedef long long int Money;  //рубли


struct mortgage { //ипотека
    double rate;
    Money loan;
    Money first_pay;
    Money monthly_pay;
};
struct mortgage alice_mortgage;
struct person {
    Money capital;
    Money salary;
    Money account;
    Money food;
    Money transport;
    Money flat_cost;
    Money rent_cost;
    Money vacation;
};
struct person alice;
struct person bob;
void salary(const int month) { //рост зарплаты
    if (month == 1) {
        alice.salary *= 1.07;
        bob.salary *= 1.07;
    }
}
void inflation(int month) { //рост цен
    if (month == 1) {
        alice.flat_cost *= 1.02;
        alice.food *= 1.07;
        alice.transport *= 1.07;
        bob.rent_cost *= 1.02;
        bob.food *= 1.07;
        bob.transport *= 1.07;
    }
}
void bob_vacation(int month, int year) { //каждый пятый год в январе и августе Боб ездит на отдых
    if (year % 5 == 0 && (month == 8 || month == 1)) {
        bob.vacation = 200 * 1000 * 100 * pow(1.1, year - 2024);
    }
    else bob.vacation = 0;
}
void init_alice() {
    alice_mortgage.rate = 23.1 / 100 / 12;
    alice_mortgage.loan = 15 * 1000 * 1000 * 100;
    alice_mortgage.first_pay = 1.5 * 1000 * 1000 * 100;
    alice_mortgage.monthly_pay = (alice_mortgage.loan - alice_mortgage.first_pay) * (alice_mortgage.rate*pow((1+alice_mortgage.rate),30*12))/(pow((1 + alice_mortgage.rate), 30*12)-1);
    alice.flat_cost = 15 * 1000 * 1000 * 100;
    alice.account = 5 * 1000 * 1000 * 100+ alice_mortgage.loan;
    alice.salary = 300 * 1000 * 100;
    alice.food = 20 * 1000 * 100;
    alice.transport = 10 * 1000 * 100;
    alice.capital = alice.account + alice.flat_cost;
}
void init_bob() {
    bob.rent_cost = 90 * 1000 * 100;
    bob.salary = 300 * 1000 * 100;
    bob.food = 25 * 1000 * 100;
    bob.transport = 20 * 1000 * 100;
    bob.account = 5 * 1000 * 1000 * 100;
    bob.capital = bob.account;
}
void printresult() {
    printf("Alice capital: %lld\n", alice.capital);
    printf("-------------------------\n");
    printf("Bob capital: %lld\n", bob.capital);
}
void simulation() {
    int year = 2024;
    int month = 9;
    while (!(year == 2024 + 30 && month == 9)) {
        if (year == 2024 && month == 9) alice.account -= alice.flat_cost;
        salary(month);
        inflation(month);
        bob_vacation(month, year);
        month++;
        if (month == 13) {
            month = 1;
            year = year++;
        }
        alice.account += alice.salary;
        alice.account -= (alice.food + alice.transport + alice_mortgage.monthly_pay);
        alice.capital = alice.account + alice.flat_cost;
        bob.account += bob.salary;
        bob.account -= (bob.food + bob.transport + bob.rent_cost + bob.vacation);
        bob.capital = bob.account;

    }
    printresult();
}
int main()
{
  init_alice();
  init_bob();

  simulation();

  return(0);
}
