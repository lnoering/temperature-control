#pragma once
#ifndef Temperature_h
#define Temperature_h

#ifdef __cplusplus

#include <Arduino.h>       // for delayMicroseconds, digitalPinToBitMask, etc

class Temperature
{
    public:
        Temperature(uint8_t pin, float temperature, unsigned int time); // constructor
        virtual ~Temperature(); // destructor

        void setControl(float temperature);
        void control(float temperature);
        void setOffset(int time);
    private:
        float           _temperature;
        unsigned int    _timeToControl;
        uint8_t         _pin;
        unsigned int    _timeCount;
};

#endif
#endif

