#include "actuators.h"
#include "Arduino.h"


Heater::Heater(const unsigned int pin) : m_pin(pin), m_need_en(0)
{
    pinMode(m_pin, OUTPUT);
}


void Heater::request_enable()
{
    m_need_en = 1;
}


void Heater::apply()
{
    digitalWrite(m_pin, m_need_en);

    m_need_en = 0;
}


Vent::Vent(const unsigned int pin) : m_pin(pin), m_need_en(0)
{
    pinMode(m_pin, OUTPUT);
}


void Vent::request_enable()
{
    m_need_en = 1;
}


void Vent::apply()
{
    digitalWrite(m_pin, m_need_en);

    m_need_en = 0;
}


Pump::Pump(const unsigned int pin) : m_pin(pin), m_need_en(0)
{
    pinMode(m_pin, OUTPUT);
}


void Pump::request_enable()
{
    m_need_en = 1;
}


void Pump::apply()
{
    digitalWrite(m_pin, m_need_en);

    m_need_en = 0;
}


Light::Light(const unsigned int pin) : m_pin(pin), m_need_en(0)
{
    pinMode(m_pin, OUTPUT);
}


void Light::request_enable()
{
    m_need_en = 1;
}


void Light::apply()
{
    digitalWrite(m_pin, m_need_en);

    m_need_en = 0;
}
