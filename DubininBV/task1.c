#include <stdio.h>
#include <math.h>
#include <stdlib.h>

typedef long long int Cash; // в копейках

const float infl = 0.08;
const float depositRate = 0.2;
const short period = 30;
const short firstYear = 2024;
const short firstMonth = 9;
const float mortgageRate = 0.17;
const Cash aliceFlatCost = 14 * 1000 * 1000 * 100;


Cash mortgage_calculation(Cash creditSumm, float rate, short duration)
{
    float monthRate = rate / 12;
    float finalRate = pow(1 + monthRate, duration * 12);
    return (Cash) (creditSumm * monthRate * finalRate / (finalRate - 1));
}




typedef struct Date
{
    short monthNumber;
    short year;
} Date;

Date date;


void time_init(short y, short m)
{
    date.monthNumber = m;
    date.year = y;
};


void time_flow()
{
    date.monthNumber = date.monthNumber % 12 + 1;
    if (date.monthNumber == 1) ++date.year;
}




typedef struct Person
{
    Cash capital;
    Cash salary;
    Cash flatPay;
    Cash monthSpending;
    short unexpectedSpendingPP;
} Person;


Person alice;
Person bob;


void alice_init(Cash salary, Cash capital, Cash flatPay, Cash monthSpending, short unexpectedSpanding)
{
    alice.salary = salary;
    alice.capital = capital;
    alice.flatPay = flatPay;
    alice.monthSpending = monthSpending;
    alice.unexpectedSpendingPP = unexpectedSpanding;
}

void alice_salary_incom()
{
    alice.capital += alice.salary;
    if (((date.year - firstYear) % 9)  == 0) alice.salary *= 1.08;  // условие, что алиса раз в 9 лет находит новую работу
}                                                                   // с зп на 8% больше


void alice_month_spending()
{        
    alice.capital -= (Cash) alice.monthSpending*(1 + (rand() % alice.unexpectedSpendingPP) / 100);
}


void alice_flat_pay()
{
    alice.capital -= alice.flatPay;
}


void alice_flat_inflation_income()// разнице стоимости квартиры в год покупке и в конце рассматриваемого периода, появившаяся из-за инфляции
{
    alice.capital += (aliceFlatCost * pow((1 + infl), period)) - aliceFlatCost;
}


void alice_inflation()
{
    if (date.monthNumber == 1){
        alice.salary *= (1 + infl);
        alice.flatPay *= (1 + infl);
        alice.monthSpending *= (1 + infl);
    }
}


void alice_deposit()
{
    alice.capital *= (1 + depositRate / 12);
}


void bob_init(Cash salary, Cash capital, Cash flatPay, Cash monthSpending, short unexpectedSpanding)
{
    bob.salary = salary;
    bob.capital = capital;
    bob.flatPay = flatPay;
    bob.monthSpending = monthSpending;
    bob.unexpectedSpendingPP = unexpectedSpanding;
}


void bob_salary_incom()
{
    bob.capital += bob.salary;
    if ((date.year == 2038) && (date.monthNumber == 3)) bob.salary = 0;                // условие, что боб в 2038 году в марте потерял работу
    if ((date.year == 2038) && (date.monthNumber == 5)) bob.salary = 320 * 1000 * 100; // на 2 месяц, но потом нашел новую с зп 320 тыс
}


void bob_flat_pay()
{
    bob.capital -= bob.flatPay;
}


void bob_month_spending()
{
    bob.capital -= (Cash) bob.monthSpending*(1 + (rand() % bob.unexpectedSpendingPP) / 100);
}


void bob_inflation()
{
    if (date.monthNumber == 1){
        bob.salary *= (1 + infl);
        bob.flatPay *= (1 + infl);
        bob.monthSpending *= (1 + infl);
    }
}


void bob_deposit()
{
    bob.capital *= (1 + depositRate / 12);
}




void print_results()
{
    printf("Alice result: %lld \n", alice.capital);
    printf("Bob result: %lld \n", bob.capital);
    printf("\n");
}


void simulation()
{
    short lastYear = firstYear+period;

    while (!((date.year == lastYear) && (date.monthNumber == firstMonth))) {
        time_flow();

        alice_salary_incom();
        bob_salary_incom();

        alice_flat_pay();
        bob_flat_pay();

        alice_month_spending();
        bob_month_spending();

        alice_deposit();
        bob_deposit();

        alice_inflation();
        bob_inflation();            
    }
    alice_flat_inflation_income();

    print_results();
}


int main()
{
    time_init(firstYear, firstMonth+1);

    alice_init((Cash) (300 * 1000 * 100), (Cash) (0), 
                mortgage_calculation(aliceFlatCost, mortgageRate, period), 
                (Cash) (70 * 1000 * 100), (short) 20);

    bob_init((Cash) (300 * 1000 * 100), (Cash) (1000 * 1000 * 100), 
                (Cash) (30 * 1000 * 100), 
                (Cash) (70 * 1000 * 100), (short) 20);

    simulation();

    return 1;
}