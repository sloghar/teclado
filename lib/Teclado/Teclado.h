#ifndef Teclado_h
#define Teclado_h

#define DEBOUNCE 250

#define ROW1    12
#define ROW2    14
#define ROW3    27
#define ROW4    26

#define COLUMN1 33
#define COLUMN2 32
#define COLUMN3 35
#define COLUMN4 34

#define CERO 0
#define UNO 1
#define DOS 2
#define TRES 3
#define CUATRO 4
#define CINCO 5
#define SEIS 6
#define SIETE 7
#define OCHO 8
#define NUEVE 9
#define ASTERISCO 10
#define NUMERAL 11
#define A 12
#define B 13
#define C 14
#define D 15
#define OK 16
#define BORRAR 17
#define NO_INPUT 255

#include "Arduino.h"

class Teclado{
    private:
        uint8_t _row1();
        uint8_t _row2();
        uint8_t _row3();
        uint8_t _row4();
    public:
        Teclado(/* args */);
        uint8_t obtenerTeclaPulsada();
};


#endif