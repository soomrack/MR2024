#include <stdio.h>
#include <math.h>
#include <stdbool.h>

typedef long long int Money;


typedef struct Person {
    Money salary;
    Money capital;
    Money person_flat_payment;  // ежемесечная плата за ипотеку
    Money monthly_expences;  // ежемесячные расходы
    Money apartment_coast;
    bool mortgage;
} Person;


typedef struct Date {
    int month;
    int year;
} Date;


Date begin_date;
Date last_date;
Date now_date;


void date_init () {
    begin_date.month = 1;
    begin_date.year = 2024;
    last_date.month = 1;
    last_date.year = 2054;
}


Person flat_payment (Money initial_payment, float percent, Money mortgage_summ, Money mortgage_years, Person person) {
    if (person.mortgage == true) {
        person.capital -= 1000000;  // первый взнос ипотеки
        Money periods = mortgage_years * 12;  // число периодов в месяцах
        person.person_flat_payment = (((percent / 12) * pow(1 + (percent / 12),periods))
                                      / (pow(1 + (percent / 12),periods) - 1)) * mortgage_summ;  // рассчет ежемесячного платежа
    } else {
        person.person_flat_payment = 30000;
    }
    return person;
}


Person init (bool mortgage, Money begin_capital) {
    Person person;
    person.capital = begin_capital;
    person.salary = 200000;
    person.monthly_expences = 30000;
    person.mortgage = mortgage;
    person.apartment_coast = 13000 * 1000 * mortgage;  // стоимость квартиры
    person = flat_payment(1000000, 0.16, person.apartment_coast, 30, person);  // рассчет ежемесячного платежа за ипотеку/аренду
    return person;
}



Person apartment_payment (Person person) {
    person.capital = person.capital - person.person_flat_payment;  //ежем. доходы/расходы
    if(person.mortgage == false && now_date.month == 12) person.person_flat_payment *= 1.07;  // учет инфляции аренды жилья
    
    else if(person.mortgage == true && now_date.month == 12)person.apartment_coast *= 1.07; //учет инфляции в стоимости жилья
    return person;
}


Person salary (Person person) {
    person.capital += person.salary;
    if(now_date.month == 12) person.salary *= 1.07;  // индексация зарплаты
     else if(person.mortgage == false) {
        if(now_date.month == 8)person.capital -= person.salary;  //ежегодный отпуск Боба за свой счет
    return person;
}
}

Person monthly_expences (Person person) {
    person.capital -= person.monthly_expences;
    if(now_date.month == 12) person.monthly_expences *= 1.07;  // учет инфляции потребительской корзины
    return person;
}


Person invest (Person person) {
    person.capital *= 1.016;  //учет процентов банковского вклада
    return person;
}


Person simulation (Person person) {
    now_date = begin_date;

    while ((now_date.year < last_date.year) || (now_date.month < last_date.month)) {
        person = salary(person);
        person = monthly_expences(person);
        person = work_incident(person);
        person = apartment_payment(person);
        person = invest(person);

        now_date.month++;
        if(now_date.month == 13) {
            now_date.month = 1;
            now_date.year++;
        }
    }
    return person;
}


void results (Person Alice, Person Bob) {
    printf("Alice Capital %llu\n", Alice.capital + Alice.apartment_coast);
    printf("Bob capital %llu\n", Bob.capital);
}


int main () {
    date_init();

    Person Alice = init(1, 1500 * 1000);
    Person Bob = init(0, 1000 * 1000);

    Alice = simulation(Alice);
    Bob = simulation(Bob);

    results(Alice, Bob);
}
