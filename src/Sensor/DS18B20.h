#pragma once
#ifndef DS18B20_h
#define DS18B20_h

#ifdef __cplusplus

#include <OneWire.h>

class DS18B20
{
    public:
        DS18B20(uint8_t pin); // constructor
        virtual ~DS18B20(); // destructor

        void read();
        float getTemperature(char format);
        void setTemperatureOffset(float offset);
    private:
        uint8_t     _pin;
        byte        present = 0,
                    
                    addr[8];
        float       _celsius;
        float       _offset;
        OneWire     *_ds;
};

#endif
#endif

