#include <stdio.h>


typedef long long int Money;  //Тип данных для денежных переменных


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


typedef struct
{
    Money balance;   // Количество денег на счету
    Money salary;
    Money salary_bonus;  // Премия в конце года
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
    House personal_home; // Дом где живёт 
    House another_houses[10];
    House desired_house;
} Hero;


House one_bedroom_apartment = {.price = 12 * 1000 * 1000 * 100, .area = 60, .rent = 30 * 1000 * 100}; //2-х комнатная квартира
House two_berdroom_apartment = {.price = 16 * 1000 * 1000 * 100, .area = 85, .rent = 50 * 1000 * 100}; //3-х комнатная квартира
House country_house = {.price = 15 * 1000 * 1000 * 100, .area = 130, .rent = 60 * 1000 * 100}; //Загородний дом
House penthouse = {.price = (Money)350 * 1000 * 1000 * 100, .area = 380, .rent = 800 * 1000 * 100}; //Пентхаус


House catalog[4];
const int catalog_size = 4;

void _init_catalog()
{
    catalog[0] = one_bedroom_apartment;
    catalog[1] = two_berdroom_apartment; 
    catalog[2] = country_house;
    catalog[3] = penthouse;
    
}


Hero midas;  //Мидас выбрал не заморачиваться и взять Ипотеку
Hero arno;  //Арно живёт в аренду и копит деньги


const int CURRENT_YEAR = 2024;
const int MONTH = 12;
int inflation = 7;
int indexation = 7;
Money comunal_per_area = 2 * 1000 * 100; //Цена комунальных услуг за метр квадратный

void _init_midas()  //Инициализация переменных Мидаса
{
    midas = (Hero){   
        .personal_home = one_bedroom_apartment,
        .balance = 0,
        .salary = 300 * 1000 * 100,
        .salary_bonus = 100 * 1000 * 100,
        .food_payment = 20 * 1000 * 100,
        .comunal_payment = midas.personal_home.area * comunal_per_area, // Зависит от площади
        .another_payment = 35 * 1000 * 100,
        .vacation_cost = 300 * 1000 * 100,
        .ipotek_payment = 200 * 1000 * 100,
        .deposite_percent = 0.18,
        .inflation = 0.07,
        .indexation = 0.07,
        .amount_of_flats = 1, 
        .desired_house = one_bedroom_apartment
    };
}


void _init_arno()  //Инициализация Арно 
{
    arno = (Hero){
        .personal_home = one_bedroom_apartment,
        .balance = 1000 * 1000 * 100,
        .salary = 200 * 1000 * 100,
        .salary_bonus = 300 * 1000 * 100, //Премия в конце года
        .food_payment = 25 * 1000 * 100 ,
        .comunal_payment =  arno.personal_home.area * comunal_per_area, //Зависит от площади
        .another_payment = 45 * 1000 * 100,
        .vacation_cost = 250 * 1000 * 100,
        .deposite_percent = 0.18,
        .inflation = 0.07,
        .indexation = 0.07,
        .amount_of_flats = 0,
        .desired_house = one_bedroom_apartment,
        .another_houses[0] = one_bedroom_apartment
    };
}


void personal_inflation_cost(Hero* hero)  //Инфляция цен 2
{
    hero -> food_payment += (Money)(hero -> food_payment * hero -> inflation);
    hero -> comunal_payment += (Money)(hero -> comunal_payment * hero -> inflation); 
    hero -> another_payment += (Money)(hero -> another_payment * hero -> inflation); 
    hero -> vacation_cost += (Money)(hero -> vacation_cost * hero -> inflation);  
    hero -> personal_home.price += (Money)(hero -> personal_home.price * hero -> inflation);
    hero -> personal_home.rent += (Money)(hero -> personal_home.rent * hero -> inflation);
}


void inflation_cost()
{
    one_bedroom_apartment.price += one_bedroom_apartment.price * inflation;
    one_bedroom_apartment.rent += one_bedroom_apartment.rent * inflation;
    two_berdroom_apartment.price += two_berdroom_apartment.price * inflation;
    two_berdroom_apartment.rent += two_berdroom_apartment.rent * inflation;
    country_house.price += country_house.price * inflation;
    country_house.rent += country_house.rent * inflation;
    penthouse.price += penthouse.price * inflation;
    penthouse.rent += penthouse.rent * inflation;
}


void personal_indexation(Hero* hero) //Индекса  ция зарплат и премий
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
    if (hero->amount_of_flats == 0)
    {
        hero->balance -= hero->personal_home.rent;
    }
    if (current_month == 12){
        hero -> balance += hero -> salary_bonus;
        hero -> balance -= hero -> vacation_cost;
    }
}



//Написать функцию для покупки квартир, дач и недвижимостей
void house_operations(Hero* hero)
{

}


void buy_house(Hero* hero)
{
    hero -> balance -= hero -> desired_house.price;
    hero -> another_houses[hero->amount_of_flats] = hero->desired_house;
    hero -> amount_of_flats += 1;
}


House house_desire(Hero* hero)
{
    if (hero->amount_of_flats == 0){
        double max[2] = {1000000000, 0};
        for (int i = 0; i < 2; i++){
            double efficient[2] = {(double)catalog[i].price / catalog[i].area, (double)i};
            if (efficient[0] > max[0])
            {
                max[0] = efficient[0];
                max[1] = efficient[1];
            }
        }
        return catalog[(int)max[1]];
    }
    else if(hero->amount_of_flats < 8){
        double max[2] = {1000000000, 0};
        for (int i = 0; i < catalog_size; i++){
            double efficient[2] = {(double)catalog[i].price / catalog[i].area + (double)catalog[i].price / (catalog[i].rent / 1000 * 100) , (double)i};
            if (efficient[0] <= max[0])
            {
                max[0] = efficient[0];
                max[1] = efficient[1];
            }
        }
        return catalog[(int)max[1]];
    } 
    else{
        double best[2] = {100000000000, 0};
        for (int i = 0; i < catalog_size; i++){
            double efficient[2] = {(double)catalog[i].price / catalog[i].area, (double)i};
            if (catalog[i].area > 250 && efficient[0] < best[0]){
                best[0] = efficient[0];
                best[1] = efficient[1];
            }
        }
        if (best[0] == 100000000000){
            return hero->desired_house;
        }
        else
        {
            return catalog[(int)best[1]];
        }
    }
}


void house_misery(Hero* hero) //Нужна ли в данный момент квартира
{
    if(hero->amount_of_flats == 0 && hero->balance > (Money)(hero->desired_house.price * 1.2)){
        buy_house(&*hero);
        hero->desired_house = house_desire(&*hero);
    }
    else if (hero->amount_of_flats > 0 && hero->balance > (Money)hero->desired_house.price * 1.2 
    && (hero->balance * hero->deposite_percent) < (hero->desired_house.rent * 12) && hero->amount_of_flats < 8)
    {
        buy_house(&*hero);
        hero->desired_house = house_desire(&*hero);
    }
    else if (hero->amount_of_flats >= 8 && hero->balance > hero->desired_house.price * 3){
        buy_house(&*hero);
        hero->desired_house = house_desire(&*hero);
    }
}



//Написать функцию для изменения процентов вклада, инфляции и индексации
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
            personal_inflation_cost(&*hero);
            personal_indexation(&*hero);
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
    _init_catalog();

    
    simulation(&midas, 9, 2054);
    simulation(&arno, 9, 2054);

    printf("midas balance %lld\n", (midas.balance + midas.personal_home.price) / 100); 
    printf("arno balance %lld\n", arno.balance / 100);
    printf("house cost %lld\n", (midas.personal_home.price) / 100);  //Цена квартиры спустя 30 лет
    printf("\n");
    
   /*
    printf("%lld\n", catalog[0].price);
    printf("%lld\n", catalog[1].price);
    printf("%lld\n", catalog[2].price);
    printf("%lld\n", catalog[3].price);
    printf("%lld\n", house_desire(&midas).price);
    printf("%lld\n", house_desire(&arno).price);
    */
    return 1;
    
}
