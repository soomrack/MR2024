#include <stdio.h>
#include <math.h>
#include <stdbool.h>

typedef long long money;


typedef struct person{
	money salary;  //зарплата персоны
	money capital;  //капиталл персоны
	money person_flat_payment;  //ежемесечная плата за ипотеку
	money monthly_expences;  //ежемесячные расходы
} person;


typedef struct date{
	long long month;
	long long year;
} date;


money mortgage_payment(money initial_payment, float percent, money mortgage_summ, money mortgage_years){
	//printf("Mortage\n");
	money periods = mortgage_years * 12;  //число периодов в месяцах
	money mortgage_month_payment = (((percent / 12) * pow(1 + (percent / 12),periods)) / (pow(1 + (percent / 12),periods) - 1)) * mortgage_summ;  //рассчет ежемесячного платежа
	return mortgage_month_payment;
}


person simulation (person Person, date now_date, date last_date, bool mortgage){
	//printf("Sim\n");
	if(mortgage == true){
		Person.capital -= 1000000;  //первый взнос ипотеки
		Person.person_flat_payment = mortgage_payment(1000000,0.16,13000000,30);  //рассчет ежемесячного платежа ипотеки
	}
	else{ 
		Person.person_flat_payment = 30000;  //учет аренды
	} 
	
	while ((now_date.year < last_date.year) ^ (now_date.month < last_date.month)){
		
		Person.capital = Person.capital + Person.salary - Person.monthly_expences - Person.person_flat_payment;  //рассчет капиталла 
		Person.capital *= 1.0166;
		
		now_date.month ++ ;
		if(now_date.month == 13){
			if(mortgage == false) Person.person_flat_payment *= 1.07;  //учет инфляции аренды жилья
			Person.monthly_expences *= 1.07;  //учет инфляции потребительской корзины
			Person.salary *= 1.07;  //индексация зарплаты
			now_date.month = 1;
			now_date.year ++ ;
		}
	}
	//printf("Sim end\n");
	return Person;
}


int main (){
    date begin_date;
    date last_date;
    begin_date.month = 1;
    begin_date.year = 2024;
    last_date.month = 1;
    last_date.year = 2054;
    
    //создание Alice и параметров персоны
    person Alice;
    Alice.capital = 1000000;
    Alice.salary = 200000;
    Alice.monthly_expences = 30000;
    
    //создание Bob и параметров персоны
    person Bob;
    Bob.capital = 1000000;
    Bob.salary = 200000;
    Bob.monthly_expences = 30000;
    
    Bob = simulation (Bob, begin_date, last_date, 0);
    Alice = simulation (Alice, begin_date, last_date, 1);
    printf("Alice Capital %llu\n", Alice.capital);
    printf("Bob capital %llu\n", Bob.capital);
}