#include "Temperature.h"

#include <Arduino.h>       // for delayMicroseconds, digitalPinToBitMask, etc

Temperature::Temperature(uint8_t pin, float temperature, unsigned int time)
{
    _pin = pin;
    setControl(temperature);
    _timeToControl = time;

    pinMode(_pin, OUTPUT);
}

void Temperature::control(float temperature)
{
    //offfset de tempo para não ficar acionando o relé a todo momento.
    if (_timeCount > 0) {
        _timeCount --;
        return;
    }

    int out = digitalRead(_pin);

    if (temperature >= _temperature) {
        digitalWrite(_pin, HIGH);
    } else {
        digitalWrite(_pin, LOW);
    }

    if (out != digitalRead(_pin)) {
        _timeCount = _timeToControl;
    }

}

void Temperature::setControl(float temperature)
{
    _temperature = temperature;
}

void Temperature::setOffset(int time)
{
    _timeToControl = time;
}

Temperature::~Temperature() 
{

}


