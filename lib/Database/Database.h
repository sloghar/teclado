#ifndef Database_h
#define Database_h

#define CANTIDAD_DE_ARCHIVOS_CODE 30
#define MAXIMA_CANTIDAD_DE_CODIGOS_POR_ARCHIVO 128
#define LENGHT_DE_LO_CODIGOS 6

#include "Arduino.h"
#include "ArduinoJson.h"

class Database{
    private:
        uint16_t _direccion;
    public:
        Database(/* args */);
        bool guardarCodigo(uint8_t *arr);
        bool borrarCodigo(uint8_t archivo, uint8_t indice);
        bool compararCodigo(uint8_t *arr);
        bool borrarTodo();

        //Manejo de la dirección
        bool estaSeteadaLaDireccion();
        uint16_t obtenerDireccion();
        bool setearDireccion(uint16_t direccion);
        void manejadorDeDireccion();
        bool introducirContrasena();
        bool cambioDeDireccion();

        //Funciones para manejar los codigos usados
        bool revisarColaDeCodigosUsados(uint8_t *codigo, uint32_t *timestamp);
        bool guardarCodigoUsado(uint32_t timestamp, uint8_t *codigo);
        bool borrarCodigoUsado();

};



#endif