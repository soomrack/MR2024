#include <TroykaDHT.h>

#define LUMINOSITY_SENSOR_PIN 0
#define SOIL_HUMIDITRY_SENSOR_PIN 1
#define DHT_SENSOR_PIN 8
#define LED_PIN 6
#define WATER_PUMP_PIN 5
#define HEAT_PIN 4
#define FAN_PIN 7

float MIN_TEMP = 25.0;
float MAX_TEMP = 30.0;

float TARGET_AIR_HUMIDITY = 70;

const int NIGHT_TIME_START = 2;
const int NIGHT_TIME_END = 1;

DHT dht_sensor(DHT_SENSOR_PIN, DHT11);

class State {
  private :
    float temp;
    int soil_humidity;
    float air_humidity;
    int luminosity;
  public :
    State();
    void update();
    float get_temp();
    int get_soil_humidity();
    float get_air_humidity();
    int get_luminosity();
};

State::State()
{
  temp = 0;
  soil_humidity = 0;
  air_humidity = 0;
  luminosity = 0;
}

void State::update()
{
  dht_sensor.read();
  this->soil_humidity = analogRead(SOIL_HUMIDITRY_SENSOR_PIN);
  this->air_humidity = dht_sensor.getHumidity();
  this->temp = dht_sensor.getTemperatureC();
  this->luminosity = analogRead(LUMINOSITY_SENSOR_PIN);
}

float State::get_temp()
{
  return this->temp;
}

int State::get_soil_humidity()
{
  return this->soil_humidity;
}

float State::get_air_humidity()
{
  return this->air_humidity;
}

int State::get_luminosity()
{
  return this->luminosity;
}


State state;


void setup()
{
  Serial.begin(9600);
  dht_sensor.begin();

  pinMode(LUMINOSITY_SENSOR_PIN, INPUT);
  pinMode(SOIL_HUMIDITRY_SENSOR_PIN, INPUT);
  pinMode(DHT_SENSOR_PIN, INPUT);

  pinMode(LED_PIN, OUTPUT);
  pinMode(HEAT_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(WATER_PUMP_PIN, OUTPUT);

  digitalWrite(HEAT_PIN, LOW);
  digitalWrite(FAN_PIN, LOW);
  digitalWrite(WATER_PUMP_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
}


unsigned long time() {
  return millis() / 60000;
}


bool is_night()
{
  Serial.println(time() % 3);
  return (time() % 3 == 2);
}


void adjust_air_humidity_and_temperature() {
  switch(dht_sensor.getState()) {
    case DHT_OK:
      if (state.get_air_humidity() <= TARGET_AIR_HUMIDITY) {
        if(state.get_temp() <= MIN_TEMP){
          digitalWrite(FAN_PIN, HIGH);
          digitalWrite(HEAT_PIN, HIGH);        
        } else if (state.get_temp() >= MAX_TEMP){
          digitalWrite(FAN_PIN, HIGH);
        }
      } else {
        if(state.get_temp() <= MIN_TEMP){
          digitalWrite(FAN_PIN, HIGH);
          digitalWrite(HEAT_PIN, HIGH);        
        } else {
          digitalWrite(FAN_PIN, HIGH);
        }
      }
      break;
    case DHT_ERROR_CHECKSUM:
      Serial.println("Checksum error");
      break;
    case DHT_ERROR_TIMEOUT:
      Serial.println("Timeout error");
      break;
    case DHT_ERROR_NO_REPLY:
      Serial.println("Connection error");
      break;
    default:
        Serial.println("Unknown error");
        break;
  }
}


void adjust_soil_humidity()
{
  if (state.get_soil_humidity() > 350) {
    digitalWrite(WATER_PUMP_PIN, HIGH);
  } else {
    digitalWrite(WATER_PUMP_PIN, LOW);
  }
}


void adjust_light()
{
  if ( is_night() ) {
    digitalWrite(LED_PIN, LOW);
    return;
  }

  if ( state.get_luminosity() < 500 ) {
    digitalWrite(LED_PIN, LOW);
  } else {
    digitalWrite(LED_PIN, HIGH);
  }
}


void loop()
{
  state.update();
  adjust_soil_humidity();
  adjust_air_humidity_and_temperature();
  adjust_light();
}
