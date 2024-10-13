#include <stdio.h>


typedef long long int Money;  //Тип данных для денежных переменных


typedef struct
{
    Money balance;   // Количество денег на счету
    Money salary;
    Money salary_bonus;  // Премия в конце года
    Money flat_cost;       // Перменная для сравнения стоимости квартиры через 30 лет
    Money rent;
    Money food_payment; // Ежемесечные траты за еду
    Money comunal_payment;   // Траты на комунальные услуги
    Money another_payment; //Траты на одежду, медикаменты и прочее
    Money vacation_cost;    // Ежегодные траты на отпуск
    Money ipotek_payment;  // Ежемесечная плата за ипотеку
    double deposite_percent;
    double inflation;
    double indexation;
    int amount_of_flats;
} Hero;


typedef struct 
{
    int month;
    int year;
} Date;


typedef struct
{
    Money price;
    int area;
    Money rent;
} House;


House one_bedroom_apartment = {.price = 12 * 1000 * 1000 * 100, .area = 60, .rent = 30 * 1000 * 100}; //2-х комнатная квартира
House two_berdroom_apartment = {.price = 16 * 1000 * 1000 * 100, .area = 80, .rent = 50 * 1000 * 100}; //3-х комнатная квартира
House country_house = {.price = 15 * 1000 * 1000 * 100, .area = 130, .rent = 60 * 1000 * 100}; //Загородний дом
House penthouse = {.price = 350 * 1000 * 1000 * 100, .area = 363, .rent = 800 * 1000 * 100}; //Пентхаус


Hero midas;  //Мидас выбрал не заморачиваться и взять Ипотеку
Hero arno;  //Арно живёт в аренду и копит деньги

const int CURRENT_YEAR = 2024;
const int MONTH = 12;


void _init_midas()  //Инициализация переменных Мидаса
{
    midas = (Hero){   
        .balance = 0,
        .salary = 300 * 1000 * 100,
        .salary_bonus = 100 * 1000 * 100,
        .flat_cost = one_bedroom_apartment.price, //Берёт 2-х комнатную квартиру в ипотеку
        .food_payment = 20 * 1000 * 100,
        .comunal_payment = 15 * 1000 * 100,
        .another_payment = 35 * 1000 * 100,
        .vacation_cost = 300 * 1000 * 100,
        .ipotek_payment = 200 * 1000 * 100,
        .deposite_percent = 0.18,
        .inflation = 0.07,
        .indexation = 0.07,
        .amount_of_flats = 1
    };
}


void _init_arno()  //Инициализация Арно 
{
    arno = (Hero){
        .balance = 1000 * 1000 * 100,
        .salary = 200 * 1000 * 100,
        .salary_bonus = 300 * 1000 * 100, //Премия в конце года
        .flat_cost = two_berdroom_apartment.price, //Берёт 3-ёх комнатную квартиру, когда накопит денег
        .food_payment = 25 * 1000 * 100 ,
        .comunal_payment = 15 * 1000 * 100,
        .another_payment = 45 * 1000 * 100,
        .vacation_cost = 250 * 1000 * 100,
        .deposite_percent = 0.18,
        .inflation = 0.07,
        .indexation = 0.07,
        .amount_of_flats = 0
    };
}


void cost_inflation(Hero* hero)  //Инфляция цен
{
    hero -> food_payment += (Money)(hero -> food_payment * hero -> inflation);
    hero -> comunal_payment += (Money)(hero -> comunal_payment * hero -> inflation); 
    hero -> another_payment += (Money)(hero -> another_payment * hero -> inflation); 
    hero -> vacation_cost += (Money)(hero -> vacation_cost * hero -> inflation);  
    hero -> flat_cost += (Money)(hero -> flat_cost * hero -> inflation);
}


void indexation(Hero* hero) //Индексация зарплат и премий
{
    hero -> salary += (Money)(hero -> salary * hero -> indexation);
    hero -> salary_bonus += (Money)(hero -> salary_bonus * hero -> indexation);
}


void balance_recount(Hero* hero, int current_month)
{
    hero->balance += (Money)((double)(hero->balance) * (hero->deposite_percent / 12));
    hero->balance += hero->salary;
    hero->balance -= hero->food_payment;
    hero->balance -= hero->comunal_payment;
    hero->balance -= hero->another_payment;  
    hero->balance -= hero->ipotek_payment;
    if (current_month == 12){
        hero -> balance += hero -> salary_bonus;
        hero -> balance -= hero -> vacation_cost;
    }
}



//Написать функцию для покупки квартир, дач и недвижимостей
void house_operations(Hero* hero)
{
    if (hero -> amount_of_flats > 0){
        
    }else{
        if (hero -> balance >= hero -> flat_cost){
            buy_flat(&*hero);
        }
    }
}


void buy_flat(Hero* hero)
{
    hero -> balance -= hero -> flat_cost;
    hero -> amount_of_flats = 1;
    hero -> rent = 0;
}
//Написать функцию для смены работы
//Написть функцию для получения отпуска
//Написать функцию покупки машины
//Написать функцию домашних животных с рандомайзером
//Написать функцию для семьи и детей (В пользу Мидаса)
//Написать функцию для расчёта ипотеки и поиска размера квартиры


void time_duration(Hero* hero, Date* current_date) //изменение годов и месяцев
{
    current_date -> month += 1;
    if (current_date -> month >= 13){
            current_date -> month = 1;
            current_date -> year += 1;
            cost_inflation(&*hero);
            indexation(&*hero);
        }
}


void simulation(Hero* hero, int current_month, int final_year)   //симмуляция на до определённого года
{
    Date current_date = {.month = current_month, .year = CURRENT_YEAR};
    Date final_date = {.month = current_month, .year = final_year};
    while (current_date.year < final_date.year || current_date.month < final_date.month){
        balance_recount(&*hero, current_date.month);
        time_duration(&*hero, &current_date);  
    }
}


int main()
{
    _init_midas();
    _init_arno();

    simulation(&midas, 9, 2054);
    simulation(&arno, 9, 2054);

    printf("midas balance %lld\n", (midas.balance + midas.flat_cost) / 100); // Сделать одной функцией
    printf("arno balance %lld\n", arno.balance / 100);
    printf("house cost %lld\n", (midas.flat_cost) / 100);  //Цена квартиры спустя 30 лет
    printf("\n");
    return 1;
}
