#include <stdio.h>


typedef long long int Money;


typedef struct Hero
{
    Money balance;
    Money salary;
    Money borrow;
    Money payment_life;
    Money payment_ipotek;
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
        .balance = 1000 * 1000 * 100,
        .salary = 400 * 1000 * 100,
        .borrow = 12 * 1000 * 1000 * 100,
        .payment_life = 70 * 1000 * 100,
        .payment_ipotek = 200 * 1000 * 100,
        .deposite_percent = 0.20,
        .inflation = 0.07,
        .indexation = 0.07
    };
}


void _init_arno()
{
    arno = (Hero){
        .balance = 1000 * 1000 * 100,
        .salary = 350 * 1000 * 100,
        .borrow = 0,
        .payment_life = 50 * 1000 * 100 + 45 * 1000 * 100,
        .payment_ipotek = 0,
        .deposite_percent = 0.20,
        .inflation = 0.07,
        .indexation = 0.07
    };
}


void debt_repayment(struct Hero* hero)
{
    if (hero->borrow > hero->balance) {
        hero->borrow = hero->borrow - hero->balance;
        hero->balance = 0;
    }
    else if (hero->borrow <= hero->balance) {
        hero->balance = hero->balance - hero->borrow;
        hero->borrow = 0;
    }
}


void cost_inflation(Hero* hero)
{
    hero -> payment_life += (Money)(hero -> payment_life * hero -> inflation); 
}


void indexation(Hero* hero)
{
    hero -> salary += (Money)(hero -> salary * hero -> indexation);
}


void simulation(Hero* hero)
{
    for (int current_year = 2020; current_year < year; current_year++){
        for (int i = 0; i < month; i++) {
            hero->balance += (Money)((double)(hero->balance) * (hero->deposite_percent / 12));
            hero->balance += hero->salary;
            hero->balance -= hero->payment_life;
            hero->balance -= hero->payment_ipotek;
        }
        cost_inflation(&*hero);
        indexation(&*hero);
    }
    hero->borrow = 0;
}



int main()
{
    _init_midas();
    _init_arno();

    debt_repayment(&midas);
    debt_repayment(&arno);
    printf("\n");

    printf("midas balance %lld\n", midas.balance / 100);
    printf("arno balance %lld\n", arno.balance / 100);

    printf("midas payment %lld\n", midas.payment_life / 100);
    printf("arno payment %lld\n", arno.payment_life / 100);
    printf("\n");

    simulation(&midas);
    simulation(&arno);


    printf("midas balance %lld\n", midas.balance / 100);
    printf("arno balance %lld\n", arno.balance / 100);
    printf("midas payment %lld\n", midas.payment_life / 100);
    printf("arno payment %lld\n", arno.payment_life / 100);
    printf("\n");
    return 1;
}
