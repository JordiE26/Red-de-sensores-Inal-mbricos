/* UNIVERSIDAD LAICA “ELOY ALFARO” DE MANABÍ
   FACULTAD DE CIENCIAS INFORMÁTICAS
                TEMA: 
“PROTOTIPO Y DISEÑO DE UNA RED DE SENSORES INALÁMBRICOS
    PARA EL CONTROL  DE UN HUERTO UNIFAMILIAR 
         POR MEDIO DE UN APLICATIVO WEB, 
 EN LA PARROQUIA ELOY ALFARO DE LA CIUDAD DE MANTA.”
                AUTORES:
Sr. Estupiñan Rosero Jordi Ariel
Sr. García Choez Jorge Enrique */

//Programa para obtener la direccion MAC de la placa NodeMCU8266

#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif

void setup(){
  Serial.begin(115200);
  Serial.println();
  Serial.print("Direccion MAC de la Placa ESP:  ");
  Serial.println(WiFi.macAddress());
}
 
void loop(){

}
