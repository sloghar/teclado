#include <SPI.h>
#include <Arduino.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <Teclado.h>
#include <Rele.h>
#include <Buzzer.h>
#include <Database.h>
#include <Lora.h>
#include <SPIFFS.h>
#include <QR.h>
#include <RTClib.h>

#define FIRMWARE_VERSION 1.0
#define TIEMPO_MAXIMO 7000
#define BUZZER 15
#define RELE 13

LiquidCrystal_I2C lcd(0x27, 16, 2);
Teclado teclado;
Buzzer buzzer(BUZZER);
Rele rele(RELE);
Database database;
Lora lora(&database);
QR qr;
RTC_DS3231 rtc;

#define CODE_LENGTH 6
bool enviarCodigo = false;
bool obtenerCodigoUsado = true;
uint8_t position = 0;
uint8_t codigo[CODE_LENGTH] = {255,255,255,255,255,255};

#define TIEMPO_PARA_REVISAR_LA_COLA 10000
uint32_t timestampParaEnviarCodigoUsado = millis();
uint8_t codigoUsado[CODE_LENGTH];
uint32_t timestampDeCodigoUsado;

//VAARIABLES PARA LA DIRECCION

bool habilitador = false, habilitador2 = false;

void introducir_contrasena(){
  while(true){
        Serial.print("\nIntroduce la contraseña y presiona enter: \n>>>");

        while(true){
          if(Serial.available() > 0){
            String pass = Serial.readStringUntil('\n');

            if(pass != "igemas2024"){
              Serial.print("\nContraseña incorrecta");
              break;
            }
            else
              habilitador = true;
          }

          if(habilitador){
            Serial.print("\nIntroduce la dirección y presiona enter: \n>>>");

            while (true)
            {
              if (Serial.available() > 0 )
              {
                String dir = Serial.readStringUntil('\n');
                uint16_t direc = dir.toInt();

                database.setearDireccion(direc);

                habilitador2 = true;
                break;
              }
              
            }

            break;
            
          }
        }

        if(habilitador2)
          break;
  }
}

void seteado_de_direccion(){

  bool direccion_seteada = database.estaSeteadaLaDireccion();

    if(!direccion_seteada){
      introducir_contrasena();
    }
}

void cambio_de_direccion(){
  Serial.println("¿Quieres cambiar la dirección? escribe \"s\" y presiona enter en caso afirmativo");
  Serial.print("Tienes 5 segundos para hacerlo\n>>>");

  uint32_t tiempo = millis();

  while (true)
  {
    if((millis() - tiempo) < 5000){
      if(Serial.available() > 0){
        String afirmativo = Serial.readStringUntil('\n');

        if(afirmativo == "s"){
          introducir_contrasena();
          break;
        }
        else{
          break;
        }
      }
    }
    else{
      Serial.println("\nEl tiempo para cambiar la dirección ha concluido\nInicio del dispositivo");
      break;
    }
  }
  


}

void manejo_de_direccion(){
  seteado_de_direccion();

  habilitador = false;
  habilitador2 = false;

  cambio_de_direccion();
}




void arrayPush(uint8_t value){

    if(position < CODE_LENGTH){
        codigo[position] = value;
        position++;
    }
}

void arrayDelete(){
    if(position > 0){
        position--;
    }
}

void arrayDestroy(){
    for(uint8_t i = 0; i < CODE_LENGTH; i++){
        codigo[i] = 255;
    }
    position = 0;
}

void arrayPrint(){
    lcd.setCursor(0, 1);
    lcd.print(F("                "));

    for (uint8_t i = 0; i < position; i++){
        lcd.setCursor(5 + i, 1);
        String c;

        if(codigo[i] == 12){
            c = "A";
        } else if(codigo[i] == 13){
            c = "B";
        } else if(codigo[i] == 14){
            c = "C";
        } else if(codigo[i] == 15){
            c = "D";
        } else {
            c = String(codigo[i]);
        }

        lcd.print(c);
    }
}

void codigoIncorrecto(){
    lcd.clear();
    lcd.print(F("     CODIGO"));
    lcd.setCursor(0, 1);
    lcd.print(F("    INVALIDO"));
    buzzer.sonidoDeCodigoIncorrecto();
}

void codigoCorrecto(){
    lcd.clear();
    lcd.print(F("     CODIGO"));
    lcd.setCursor(0, 1);
    lcd.print(F("     VALIDO"));

    rele.encender();
    buzzer.sonidoDeCodigoCorrecto();
    rele.apagar();

    lcd.clear();
    lcd.print(F("    ESPERE UN"));
    lcd.setCursor(0, 1);
    lcd.print(F("     MOMENTO"));
}

void espereUnMomento(){
    lcd.clear();
    lcd.print(F("    ESPERE UN"));
    lcd.setCursor(0, 1);
    lcd.print(F("     MOMENTO"));
}

void validandoCodigo(){
    lcd.clear();
    lcd.print(F("    VALIDANDO"));
    lcd.setCursor(0, 1);
    lcd.print(F("    EL CODIGO"));
}

void introducirCodigo(){
    lcd.clear();
    lcd.print(F("   INTRODUCIR"));
    lcd.setCursor(0, 1);
    lcd.print(F("     CODIGO"));  

    for(uint8_t i = 0; i < 2; i++){
        delay(250);
        lcd.noBacklight();
        delay(250);
        lcd.backlight();
    }
}

void setup() {
    Serial.begin(9600);
    while(!Serial){}

    Serial.println(F("\n\n\n"));
    Serial.print(F("[firmware version] : "));
    Serial.println(FIRMWARE_VERSION);

    lcd.init();
    lcd.print(F("   iGEMAS CA"));
    lcd.setCursor(0, 1);
    lcd.print(F(" CONTROL ACCESO"));

    for(uint8_t i = 0; i < 5; i++){
        delay(250);
        lcd.noBacklight();
        delay(250);
        lcd.backlight();
    }

    if (!rtc.begin()) {
        while(true){
            Serial.println("[error] : No se encuentra el RTC DS3231");
            delay(250);
        }
    }

    manejo_de_direccion();

    uint16_t direccion = database.obtenerDireccion();
    lora.setearDireccion(direccion);

    Serial.println("\n\n[dirección] : " + String(direccion));

    lcd.clear();
    lcd.print(F("   INTRODUCIR"));
    lcd.setCursor(0, 1);
    lcd.print(F("     CODIGO"));
}

void loop() {

    lora.revisarModuloLora();

    if(qr.revisarModuloQR()){
        uint8_t codigoQR[CODE_LENGTH];
        qr.obtenerCodigo(codigoQR);

        validandoCodigo();

        Serial.print(F("\n[debug] : codigo introducido por modulo lector de codigos qr\n    "));
        for (uint8_t i = 0; i < CODE_LENGTH; i++){
            String c;

            if(codigoQR[i] == 12){
                c = "A";
            } else if(codigoQR[i] == 13){
                c = "B";
            } else if(codigoQR[i] == 14){
                c = "C";
            } else if(codigoQR[i] == 15){
                c = "D";
            } else {
                c = String(codigoQR[i]);
            }

            if(codigoQR[i] != 255){
                Serial.print(c);
            }
        }

        DateTime now = rtc.now();
        uint32_t timestamp = now.unixtime();
        Serial.print(F("\n[debug] : timestamp "));
        Serial.println(timestamp);

        if(database.compararCodigo(codigoQR)){

            codigoCorrecto();

            if(!lora.enviarCodigoUsado(codigoQR, timestamp)){
                //TODO: se debe guardar el codigo usado y se debe enviar más tarde
                database.guardarCodigoUsado(timestamp, codigoQR);
            }
            
        } else {
            codigoIncorrecto();
        }

        introducirCodigo();
    }

    uint8_t teclaPulsada = teclado.obtenerTeclaPulsada();

    if(teclaPulsada != NO_INPUT && teclaPulsada != OK && teclaPulsada != BORRAR){
    
        buzzer.sonidoDeTeclaPulsada();
        lcd.clear();
        lcd.print(F("     CODIGO"));

        arrayPush(teclaPulsada);
        arrayPrint();

        uint32_t marcaDeTiempo = millis();

        while((millis() - marcaDeTiempo) < TIEMPO_MAXIMO){

            teclaPulsada = teclado.obtenerTeclaPulsada();

            if(teclaPulsada != NO_INPUT && teclaPulsada != OK && teclaPulsada != BORRAR){
                buzzer.sonidoDeTeclaPulsada();
                arrayPush(teclaPulsada);
                arrayPrint();
                marcaDeTiempo = millis();

            } else if(teclaPulsada == OK){

                Serial.print(F("\n[debug] : codigo introducido por teclado\n    "));
                for (uint8_t i = 0; i < CODE_LENGTH; i++){
                    String c;

                    if(codigo[i] == 12){
                        c = "A";
                    } else if(codigo[i] == 13){
                        c = "B";
                    } else if(codigo[i] == 14){
                        c = "C";
                    } else if(codigo[i] == 15){
                        c = "D";
                    } else {
                        c = String(codigo[i]);
                    }

                    if(codigo[i] != 255){
                        Serial.print(c);
                    }

                }
                
                DateTime now = rtc.now();
                uint32_t timestamp = now.unixtime();
                Serial.print(F("\n[debug] : timestamp "));
                Serial.println(timestamp);

                if(position != CODE_LENGTH){
                    codigoIncorrecto();
                    break;
                }

                validandoCodigo();

                if(database.compararCodigo(codigo)){
                    codigoCorrecto();

                    if(!lora.enviarCodigoUsado(codigo, timestamp)){
                        //TODO: se debe guardar el codigo usado y se debe enviar más tarde
                        database.guardarCodigoUsado(timestamp,codigo);
                    }

                } else {
                    codigoIncorrecto();
                }

                break;

            } else if(teclaPulsada == BORRAR){
                buzzer.sonidoDeTeclaPulsada();

                arrayDelete();
                arrayPrint();
                marcaDeTiempo = millis();
            }
        }

        arrayDestroy();
        introducirCodigo();
    }


    if((millis() - timestampParaEnviarCodigoUsado) > TIEMPO_PARA_REVISAR_LA_COLA){
        if(database.revisarColaDeCodigosUsados(codigoUsado, &timestampDeCodigoUsado)){

            Serial.println(F("[debug] : hay un codigo en la cola de los codigos usados"));
            Serial.println(F("[debug] : se va a enviar al Hub"));

            espereUnMomento();

            if(lora.enviarCodigoUsado(codigoUsado, timestampDeCodigoUsado)){
                Serial.println(F("[debug] : el codigo usado en la cola se ha enviado correctamente"));
                database.borrarCodigoUsado();
            } else  {
                Serial.println(F("[error] : no se ha podido establecer comunicación con el Hub. Se intentará más tarde"));
            }

            introducirCodigo();
        }

        timestampParaEnviarCodigoUsado = millis();
    }


}

