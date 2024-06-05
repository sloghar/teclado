#include "Arduino.h"
#include "Database.h"
#include "ArduinoJson.h"
#include "SPIFFS.h"


Database::Database(){
    if(!SPIFFS.begin()){
        while (true){
            Serial.println(F("[error] : error con la SPIFFS"));
            delay(500);
        }
    }
}

bool Database::revisarColaDeCodigosUsados(uint8_t *codigo, uint32_t *timestamp){
    File archivo = SPIFFS.open("/cola.json", "r");
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, archivo);

    archivo.close();
    if (error) {
        Serial.println("[error] : error al deserializar cola.json en revisarColaDeCodigosUsados() - Database.cpp");
        Serial.println(error.c_str());
        return false;
    }

    if(doc.size() == 0){
        return false;
    }

    JsonArray codigoUsado = doc[0]["code"];
    for(uint8_t i = 0; i < LENGHT_DE_LO_CODIGOS; i++){
        codigo[i] = codigoUsado[i];
    }

    *timestamp = doc[0]["ti"];

    return true;
}

bool Database::guardarCodigoUsado(uint32_t timestamp, uint8_t *arr){

    File archivo = SPIFFS.open("/cola.json", "r");
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, archivo);
    archivo.close();

    if (error) {
        Serial.println("[error] :  error al deserializar cola.json en guardarCodigoUsado() - Database.cpp");
        Serial.println(error.c_str());
        return false;
    }

    JsonDocument doc1;
    doc1["ti"] = timestamp;
    JsonArray code = doc1["code"].to<JsonArray>();
        
    for (uint8_t j = 0; j < LENGHT_DE_LO_CODIGOS; j++){
        code.add(arr[j]);
    }
        
    SPIFFS.remove("/cola.json");
    File nuevoArchivo = SPIFFS.open("/cola.json", "w");
    doc.add(doc1);
    serializeJson(doc, nuevoArchivo);
    nuevoArchivo.close();

    return true;
}

bool Database::borrarCodigoUsado(){
    File archivo = SPIFFS.open("/cola.json", "r");
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, archivo);

    archivo.close();
    if (error) {
        Serial.print("[error] : error al deserializar cola.json en borrarCodigoUsado() - Database.cpp");
        Serial.println(error.c_str());
        return false;
    }

    JsonArray arrayDeCodigosUsados = doc.as<JsonArray>();
    arrayDeCodigosUsados.remove(0);

    SPIFFS.remove("/cola.json");
    File archivoNuevo = SPIFFS.open("/cola.json", "w");
    serializeJson(doc, archivoNuevo);
    archivoNuevo.close();

    return true;
}

bool Database::guardarCodigo(uint8_t *arr){
    for(uint8_t i = 0; i < CANTIDAD_DE_ARCHIVOS_CODE; i++){

        File codigos = SPIFFS.open("/codes" + String(i) + ".json", "r");
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, codigos);
        codigos.close();

        if (error) {
            Serial.println("[error] :  error al deserializar codes" + String(i) + ".json en guardarCodigo() - Database.cpp");
            Serial.println(error.c_str());
            return false;
        }

        if(!(doc.size() < MAXIMA_CANTIDAD_DE_CODIGOS_POR_ARCHIVO)){
            continue;
        }

        JsonDocument doc1;
        JsonArray code = doc1["code"].to<JsonArray>();
        
        for (uint8_t j = 0; j < LENGHT_DE_LO_CODIGOS; j++){
            code.add(arr[j]);
        }
        
        SPIFFS.remove("/codes" + String(i) + ".json");
        File codigosNuevos = SPIFFS.open("/codes" + String(i) + ".json", "w");
        doc.add(doc1);
        serializeJson(doc, codigosNuevos);
        codigosNuevos.close();

        return true;
    }

    return false;
}

bool Database::borrarCodigo(uint8_t archivo, uint8_t indice){

    File codigos = SPIFFS.open("/codes" + String(archivo) + ".json", "r");
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, codigos);
    codigos.close();

    if (error) {
        Serial.println("[error] :  error al deserializar codes" + String(archivo) + ".json en borrarCodigo() - Database.cpp");
        Serial.println(error.c_str());
        return false;
    }

    JsonArray arrayDeCodigos = doc.as<JsonArray>();
    
    arrayDeCodigos.remove(indice);
    SPIFFS.remove("/codes" + String(archivo) + ".json");
    File codigosNuevos = SPIFFS.open("/codes" + String(archivo) + ".json", "w");
    serializeJson(doc, codigosNuevos);
    codigosNuevos.close();

    return true;
}

bool Database::compararCodigo(uint8_t *arr){
     for(uint8_t i = 0; i < CANTIDAD_DE_ARCHIVOS_CODE; i++){
        File codigos = SPIFFS.open("/codes" + String(i) + ".json", "r");
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, codigos);
        codigos.close();

        if (error) {
            Serial.println("[error] :  error al deserializar codes" + String(i) + ".json en compararCodigo() - Database.cpp");
            Serial.println(error.c_str());
            return false;
        }

        uint8_t indice = 0;
        JsonArray arrayDeCodigos = doc.as<JsonArray>();
        for (JsonObject item : arrayDeCodigos) {

            JsonArray code = item["code"];
            bool codigosIguales = true;

            for(uint8_t j = 0; j < LENGHT_DE_LO_CODIGOS; j++){
                uint8_t numero = code[j];
                if(numero != arr[j]){
                    codigosIguales = false;
                    break;
                }
            }

            if(!codigosIguales){
                indice++;
                continue;
            }

            if(borrarCodigo(i, indice)){
                return true;
            } 
            
            return false;
        }
    }

    return false;
}

bool Database::borrarTodo(){
    for(uint8_t i = 0; i < CANTIDAD_DE_ARCHIVOS_CODE; i++){
        SPIFFS.remove("/codes" + String(i) + ".json");
        File nuevoArchivo = SPIFFS.open("/codes" + String(i) + ".json", "w");

        JsonDocument doc;
        doc.to<JsonArray>();
        serializeJson(doc, nuevoArchivo);
        nuevoArchivo.close();
    }

    return true;  
}

bool Database::estaSeteadaLaDireccion(){
    File direccion = SPIFFS.open("/direccion.json", "r");

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, direccion);

    direccion.close();
    if (error) {
        Serial.print("[error] : error al deserializar el archivo direccion.json - Database.cpp");
        Serial.println(error.c_str());
        return false;
    }

    uint8_t set = doc["set"];

    if(!set){
        return false;
    }

    return true;
}

uint16_t Database::obtenerDireccion(){
    File archivoDireccion = SPIFFS.open("/direccion.json", "r");

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, archivoDireccion);

    archivoDireccion.close();
    if (error) {
        Serial.print("[error] : error al deserializar el archivo direccion.json - Database.cpp");
        Serial.println(error.c_str());
        return false;
    }

    uint8_t direccion = doc["direccion"];

    return direccion;
}

bool Database::setearDireccion(uint16_t direccion){

    _direccion = direccion;

    JsonDocument doc;

    doc["set"] = 1;
    doc["direccion"] = direccion;

    SPIFFS.remove("/direccion.json");
    File archivoDireccion = SPIFFS.open("/direccion.json", "w");

    serializeJson(doc, archivoDireccion);
    archivoDireccion.close();

    return true;
}
