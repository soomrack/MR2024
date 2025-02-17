//файл переменной конфигурации
#pragma once

#define PIN_LIGHT_SENSOR A0
#define PIN_EARTH_WET_SENSOR A1
#define PIN_HEAT 4
#define PIN_WATER_PUMP 5
#define PIN_UV_LED_LIGHT 6
#define PIN_COOLER 7
#define PIN_TEMP_WET_SENSOR 12

#define BASE_TIME_DAY 86400000 //в мс - одни сутки
#define BASE_TIME_SUNTIME 43200000 //в мс - половина суток
#define BASE_TIME_NIGHT 43200000 //в мс - половиина суток
#define TIME_CHECK_SENSORS 10 //период времени опроса датчиков

/*#define WORK_TIME_WATER_PUMP 20000 //время работы помпы в случае если почва недостаточно влажная
#define WORK_TIME_UV_LED 60000 //время работы подсветки, если света недостаточно
#define WORK_TIME_COOLER 60000 //время работы вентилятора в случае если кислорода в воздухе недостаточно
#define WORK_TIME_HEATER 20000 //время работы нагревателя, если в камере стало холодно
                               //стоит учитывать, что работа нагревателя в контексте этой теплицы наиболее хорошо 
                               //показывает себя при включении одновременно с вентилятором*/


#define CFG_FLOWER_SLEEP_MODE 1 //если 1, то система дает растению поспать
                              //если 0, то подстветка включается ночью и не дает растению спать и оно бодрствует 24/7  

#if !(defined(BASE_TIME_DAY) && defined(BASE_TIME_SUNTIME) && defined(BASE_TIME_NIGHT))  
# error "Base times of day are not defined"
#endif

#if !defined(TIME_CHECK_SENSORS)   
# error "Time of checking snsors is not defined"
#endif

/*#if !(defined(WORK_TIME_WATER_PUMP) && defined(WORK_TIME_UV_LED) && defined(WORK_TIME_COOLER) && defined(WORK_TIME_HEATER)) 
# error "Work times of mechanism are not defined"
#endif*/
