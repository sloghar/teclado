#include "Arduino.h"
#include "QR.h"


QR::QR(){
    Serial1.begin(9600, SERIAL_8N1, 4, 2);
    while(!Serial1);
}

bool QR::revisarModuloQR(){
    uint8_t len = 0;

    while(Serial1.available() > len){
        len = Serial1.available();
        delayMicroseconds(1562);   
    } 

    if(len == CODE_LENGTH)  {
        for (uint8_t i = 0; i < CODE_LENGTH; i++){
            uint8_t c = Serial1.read();

            if(c >= 48 && c <= 57){
                _codigo[i] = c - 48;
            } else if(c >= 41 && c <= 44){
                _codigo[i] = c - 29;
            } else {
                while(Serial1.available()){
                    Serial1.read();
                }
                return false;
            }
        }

        return true;
    }

    return false;
}

bool QR::obtenerCodigo(uint8_t *codigo){
    for (uint8_t i = 0; i < CODE_LENGTH; i++){
        codigo[i] = _codigo[i];
    }

    return true;
}
