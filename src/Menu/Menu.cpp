#include "Menu.h"

#include <Arduino.h>       // for delayMicroseconds, digitalPinToBitMask, etc

const char *txtFormats[] = {"C","F"};

Menu::Menu(LiquidCrystal &lcd, char *menuOptions[], uint8_t btnLeft, uint8_t btnRight, uint8_t btnUp, uint8_t btnDown, uint8_t enter)
: _lcd(lcd)
{
    _totOptions = 6;

    _txMENU = menuOptions;

    _btnLeft = new Button(btnLeft, false, 10);
    _btnRight = new Button(btnRight, false, 10);
    _btnUp = new Button(btnUp, false, 10);
    _btnDown = new Button(btnDown, false, 10);
    _btnEnter = new Button(enter, false);


}

Menu::~Menu() 
{
    delete _btnLeft;
    delete _btnRight;
    delete _btnUp;
    delete _btnDown;
    delete _btnEnter;
}

void Menu::setup()
{
    readConfiguration();

    _lcd.createChar(_iARROW, _bARROW);

    _lcd.clear();
}

void Menu::loop()
{
    btnPressed = readButtons();

    if( btnPressed == MENUBUTTON::Ok ){
        openMenu();
    }
}

void Menu::render(char * render, byte x, byte y)
{
    _lcd.setCursor(x,y);
    _lcd.print(render);
}

void Menu::openMenu()
{
    byte idxMenu       = 0;
    boolean exitMenu   = false;
    boolean forcePrint = true;
    char format[_columnsLCD];

    _lcd.clear();

    while( !exitMenu )
    {
        btnPressed = readButtons();

        if( btnPressed == MENUBUTTON::Up )
        {
            if(idxMenu-1 >= 0) {
                idxMenu--;
            } else {
                idxMenu = _totOptions-1;
            }
        }
        
        if( btnPressed == MENUBUTTON::Down )
        {
            if (idxMenu+1 < _totOptions) {
                idxMenu++;
            } else {
                idxMenu = 0;
            }
            
        }
        else if( btnPressed == MENUBUTTON::Ok )
        {
            switch( idxMenu )
            {
                case 0:     
                        sprintf(format,"%c%c",char(223),getTemperatureFormat());
                        openSubMenu( idxMenu, MENUTYPE::Float,   &memory.d.setPoint, -20.0, 99.9, format); 
                    break;
                case 1: openSubMenu( idxMenu, MENUTYPE::Number,  &memory.d.delay, 0, 60, "s"    ); break;
                case 2: openSubMenu( idxMenu, MENUTYPE::List,  &memory.d.format,  txtFormats    ); break;
                case 3: 
                        sprintf(format,"%c%c",char(223),getTemperatureFormat());
                        openSubMenu( idxMenu, MENUTYPE::Float,   &memory.d.offsetTemp, -20.0, 99.9, format); 
                    break;
                case 4: writeConfiguration(); exitMenu = true;                                     break;
                case 5: readConfiguration();  exitMenu = true;                                     break;
            }
            forcePrint = true;
        }


        if( !exitMenu && (forcePrint || btnPressed != MENUBUTTON::Unknown) )
        {
            forcePrint = false;

            static const byte endFor1 = (_totOptions+_rowsLCD-1)/_rowsLCD;
            int graphMenu     = 0;

            for( int i=1 ; i<=endFor1 ; i++ )
            {
                if( idxMenu < i*_rowsLCD )
                {
                    graphMenu = (i-1) * _rowsLCD;
                    break;
                }
            }

            byte endFor2 = graphMenu+_rowsLCD;

            //Criar o tamanho de colunas em branco conforme a config do display.
            String print;            
            for(char y=0; y < _columnsLCD; y++)
            {
                print += ' ';
            }

            for( int i=graphMenu, j=0; i< endFor2 ; i++, j++ )
            {
                _lcd.setCursor(1, j);
                if (i<_totOptions) {
                    _lcd.print(_txMENU[i]);
                } else {
                    _lcd.print(print);
                }
            }

            for( int i=0 ; i<_rowsLCD ; i++ )
            {
                _lcd.setCursor(0, i);
                _lcd.print(" ");
            }
            _lcd.setCursor(0, idxMenu % _rowsLCD );
            _lcd.write(_iARROW);
        }
    }

    _lcd.clear();
}



void Menu::openSubMenu (byte menuID, MENUTYPE screen, int * value, int minValue, int maxValue, char * format = "")
{
    boolean exitSubMenu = false;
    boolean forcePrint  = true;

    _lcd.clear();

    while( !exitSubMenu )
    {
        btnPressed = readButtons();

        if( btnPressed == MENUBUTTON::Ok )
        {
            exitSubMenu = true;
        }
        else if( btnPressed == MENUBUTTON::Up && (*value)-1 >= minValue )
        {
            (*value)--;
        }
        else if( btnPressed == MENUBUTTON::Down && (*value)+1 <= maxValue )
        {
            (*value)++;
        }


        if( !exitSubMenu && (forcePrint || btnPressed != MENUBUTTON::Unknown) )
        {
            forcePrint = false;

            _lcd.setCursor(0,0);
            _lcd.print(_txMENU[menuID]);

            _lcd.setCursor(0,1);
            _lcd.print("<");
            _lcd.setCursor(_columnsLCD-1,1);
            _lcd.print(">");

            switch (screen) {
                case MENUTYPE::Flag :
                        _lcd.setCursor(_columnsLCD/2-1, 1);
                        _lcd.print((*value) == 0 ? "Não" : "Sim");
                    break;
                case MENUTYPE::Number :
                        _lcd.setCursor(_columnsLCD/2-1, 1);
                        _lcd.print((*value));
                        _lcd.print(format);
                        _lcd.print(" ");
                    break;
                case MENUTYPE::Float :
                        _lcd.setCursor(_columnsLCD/2-1, 1);
                        _lcd.print((*value));
                        _lcd.print(" ");
                    break;
                default:
                    break;
            }
        }

    }

    _lcd.clear();
}

void Menu::readConfiguration()
{
    for( int i=0 ; i < sizeof(memory.d) ; i++  )
        memory.b[i] = EEPROM.read(i);

    // memory.d.offsetTemp = 0.5;

    writeConfiguration();
}

void Menu::writeConfiguration()
{
    for( int i=0 ; i<sizeof(memory.d) ; i++  )
        EEPROM.write( i, memory.b[i] );
}

/**
 *
 * @param menuID    ID do menu principal para colocar o titulo no submenu
 * @param screen    Tipo que representa o submenu
 * @param value     Ponteiro para o valor na STRUCT da EEPROM
 * @param minValue  Valor minimo que pode ter a variavel.
 * @param maxValue  Valor maximo que pode ter a variavel.
 */

void Menu::openSubMenu (byte menuID, MENUTYPE screen, float *value, float minValue, float maxValue, char * format = "")
{
    boolean exitSubMenu = false;
    boolean forcePrint  = true;
    boolean isFloat = true;
    float valueAction = 0.1;

    _lcd.clear();

    while( !exitSubMenu )
    {
        btnPressed = readButtons();

        if( btnPressed == MENUBUTTON::Ok )
        {
            if(!isFloat) {
                exitSubMenu = true;
            }
            valueAction = 1;
            isFloat = false;
        }
        else if( btnPressed == MENUBUTTON::Up && (*value)-valueAction >= minValue )
        {
            (*value) -= valueAction;
        }
        else if( btnPressed == MENUBUTTON::Down && (*value)+valueAction <= maxValue )
        {
            (*value) += valueAction;
        }

        if( !exitSubMenu && (forcePrint || btnPressed != MENUBUTTON::Unknown) )
        {
            forcePrint = false;

            _lcd.setCursor(0,0);
            _lcd.print(_txMENU[menuID]);

            _lcd.setCursor(0,1);
            _lcd.print("<");
            _lcd.setCursor(_columnsLCD-1,1);
            _lcd.print(">");

            _lcd.setCursor(_columnsLCD/2-5, 1);
            
            // char display[_columnsLCD] = "";
            _lcd.print((*value));
            // sprintf(display,"%c%c",char(223),getTemperatureFormat());

            _lcd.print(format);
            _lcd.print(" ");
        }
    }
    _lcd.clear();
}

void Menu::openSubMenu (byte menuID, MENUTYPE screen, byte * value, const char * options[])
{
    boolean exitSubMenu = false;
    boolean forcePrint  = true;

    int minValue = 0;
    int maxValue = sizeof(options)-1;

    _lcd.clear();

    while( !exitSubMenu )
    {
        btnPressed = readButtons();

        if( btnPressed == MENUBUTTON::Ok )
        {
            exitSubMenu = true;
        }
        else if( btnPressed == MENUBUTTON::Up && (*value)-1 >= minValue )
        {
            (*value) --;
        }
        else if( btnPressed == MENUBUTTON::Down && (*value)+1 <= maxValue )
        {
            (*value) ++;
        }

        if( !exitSubMenu && (forcePrint || btnPressed != MENUBUTTON::Unknown) )
        {
            _lcd.setCursor(0,0);
            _lcd.print(_txMENU[menuID]);

            _lcd.setCursor(0,1);
            _lcd.print("<");
            _lcd.setCursor(_columnsLCD-1,1);
            _lcd.print(">");

            _lcd.setCursor(_columnsLCD/2-1, 1);
            _lcd.print(options[(*value)]);
            _lcd.print(" ");

            forcePrint = false;        
        }
    }
    _lcd.clear();
}

float Menu::getSetPoint()
{
    return memory.d.setPoint;
}

int Menu::getOffsetRele()
{
    return memory.d.delay;
}

char Menu::getTemperatureFormat()
{
    return *txtFormats[memory.d.format];
}

float Menu::getTemperatureOffset()
{
    return memory.d.offsetTemp;
}

MENUBUTTON Menu::readButtons()
{
    btnPressed = MENUBUTTON::Unknown;

    if (_btnLeft->check(HIGH)) {
        btnPressed = MENUBUTTON::Left;
    }

    if (_btnRight->check(HIGH)) {
        btnPressed = MENUBUTTON::Right;
    }

    if (_btnUp->check(HIGH)) {
        btnPressed = MENUBUTTON::Up;
    }

    if (_btnDown->check(HIGH)) {
        btnPressed = MENUBUTTON::Down;
    }

    if (_btnEnter->check(HIGH)) {
        btnPressed = MENUBUTTON::Ok;
    }

    return btnPressed;
}