

#ifdef ESP32
  #include <WiFi.h>
  #include <ESPAsyncWebServer.h>
  #include <SPIFFS.h>
#else
  #include <Arduino.h>
  #include <ESP8266WiFi.h>
  #include <Hash.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include <FS.h>
#endif
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <PubSubClient.h>

// ----------------------------------Modo WakeUp Sleep -----------------------------------------------

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  10        /* Tiempo de Deep Sleep en segundos */

RTC_DATA_ATTR int bootCount = 0;

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}


// --------------------------------------------------------------------------------------------


Adafruit_BMP280 bmp; // I2C

// ----------------------------------Credenciales de WiFi ----------------------------------------
const char* ssid = "WIZZ";
const char* password = "46196ace4a";

// ------------Datos para conexion a broker -------------------------------
const char* clientid ="ESP32 Zumaya";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

//-------------------------------------------------------------------------

// ---------------Creacion de servidor web por el puerto 80-------------------------------------------
AsyncWebServer server(80);

// --------------Funcion para lectura de temperatura -------------------------------------------------
String readBMP280Temperature() {
  
  float t = bmp.readTemperature();
  if (isnan(t)) {    
    Serial.println("Falla al leer BMP280");
    return "";
  }
  else {
    Serial.println(t);
    return String(t);
    delay(10000);
  }
}

// ------------------------Funcion para lectura de Presion y conversion a Hectopascales
String readBMP280Pressure() {
  float p = bmp.readPressure() / 100.0F;
  if (isnan(p)) {
    Serial.println("Falla al leer BMP280");
    return "";
  }
  else {
    Serial.println(p);
    return String(p);
    delay(10000);
  }
}

//----------------------------------------------------------------------------------------------

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conectar a MQTT...");
      if (client.connect(clientid)) {
      Serial.println("conectado");
      Serial.print("Codigo: ");
      Serial.println(client.state());
      }else {
      Serial.print("Fallo, rc=");
      Serial.print(client.state());
      Serial.println(" Intento en 5 segundos...");
      delay(5000);}}}

// --------------------------------------Funcion Dormir Modo WakeUp Sleep -------------------------------------------

void dormir() {
  print_wakeup_reason();

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Se configuro el ESP32 para que se durmiera " + String(TIME_TO_SLEEP) + " segundos");

  Serial.println("Angoi a mimir");
  Serial.flush(); 
  esp_deep_sleep_start();
  Serial.println("Esto no debe de aparecer");
}
// -----------------------------------------------------------------------------------------------------------------
 
void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  delay(1000);
  client.setServer(mqtt_server, 1883);

  // ----------Checa si hay conecion con el sensor ----------------------------------------------------------------
  bool status; 
    status = bmp.begin(0x76);  
  if (!status) {
    Serial.println("No se encontro el sensor BMP280, checa la conexion de los cables!");
    while (1);
  }
 //-----------------------------------------------------------------------------------------------------------------
 
  // ----------------------Inicia SPIFFS ---------------------------------------------------------------------------
  if(!SPIFFS.begin()){
    Serial.println("Ocurrio un error al montar SPIFFS");
    return;
  }

  // ---------------------Conexion a Wi-Fi ------------------------------------------------------------------------
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(3000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  //----------------------------------------------------------------------------------------------------------------

  // Se enruta a / en la pagina web al archivo index.html en el SPIFFS
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });
  //------------------Se envian los datos del sensor -----------------------------------------------------------------
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readBMP280Temperature().c_str());
  });
 
  server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readBMP280Pressure().c_str());
  });
 //------------------------------------------------------------------------------------------------------------------
  // ----------------------------------------Inicio de servidor web -------------------------------------------------
  
  server.begin();
}


void loop(){
  if (!client.connected()) {
    reconnect(   );}
  client.loop();
  // ----------------------------Se guardan los valores de las mediciones del sensor en  variables para enviarlas por MQTT
  String temperatura = "Temperatura: ";
  temperatura += readBMP280Temperature().c_str();
  temperatura += " Celcius";
 
  String presion = "Presión: ";
  presion += readBMP280Pressure().c_str();
  presion += " Hectopascales";
 // -----------------------------------------envio de datos a MQTT ---------------------------------------------------------
  client.publish( "CIATEQ/MQTT/ESP32/ProyF001", (char*) temperatura.c_str(),1);
  client.publish( "CIATEQ/MQTT/ESP32/ProyF001", (char*) presion.c_str(),1);
  delay(30000);
  dormir();
}
