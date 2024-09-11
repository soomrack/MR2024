#include <stdio.h>

#define DURATION 30 //years
#define YEAR_START 2024
#define MONTH_START 9
#define BOB_SALARY 200 * 1000
#define ALICE_SALARY 200 * 1000
#define RENTAL_COST 30 * 1000
int bob_account = 0;
int alice_account = 0;

void bob_rent()
{
    bob_account -= RENTAL_COST;
}

void salary()
{
    bob_account += BOB_SALARY;
    alice_account += ALICE_SALARY;
    //printf("%d\n", bob_account);
}

void simulation(year, month)
{
    while(year <= YEAR_START + DURATION)
    {
        while(month <= 12)
        {
            salary();
            //printf("Прошел %d месяц\n", month);
            month++;
        }
        month = 1;
        //printf("Прошел %d год\n", year);
        year++;
    }
}



int main()
{
    simulation(YEAR_START, MONTH_START);
}