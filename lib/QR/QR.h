#ifndef QR_h
#define QR_h

#define CODE_LENGTH 6

#include "Arduino.h"
#include "Database.h"

class QR{
    private:
        Database *_database;
        uint8_t _codigo[CODE_LENGTH] = {255,255,255,255,255,255};
    public:
        QR();
        bool revisarModuloQR();
        bool obtenerCodigo(uint8_t *codigo);

};


#endif