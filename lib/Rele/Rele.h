#ifndef Rele_h
#define Rele_h

#include "Arduino.h"

class Rele{
    private:
        uint8_t _pin;
        uint16_t _tiempoDeEncendido;
    public:
        Rele(uint8_t pin);
        bool encender();
        bool apagar();
        bool encenderYApagar();
        bool configurarTiempoDeEncendido(uint16_t tiempo);
};



#endif