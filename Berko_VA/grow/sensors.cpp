#include "sensors.h"
#include "Arduino.h"

Termometer::Termometer(DHT11& sensor) : m_sensor(sensor), m_temp(0) {
}

void Termometer::update() {
    int temp = m_sensor.readTemperature();
    if (temp != DHT11::ERROR_TIMEOUT && temp != DHT11::ERROR_CHECKSUM) {
        m_temp = temp;
    }
}

int Termometer::get_temp() {
    return m_temp;
}

Hygrometer::Hygrometer(DHT11& sensor) : m_sensor(sensor), m_hum(0) {
}

void Hygrometer::update() {
    int hum = m_sensor.readHumidity();
    if (hum != DHT11::ERROR_TIMEOUT && hum != DHT11::ERROR_CHECKSUM) {
        m_hum = hum;
    }
}

int Hygrometer::get_hum() {
    return m_hum;
}

SoilHygrometer::SoilHygrometer(unsigned int pin) : m_pin(pin), m_hum(0) {
    pinMode(m_pin, INPUT);
}

void SoilHygrometer::update() {
    m_hum = map(analogRead(m_pin), 0, 1023, 100, 0);
}

int SoilHygrometer::get_hum() {
    return m_hum;
}

LightSensor::LightSensor(const unsigned int pin) : m_pin(pin), m_light(0) {
    pinMode(m_pin, INPUT);
}

void LightSensor::update() {
    m_light = !digitalRead(m_pin);
}

bool LightSensor::get_light() {
    return m_light;
}

DayTime::DayTime() : m_millis(millis()) {
}

void DayTime::update() {
    m_millis = millis();
}

int DayTime::get_day_minute() {
    unsigned long min = m_millis / 1000 / 60;
    return min % (60 * 24);
}

unsigned long DayTime::get_millis() const {
    return m_millis;
}

bool DayTime::is_in_interval(int start, int end) {
    int min = get_day_minute();
    return (min >= start) && (min <= end);
}

WorkTime::WorkTime(const DayTime& day_time, const unsigned int work_time, const unsigned int timeout)
    : m_day_time(day_time), m_work_time(work_time), m_timeout(timeout), m_is_work_time(0), m_timeout_end(timeout) {
}

void WorkTime::update() {
    if (m_day_time.get_millis() > m_timeout_end) {
        m_is_work_time = !m_is_work_time;
        int duration = (m_is_work_time ? m_work_time : m_timeout) * 1000;
        m_timeout_end = m_day_time.get_millis() + duration;
    }
}

bool WorkTime::is_work_time() {
    return m_is_work_time;
}