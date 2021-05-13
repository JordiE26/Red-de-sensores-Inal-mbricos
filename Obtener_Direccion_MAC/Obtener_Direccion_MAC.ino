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
