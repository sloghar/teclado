#ifndef Buzzer_h
#define Buzzer_h

#define TIEMPO_PULSADO_DE_TECLA 150
#define TIEMPO_DE_CODIGO_CORRECTO 2500
#define TIEMPO_DE_CODIGO_INCORRECTO 350
#define REPETIR_SONIDO_CODIGO_INCORRECTO 3

#include "Arduino.h"

class Buzzer{
    private:
        uint8_t _pin;
    public:
        Buzzer(uint8_t pin);
        void sonidoDeTeclaPulsada();
        void sonidoDeCodigoCorrecto();
        void sonidoDeCodigoIncorrecto();
};


#endif