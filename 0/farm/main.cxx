#include <stdio.h>

class Climate {
public:
    int temperature_min;
    int temperature_max;
    int humidity_dirt_min;
    int humidity_dirt_max;
public:
    void print();
};


class Thermometer {
public:
    int temperature;
public:
    void get_temperature();  // TODO: get data from arduino
};


class Heater {
public:
    bool on_temperature;
public:
    Heater();
public:
    void start() {};  // TODO: needs arduino
    void stop() {};   // TODO: needs arduino
};


class Ventilator {
public:
    bool on_temperature;
    bool on_schedule;
public:
    Ventilator();
public:
    void start() {};  // TODO: needs arduino
    void stop() {};   // TODO: needs arduino
};


class Datatime {
public:
    int hour;
public:
    void get_datatime();  // TODO: needs clocks
};


Heater::Heater()
{
    on_temperature = false;
}


Ventilator::Ventilator()
{
    on_schedule = false;
    on_temperature = false;
}


void Thermometer::get_temperature()
{
    temperature = 15;  // here in future will be a code to get temperature from arduino
}


void Climate::print()
{
    printf("Climate:\n");
    printf("Temperature max: %d\n", temperature_max);
    printf("Temperature min: %d\n", temperature_min);
    printf("Humidity dirt max: %d\n", humidity_dirt_max);
    printf("Humidity dirt min: %d\n", humidity_dirt_min);
}


void Datatime::get_datatime()
{
    hour = 15;  // in future here will be code to get datatime
}


void set_climate_tomato(Climate &climate)
{
    climate.temperature_max = 30;
    climate.temperature_min = 20;
    climate.humidity_dirt_max = 10;
    climate.humidity_dirt_min = 5; 
}


void control_temperature(Climate climate,
                         Thermometer thermometer,
                         Heater &heater,
                         Ventilator &ventilator)
{
    if(thermometer.temperature < climate.temperature_min) {
        heater.on_temperature = true;
    }

    if(thermometer.temperature > climate.temperature_max) {
        heater.on_temperature = false;
        ventilator.on_temperature = true;
    }
}


void control_ventilation(Datatime datatime,
                         Ventilator &ventilator)
{
    // Not ventilate at night (23:00 -- 06:00)
    if(datatime.hour < 6 || datatime.hour > 23) {
        ventilator.on_schedule = false;
        return;
    }
    
    // Ventilation every 4 hours for one hour
    if(datatime.hour % 4 == 0) {  
        ventilator.on_schedule = true;
    }
    else {
        ventilator.on_schedule = false;
    }
}


void do_heat(Heater heater)
{
    if(heater.on_temperature) {
        heater.start();
    }
    else{
        heater.stop();
    }
}


void do_ventilation(Ventilator ventilator)
{
    if(ventilator.on_temperature || ventilator.on_schedule) {
        ventilator.start();
    }
    else {
        ventilator.stop();
    }
}


int main()
{
    Climate climate_tomato;
    set_climate_tomato(climate_tomato);

    Datatime datatime;
    
    Thermometer thermometer_first;
    Thermometer thermometer_second;

    Heater heater_first;
    Heater heater_second;

    Ventilator ventilator_first;
    Ventilator ventilator_second;

    datatime.get_datatime();
    thermometer_first.get_temperature();
    thermometer_second.get_temperature();
    // get_humidity();
    // get_light();
    
    control_temperature(climate_tomato,
                        thermometer_second,
                        heater_first,
                        ventilator_first);
    // control_humidity();
    control_ventilation(datatime, ventilator_second);
    // control_light();

    do_heat(heater_first);
    do_heat(heater_second);
    do_ventilation(ventilator_first);
    do_ventilation(ventilator_second);
    // do_watering();
    // do_light();
    
    climate_tomato.print();
}



