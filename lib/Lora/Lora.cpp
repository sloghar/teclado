#include "Arduino.h"
#include "Lora.h"
#include "Database.h"

Lora::Lora(Database *database){
    _database = database;
    Serial2.begin(9600, SERIAL_8N1, 16, 17);
}

bool Lora::setearDireccion(uint16_t direccion){
    _direccion = direccion;
    return true;
}

bool Lora::available(uint8_t *codigoUsado){
    uint8_t len = 0;

    while(Serial2.available() > len){
        len = Serial2.available();
        delayMicroseconds(1562);   
    }   

    if(len == 0) return false; 

    uint8_t* buffer = (uint8_t*) malloc(len); 

    for(uint8_t i=0; i< len; i++){buffer[i] = Serial2.read();} 


    if(buffer[0] != TIPO_CT) return false; 

    uint16_t crcTrama = ((buffer[9+buffer[8]] & 0x00ff)<<8) | (buffer[9+buffer[8]+1] & 0x00ff);
    uint16_t crc = _calcularCRC(buffer, 9+buffer[8]);

    if(!(crcTrama == crc)) return false;

    uint8_t gatewayDireccionHigh = (DIRECCION_GATEWAY & 0xff00) >> 8;
    uint8_t gatewayDireccionLow = (DIRECCION_GATEWAY & 0x00ff);

    uint8_t dispositivoDireccionHigh= (_direccion & 0xff00) >> 8;
    uint8_t dispositivoDireccionLow= (_direccion & 0x00ff);

    if(!(buffer[1] == dispositivoDireccionHigh)) return false; 
    if(!(buffer[2] == dispositivoDireccionLow)) return false;

    if(!(buffer[3] == gatewayDireccionHigh)) return false;
    if(!(buffer[4] == gatewayDireccionLow)) return false;

    if(buffer[7] == OK_LORA){
        for(uint8_t i = 0; i < LENGHT_DE_LO_CODIGOS; i++){
            if(buffer[9 + i] != codigoUsado[i]){
                return false;
            }
        }
        return true;
    }

    return false;
}

void Lora::revisarModuloLora(){
    uint8_t len = 0;

    while(Serial2.available() > len){
        len = Serial2.available();
        delayMicroseconds(1562);   
    }   

    if(len == 0) return; 

    uint8_t* buffer = (uint8_t*) malloc(len); 

    for(uint8_t i=0; i< len; i++){buffer[i] = Serial2.read();} 


    if(buffer[0] != TIPO_CT) return; 

    uint16_t crcTrama = ((buffer[9+buffer[8]] & 0x00ff)<<8) | (buffer[9+buffer[8]+1] & 0x00ff);
    uint16_t crc = _calcularCRC(buffer, 9+buffer[8]);

    if(!(crcTrama == crc)) return;

    uint8_t gatewayDireccionHigh = (DIRECCION_GATEWAY & 0xff00) >> 8;
    uint8_t gatewayDireccionLow = (DIRECCION_GATEWAY & 0x00ff);

    uint8_t dispositivoDireccionHigh= (_direccion & 0xff00) >> 8;
    uint8_t dispositivoDireccionLow= (_direccion & 0x00ff);

    if(!(buffer[1] == dispositivoDireccionHigh|| buffer[1] == BROADCAST_CT)) return; 
    if(!(buffer[2] == dispositivoDireccionLow || buffer[2]== BROADCAST_CT)) return;

    if(!(buffer[3] == gatewayDireccionHigh)) return;
    if(!(buffer[4] == gatewayDireccionLow)) return;

    switch(buffer[7]){

        case AGREGAR_CODIGO: {
            uint8_t codigo[LENGHT_DE_LO_CODIGOS] = {buffer[9], buffer[10], buffer[11], buffer[12], buffer[13], buffer[14]};

            Serial.print(F("[debug] codigo a guardar : "));
            for (uint8_t i = 0; i < LENGHT_DE_LO_CODIGOS; i++){
                Serial.print(codigo[i]);
                Serial.print(F(" "));
            }
            Serial.println();
            
            uint8_t* aux = (uint8_t*) malloc(1); 
            aux[0] = AGREGAR_CODIGO;

            if(_database->guardarCodigo(codigo)){
                enviarTrama(TIPO_GATEWAY, DIRECCION_GATEWAY, 0, OK_LORA, 1, aux);
            } else {
                enviarTrama(TIPO_GATEWAY, DIRECCION_GATEWAY, 0, ERROR, 1, aux);
            }

            free(aux);
            break;
        }

        case BORRAR_TODO: {
            _database->borrarTodo();

            uint8_t* aux = (uint8_t*) malloc(1); 
            aux[0] = BORRAR_TODO;

            enviarTrama(TIPO_GATEWAY, DIRECCION_GATEWAY, 0, OK_LORA, 1, aux);

            free(aux);
            break;
        }

        case ESTADO_DE_DISPOSITIVO:{
            uint8_t* aux = (uint8_t*) malloc(1); 
            aux[0] = ESTADO_DE_DISPOSITIVO;
            enviarTrama(TIPO_GATEWAY, DIRECCION_GATEWAY, 0, OK_LORA, 1, aux);
            break;
        }

        /*case CAMBIAR_SEGUNDOS_DE_APERTURA: {
            uint8_t segundos = buffer[9];

            uint8_t* aux = (uint8_t*) malloc(1); 
            aux[0] = CAMBIAR_SEGUNDOS_DE_APERTURA;

            if(_database->cambiarSegundosDeAperturaDelRele(segundos)){
                enviarTrama(TIPO_GATEWAY, DIRECCION_GATEWAY, 0, OK, 1, aux);
            } else {
                enviarTrama(TIPO_GATEWAY, DIRECCION_GATEWAY, 0, ERROR, 1, aux);
            }

            free(aux);
            break;
        }*/
    }

    free(buffer);
}

bool Lora::enviarTrama(uint8_t tipo, uint16_t destino, uint16_t memoria, uint8_t comando, uint8_t len, uint8_t *data){
    uint8_t *aux = (uint8_t*) malloc(11 + len);

    aux[0] = tipo;
    aux[1] = ((destino & 0xFF00) >> 8);
    aux[2] = destino;
    aux[3] = ((_direccion & 0xFF00) >> 8);
    aux[4] = _direccion;
    aux[5] = ((memoria & 0xFF00) >> 8);
    aux[6] = memoria;
    aux[7] = comando;
    aux[8] = len;
    for (uint8_t i = 0; i < len; i++){
        aux[i + 9] = data[i];
    }

    uint16_t crc = _calcularCRC(aux, 9 + len);

    aux[9 + len] = ((crc & 0xFF00) >> 8);
    aux[10 + len] = crc;

    for (uint8_t i = 0; i < (11 + len); i++){
        Serial2.write(aux[i]);
    }

    Serial.println(F("[debug] : data de salida"));
    Serial.print(F("    [tipo destino] : "));
    Serial.println(tipo);
    Serial.print(F("    [receptor] : "));
    Serial.println(destino);
    Serial.print(F("    [emisor] : "));
    Serial.println(_direccion);
    Serial.print(F("    [memoria] : "));
    Serial.println(memoria);
    Serial.print(F("    [comando] : "));
    Serial.println(comando);
    Serial.print(F("    [length] : "));
    Serial.println(len);
    Serial.print(F("    [data] : "));
    for (uint8_t i = 0; i < len; i++){
        Serial.print(data[i]);
        Serial.print(F("-"));
    }
    Serial.print(F("\n    [crc] : "));
    Serial.println(crc);

    free(aux);

    return true;
}

bool Lora::enviarCodigoUsado(uint8_t *codigoUsado,uint32_t timestamp){

    uint8_t contador = 0;
    uint8_t *aux = (uint8_t*) malloc(LENGHT_DE_LO_CODIGOS + 4);
    aux[0] = codigoUsado[0];
    aux[1] = codigoUsado[1];
    aux[2] = codigoUsado[2];
    aux[3] = codigoUsado[3];
    aux[4] = codigoUsado[4];
    aux[5] = codigoUsado[5];
    aux[6] = timestamp >> 24;
    aux[7] = timestamp >> 16;
    aux[8] = timestamp >> 8;
    aux[9] = timestamp;

    do
    {
        enviarTrama(TIPO_GATEWAY, DIRECCION_GATEWAY, 0, REPORTAR_CODIGO_USADO, LENGHT_DE_LO_CODIGOS + 4, aux);
        uint32_t tiempo = millis();
        
        while((millis() - tiempo) < TIEMPO_ENTRE_PETICIONES_LORA){

            if(available(codigoUsado)){

                free(aux);
                return true;
            }
        }

        contador++;
    } while (contador < MAXIMO_NUMERO_DE_REPETICIONES_PARA_UNA_PETICION);

    free(aux);
    return false;
}

uint16_t Lora::_calcularCRC(uint8_t* trama, uint8_t len){

    uint16_t temp;
    uint8_t flag, temp2;

    temp = 0xFFFF;
    for (uint8_t i = 0; i < len; i++)
    {
        temp = temp ^ trama[i];
        for (uint8_t j = 1; j <= 8; j++)
        {
            flag = temp & 0x0001;
            temp >>=1;

            if (flag) temp ^= 0xA001;
        }
    }
    // Orden de byte inverso.
    temp2 = ((temp & 0xff00) >> 8);
    temp = ((temp & 0x00ff ) << 8) | temp2;
    temp &= 0xFFFF;
    // el valor devuelto ya está intercambiado
    // El byte crcLo es el primero y el byte crcHi es el último
    return temp;
}