#include <stdio.h>


typedef long long int Money;


typedef struct Hero
{
    Money balance;
    Money salary;
    Money salary_bonus;
    Money ipotek;
    Money food_payment;
    Money comunal_payment;
    Money another_payment; //Траты на одежду, медикаменты и прочее
    Money vacation_cost;
    Money ipotek_payment;
    double deposite_percent;
    double inflation;
    double indexation;
} Hero;


Hero midas;
Hero arno;


int month = 12;
int year = 2050;


void _init_midas()
{
    midas = (Hero){   
        .balance = 0,
        .salary = 300 * 1000 * 100,
        .salary_bonus = 100 * 1000 * 100,
        .ipotek = 12 * 1000 * 1000 * 100,
        .food_payment = 20 * 1000 * 100,
        .comunal_payment = 15 * 1000 * 100,
        .another_payment = 35 * 1000 * 100,
        .vacation_cost = 300 * 1000 * 100,
        .ipotek_payment = 200 * 1000 * 100,
        .deposite_percent = 0.18,
        .inflation = 0.07,
        .indexation = 0.07
    };
}


void _init_arno()
{
    arno = (Hero){
        .balance = 1000 * 1000 * 100,
        .salary = 200 * 1000 * 100,
        .salary_bonus = 300 * 1000 * 100, //Премия в конце года
        .food_payment = 25 * 1000 * 100 ,
        .comunal_payment = 15 * 1000 * 100,
        .another_payment = 45 * 1000 * 100,
        .vacation_cost = 250 * 1000 * 100,
        .ipotek_payment = 0,
        .deposite_percent = 0.18,
        .inflation = 0.07,
        .indexation = 0.07
    };
}


void cost_inflation(Hero* hero)
{
    hero -> food_payment += (Money)(hero -> food_payment * hero -> inflation);
    hero -> comunal_payment += (Money)(hero -> comunal_payment * hero -> inflation); 
    hero -> another_payment += (Money)(hero -> another_payment * hero -> inflation); 
    hero -> vacation_cost += (Money)(hero -> vacation_cost * hero -> inflation);  
    hero -> ipotek += (Money)(hero -> ipotek * hero -> inflation);
}


void indexation(Hero* hero)
{
    hero -> salary += (Money)(hero -> salary * hero -> indexation);
    hero -> salary_bonus += (Money)(hero -> salary_bonus * hero -> indexation);
}


void simulation(Hero* hero)
{
    for (int current_year = 2020; current_year < year; current_year++){
        for (int i = 0; i < month; i++) {
            hero->balance += (Money)((double)(hero->balance) * (hero->deposite_percent / 12));
            hero->balance += hero->salary;
            hero->balance -= hero->food_payment;
            hero->balance -= hero->comunal_payment;
            hero->balance -= hero->another_payment;  
            hero->balance -= hero->ipotek_payment;
        }
        hero -> balance += hero -> salary_bonus;
        hero -> balance -= hero -> vacation_cost;

        cost_inflation(&*hero);
        indexation(&*hero);
    }
}


int main()
{
    _init_midas();
    _init_arno();

    printf("midas balance %lld\n", midas.balance / 100);
    printf("arno balance %lld\n", arno.balance / 100);
    printf("\n");

    simulation(&midas);
    simulation(&arno);

    printf("midas balance %lld\n", (midas.balance + midas.ipotek) / 100);
    printf("arno balance %lld\n", arno.balance / 100);
    printf("house cost %lld\n", (midas.ipotek) / 100);
    printf("\n");
    return 1;
}
