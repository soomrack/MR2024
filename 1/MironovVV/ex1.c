#include <stdio.h>
 
#define ann_infl 10 // annual inflation in percents
#define in_on_dep 15 // income on deposits in percents/year
#define start_cap 2000000 // starting capital in rubles
#define sal 300000 // salary in rubles/month
#define oper_сosts 70000 // operating costs in rubles/month
#define rent_cost 40000 // rental cost in rubles/month
#define loan_cost 170000 // loan cost in rubles/month
#define apart_cost 11800000 // apartment cost in rubles/month
#define bill_period 30 // billing period in years

long Bob() //Bob capital calculations
{
    long capital = start_cap; //starting capital
    long salary = sal;
    for (int i = 0; i < bill_period*12; i++)
    {   //capital are calculating every month
        if (i%12==0) salary += salary*ann_infl/100;
        
        salary *= (double)(1 + ann_infl/100/12);
        capital = capital*(1+(double)(in_on_dep - ann_infl)/100/12) + salary - oper_сosts - rent_cost;
    }
    return capital;
}

long Alice()  //Alice capital calculations
{
    long capital = 0;    //starting capital and appartment cost
    long appart = apart_cost;
    long salary = sal;
    for (int i = 0; i < bill_period*12; i++)
    {   //capital are calculating every month
        if (i%12==0) salary += salary*ann_infl/100;
        
        capital = capital*(1+(double)(in_on_dep - ann_infl)/100/12) + salary - oper_сosts - loan_cost;
        appart *= (1+ann_infl/100/12);
    }
    return capital+appart;
}

int main(void)
{
    long Bob_cap = Bob();
    long Alice_cap = Alice();

    printf("Bob capital: %d\n", Bob_cap);
    printf("Alice capital: %d", Alice_cap);

    return 0;
}