#pragma once
#include <DHT11.h>

class Termometer {
private:
    DHT11& m_sensor;
    int m_temp;
public:
    Termometer(DHT11& sensor);
    void update();
    int get_temp();
};

class Hygrometer {
private:
    DHT11& m_sensor;
    int m_hum;
public:
    Hygrometer(DHT11& sensor);
    void update();
    int get_hum();
};

class SoilHygrometer {
private:
    const unsigned int m_pin;
    int m_hum;
public:
    SoilHygrometer(const unsigned int pin);
    void update();
    int get_hum();
};

class LightSensor {
private:
    const unsigned int m_pin;
    bool m_light;
public:
    LightSensor(const unsigned int pin);
    void update();
    bool get_light();
};

class DayTime {
private:
    unsigned long m_millis;
public:
    DayTime();
    void update();
    int get_day_minute();
    unsigned long get_millis() const;
    bool is_in_interval(int start, int end);
};

class WorkTime {
private:
    const DayTime& m_day_time;
    const unsigned int m_work_time;
    const unsigned int m_timeout;
    bool m_is_work_time;
    unsigned long m_timeout_end;
public:
    WorkTime(const DayTime& day_time, const unsigned int work_time, const unsigned int timeout);
    void update();
    bool is_work_time();
};