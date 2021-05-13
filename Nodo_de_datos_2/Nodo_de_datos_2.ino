
//CONFIGURACION NODO DE DATOS #2 SENSOR DE HUMEDAD DEL SUELO (FC-28)

#include <espnow.h>
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>

// Definimos las ID del nodo de datos 2 (ESP32 nodo de datos #1 = DATOS 1, ESP32 nodo de datos #2 = DATOS 2)
#define NODO_DATOS 2

#define hum_suelo A0

//MAC Address del receptor (Nodo coordinador) 
uint8_t broadcastAddress[] = {0x98, 0xF4, 0xAB, 0xDD, 0x06, 0x89};

//Estructura para enviar datos
//Debe coincidir con la estructura del receptor (Nodo coordinador)
typedef struct struct_message {
    int id;
    float temp;
    float hum;
    float humS;
    int readingId;
} struct_message;

//Crea un struct_message llamado myData
struct_message myData;

unsigned long previousMillis = 0;   // Almacena la última vez que se publico la temperatura y humedad
const long interval = 10000;        // Intervalo en el que publicar las lecturas del sensor
unsigned int readingId = 0;         //Realiza un seguimiento del número de lecturas enviadas.

// Inserta el nombre de la red a la que se va a conectar tu ESP 
constexpr char WIFI_SSID[] = "GARCIA ESTUPIÑAN";

//Buscamos la red para obtener el canal de comunicación
int32_t getWiFiChannel(const char *ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
    for (uint8_t i=0; i<n; i++) {
      if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
        return WiFi.channel(i);
      }
    }
  }
  return 0;
}

float humS = analogRead(hum_suelo)/10.24;


// Devolución de llamada cuando se envían datos
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Envio Exitoso");
  }
  else{
    Serial.println("Envio Fallido");
  }
}
 
void setup() {
  //Iniciar monitor serie
  Serial.begin(115200);
  

  // Establecer el dispositivo como una estación Wi-Fi y establecer el canal
  WiFi.mode(WIFI_STA);
  int32_t channel = getWiFiChannel(WIFI_SSID);

  WiFi.printDiag(Serial); // Para verificar el numero de canal anterior
  wifi_promiscuous_enable(1);
  wifi_set_channel(channel);
  wifi_promiscuous_enable(0);
  WiFi.printDiag(Serial); // Para verificar el numero de canal despues
  
  // Iniciar ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error al iniciar el ESP");
    return;
  }

  // Una vez inicializado el ESP con éxito, nos registramos para enviar
  // Obtener el estado del paquete enviado
   esp_now_register_send_cb(OnDataSent);

   esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  esp_now_register_send_cb(OnDataSent);
  
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  }
 
void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // Guardar la última vez que se publicó una nueva lectura
    previousMillis = currentMillis;
    //Establecer valores para enviar
    myData.id = NODO_DATOS;
    myData.humS = analogRead(hum_suelo);
    myData.readingId = readingId++;
     
    //Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

     Serial.print("loop");
  }
 }
