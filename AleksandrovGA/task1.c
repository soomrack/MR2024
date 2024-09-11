#include <stdio.h>
#include <math.h>

float everymonthPayment(float creditSum, float rate, float period){
    float rateMonth = rate / 12;
    float totalRate = pow((1 + rateMonth), (period * 12));
    return (creditSum * rateMonth * totalRate / (totalRate - 1));
}

float monthlySpending(float capital, float deposit, float salary, float foodCost, float utilitiesCost, float personalCost, float monthlyPayment){
    capital += salary;
    capital -= monthlyPayment;
    capital -= personalCost;
    capital -= utilitiesCost;
    capital -= foodCost;
    return (deposit + capital);
}

void choice(float firstCapital, float secondCapital){
    if (firstCapital > secondCapital){
        printf("Repeat after Alice.\n");
    } else if(firstCapital < secondCapital){
        printf("Repeat after Bob.\n");
    } else{
        printf("It doesn\'t matter who you repeat after.\n");
    }
}

int main(void){
    float aliceCapital = 1000000;
    float aliceSalary = 200000;
    float aliceDeposit = 0;

    float bobCapital = 1000000;
    float bobSalary = 200000;
    float bobDeposit = 0;

    float foodCost = 17000;
    float utilitiesCost = 7000;
    float personalCost = 10000;
    float rentCost = 30000;

    float inflationRate = 0.09;
    float depositRate = 0.2;
    float creditPeriod = 30;
    float creditRate = 0.16;
    float creditSum = 13000000;

    float monthlyPayment = everymonthPayment(creditSum, creditRate, creditPeriod);
    //printf("%0f\n", monthlyPayment);

    for(int year = 0; year < creditPeriod; year++){
        for(int month = 0; month < 12; month++){
            aliceDeposit = monthlySpending(aliceCapital, aliceDeposit, aliceSalary, foodCost, utilitiesCost, personalCost, monthlyPayment);
            aliceCapital = 0;
            bobDeposit = monthlySpending(bobCapital, bobDeposit, bobSalary, foodCost, utilitiesCost, personalCost, rentCost);
            bobCapital = 0;

            aliceDeposit *= (1 + depositRate / 12);
            bobDeposit *= (1 + depositRate / 12);
        }
        aliceSalary *= 1 + inflationRate;
        bobSalary *= 1 + inflationRate;
        foodCost *= 1 + inflationRate;
        utilitiesCost *= 1 + inflationRate;
        personalCost *= 1 + inflationRate;
        rentCost *= 1 + inflationRate;
        creditSum *= 1 + inflationRate;
    }
    aliceCapital = aliceDeposit + creditSum;
    bobCapital = bobDeposit;

    printf("Alice: %0.0f Rub\n", aliceCapital);
    printf("Bob: %0.0f Rub\n", bobCapital);
    choice(aliceCapital, bobCapital);
    return 0;
}