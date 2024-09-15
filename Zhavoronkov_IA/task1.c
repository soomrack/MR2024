#include <stdio.h>

typedef unsigned long long int Money;

double monthly_percent = (double)20/12/100;


typedef struct Person
{
    Money start_sum;
    Money flat_cost;
    Money start_salary;
    double bank_account;
    Money expences;
    Money debet_card;
    //ToDo: дописать переменные
} Person;

Person Alice;
Person Bob;




void Bob_init()
{
    //Задаем начальные параметры Боба в копейках
    Bob.start_sum=0*1000*1000*100;
    Bob.flat_cost=30*1000*100;
    Bob.start_salary=2*100*1000*100;
    Bob.bank_account=0.0;
    Bob.expences=40*1000*100;
    Bob.debet_card=Bob.start_sum;
}


void Alice_init()
{
    //Задаем начальные параметры Элис в копейках
    Alice.start_sum=1*1000*1000*100;
    Alice.flat_cost=30*1000*100;
    Alice.start_salary=2*100*1000*100;
    Alice.bank_account=0.0;
    Alice.expences=40*1000*100;
    Alice.debet_card=Alice.start_sum;

}


/*int new_init(const Person person)
{
    printf("Enter %s start_sum\n", person);
    scanf("%d", &person.start_sum);
    return person.start_sum;;

} */


/*int init_info()
{
    printf("Enter initial parametrs\n");
    printf("Enter 0 for standart parametrs and 1 for unique\n");
    int code;
    scanf("%d", &code);
    if (code == 0) {
        Bob_init();
        Alice_init();
        printf ("%d", Bob.flat_cost);
    } else {
        new_init(Bob);
        printf("%d",Bob);
    } 

}*/


void Bob_salary()
{
    Bob.debet_card += Bob.start_salary;
    //Учесть индексацию
}


void Bob_expences() //Коммуналка, еда, одежда и тд
{
    Bob.debet_card -= Bob.expences;
    //Учесть инфляцию 
}


void Bob_flat()
{
    Bob.debet_card -= Bob.flat_cost;
    //Учесть инфляцию
}


void Bob_bank()
{
    printf("%.4f\n",monthly_percent);
    Bob.bank_account += (double)monthly_percent*Bob.bank_account;
    Bob.bank_account += Bob.debet_card;
    printf("Bob bank account %.2f\n\n", Bob.bank_account/100);
    Bob.debet_card = 0;
}


void Bob_count()
{
    Bob_salary();
    Bob_expences();
    Bob_flat();
    Bob_bank();

}


/*void Alice_count()
{


}*/


int simulation(int month, int year)
{
    //2054
    int end_month = month;
    int end_year = year + 2;
    Bob_init();
    

    while (!(year == end_year && month == end_month + 1 ))
    {
        printf("Current period %d.%d\n",month, year);
        Bob_count();
        //Alice_count();
        month++;

        if (month == 13)
        {
            month=1;
            year++;
        }
    }
    
    printf("Bob Money: %.2f\n",Bob.bank_account/100);
    //printf("Alice Money: %.2f",Alice.bank_account/100);

}


int main() 
{
    simulation(9,2024);

}


/*Условия задачи:
Alice и Bob, стартовый капитал - 1.000.000 Р 
Alice эту сумму как первый взнос по ипотеке (15.000.000 Р) на 30 лет под 17% годовых (ставку рассчитать по онлайн-калькулятору)
Bob копит на квартиру и живет в съемной - 30.000 Р в месяц
Зарплата обоих составляет 200.000 Р в месяц
Стоит учитывать базовые потребности типа еды, комуналки, 
Все цены и зарплаты подвержены инфляции в 8% в год

*/
