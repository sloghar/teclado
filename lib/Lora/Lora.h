#ifndef Lora_h
#define Lora_h

#define OK_LORA 255
#define ERROR 254

#define TIPO_GATEWAY 50
#define TIPO_CT 57
#define BROADCAST_CT 0
#define DIRECCION_GATEWAY 1

#define AGREGAR_CODIGO 40
#define BORRAR_TODO 41
#define REPORTAR_CODIGO_USADO 42
#define ESTADO_DE_DISPOSITIVO 80

#define TIEMPO_ENTRE_PETICIONES_LORA      1500
#define MAXIMO_NUMERO_DE_REPETICIONES_PARA_UNA_PETICION 8

#include "Arduino.h"
#include "Database.h"

class Lora{
    private:
        Database *_database;
        uint16_t _direccion;
        uint16_t _calcularCRC(uint8_t* trama, uint8_t len);
    public:
        Lora(Database *database);
        void revisarModuloLora();
        bool enviarTrama(uint8_t tipo, uint16_t destino, uint16_t memoria, uint8_t comando, uint8_t len, uint8_t *data);
        bool setearDireccion(uint16_t direccions);
        bool enviarCodigoUsado(uint8_t *codigoUsado, uint32_t timestamp);
        bool available(uint8_t *codigoUsado);
};


#endif