#include "Arduino.h"
#include "Teclado.h"

Teclado::Teclado(){
    pinMode(ROW1, OUTPUT);
    pinMode(ROW2, OUTPUT);
    pinMode(ROW3, OUTPUT);
    pinMode(ROW4, OUTPUT);

    pinMode(COLUMN1, INPUT);
    pinMode(COLUMN2, INPUT);
    pinMode(COLUMN3, INPUT);
    pinMode(COLUMN4, INPUT);

    digitalWrite(ROW1, LOW);
    digitalWrite(ROW2, LOW);
    digitalWrite(ROW3, LOW);
    digitalWrite(ROW4, LOW);
}

uint8_t Teclado::obtenerTeclaPulsada(){
    uint8_t teclaPulsada = NO_INPUT;

    if((teclaPulsada = _row1()) != NO_INPUT){
        return teclaPulsada;
    } 

    if((teclaPulsada = _row2()) != NO_INPUT){
        return teclaPulsada;
    }

    if((teclaPulsada = _row3()) != NO_INPUT){
        return teclaPulsada;
    }

    if((teclaPulsada = _row4()) != NO_INPUT){
        return teclaPulsada;
    }

    return NO_INPUT;
}

uint8_t Teclado::_row1(){
    uint8_t teclaPulsada = NO_INPUT;
    digitalWrite(ROW1, HIGH);

    if(digitalRead(COLUMN1)){
        teclaPulsada = UNO;
    } else if(digitalRead(COLUMN2)){
        teclaPulsada = DOS;
    } else if(digitalRead(COLUMN3)){
        teclaPulsada = TRES;
    } else if(digitalRead(COLUMN4)){
        teclaPulsada = A;
    }

    digitalWrite(ROW1, LOW);

    if(teclaPulsada != NO_INPUT){
        delay(DEBOUNCE);
    }
    
    return teclaPulsada;
}

uint8_t Teclado::_row2(){
    uint8_t teclaPulsada = NO_INPUT;
    digitalWrite(ROW2, HIGH);

    if(digitalRead(COLUMN1)){
        teclaPulsada = CUATRO;
    } else if(digitalRead(COLUMN2)){
        teclaPulsada = CINCO;
    } else if(digitalRead(COLUMN3)){
        teclaPulsada = SEIS;
    } else if(digitalRead(COLUMN4)){
        teclaPulsada = B;
    }

    digitalWrite(ROW2, LOW);

    if(teclaPulsada != NO_INPUT){
        delay(DEBOUNCE);
    }
    
    return teclaPulsada;
}

uint8_t Teclado::_row3(){
    uint8_t teclaPulsada = NO_INPUT;
    digitalWrite(ROW3, HIGH);

    if(digitalRead(COLUMN1)){
        teclaPulsada = SIETE;
    } else if(digitalRead(COLUMN2)){
        teclaPulsada = OCHO;
    } else if(digitalRead(COLUMN3)){
        teclaPulsada = NUEVE;
    } else if(digitalRead(COLUMN4)){
        teclaPulsada = C;
    }

    digitalWrite(ROW3, LOW);

    if(teclaPulsada != NO_INPUT){
        delay(DEBOUNCE);
    }
    
    return teclaPulsada;
}

uint8_t Teclado::_row4(){
    uint8_t teclaPulsada = NO_INPUT;
    digitalWrite(ROW4, HIGH);

    if(digitalRead(COLUMN1)){
        teclaPulsada = BORRAR;
    } else if(digitalRead(COLUMN2)){
        teclaPulsada = CERO;
    } else if(digitalRead(COLUMN3)){
        teclaPulsada = OK;
    } else if(digitalRead(COLUMN4)){
        teclaPulsada = D;
    }

    digitalWrite(ROW4, LOW);

    if(teclaPulsada != NO_INPUT){
        delay(DEBOUNCE);
    }
    
    return teclaPulsada;
}


