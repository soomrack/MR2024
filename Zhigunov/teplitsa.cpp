#include <DHT11.h>

#define PIN_LIGHT_SENS A0
#define PIN_SOIL_SENS A1
#define PIN_HEAT_CTRL 4
#define PIN_PUMP_CTRL 5
#define PIN_LIGHT_CTRL 6
#define PIN_FAN_CTRL 7
#define PIN_DHT11_SENS 12

DHT11 dht( PIN_DHT11_SENS );


struct Terrarium_stat {
  int temp;
  int air_hum;
  int soil_hum;
  bool light_sens;
  long last_pump_enable;
};


class Climate {
  public:
    int min_temp;
    int max_temp;
    int min_air_hum;
    int max_air_hum;
    int min_soil_hum;
    int max_soil_hum;
    int fan_enable_time;
    int fan_disable_time;
  public:
    void print_stat();
};


// Термометр //
class Termometer {
  private:
    int temperature;
    int humidity;
    const DHT11 & sensor_;
  public:
    Termometer( DHT11 &sensor );
    void update_param();
    int get_temp();
    int get_hum();
};


Termometer::Termometer( DHT11 &sensor ) : sensor_( sensor )
{

}


void Termometer::update_param()
{
  temperature = sensor_.readTemperature();
  humidity = sensor_.readHumidity();
}


int Termometer::get_temp()
{
  update_param();
  return temperature;
}


int Termometer::get_hum()
{
  update_param();
  return humidity;
}


//  гигрометр почвы //
class Soil_hygrometer {
  private:
    int soil_humidity;
    unsigned int pin_hygrometer;
  public:
    Soil_hygrometer( unsigned int pin );
    void update_param();
    int get_hum();
};


Soil_hygrometer::Soil_hygrometer( unsigned int pin ) : pin_hygrometer( pin )
{

}


void Soil_hygrometer::update_param()
{
  soil_humidity = map( analogRead( pin_hygrometer ), 0, 1023, 100, 0 );
}


int Soil_hygrometer::get_hum()
{
  update_param();
  return soil_humidity;
}


// сенсор света //
class Lighsensor_ {
  private:
    bool light;
    unsigned int pin_lightsensor;
  public:
    Lighsensor_( unsigned int pin );
    void update_param();
    int get_light();
};


Lighsensor_::Lighsensor_( unsigned int pin ) : pin_lightsensor( pin )
{

}


void Lighsensor_::update_param()
{
  int l = map( analogRead( pin_lightsensor ), 0, 1023, 100, 0 );
  if ( l > 50 ) light = 0;
  else light = 1;
}


int Lighsensor_::get_light()
{
  update_param();
  return light;
}


// нагреввтель //
class Heater {
  private:
    unsigned int heater_pin;
  public:
    bool on_temperature;

  public:
    Heater( unsigned int pin );
    void change( bool stat );
};


Heater::Heater( unsigned int pin ) : heater_pin( pin )
{
  on_temperature = false;
}

void Heater::change( bool stat )
{
  digitalWrite( heater_pin, stat );
  on_temperature = stat;
}


// вентилятор //
class Fan {
  private:
    unsigned int pin_fan;
  public:
    bool on_blowing;
  public:
    Fan( unsigned int pin );
    void change( bool stat );
};


Fan::Fan( unsigned int pin ) : pin_fan( pin )
{
  on_blowing = false;
}


void Fan::change( bool stat )
{
  digitalWrite( pin_fan, stat );
  on_blowing = stat;
}


// насос //
class Pump {
  private:
    unsigned int pin_pump;
  public:
    bool on_floating;
  public:
    Pump( unsigned int pin );
    void change( bool stat );
};


Pump::Pump( unsigned int pin ) : pin_pump( pin )
{
  on_floating = false;
}

void Pump::change( bool stat )
{
  digitalWrite( pin_pump, stat );
  on_floating = stat;
}


// свет //
class Light {
  private:
    unsigned int pin_light;
  public:
    bool on_ligt;
  public:
    Light( unsigned int pin );
    void change( bool stat );
};


Light::Light( unsigned int pin ) : pin_light( pin )
{
  on_ligt = false;
}

void Light::change(bool stat)
{
  digitalWrite( pin_light, stat );
  on_ligt = stat;
}


// контроль времени //
class Datatime {
  private:
    int hour;
    unsigned long seconds;
  public:
    unsigned long get_seconds();
    int get_hour();
};


unsigned long Datatime::get_seconds() {
  seconds = round( millis() / 1000 );
  seconds *= 1;
  return seconds;
}


int Datatime::get_hour() {
  hour = ( get_seconds() / 60 / 60 ) % 24;
  return hour;
}


// параметры климата //
void set_climate_plant( Climate &climate ) {
  climate.min_temp = 20;  //C
  climate.max_temp = 25;  //C
  climate.min_air_hum = 50;  //%
  climate.max_air_hum = 80;  //%
  climate.min_soil_hum = 10;  //%
  climate.max_soil_hum = 30;  //%
  climate.fan_enable_time = 7;  //hours
  climate.fan_disable_time = 21;  //hours
}


// инициализация пинов //
void pin_enable() {
  pinMode( PIN_LIGHT_SENS, INPUT );
  pinMode( PIN_SOIL_SENS, INPUT );
  pinMode( PIN_HEAT_CTRL, OUTPUT );
  pinMode( PIN_PUMP_CTRL, OUTPUT );
  pinMode( PIN_LIGHT_CTRL, OUTPUT );
  pinMode( PIN_FAN_CTRL, OUTPUT );
}





Climate plant;

Soil_hygrometer soil_hygrometer( PIN_SOIL_SENS );
Lighsensor_ lighsensor( PIN_LIGHT_SENS );
Heater heater( PIN_HEAT_CTRL );
Fan fan( PIN_FAN_CTRL );
Pump pump( PIN_PUMP_CTRL );
Light light( PIN_LIGHT_CTRL );
Terrarium_stat terrarium_stat;
Datatime data;
Termometer termometr( dht );


void pump_control( Pump& pump, Climate& climate, Heater& heater ) {
  if ( pump.on_floating == 0 && heater.on_temperature == 0 ) {
    if ( terrarium_stat.soil_hum < climate.min_soil_hum && terrarium_stat.last_pump_enable + 20 * 60 < data.get_seconds() ) {
      pump.on_floating = 1;
      terrarium_stat.last_pump_enable = data.get_seconds();
    }
  }
  if ( pump.on_floating == 1 && data.get_seconds() - terrarium_stat.last_pump_enable > 15 ) {
    pump.on_floating = 0;
  }
}


void heat_control( Pump& pump, Climate& climate, Heater& heater, Fan& fan ) {
  if ( terrarium_stat.temp < climate.min_temp && pump.on_floating == 0 ) {
    fan.on_blowing = 1;
    heater.on_temperature = 1;
  }
  if ( terrarium_stat.temp > climate.min_temp + 3 ) {
    if ( terrarium_stat.temp > climate.max_temp ) fan.on_blowing = 1;
    else fan.on_blowing = 0;
    heater.on_temperature = 0;
  }
}


void fan_control( Climate& climate, Fan& fan ) {
  if ( data.get_hour() > 8 && data.get_hour() < 21 ) fan.on_blowing = 1;
  else fan.on_blowing = 0;
  if ( terrarium_stat.air_hum > climate.max_air_hum ) fan.on_blowing = 1;
}


void light_control( Light& light, Lighsensor_& lighsensor_ ) {
  if ( lighsensor_.get_light() == 1 )light.on_ligt = 1;
  else light.on_ligt = 0;
}


void setup() {
  set_climate_plant( plant );
  Serial.begin( 9600 );
  pin_enable();
  terrarium_stat.last_pump_enable = 0;
}

void loop() {
  if ( data.get_seconds() % 60 == 0 ) {
    terrarium_stat.temp = termometr.get_temp();
    terrarium_stat.air_hum = termometr.get_hum();
  }
  terrarium_stat.soil_hum = soil_hygrometer.get_hum();
  terrarium_stat.light_sens = lighsensor.get_light();

  pump_control( pump, plant, heater );
  heat_control( pump, plant, heater, fan );
  fan_control( plant, fan );
  light_control( light, lighsensor );


  pump.change( pump.on_floating );
  fan.change( fan.on_blowing );
  light.change( light.on_ligt );

  Serial.print( "temperature: " );
  Serial.print( terrarium_stat.temp );
  Serial.print( " Humidity: " );
  Serial.print( terrarium_stat.air_hum );
  Serial.print( " Soil Humidity: " );
  Serial.print( terrarium_stat.soil_hum );
  Serial.print( " Light status: " );
  Serial.print( terrarium_stat.light_sens );
  Serial.print( " Pump status: " );
  Serial.print( pump.on_floating );
  Serial.print( " Heat status: " );
  Serial.print( heater.on_temperature );
  Serial.print( " Fan status: " );
  Serial.print( fan.on_blowing );
  Serial.print( " Time seconds: " );
  Serial.println( data.get_seconds() );
}