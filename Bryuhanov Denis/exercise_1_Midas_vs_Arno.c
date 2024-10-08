#include <stdio.h>


typedef long long int Money;  //Тип данных для денежных переменных


typedef struct Hero
{
    Money balance;   // Количество денег на счету
    Money salary;
    Money salary_bonus;  // Премия в конце года
    Money ipotek;       // Перменная для сравнения стоимости ипотеки через 30 лет
    Money food_payment; // Ежемесечные траты за еду
    Money comunal_payment;   // Траты на комунальные услуги
    Money another_payment; //Траты на одежду, медикаменты и прочее
    Money vacation_cost;    // Ежегодные траты на отпуск
    Money ipotek_payment;  // Ежемесечная плата за ипотеку
    double deposite_percent;
    double inflation;
    double indexation;
} Hero;


Hero midas;  //Мидас выбрал не заморачиваться и взять Ипотеку
Hero arno;

const int CURRENT_YEAR = 2024;
const int MONTH = 12;
const int FINAL_YEAR = 2054;


void _init_midas()  //Инициализация переменных Мидаса
{
    midas = (Hero){   
        .balance = 0,
        .salary = 300 * 1000 * 100,
        .salary_bonus = 100 * 1000 * 100,
        .ipotek = 12 * 1000 * 1000 * 100,
        .food_payment = 20 * 1000 * 100,
        .comunal_payment = 15 * 1000 * 100,
        .another_payment = 35 * 1000 * 100,
        .vacation_cost = 300 * 1000 * 100,
        .ipotek_payment = 200 * 1000 * 100,
        .deposite_percent = 0.18,
        .inflation = 0.07,
        .indexation = 0.07
    };
}


void _init_arno()  //Инициализация Арно 
{
    arno = (Hero){
        .balance = 1000 * 1000 * 100,
        .salary = 200 * 1000 * 100,
        .salary_bonus = 300 * 1000 * 100, //Премия в конце года
        .food_payment = 25 * 1000 * 100 ,
        .comunal_payment = 15 * 1000 * 100,
        .another_payment = 45 * 1000 * 100,
        .vacation_cost = 250 * 1000 * 100,
        .ipotek_payment = 0,
        .deposite_percent = 0.18,
        .inflation = 0.07,
        .indexation = 0.07
    };
}


void cost_inflation(Hero* hero)  //Инфляция цен
{
    hero -> food_payment += (Money)(hero -> food_payment * hero -> inflation);
    hero -> comunal_payment += (Money)(hero -> comunal_payment * hero -> inflation); 
    hero -> another_payment += (Money)(hero -> another_payment * hero -> inflation); 
    hero -> vacation_cost += (Money)(hero -> vacation_cost * hero -> inflation);  
    hero -> ipotek += (Money)(hero -> ipotek * hero -> inflation);
}


void indexation(Hero* hero) //Индексация зарплат и премий
{
    hero -> salary += (Money)(hero -> salary * hero -> indexation);
    hero -> salary_bonus += (Money)(hero -> salary_bonus * hero -> indexation);
}


void simulation(Hero* hero, int current_month, int final_year)   // симмулиция 30 лет
{
    int current_year = CURRENT_YEAR;
    int final_month = current_month;
    while (current_year < final_year || current_month < final_month){
        hero->balance += (Money)((double)(hero->balance) * (hero->deposite_percent / 12));
        hero->balance += hero->salary;
        hero->balance -= hero->food_payment;
        hero->balance -= hero->comunal_payment;
        hero->balance -= hero->another_payment;  
        hero->balance -= hero->ipotek_payment;
        current_month += 1;
        if (current_month >= 13){
            current_month = 1;
            current_year += 1;

            hero -> balance += hero -> salary_bonus;
            hero -> balance -= hero -> vacation_cost;

            cost_inflation(&*hero);
            indexation(&*hero);
        }
        
       
    }
}


int main()
{
    _init_midas();
    _init_arno();

    simulation(&midas, 9, 2054);
    simulation(&arno, 9, 2054);

    printf("midas balance %lld\n", (midas.balance + midas.ipotek) / 100);
    printf("arno balance %lld\n", arno.balance / 100);
    printf("house cost %lld\n", (midas.ipotek) / 100);  //Цена квартиры спустя 30 лет
    printf("\n");
    return 1;
}
