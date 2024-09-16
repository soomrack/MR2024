#include <stdio.h>

typedef struct Hero
{
    int balance;
    int salary;
    int borrow;
    int payment_life;
    int payment_ipotek;
} Hero;

Hero midas;
Hero arno;

void _init_midas()
{
    midas.balance = 1000 * 1000;
    midas.salary = 400 * 1000;
    midas.borrow = 12 * 1000 * 1000;
    midas.payment_life = 70 * 1000;
    midas.payment_ipotek = 200 * 1000;
}

void _init_arno()
{
    arno.balance = 1000 * 1000;
    arno.salary = 350 * 1000;
    arno.borrow = 0;
    arno.payment_life = 50 * 1000;
    arno.payment_ipotek = 0;
}

int main()
{
    _init_midas();
    _init_arno();
    printf("%d\n", midas.balance);
    printf("%d\n", arno.payment_life);
    printf("Ingore")

    return 1;
}