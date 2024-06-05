#include "Arduino.h"
#include "Buzzer.h"

Buzzer::Buzzer(uint8_t pin){
    _pin = pin;
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);

    ledcSetup(0, 4000, 8);
    ledcWrite(0, 123);
}

void Buzzer::sonidoDeTeclaPulsada(){
    ledcAttachPin(_pin, 0);
    delay(TIEMPO_PULSADO_DE_TECLA);
    ledcDetachPin(_pin);
}

void Buzzer::sonidoDeCodigoCorrecto(){
    ledcAttachPin(_pin, 0);
    delay(TIEMPO_DE_CODIGO_CORRECTO);
    ledcDetachPin(_pin);
}

void Buzzer::sonidoDeCodigoIncorrecto(){
    for (uint8_t i = 0; i < REPETIR_SONIDO_CODIGO_INCORRECTO; i++){
        ledcAttachPin(_pin, 0);
        delay(TIEMPO_DE_CODIGO_INCORRECTO);
        ledcDetachPin(_pin);
        delay(TIEMPO_DE_CODIGO_INCORRECTO);
    }
}
