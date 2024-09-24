#include <stdio.h>
#include <math.h>
#include <stdbool.h>

typedef long long int Money;


typedef struct Person {
    Money salary;
    Money capital;
    Money person_flat_payment;  // ежемесечная плата за ипотеку
    Money monthly_expences;  // ежемесячные расходы
    bool mortgage;
} Person;


typedef struct Date {
    int month;
    int year;
} Date;


Date begin_date;
Date last_date;
Date now_date;


Person init (bool mortgage, Money begin_capital) {
    Person person;
    person.capital = begin_capital;
    person.salary = 200000;
    person.monthly_expences = 30000;
    person.mortgage = mortgage;
    return person;
}


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


Person year_inflation (Person person) {
    if(person.mortgage == false) person.person_flat_payment *= 1.07;  // учет инфляции аренды жилья
    person.monthly_expences *= 1.07;  // учет инфляции потребительской корзины
    person.salary *= 1.07;  // индексация зарплаты
    return person;
}


Person work_incident (Person person){
	if(person.mortgage == false){
		if(now_date.month == 8)person.capital -= person.salary;  //ежегодный отпуск Боба за свой счет
		else if ((now_date.month == 3 ^ now_date.month == 4) && now_date.year ==2036) person.capital -= person.salary;  //потеря работы Бобом
	}
	return person;
}


Person simulation (Person person) {
	now_date = begin_date;
    Money apartment_coast = 13000 * 1000;  // стоимость квартиры

    person = flat_payment(1000000, 0.16, apartment_coast, 30, person);  // рассчет ежемесячного платежа за ипотеку/аренду

    while ((now_date.year < last_date.year) ^ (now_date.month < last_date.month)) {

        person.capital = person.capital + person.salary - person.monthly_expences - person.person_flat_payment;  // рассчет капиталла
       	person = work_incident(person);
        person.capital *= 1.0166;

        now_date.month++;
        if(now_date.month == 13) {
            person = year_inflation(person);  // учет инфляции
            now_date.month = 1;
            now_date.year++;
        }
    }
    person.capital += apartment_coast * person.mortgage;  // учет квартиры в конечном капитале
    return person;
}


void results (Person Alice, Person Bob) {
    printf("Alice Capital %llu\n", Alice.capital);
    printf("Bob capital %llu\n", Bob.capital);
}


int main () {
    date_init(begin_date, last_date);

    Person Alice = init(1, 1500 * 1000);
    Person Bob = init(0, 1000 * 1000);

    Alice = simulation(Alice);
    Bob = simulation(Bob);

    results(Alice, Bob);
}
