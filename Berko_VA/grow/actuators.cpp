#include "actuators.h"
#include "Arduino.h"

Actuator::Actuator(const unsigned int pin) : m_pin(pin), m_need_en(0) {
    pinMode(m_pin, OUTPUT);
}

void Actuator::request_enable() {
    m_need_en = 1;
}

void Actuator::apply() {
    digitalWrite(m_pin, m_need_en);
    m_need_en = 0;
}