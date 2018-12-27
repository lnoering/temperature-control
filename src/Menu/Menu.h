#pragma once
#ifndef Menu_h
#define Menu_h

#ifdef __cplusplus

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <Button/Button.h>

#if !defined(ARRAY_SIZE)
    #define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))
#endif
enum MENUBUTTON { Unknown, Ok, Up, Down, Left, Right };
enum MENUTYPE { Menu1, Menu2, Flag, Number };

/* CONFIGURACOES */
struct MYDATA{             // Estrutura STRUCT com as variables que armazenarao ds datos que na memoria EEPROM
    float setPoint;
    int delay;
};
union MEMORY{             // Estrutura UNION para facilitar a leitura e escrita na EEPROM da estrutura STRUCT
    MYDATA d;
    byte b[sizeof(MYDATA)];
};

class Menu
{
    public:
        Menu(LiquidCrystal &lcd, char *menuOptions[], uint8_t btnLeft, uint8_t btnRight, uint8_t btnUp, uint8_t btnDown, uint8_t enter);
        virtual ~Menu(); 
        void control(float temperature);
    
        void setup();
        void loop();
        void render(char * render, byte x, byte y);
        void openMenu();    
        void readConfiguration();
        void writeConfiguration();
        MENUBUTTON readButtons();
        void openSubMenu(byte menuID, MENUTYPE screen, float *value, float minValue, float maxValue );
        void openSubMenu(byte menuID, MENUTYPE screen, int *value, int minValue, int maxValue );
        
        float getSetPoint();
        int getOffsetRele();
    private:
        float           _temperature;
        unsigned int    _timeToControl;
        uint8_t         _pin;
        unsigned int    _timeCount;
        MENUBUTTON btnPressed;
        MEMORY memory;

        LiquidCrystal &_lcd;
        unsigned int _columnsLCD = 20;    
        unsigned int _rowsLCD = 4;    
        
        char **_txMENU;
        byte _totOptions;

        const byte _iARROW = 0;
        byte _bARROW[8] = {
            B00000, B00100, B00110, B11111,
            B00110, B00100, B00000, B00000
        };

        Button *_btnLeft;
        Button *_btnRight;
        Button *_btnUp;
        Button *_btnDown;
        Button *_btnEnter;
          
};

#endif
#endif

