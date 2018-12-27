#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Sensor/DS18B20.h>
#include <Control/Temperature.h>
#include <Menu/Menu.h>
#include "TimerOne.h"

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

float temperature;
unsigned int seconds = 0;

DS18B20 _ds(A0);
Temperature _temperature(A1,28,10);


char *txtMENU[] = {                                // Los textos del menu principal, la longitud maxima = columnsLCD-1, rellenar caracteres sobrantes con espacios.
    "Set Point Tempe.   ",
    "Offset Rele        ",
    "Salvar e Sair      ",
    "Sair               ",
};
Menu menu(lcd, txtMENU, 11,13,12,10,9);



void setup() {  
    lcd.begin(20, 4);

    menu.setup();

    // Configuração do timer1 
    TCCR1A = 0;                         //confira timer para operação normal pinos OC1A e OC1B desconectados
    TCCR1B = 0;                         //limpa registrador
    TCCR1B |= (1<<CS10)|(1 << CS12);    // configura prescaler para 1024: CS12 = 1 e CS10 = 1
    
    TCNT1 = 0xC2F7;                     // incia timer com valor para que estouro ocorra em 1 segundo
                                        // 65536-(16MHz/1024/1Hz) = 49911 = 0xC2F7
    
    TIMSK1 |= (1 << TOIE1);             // habilita a interrupção do TIMER1
}

void loop() {
    char display[20] = "",
         temp[8] = "";

    menu.loop();
    dtostrf(_ds.getTemperature(),4,1,temp);
    sprintf(display,"%s%s%c%c","Temperatura ",temp,char(223),'C');
    menu.render(display, 0, 0);
    _temperature.setControl(menu.getSetPoint());
    _temperature.setOffset(menu.getOffsetRele());
    dtostrf(menu.getSetPoint(),4,1,temp);
    sprintf(display,"%s%s%c%c","Set Point ",temp,char(223),'C');
    menu.render(display, 0, 1);
}

ISR(TIMER1_OVF_vect)
{
    TCNT1 = 0xC2F7;                                 // Renicia TIMER
    _ds.read();
    _temperature.control(_ds.getTemperature());
}