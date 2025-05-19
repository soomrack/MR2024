#pragma once

class Actuator {
private:
    const unsigned int m_pin;
    bool m_need_en;
public:
    Actuator(const unsigned int pin);
    void request_enable();
    void apply();
};