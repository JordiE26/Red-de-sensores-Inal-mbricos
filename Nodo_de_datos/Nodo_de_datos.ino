
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

/*CONFIGURACION NODO DE DATOS #1 SENSOR DE HUMEDAD Y TEMPERATURA (DHT22)*/

#include <espnow.h>
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Definimos las ID del nodo de datos 2 (ESP32 nodo de datos #1 = DATOS 1, ESP32 nodo de datos #2 = DATOS 2)
#define NODO_DATOS 1

//Pin analogico conectado al sensor DHT22
#define DHTPIN D6

//Tipo de sensor en uso
#define DHTTYPE DHT22 // DHT 22 (AM2302)

DHT Nododatos (DHTPIN, DHTTYPE);

//Direccion MAC del receptor (Nodo coordinador) 
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

float readTemperature() {
  // Lee la temperatura en grados Celsius
  float t = Nododatos.readTemperature();
       // Leer la temperatura como Fahrenheit (isFahrenheit = true)
      //float t = Nododatos.readTemperature(true);
  // Comprueba si alguna lectura falló para volver a realizar la lectura.
  if (isnan(t)) {    
    Serial.println("¡Error de lectura del sensor DTH22!");
    return 0;
  }
  else {
    Serial.println(t);
    return t;
  }
}

float readHumidity() {
  // Lee la humedad
  float h = Nododatos.readHumidity();
  if (isnan(h)) {
    Serial.println("¡Error de lectura del sensor DTH22!");
    return 0;
  }
  else {
    Serial.println(h);
    return h;
  }
}

// Devolución de llamada cuando se envían datos
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Estado del último envío de paquete: ");
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
  //Iniciar Sensor dht
  Nododatos.begin();

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
    myData.temp = readTemperature();
    myData.hum = readHumidity();
    myData.readingId = readingId++;
     
    //Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

     Serial.print(" ");
  }
}
