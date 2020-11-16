#include <WiFi.h>
#include <PubSubClient.h>


const char* ssid     = "WIZZ"; // Your ssid
const char* password = "46196ace4a"; // Your Password 

const char* clientid ="ESP32 Zumaya";


const char* mqtt_server = "broker.hivemq.com";
//const char* mqtt_server = "192.168.100.156";


WiFiClient espClient;
PubSubClient client(espClient);
 
void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");}
  Serial.println("");
  Serial.println("WiFi connected");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
      if (client.connect(clientid)) {
      Serial.println("connected");
      Serial.print("Return code: ");
      Serial.println(client.state());
      }else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);}}}
void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);}

void loop() {
 if (!client.connected()) {
    reconnect(   );}
  client.loop();
  client.publish( "CIATEQ/MQTT/ESP32/Prac8", "Hello From... ESP 32 Zumaya",1);
  delay(1000);
}
