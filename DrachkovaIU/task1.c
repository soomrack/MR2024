#include <stdio.h>

typedef long long int Money;

typedef struct
{
    Money sum;
    Money first_pay;
    Money monthly_payment;
    double rate;
} Mortgage;

typedef struct
{
    Money pledge;
    Money monthly_chek;
} Rent;

typedef struct     
{
    Money salary;
    Money count;
    Mortgage mortgage;
    Rent rent;
} Person; 


Person alice;
Person bob;

//Alice


void alice_init()
{
    alice.salary = 200 * 1000 * 100;   //коп.
    alice.count = 1000 * 1000 * 100;

    alice.mortgage.sum = 14 * 1000 * 1000 * 100;
    alice.mortgage.first_pay = 1000 * 1000 * 100;
    alice.mortgage.monthly_payment = 241582 * 100; //source: https://www.sberbank.ru/ru/person/credits/home/ipotechniy-kalkulyator
    alice.count -= alice.mortgage.first_pay;
}


void print_alice()
{
    printf("Alice capital - %lld \n", alice.count);
    printf("Alice capital - %lld \n", (Money)(alice.count / 100)); //руб.
    printf("Alice capital - %lld \n", (Money)(alice.count / 1000 / 1000));
}


void alice_salary(const int month)
{
    if(month == 1){
        alice.salary *= 1.07; //Индексация зп
    }
    
    alice.count += alice.salary;
}

void alice_mortgage()
{
    alice.count -= alice.mortgage.monthly_payment;
}


void alice_comp(const int month, const int year)
{
    if( month == 11 && year == 2027) {
        alice.count -= 200 * 1000 * 100;  //Покупка ноутбука
    }
}



//Bob

void bob_init()
{
    bob.salary = 200 * 1000 * 100; //коп.
    bob.count = 1000 * 1000 *100;

    bob.rent.pledge = 30 * 1000 * 100;
    bob.rent.monthly_chek = 37 * 1000 * 100;  //Аренда + комуналка сразу в счёт за квартиру
    bob.count -= bob.rent.pledge;
}


void bob_salary(const int month)
{
    if(month == 1){
        bob.salary *= 1.07; //Индексация зп
    }
    
    bob.count += bob.salary;
}

void bob_rent()
{
    bob.count -= bob.rent.monthly_chek;
}


void print_bob()
{
    printf("Bob capital - %lld \n", bob.count);
    printf("Bob capital - %lld \n", (Money)(bob.count / 100)); //руб.
    printf("Bob capital - %lld \n", (Money)(bob.count / 100 / 1000 / 1000));


}




void simulation()
{
    int month = 9;
    int year = 2024;

    while ( !((year == 2024 + 30) && (month == 9)) ) {
        alice_salary(month);
        alice_mortgage();
        alice_comp(month, year);

        bob_salary(month);
        bob_rent();
        
        month ++;
        if(month == 13) {
            month == 1;
            year++;
        }
    }
}




//End

int main() 
{
    alice_init();
    bob_init();

    simulation();

    print_alice();
    print_bob();
    return 0;
}

