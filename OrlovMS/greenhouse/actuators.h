#pragma once


class Heater
{
private:
    const unsigned int m_pin;
    bool m_need_en;
public:
    Heater(const unsigned int pin);
    void request_enable();
    void apply();
};


class Vent
{
private:
    const unsigned int m_pin;
    bool m_need_en;
public:
    Vent(const unsigned int pin);
    void request_enable();
    void apply();
};


class Pump
{
private:
    const unsigned int m_pin;
    bool m_need_en;
public:
    Pump(const unsigned int pin);
    void request_enable();
    void apply();
};


class Light
{
private:
    const unsigned int m_pin;
    bool m_need_en;
public:
    Light(const unsigned int pin);
    void request_enable();
    void apply();
};
