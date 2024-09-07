#include <stdio.h>
#include <math.h>
#include <locale.h>

// Функция для расчета платежа по ипотеке прям тут, чтобы было удобнее
// Аналогична всем онлайн-калькуляторам, в том числе calcus.ru
float calc_mortgage_payment(float cost, float rate, float duration) {
    float month_rate = rate / 12; // Ежемесячная ставка
    float whole_rate = pow((1 + month_rate), (duration * 12)); // Общая ставка
    float payment = cost * month_rate * whole_rate / (whole_rate - 1); // Ежемесячный платеж
    return payment;
}

int main() {
    setlocale(LC_NUMERIC, "");
    
    float alice_capital = 1000000;
    float bob_capital = 1000000;

    float alice_salary = 200000;
    float bob_salary = 200000;

    float alice_deposit = 0;
    float bob_deposit = 0;

    float food_cost = 20000;
    float service_cost = 6000;
    float personal_cost = 15000;
    float rent_cost = 30000;

    float inflation_rate = 0.09;
    float deposit_rate = 0.2;
    float mortgage_duration = 30;
    float mortgage_rate = 0.16;
    float apartment_cost = 13000000;
    float mortgage_payment = calc_mortgage_payment(apartment_cost, mortgage_rate, mortgage_duration);
    // printf("%.2f\n", mortgage_payment);

    for (int year = 0; year < mortgage_duration; year++) {
        // printf("Year %d\n", year);
        for (int month = 0; month < 12; month++) {
            alice_capital += alice_salary;
            alice_capital -= food_cost;
            alice_capital -= service_cost;
            alice_capital -= personal_cost;
            alice_capital -= mortgage_payment;
            alice_deposit += alice_capital;
            alice_capital = 0;

            bob_capital += bob_salary;
            bob_capital -= food_cost;
            bob_capital -= service_cost;
            bob_capital -= personal_cost;
            bob_capital -= rent_cost;
            bob_deposit += bob_capital;
            bob_capital = 0;

            alice_deposit *= (1 + deposit_rate / 12);
            bob_deposit *= (1 + deposit_rate / 12);
        }

        food_cost *= 1 + inflation_rate;
        service_cost *= 1 + inflation_rate;
        personal_cost *= 1 + inflation_rate;
        alice_salary *= 1 + inflation_rate;
        bob_salary *= 1 + inflation_rate;
        apartment_cost *= 1 + inflation_rate;
        rent_cost *= 1 + inflation_rate; // Подорожание аренды жилья
    }

    float alice_final_capital = alice_deposit + apartment_cost;
    float bob_final_capital = bob_deposit;
    
    printf("Alice Capital is %'0.0f Rub\n", alice_final_capital);
    printf("  Bob Capital is %'0.0f Rub\n", bob_final_capital);

    if(alice_final_capital > bob_final_capital) {
        printf("Be like Alice\n");
    } else if(alice_final_capital < bob_final_capital) {
        printf("Be like Bob\n");
    } else {
        printf("Equal Results\n");
    }

    return 0;
}