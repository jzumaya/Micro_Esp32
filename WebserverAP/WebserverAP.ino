#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
 
const char* ssid     = "ACA";
const char* password = "123456789";

AsyncWebServer server(80);

#define ledPin  2    // LED en terminal 2
String ledState;

// Replaces placeholder with LED state value
String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if(digitalRead(ledPin)){
      ledState = "ON";
    }
    else{
      ledState = "OFF";
    }
    Serial.print(ledState);
    return ledState;
  }
  return String();
}


void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

   // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

// Conecta a la red wifi.
  Serial.println();
  Serial.print("Configurando Punto de Acceso:  ");
  Serial.println(ssid);
 
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  // Esta es la IP
  Serial.print("Esta es la IP para conectar: ");
  Serial.print("http://");
  Serial.println(myIP);

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, HIGH);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, LOW);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Start server
  server.begin();

}
 
void loop() {

   
 
}
