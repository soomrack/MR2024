#include <stdio.h>


typedef long long int Money;  // Тип данных для денежных переменных


typedef struct 
{
    int month;
    int year;
} Date;


typedef struct
{
    char* name;
    Money price;
    int area;
    Money rent;
} House;


typedef struct 
{
    char* name;
    char* specialization; // Специализация
    Money salary;
    Money vacation_pay; // Отпускные
    Money annual_bonus; // Годовая премия
    Money quarter_bonus; // Квартальная премия
    Money year_income;
    int required_expirience; // Необходимый опыт работы
    int working_conditions; // Условия работы от 1 до 100

} Company;



typedef struct
{
    char* name;
    Money capital; // Сумма всех активов, пассивов и денег на банковском счёте
    Money bank_account; // Счёт в банке, который сразу кладётся на вклад
    double deposite_percent;
    char* specialization; 
    Company current_work;
    int current_restraint; // Оставшееся выдержка, падает каждый год, возрастает после отпуска   
    int restraint;  // Выдержка, чем выше, тем дольше готов находиться на неудобной работе
    int experience; // Стаж, опыт работы
    Money food_payment;  // Ежемесечные траты за еду
    Money comunal_payment;  // Траты на комунальные услуги
    Money another_payment;  //Траты на одежду, медикаменты и прочее
    Money vacation_cost;  // Ежегодные траты на отпуск
    Money ipotek_payment;  // Ежемесечная плата за ипотеку
    double inflation;  // Сделать общей
    double indexation;  // Сделать общей
    int amount_of_flats;  // Изменить название 
    House houses[10];  // Дома, которыми владеет 
    House desired_house;  // Дом, который желает купить следующим
} Hero;


House one_bedroom_apartment = {
    .name = "One bedroom apartment",
    .price = 12 * 1000 * 1000 * 100, 
    .area = 60, 
    .rent = 30 * 1000 * 100};  // 2-х комнатная квартира
House two_berdroom_apartment = {
    .name = "Two bedroom apartment",
    .price = 16 * 1000 * 1000 * 100, 
    .area = 85, 
    .rent = 50 * 1000 * 100};  // 3-х комнатная квартира
House country_house = {
    .name = "Country house",
    .price = 15 * 1000 * 1000 * 100, 
    .area = 130, 
    .rent = 60 * 1000 * 100};  // Загородний дом
House penthouse = {
    .name = "penthouse",
    .price = (Money)350 * 1000 * 1000 * 100, 
    .area = 380, 
    .rent = 800 * 1000 * 100};  // Пентхаус


House house_catalog[4];  // Каталог видов жилья
const int house_catalog_size = 4; 


void init_house_catalog()  // Инициализация каталога домов
{
    house_catalog[0] = one_bedroom_apartment;
    house_catalog[1] = two_berdroom_apartment; 
    house_catalog[2] = country_house;
    house_catalog[3] = penthouse;
    // увеличть в ручную размер каталога, при добавлении нового типа жилья
}


Company F_IT = {
    .name = "fIT",
    .specialization = "IT",
    .salary = 300 * 1000 * 100,
    .vacation_pay = 600 * 1000 * 100,
    .annual_bonus = 300 * 1000 * 100,
    .quarter_bonus = 100 * 1000 * 100,
    .working_conditions = 42,
    .required_expirience = 0,
};

Company S_IT = {
    .name = "sIT",
    .specialization = "IT",
    .salary = 350 * 1000 * 100,
    .vacation_pay = 700 * 1000 * 100,
    .annual_bonus = 250 * 1000 * 100,
    .quarter_bonus = 120 * 1000 * 100,
    .working_conditions = 31,
    .required_expirience = 0,
};

Company T_IT = {
    .name = "tIT",
    .specialization = "IT",
    .salary = 280 * 1000 * 100,
    .vacation_pay = 800 * 1000 * 100,
    .annual_bonus = 400 * 1000 * 100,
    .quarter_bonus = 80 * 1000 * 100,
    .working_conditions = 60,
    .required_expirience = 0,
};

Company Fourth_IT = {
    .name = "fourthIT",
    .specialization = "IT",
    .salary = 500 * 1000 * 100,
    .vacation_pay = 1000 * 1000 * 100,
    .annual_bonus = 500 * 1000 * 100,
    .quarter_bonus = 200 * 1000 * 100,
    .working_conditions = 51,
    .required_expirience = 5,
};

Company Fifth_IT = {
    .name = "fifithIT",
    .specialization = "IT",
    .salary = 1500 * 1000 * 100,
    .vacation_pay = 1000 * 1000 * 100,
    .annual_bonus = 1000 * 1000 * 100,
    .quarter_bonus = 500 * 1000 * 100,
    .working_conditions = 80,
    .required_expirience = 15,
};

Company F_Engine = {
    .name = "fEngine",
    .specialization = "Engineering",
    .salary = 400 * 1000 * 100,
    .vacation_pay = 600 * 1000 * 100,
    .annual_bonus = 300 * 1000 * 100,
    .quarter_bonus = 100 * 1000 * 100,
    .working_conditions = 38,
    .required_expirience = 0,
};

Company S_Engine = {
    .name = "sEngine",
    .specialization = "Engineering",
    .salary = 450 * 1000 * 100,
    .vacation_pay = 500 * 1000 * 100,
    .annual_bonus = 200 * 1000 * 100,
    .quarter_bonus = 150 * 1000 * 100,
    .working_conditions = 20,
    .required_expirience = 0,
};

Company T_Engine = {
    .name = "tEngine",
    .specialization = "Engineering",
    .salary = 350 * 1000 * 100,
    .vacation_pay = 800 * 1000 * 100,
    .annual_bonus = 400 * 1000 * 100,
    .quarter_bonus = 50 * 1000 * 100,
    .working_conditions = 78,
    .required_expirience = 0,
};

Company Fourth_Engine = {
    .name = "fourthEngine",
    .specialization = "Engineering",
    .salary = 700 * 1000 * 100,
    .vacation_pay = 1200 * 1000 * 100,
    .annual_bonus = 600 * 1000 * 100,
    .quarter_bonus = 10 * 1000 * 100,
    .working_conditions = 68,
    .required_expirience = 8,
};

Company Fifth_Engine = {
    .name = "fifthEngine",
    .specialization = "Engineering",
    .salary = 1000 * 1000 * 100,
    .vacation_pay = 2000 * 1000 * 100,
    .annual_bonus = 500 * 1000 * 100,
    .quarter_bonus = 200 * 1000 * 100,
    .working_conditions = 58,
    .required_expirience = 12,
};


Company company_catalog[10];  // Каталог компаний
const int company_catalog_size = 10; 


void init_company_catalog()  // Инициализация каталога компаний
{
    company_catalog[0] = F_IT;
    F_IT.year_income += F_IT.annual_bonus + 11 * F_IT.salary + F_IT.vacation_pay + F_IT.quarter_bonus * 4;
    company_catalog[1] = S_IT;
    S_IT.year_income += S_IT.annual_bonus + 11 * S_IT.salary + S_IT.vacation_pay + S_IT.quarter_bonus * 4;
    company_catalog[2] = T_IT;
    T_IT.year_income += T_IT.annual_bonus + 11 * T_IT.salary + T_IT.vacation_pay + T_IT.quarter_bonus * 4;
    company_catalog[3] = Fourth_IT;
    Fourth_IT.year_income += Fourth_IT.annual_bonus + 11 * Fourth_IT.salary + Fourth_IT.vacation_pay + Fourth_IT.quarter_bonus * 4;
    company_catalog[4] = Fifth_IT;
    Fifth_IT.year_income += Fifth_IT.annual_bonus + 11 * Fifth_IT.salary + Fifth_IT.vacation_pay + Fifth_IT.quarter_bonus * 4;
    company_catalog[5] = F_Engine;
    F_Engine.year_income += F_Engine.annual_bonus + 11 * F_Engine.salary + F_Engine.vacation_pay + F_Engine.quarter_bonus * 4;
    company_catalog[6] = S_Engine;
    S_Engine.year_income += S_Engine.annual_bonus + 11 * S_Engine.salary + S_Engine.vacation_pay + S_Engine.quarter_bonus * 4;
    company_catalog[7] = T_Engine;
    T_Engine.year_income += T_Engine.annual_bonus + 11 * T_Engine.salary + T_Engine.vacation_pay + T_Engine.quarter_bonus * 4;
    company_catalog[8] = Fourth_Engine;
    Fourth_Engine.year_income += Fourth_Engine.annual_bonus + 11 * Fourth_Engine.salary + Fourth_Engine.vacation_pay + Fourth_Engine.quarter_bonus * 4;
    company_catalog[9] = Fifth_Engine;
    Fifth_Engine.year_income += Fifth_Engine.annual_bonus + 11 * Fifth_Engine.salary + Fifth_Engine.vacation_pay + Fifth_Engine.quarter_bonus * 4;
    // увеличть в ручную размер каталога, при добавлении нового места работы
}


Hero midas;  // Мидас выбрал не заморачиваться и взять Ипотеку
Hero arno;  // Арно живёт в аренду и копит деньги


const int START_YEAR = 2024;
const int MONTH = 12;
double key_rate = 0.19;
double inflation = 0.07;  
Money comunal_per_area = 2 * 1000 * 100;  // Цена комунальных услуг за метр квадратный

void init_midas()  // Инициализация переменных Мидаса
{
    midas = (Hero){   
        .name = "Midas",
        .houses[0] = one_bedroom_apartment,
        .bank_account = 0,
        .specialization = "Engineering",
        .current_work =  F_Engine,
        .current_restraint = 200,
        .restraint = 200,  // Имеет большую выдержку
        .experience = 0,
        .food_payment = 25 * 1000 * 100,
        .comunal_payment = midas.houses[0].area * comunal_per_area,  // Зависит от площади
        .another_payment = 35 * 1000 * 100,
        .vacation_cost = 300 * 1000 * 100,
        .ipotek_payment = 200 * 1000 * 100,
        .deposite_percent = 0.18,
        .indexation = 0.07,
        .amount_of_flats = 1, 
        .desired_house = one_bedroom_apartment  
    };
}


void init_arno()  // Инициализация Арно 
{
    arno = (Hero){
        .name = "Arno",
        .houses[0] = one_bedroom_apartment,
        .bank_account = 1000 * 1000 * 100,
        .specialization = "IT",
        .current_work = F_IT,
        .current_restraint = 120,
        .restraint = 120, // Имеет нормальную выдержку
        .experience = 0,
        .food_payment = 25 * 1000 * 100 ,
        .comunal_payment =  arno.houses[0].area * comunal_per_area,  // Зависит от площади
        .another_payment = 45 * 1000 * 100,
        .vacation_cost = 300 * 1000 * 100,
        .deposite_percent = 0.18,
        .indexation = 0.07,
        .amount_of_flats = 0,
        .desired_house = one_bedroom_apartment
    };
}


// Написать функцию для покупки квартир, дач и недвижимостей
void buy_house(Hero* hero)
{
    hero -> bank_account -= hero -> desired_house.price;
    hero -> houses[hero->amount_of_flats] = hero->desired_house;
    hero -> amount_of_flats += 1;
}


House house_desire(Hero* hero)  // Функция для выбора покупки следующего жилья
{
    if (hero->amount_of_flats == 0){  // Добавить комментариии к условиям выбора квартиры 
        double best[2] = {1000000000, 0};
        for (int i = 0; i < 2; i++){
            // Первое жильё выбирается среди квартиры с лучшей ценой руб/м^2
            double efficient[2] = {(double)house_catalog[i].price / house_catalog[i].area, (double)i};
            if (efficient[0] > best[0])
            {
                best[0] = efficient[0];
                best[1] = efficient[1];
            }
        }
        return house_catalog[(int)best[1]];
    }
    else if(hero->amount_of_flats < 8){
        double best[2] = {1000000000, 0};
        for (int i = 0; i < house_catalog_size; i++){
            // Выбирается квартира самой выгодной арендой за свою цену, а также смотриться привлекательность для покупателя ()
            double efficient[2] = {(double)house_catalog[i].rent * 0.25 / (house_catalog[i].area * 1000 * 100) + (double)house_catalog[i].price / (house_catalog[i].rent) , (double)i};
            if (efficient[0] <= best[0])
            {
                best[0] = efficient[0];
                best[1] = efficient[1];
            }
        }
        return house_catalog[(int)best[1]];
    } 
    else{
        double best[2] = {100000000000, 0};
        for (int i = 0; i < house_catalog_size; i++){
            // Больше 6 квартир на сдачу и 1 одной личной героям не надо.
            // Поэтому он будет копить на покупку роскошных домов - пентхаусов, коттеджей, дворцов и т.д. 
            double efficient[2] = {(double)house_catalog[i].price / house_catalog[i].area, (double)i};
            if (house_catalog[i].area > 250 && efficient[0] < best[0]){
                best[0] = efficient[0];
                best[1] = efficient[1];
            }
        }
        if (best[0] == 100000000000){
            return hero->desired_house;
        }
        else
        {
            return house_catalog[(int)best[1]];
        }
    }
}


int house_want_to_buy(Hero* hero)  // Нужна ли в данный момент квартира
{
    if(hero->amount_of_flats == 0 && hero->bank_account > (Money)(hero->desired_house.price * 1.2)){
        return 1;
    }
    else if (hero->amount_of_flats > 0 && hero->bank_account > (Money)hero->desired_house.price * 1.2 
    && (hero->bank_account * hero->deposite_percent) < (hero->desired_house.rent * 12) && hero->amount_of_flats < 8)
    {
        return 1;
    }
    else if (hero->amount_of_flats >= 8 && hero->bank_account > hero->desired_house.price * 3){
        return 1;
    }
    return 0;
}


void house_operations(Hero* hero)  // Основаня функция для работы с домами
{
    hero->desired_house = house_desire(&*hero);
    if (house_want_to_buy(&*hero)){
        buy_house(&*hero);
        hero->desired_house = house_desire(&*hero);
    }
}


// Написать функцию домашних животных с рандомайзером
// Написать функцию для семьи и детей (В пользу Мидаса)
// Написать функцию для расчёта ипотеки и поиска размера квартиры


Company job_change(Hero* hero)
{
    Money best_year_income = 0; //Лучшая суммарная годовая зарплата
    int best_conditions = 0; // Лучшие условия
    Company best_companies[2];
    
    for (int i = 0; i < company_catalog_size; i ++ ){
        if (hero->specialization == company_catalog[i].specialization && hero->experience >= company_catalog[i].required_expirience){
            if (best_year_income < company_catalog[i].year_income){
                best_year_income = company_catalog[i].year_income;
                best_companies[0] = company_catalog[i];
            }
            if (best_conditions < company_catalog[i].working_conditions){
                best_conditions = company_catalog[i].working_conditions;
                best_companies[1] = company_catalog[i];
            }
        }
    }
    
    if (best_companies[0].name == best_companies[1].name)
    {
        return best_companies[0];
    }
    else if ((Money)(best_companies[1].salary * 1.2) >= best_companies[0].salary && best_companies[1].working_conditions > best_companies[0].working_conditions)
    {
        return best_companies[1];
    }
    else{
        return best_companies[0];
    }
       
}


void restraint_change(Hero* hero, const int current_month)
{
    if (current_month % 6 == 0) // Герой раз в полгода задумаеваться как он устал
    {
       hero->current_restraint -= (int)(100 - hero->current_work.working_conditions) / 2; 
    }
    
    if (hero->current_restraint <= 0)
    {
        printf("%s Job changed\n", hero->name);
        hero->current_work = job_change(&*hero); // Устал от работы, выбирает лучший вариант
        hero->current_restraint = hero->restraint;
    }
    if (current_month == 7) 
    {
        hero->current_restraint += 20; //Отдохнул в отпуске
    }
}


void experience_cup(Hero* hero, const int current_month, const int start_month)
{
    if (current_month == start_month){
        hero->experience += 1;
    }
}


void inflation_change(Date* start_date, Date* current_date)
{
    if ((current_date->year - start_date->year) % 6 == 0 && current_date->month == 2)
    {
        inflation = 0.11;
    }
    else if (inflation > 0.04 && current_date->month == 2)
    {
        inflation -= 0.015;
    }
}


void percent_change(Date* start_date, Date* current_date)
{
    if ((current_date->year - start_date->year) % 6 == 0 && current_date->month == 2)
    {
        key_rate = 0.2;

    }
    else if (key_rate > 0.05 && current_date->month == 2)
    {
        key_rate /= 1.23;
    }
}


void global_inflation(const int current_month)  // Дорожание товаров, которыми не пользуются (происходит в январе)
{
    if (current_month == 11)
    {
        for (int i = 0; i < house_catalog_size; i++){
            house_catalog[i].price += (Money)(house_catalog[i].price * inflation);
            house_catalog[i].rent += (Money)(house_catalog[i].rent * inflation);   
        }
        for (int i = 0; i < company_catalog_size; i++){
            company_catalog[i].salary += (Money)(company_catalog[i].salary * inflation);
            company_catalog[i].annual_bonus += (Money)(company_catalog[i].annual_bonus * inflation);
            company_catalog[i].quarter_bonus += (Money)(company_catalog[i].quarter_bonus * inflation);
            company_catalog[i].vacation_pay += (Money)(company_catalog[i].vacation_pay * inflation);
        }
    }
    
}


void personal_inflation_cost(Hero* hero, const int current_month)  // Инфляция цен которыми пользуется Героей (происходит в январе)
{
    if (current_month == 1){
        hero -> food_payment += (Money)(hero -> food_payment * inflation);
        hero -> comunal_payment += (Money)(hero -> comunal_payment * inflation); 
        hero -> another_payment += (Money)(hero -> another_payment * inflation); 
        hero -> vacation_cost += (Money)(hero -> vacation_cost * inflation);  
        hero -> houses[0].price += (Money)(hero -> houses[0].price * inflation);
        hero -> houses[0].rent += (Money)(hero -> houses[0].rent * inflation);
    }
}


void personal_indexation(Hero* hero, const int current_month)  // Индексация зарплат и премий (происходит в октябре)
{
    hero->indexation = inflation;
    if (current_month == 10)
    {
        hero->current_work.salary += (Money)(hero->current_work.salary * hero->indexation);
        hero->current_work.annual_bonus += (Money)(hero->current_work.annual_bonus * hero->indexation);
        hero->current_work.quarter_bonus += (Money)(hero->current_work.quarter_bonus * hero->indexation);
        hero->current_work.vacation_pay += (Money)(hero->current_work.vacation_pay * hero->indexation);
    }
    
}


void account_recount(Hero* hero, const int current_month)
{
    hero->deposite_percent = key_rate - 0.01;

    if (current_month == 7) // Уходит в неоплачиваемый отпуск
    {
        hero->bank_account += hero->current_work.vacation_pay;
        hero->bank_account -= hero->vacation_cost;
    }
    else {
        hero->bank_account += hero->current_work.salary;
        hero->bank_account -= hero->food_payment;
        hero->bank_account -= hero->comunal_payment;
    }

    hero->bank_account += (Money)((double)(hero->bank_account) * (hero->deposite_percent / 12));
    hero->bank_account -= hero->another_payment;  
    hero->bank_account -= hero->ipotek_payment;

    if (hero->amount_of_flats == 0){
        hero->bank_account -= hero->houses[0].rent;
    }

    if (current_month % 3 == 0){
        hero->bank_account += hero->current_work.quarter_bonus;
    }
    
    if (current_month == 12){
        hero->bank_account += hero->current_work.annual_bonus;
    }

}


void capital_recount(Hero* hero, const int current_month) // Пересчёт суммы капитала
{
    hero->capital = (Money)(hero->bank_account); 
    for (int i = 0; i < hero->amount_of_flats; i++){
        hero->capital +=(Money)hero->houses[i].price;
    }
}


void next_month(Date* current_date)  // Наступление следующего месяца
{
    current_date -> month += 1;
    if (current_date -> month >= 13){
            current_date -> month = 1;
            current_date -> year += 1;
    }
}


void simulation(Hero hero_list[], const int hero_list_size, const int start_month, const int final_year)  // Симмуляция на до определённого года и того месяца
{
    Date start_date = {
        .month = start_month,
        .year = START_YEAR,};
    Date current_date = {
        .month = start_month, 
        .year = START_YEAR};
    Date final_date = {
        .month = start_month,
        .year = final_year};

    while (current_date.year < final_date.year || current_date.month < final_date.month){
        for (int i = 0; i < hero_list_size; i++)
        {
            account_recount(&hero_list[i], current_date.month);
            experience_cup(&hero_list[i], current_date.month, start_date.month);
            restraint_change(&hero_list[i], current_date.month);
            personal_inflation_cost(&hero_list[i], current_date.month);
            personal_indexation(&hero_list[i], current_date.month); 
            capital_recount(&hero_list[i], current_date.month);
            house_operations(&hero_list[i]);
        }
        global_inflation(current_date.month);
        percent_change(&start_date, &current_date);
        inflation_change(&start_date, &current_date);
        next_month(&current_date);
    }
}


void final_conclusion(Hero hero_list[], const int hero_list_size){

    for (int i = 0; i < hero_list_size; i++)
    {
        printf("%s capital %lld\n", hero_list[i].name, (hero_list[i].capital) / 100);  // Вывод капитала Героя
        printf("%s flats %lld\n", hero_list[i].name, (hero_list[i].amount_of_flats)); 
    }

    for (int i = 0; i < house_catalog_size; i++){
        printf("%s price %lld and rent %lld\n", house_catalog[i].name, house_catalog[i].price / 100, house_catalog[i].rent / 100); // Цена квартир спустя 30 лет
    } 
    printf("\n");
   
}


int main()
{   
    // Инициализируем параметры Героев и каталога
    init_midas();
    init_arno();
    init_house_catalog();
    init_company_catalog();

    Hero hero_list[] = {midas, arno}; 
    int hero_list_size = (int)(sizeof(hero_list)/sizeof(hero_list[0]));
    
    simulation(hero_list, hero_list_size, 9, 2054);  // Запускаем симуляцию до 2054 года
    final_conclusion(hero_list, hero_list_size);  // Итоговый вывод

    return 1;
}
