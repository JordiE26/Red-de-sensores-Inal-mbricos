
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

/*CONFIGURACION DEL NODO COORDINADOR Y PAGINA WEB*/

#include <espnow.h>
#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"
#include "ESPAsyncTCP.h"
#include <Arduino_JSON.h>


// Remplazar credenciales de la red
const char* ssid = "GARCIA ESTUPIÑAN";
const char* password = "forever18";

//Estructura para enviar datos
//Debe coincidir con la estructura del remitente (Nodos de datos)
typedef struct struct_message {
  int id;
  float temp;
  float hum;
  float humS;
  unsigned int readingId;
} struct_message;

struct_message incomingReadings;

JSONVar board;

AsyncWebServer server(80);
AsyncEventSource events("/events");

// Devolución de llamada cuando se reciban datos
void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len) { 
  // Copia la dirección mac del remitente en una cadena 
  char macStr[18];
  Serial.print("Paquetes recibidos de: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  
  board["id"] = incomingReadings.id;
  board["temperatura"] = incomingReadings.temp;
  board["humedad"] = incomingReadings.hum;
  board["humedadS"] = incomingReadings.humS;
  board["readingId"] = String(incomingReadings.readingId);
  String jsonString = JSON.stringify(board);
  events.send(jsonString.c_str(), "new_readings", millis());
  
  Serial.printf("Nodo %u: %u bytes\n", incomingReadings.id, len);
  Serial.printf("Temperatura: %4.2f \n", incomingReadings.temp);
  Serial.printf("Humedad: %4.2f \n", incomingReadings.hum);
  Serial.printf("Humedad del suelo: %4.2f \n", incomingReadings.humS);
  Serial.printf("Lectura N°: %d \n", incomingReadings.readingId);
  Serial.println();
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>CONTROL Y MONITOREO DEL HUERTO</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h1 {  font-size: 2rem;}
    body {  margin: 0;}
    .topnav { overflow: hidden; background-color: #00FA09; color: white; font-size: 1.7rem; }
    .content { padding: 20px; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 700px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(320px, 1fr)); }
    .reading { font-size: 2.8rem; }
    .timestamp { color: #000000; font-size: 1rem; }
    .card-title{ font-size: 1.2rem; font-weight : bold; }
    .card.temperatura { color: #EBC82C; }
    .card.humedad { color: #1448EB; }
    .card.humedadsuelo { color: #50B8B4; }
  </style>
</head>
<body>
  <div class="topnav">
    <h1>CONTROL Y MONITOREO DEL HUERTO</h1>
  </div>
  <div class="content">
    <div class="cards">
      <div class="card temperatura">
        <p class="card-title"><i class="fas fa-sun"></i> NODO 1 - TEMPERATURA</p><p><span class="reading"><span id="t1"></span> &deg;C</span></p><p class="timestamp">Ultima lectura: <span id="rt1"></span></p>
      </div>
      <div class="card humedad">
        <p class="card-title"><i class="fas fa-wind"></i> NODO 1 - HUMEDAD</p><p><span class="reading"><span id="h1"></span> &percnt;</span></p><p class="timestamp">Ultima lectura: <span id="rh1"></span></p>
      </div>

      <div class="card humedadsuelo">
        <p class="card-title"><i class="fas fa-tint"></i> NODO 2 - HUMEDAD DEL SUELO</p><p><span class="reading"><span id="s2"></span> &percnt;</span></p><p class="timestamp">Ultima lectura: <span id="rs2"></span></p>
      </div>
    </div>
  </div>
<script>
function getDateTime() {
  var currentdate = new Date();
  var datetime = currentdate.getDate() + "/"
  + (currentdate.getMonth()+1) + "/"
  + currentdate.getFullYear() + " Hora "
  + currentdate.getHours() + ":"
  + currentdate.getMinutes() + ":"
  + currentdate.getSeconds();
  return datetime;
}
if (!!window.EventSource) {
 var source = new EventSource('/events');
 
 source.addEventListener('open', function(e) {
  console.log("Evento Conectado");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Evento Desconectado");
  }
 }, false);
 
 source.addEventListener('message', function(e) {
  console.log("Mensaje", e.data);
 }, false);
 
 source.addEventListener('new_readings', function(e) {
  console.log("Nueva_Lectura", e.data);
  var obj = JSON.parse(e.data);
  document.getElementById("t"+obj.id).innerHTML = obj.temperatura.toFixed(2);
  document.getElementById("h"+obj.id).innerHTML = obj.humedad.toFixed(2);
  document.getElementById("rt"+obj.id).innerHTML = getDateTime();
  document.getElementById("rh"+obj.id).innerHTML = getDateTime();
 }, false);

  source.addEventListener('new_readings', function(e) {
  console.log("Nueva_Lectura", e.data);
  var obj = JSON.parse(e.data);
  document.getElementById("s"+obj.id).innerHTML = obj.humedadS.toFixed(2);
  document.getElementById("rs"+obj.id).innerHTML = getDateTime();
 }, false);
}
</script>
</body>
</html>)rawliteral";

void setup() {
  // Inicializa monitor serie
  Serial.begin(115200);

  // Configure el dispositivo como una estación y un punto de acceso suave simultáneamente
  WiFi.mode(WIFI_AP_STA);
  
  // Establecer el dispositivo como una estación Wi-Fi 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Configuración como estación Wi-Fi ...");
  }
  Serial.print("Dirección IP de la estación: ");
  Serial.println(WiFi.localIP());
  Serial.print("Canal Wi-Fi : ");
  Serial.println(WiFi.channel());

  // Inicializa ESP
  if (esp_now_init() != 0) {
    Serial.println("Error al inicializar ESP!");
    return;
  }
  
 //Una vez que ESP se inicie con éxito, nos registraremos para recv CB
  // Para obtener información del paquete recv
  esp_now_register_recv_cb(OnDataRecv);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
   
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("¡Cliente reconectado! El último ID de mensaje que obtuvo es: %u\n", client->lastId());
    }
    // enviar evento con mensaje "¡Hola!", id actual millis
    // y establezca el retardo de reconexión en 1 segundo
    client->send("Hola!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  server.begin();
}
 
void loop() {
  static unsigned long lastEventTime = millis();
  static const unsigned long EVENT_INTERVAL_MS = 5000;
  if ((millis() - lastEventTime) > EVENT_INTERVAL_MS) {
    events.send("ping",NULL,millis());
    lastEventTime = millis();

  }
  
}
