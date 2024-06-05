#include "Arduino.h"
#include "Rele.h"

Rele::Rele(uint8_t pin){
    _pin = pin;
    _tiempoDeEncendido = 0;
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

bool Rele::encender(){
    digitalWrite(_pin, HIGH);
    return true;
}

bool Rele::apagar(){
    digitalWrite(_pin, LOW);
    return true;
}

bool Rele::encenderYApagar(){
    encender();
    delay(_tiempoDeEncendido);
    apagar();
    return true;
}

bool Rele::configurarTiempoDeEncendido(uint16_t tiempo){
    _tiempoDeEncendido = tiempo;
    return true;
}
