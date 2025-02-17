#include <DFRobot_DHT11.h>
#include <Wire.h>
#include <RTClib.h>

// Класс для хранения конфигурации
class climate_config {
public:
    int light_threshold = 70;
    int soil_humidity_threshold = 40;
    int air_humidity_threshold = 70;
    int temp_high_threshold = 25;
    int temp_low_threshold = 20;
    int time_sunrise = 7;
    int time_sunset = 20;
    int ventilation_timer = 3;
    int pump_on_seconds = 2;
    int pump_off_seconds = 3;
};

// Базовый класс для устройств
class device {
public:
    device(uint8_t pin) : pin_(pin) {
        pinMode(pin_, OUTPUT);
    }

    void set_state(bool new_state) {
        state_ = new_state;
        digitalWrite(pin_, state_ ? HIGH : LOW);
    }

    bool get_state() const { return state_; }

protected:
    uint8_t pin_;
    bool state_ = false;
};

// Класс для управления вентилятором
class fan : public device {
public:
    fan(uint8_t pin) : device(pin) {}

    void update_triggers(bool humidity, bool temp, bool timer) {
        humidity_trigger_ = humidity;
        temp_trigger_ = temp;
        timer_trigger_ = timer;
        set_state(humidity_trigger_  temp_trigger_  timer_trigger_);
    }

private:
    bool humidity_trigger_ = false;
    bool temp_trigger_ = false;
    bool timer_trigger_ = false;
};

// Класс для сенсорных данных
class sensor_data {
public:
    int air_humidity = 0;
    int air_temperature = 0;
    int light_level = 0;
    int soil_humidity = 0;
    int current_time = 0;
};

// Интерфейс наблюдателя
class observer {
public:
    virtual void update(const String& message) = 0;
};

// Конкретный наблюдатель для вывода в Serial
class serial_observer : public observer {
public:
    void update(const String& message) override {
        Serial.println("NOTIFICATION: " + message);
    }
};

// Главный контроллер теплицы
class greenhouse_controller {
public:
    greenhouse_controller(uint8_t fan_pin, uint8_t light_pin,
                         uint8_t pump_pin, uint8_t heater_pin,
                         uint8_t dht_pin)
        : fan_(fan_pin), light_(light_pin), pump_(pump_pin),
          heater_(heater_pin), dht_pin_(dht_pin) {}

    void initialize() {
        pinMode(dht_pin_, INPUT);
        if (!rtc_.begin()) {
            notify("RTC initialization failed!");
        }
        dht_.read(dht_pin_);
        attach_observer(&serial_observer_);
    }

    void attach_observer(observer* obs) {
        observer_ = obs;
    }

    void update_sensors() {
        unsigned long current_time = millis();
        if (current_time - last_sensor_update_ >= 500) {
            update_dht();
            update_light();
            update_soil();
            update_time();
            last_sensor_update_ = current_time;
        }
    }

    void control_systems() {
        control_light();
        control_temperature();
        control_ventilation();
        control_watering();
    }

private:
    void update_dht() {
        dht_.read(dht_pin_);
        
        // Проверка на ошибки через значения датчика
        if(dht_.humidity == 0 || dht_.temperature == 0) {
            notify("DHT sensor error!");
            return;
        }
        
        data_.air_humidity = dht_.humidity;
        data_.air_temperature = dht_.temperature;
    }

    void update_light() {
        int light_level_1 = map(analogRead(A0), 980, 38, 0, 100);
        int light_level_2 = map(analogRead(A0), 980, 38, 0, 100);
        int light_level_3 = map(analogRead(A0), 980, 38, 0, 100);

        if (abs(light_level_1 - light_level_2) > 10 && 
            abs(light_level_1 - light_level_3) > 10) {
            notify("Light sensor 1 error!");
            data_.light_level = (light_level_2 + light_level_3) / 2;
            return;
        }

        data_.light_level = (light_level_1 + light_level_2 + light_level_3) / 3;
    }

    void update_soil() {
        data_.soil_humidity = map(analogRead(A1), 1023, 0, 0, 100);
    }
    void update_time() {
        DateTime now = rtc_.now();
        if (!now.isValid()) {
            notify("RTC read error!");
            return;
        }
        data_.current_time = now.hour();
    }

    void control_light() {
        bool time_condition = (data_.current_time > config_.time_sunrise) && 
                             (data_.current_time < config_.time_sunset);
        bool light_condition = data_.light_level < config_.light_threshold;
        light_.set_state(time_condition && light_condition);
    }

    void control_temperature() {
        if (data_.air_temperature < config_.temp_low_threshold) {
            heater_.set_state(true);
            fan_.update_triggers(false, true, false);
        } else if (data_.air_temperature > config_.temp_high_threshold) {
            heater_.set_state(false);
            fan_.update_triggers(false, true, false);
        } else {
            heater_.set_state(false);
            fan_.update_triggers(false, false, false);
        }
    }

    void control_ventilation() {
        bool timer_condition = data_.current_time % config_.ventilation_timer == 0;
        bool humidity_condition = data_.air_humidity > config_.air_humidith_threshold;
        fan_.update_triggers(humidity_condition, false, timer_condition);
    }

    void control_watering() {
        bool humidity_condition = data_.soil_humidity < config_.soil_humidity_threshold;
        bool timer_condition = check_pump_timer();
        pump_.set_state(humidity_condition && timer_condition);
    }

    bool check_pump_timer() {
        unsigned long current_time = millis();
        if (current_time - pump_start_time_ < config_.pump_on_seconds * 1000) {
            return true;
        } else if (current_time - pump_start_time_ < 
                  (config_.pump_on_seconds + config_.pump_off_seconds) * 1000) {
            return false;
        } else {
            pump_start_time_ = current_time;
            return true;
        }
    }

    void notify(const String& message) {
        if (observer_) {
            observer_->update(message);
        }
    }

    climate_config config_;
    sensor_data data_;
    DFRobot_DHT11 dht_;
    RTC_DS3231 rtc_;
    const uint8_t dht_pin_;
    
    fan fan_;
    device light_;
    device pump_;
    device heater_;
    
    unsigned long last_sensor_update_ = 0;
    unsigned long pump_start_time_ = 0;
    
    observer* observer_ = nullptr;
    serial_observer serial_observer_;
};

// Использование в программе
greenhouse_controller greenhouse(7, 6, 5, 4, 13);

void setup() {
    Serial.begin(9600);
    greenhouse.initialize();
}

void loop() {
    greenhouse.update_sensors();
    greenhouse.control_systems();
    delay(1);
}