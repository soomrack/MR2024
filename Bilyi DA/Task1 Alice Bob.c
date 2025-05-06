#include <stdio.h>

typedef long long int Money;    

struct Person {
    Money account;              
    Money deposit;              
    Money salary;                
    Money monthly_expenses;      
    float indexation;           
};
typedef struct Person Person;    

struct Finance{
    Money mortgage_payment;      
    Money rent;                  
    Money house_cost;            
    float inflation;             
    float deposit_rate;          
};
typedef struct Finance Finance;  

Person alice;                    
Person bob;                      
Finance finance;                 

void initialize_person(Person* person, const Money account, const Money salary, const float indexation,
const Money monthly_expenses)
{
    person->account = account;                  
    person->deposit = 0;
    person->salary = salary;
    person->indexation = indexation;
    person->monthly_expenses = monthly_expenses;
}

void initialize_finance(Finance* finance, const float mortgage_payment, const float deposit_rate, Money rent,
const float inflation, const Money house_cost) {
    finance->mortgage_payment = mortgage_payment;
    finance->deposit_rate = deposit_rate;
    finance->rent = rent;
    finance->inflation = inflation;
    finance->house_cost = house_cost;
}

void alice_salary(const int current_month, const int indexation_month) {
    alice.account += alice.salary;
    if (current_month == indexation_month) {
        alice.salary *= alice.indexation;
    }
}

void bob_salary(const int current_month, const int indexation_month) {
    bob.account += bob.salary;
    if (current_month == indexation_month) {
        bob.salary *= bob.indexation;
    }
}
// 
void alice_deposit() {
    alice.deposit += alice.account;
    alice.account = 0;
    alice.deposit *= finance.deposit_rate;
}

void bob_deposit() {
    bob.deposit += bob.account;
    bob.account = 0;
    bob.deposit *= finance.deposit_rate;
}

void alice_expenses() {
    alice.account -= alice.monthly_expenses;
}

void bob_expenses() {
    bob.account -= bob.monthly_expenses;
}

void alice_housing(const Money payment) {
    alice.account -= finance.mortgage_payment;
}

void bob_housing(const Money payment) {
    bob.account -= finance.rent;
}

void update_inflation() {
    finance.house_cost *= finance.inflation;
    finance.rent *= finance.inflation;
}

void check_bob_purchase(Person* bob, Finance* finance) {
    if (bob->deposit >= finance->house_cost) {
        bob->deposit -= finance->house_cost;
        finance->rent = 0;
    }
}

void simulate() {
    int year = 2024;
    int month = 9;
    const int indexation_month = 9;

    while( !((year == 2024 + 30) && (month == 10)) ) {

        alice_salary(month, indexation_month);
        alice_expenses(alice.monthly_expenses);
        alice_housing(finance.mortgage_payment);
        alice_deposit(finance.deposit_rate);


        bob_salary(month, indexation_month);
        bob_expenses(bob.monthly_expenses);
        bob_housing(finance.rent);
        bob_deposit(finance.deposit_rate);
        check_bob_purchase(&bob, &finance);

        // if(month == indexation_month){
        //     update_inflation(&finance);
        // }

        ++month;
        if(month == 13) {
            month = 1;
            ++year;
        }
    }
}

void compare_final_capital() {
    Money alice_final = alice.deposit + finance.house_cost;
    Money bob_final = bob.deposit + (finance.rent == 0 ? finance.house_cost : 0);

    printf("Alice capital: %lld\n", alice_final);
    printf("Bob capital: %lld\n", bob_final);

    if (alice_final > bob_final) {
        printf("Alice strategy is better.\n");
    }
    else if (alice_final < bob_final) {
        printf("Bob strategy is better.\n");
    }
    else {
        printf("Strategies are similar.\n");
    }
}

int main() {
    initialize_person(&alice, 1000000, 200000, 1.09, 40000);
    initialize_person(&bob, 1000000, 200000, 1.09, 40000);
    initialize_finance(&finance, 40000, 1.16, 40000, 1.09, 20000000);

    simulate();

    compare_final_capital();

    return 0;
}
