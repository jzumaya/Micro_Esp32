/*
 WiFi Web Server Roomba start
 A simple web server that lets start you a iRobot Roomba via the web.
 This sketch will print the IP address of your WiFi Shield (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to start cleaning or dock the roomba.
 If the IP address of your shield is yourAddress:
 http://yourAddress/H sends cleaning command
 http://yourAddress/L sends dock command
 
 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.
 Circuit:
 * ESP32
 * Roomba wakeup pin attached to pin 5
 * Roomba RXD pin attached to pin 17
 
 created for arduino 
ported for esp32 devboard V1
17.02.2019 by www.rohrhofer.org
 
 */

#include <WiFi.h>
#include <HardwareSerial.h>

HardwareSerial SerialRoomba(1);

const char* ssid     = "WIZZ";
const char* password = "46196ace4a";

WiFiServer server(80);

void setup()
{
    Serial.begin(115200);
    SerialRoomba.begin(115200,SERIAL_8N1, 16, 17);    //Baud rate, parity mode, RX, TX

    
    pinMode(5, OUTPUT);      // set the LED pin mode
    pinMode(2, OUTPUT);      // set the LED pin mode "OnboardLED"

    delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();

}

int value = 0;

void loop(){
 WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.println("<body><center><font face='Arial'>");
            client.println("<h1><font color='#0F3376' size=+7>Roomba V2.</font></h1>");
            
            client.println("<h2><font color='#009900' size=+5>ESP32</font></h2>");
            client.println("<br><br>");
            client.println("<h1><a href='H'><button style=\"height:200px;width:340px;background-color:#008CBA;border: none;border-radius: 20px;color: white;padding: 16px 40px;text-decoration: none;font-size: 30px;margin: 2px;cursor: pointer;\">CLEAN</button></h1></a>");
            client.println("<br><br>");
            client.println("<h1><a href='L'><button style=\"height:200px;width:340px;background-color:#F44336;border: none;border-radius: 20px;color: white;padding: 16px 40px;text-decoration: none;font-size: 30px;margin: 2px;cursor: pointer;\">DOCK</button></h1></a>");
            client.println("</font></center></body></html>");
           
            
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          //digitalWrite(5, HIGH);               // GET /H turns the LED on
          digitalWrite(2, HIGH);               // GET /H turns the LED on
          // Wakeup Roomba
            //digitalWrite(5, HIGH);
            //delay(100);
            digitalWrite(5, LOW);
            delay(100);
            digitalWrite(5, HIGH);
            delay(100);
            digitalWrite(5, LOW);
            delay(1000);
  
          //  Start Roomba -------- 
              SerialRoomba.write(128);  //Start
              //SerialRoomba.write(131);  //Safe mode
              SerialRoomba.write(135);  //Clean
              
              delay(1000);
              digitalWrite(2, LOW);                // GET /L turns the LED off
          
        }
        if (currentLine.endsWith("GET /L")) {
          //digitalWrite(5, LOW);                // GET /L turns the LED off

              //  Start Roomba -------- 
              //SerialRoomba.write(128);  //Start
              SerialRoomba.write(143);  //Seek Dock
              delay(1000);
              digitalWrite(2, LOW);                // GET /L turns the LED off
              
          
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
