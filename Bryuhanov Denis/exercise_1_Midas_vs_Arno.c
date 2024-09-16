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
    int inflation;
} Hero;


Hero midas;
Hero arno;

int month = 12;
int year = 30;

void _init_midas()
{
    midas = (Hero){   
        .balance = 1000 * 1000 * 100,
        .salary = 400 * 1000 * 100,
        .borrow = 12 * 1000 * 1000 * 100,
        .payment_life = 70 * 1000 * 100,
        .payment_ipotek = 200 * 1000 * 100,
        .deposite_percent = 20,
        .inflation = 7
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
        .deposite_percent = 20,
        .inflation = 7
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


void simulation(Hero* hero)
{
    for (int i = 0; i < month * year; i++) {
        hero->balance += hero->balance * (hero->deposite_percent) / (100 * 12);
        hero->balance += hero->salary;
        hero->balance -= hero->payment_life;
        hero->balance -= hero->payment_ipotek;
    }
    hero->borrow = 0;
}

int main()
{
    _init_midas(&midas);
    _init_arno();

    debt_repayment(&midas);
    debt_repayment(&arno);
    printf("\n");

    printf("midas balance %lld\n", midas.balance / 100);
    printf("arno balance %lld\n", arno.balance / 100);

    printf("midas borrow %lld\n", midas.borrow / 100);
    printf("arno borrow %lld\n", arno.borrow / 100);
    printf("\n");

    simulation(&midas);
    simulation(&arno);


    printf("midas balance %lld\n", midas.balance / 100);
    printf("arno balance %lld\n", arno.balance / 100);
    printf("midas borrow %lld\n", midas.borrow / 100);
    printf("arno borrow %lld\n", arno.borrow / 100);
    printf("\n");

    printf("exe\n");
    return 1;
}
