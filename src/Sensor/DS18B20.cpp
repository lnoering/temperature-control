#include "DS18B20.h"

DS18B20::DS18B20(uint8_t pin)
{
    _ds = new OneWire(pin);
    _pin = pin;
}

DS18B20::~DS18B20() 
{
    delete _ds;
}

float DS18B20::getTemperature()
{
    return _celsius;
}

void DS18B20::read()
{
     
    if ( !_ds->search(addr)) {
        _ds->reset_search();
        // delay(250);
        return;
    }
    byte    i,
            type_s,
            data[12];


    // the first ROM byte indicates which chip
    switch (addr[0]) {
        case 0x10:
            type_s = 1;
        break;
        case 0x28:
            type_s = 0;
        break;
        case 0x22:
            type_s = 0;
        break;
        default:
            return;
    } 

    _ds->reset();
    _ds->select(addr);
    _ds->write(0x44, 1);        // start conversion, with parasite power on at the end
  
    //delay(1000);     // maybe 750ms is enough, maybe not
    // we might do a ds.depower() here, but the reset will take care of it.
  
    present = _ds->reset();
    _ds->select(addr);    
    _ds->write(0xBE);         // Read Scratchpad

    for ( i = 0; i < 9; i++) {           // we need 9 bytes
        data[i] = _ds->read();
    }

    // Convert the data to actual temperature
    // because the result is a 16 bit signed integer, it should
    // be stored to an "int16_t" type, which is always 16 bits
    // even when compiled on a 32 bit processor.
    int16_t raw = (data[1] << 8) | data[0];
    if (type_s) {
        raw = raw << 3; // 9 bit resolution default
        if (data[7] == 0x10) {
            // "count remain" gives full 12 bit resolution
            raw = (raw & 0xFFF0) + 12 - data[6];
        }
    } else {
        byte cfg = (data[4] & 0x60);
        // at lower res, the low bits are undefined, so let's zero them
        if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
        else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
        else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
        //// default is 12 bit resolution, 750 ms conversion time
    }
    _celsius = (float)raw / 16.0;
    // fahrenheit = celsius * 1.8 + 32.0;
}
