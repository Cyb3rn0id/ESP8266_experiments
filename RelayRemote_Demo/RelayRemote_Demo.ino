/*

RelayRemote Demo
This application runs on an ESP8266-based board
connected via Wi-Fi in a local network
You can control status of 4 relay connected on ESP8266 GPIOS 12,13,14 and 16 
using http request or using the Android App "Relay Remote"

The MIT License (MIT)

Copyright (c) 2016 Giovanni Bernardo

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORTOR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

(C)2016 Bernardo Giovanni
http://www.settorezero.com
http://www.facebook.com/settorezero
http://www.twitter.com/settorezero

*/

#include <ESP8266WiFi.h>

// put here your wi-fi network data
const char* ssid = "Gianni";
const char* password = "giannino";
#define DEBUG // comment if you don't want use serial debug

#define Relay1  14 // On LinkNodeR4 is S3 (LED D4)  - on NodeMCU devkit is D5 - on LinkNodeD1 is D13/SCK
#define Relay2  12 // On LinkNodeR4 is S4 (LED D10) - on NodeMCU devkit is D6 - on LinkNodeD1 is D12/MISO
#define Relay3  13 // On LinkNodeR4 is S5 (LED D8)  - on NodeMCU devkit is D7 - on LinkNodeD1 is D11/MOSI
#define Relay4  16 // On LinkNodeR4 is S2 (LED D3)  - on NodeMCU devkit is D0 - on LinkNodeD1 is D2

// NOTE:
// Relay 4 (S2) on LinkNode R4 turns on for about 1 second at start-up
// and then turns off. Do not connect anything must be off at start-up!

int RelayStatus[4]; // Here I'll store the status of 4 digital pins
WiFiServer server(80);

void setup() 
  {
  #ifdef DEBUG
    Serial.begin(115200);
    delay(10);
  #endif

  // Relay pins as output, off at start-up
  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  pinMode(Relay3, OUTPUT);
  pinMode(Relay4, OUTPUT);
  digitalWrite(Relay1, LOW);
  digitalWrite(Relay2, LOW);
  digitalWrite(Relay3, LOW);
  digitalWrite(Relay4, LOW);

  // reset relays status array
  for (int i=0; i<4; i++)
    {
    RelayStatus[i]=LOW;
    }
 
  #ifdef DEBUG 
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
  #endif

  WiFi.begin(ssid, password); // connect to your wi-fi network

  #ifdef DEBUG
    int TimeOut=0; // variable used to debug out too much retries
  #endif
  
  while (WiFi.status() != WL_CONNECTED) 
    {
    delay(500);
    #ifdef DEBUG
      Serial.print(".");
      if ((TimeOut++)==30)
          {
          Serial.println("Too much time! SSID/Password are ok?");
          TimeOut=0;  
          }
    #endif
    }

  #ifdef DEBUG
    Serial.println("");
    Serial.println("*Connected*");
  #endif
  
  server.begin(); // start server
  
  #ifdef DEBUG
    Serial.println("Server started");
    Serial.print("This Board URL is: ");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println("");
  #endif
  } // end setup
 
void loop() 
  {

  // check if have serial data
  if (Serial.available() > 0) 
    {
    char q = Serial.read();
    if (q=='i' || q=='I')
      {
      Serial.print("IP address is: ");
      Serial.println(WiFi.localIP());
      }
    }
  
  // checking for an incoming connection
  WiFiClient client = server.available();
  if (!client) 
    {
    return;
    }
 
  // wait a client request
  #ifdef DEBUG
    Serial.println("new client");
  #endif
  
  while (!client.available())
    {
    delay(1);
    }

  // get the request until the carriage return
  String request = client.readStringUntil('\r');
  #ifdef DEBUG
    Serial.println(request);
  #endif
  client.flush(); // Waits until all outgoing characters in buffer have been sent

  // request contains instructions for Relay1 ?
  if (request.indexOf("R1=1") != -1)  
    {
    digitalWrite(Relay1, HIGH);
    RelayStatus[0] = HIGH;
    }
  if (request.indexOf("R1=0") != -1)  
    {
    digitalWrite(Relay1, LOW);
    RelayStatus[0] = LOW;
    }
  // request contains instructions for Relay2 ?
  if (request.indexOf("R2=1") != -1)  
    {
    digitalWrite(Relay2, HIGH);
    RelayStatus[1] = HIGH;
    }
  if (request.indexOf("R2=0") != -1)  
    {
    digitalWrite(Relay2, LOW);
    RelayStatus[1] = LOW;
    }
  // request contains instructions for Relay3 ?
  if (request.indexOf("R3=1") != -1)  
    {
    digitalWrite(Relay3, HIGH);
    RelayStatus[2] = HIGH;
    }
  if (request.indexOf("R3=0") != -1)  
    {
    digitalWrite(Relay3, LOW);
    RelayStatus[2] = LOW;
    }
  // request contains instructions for Relay4 ?
  if (request.indexOf("R4=1") != -1)  
    {
    digitalWrite(Relay4, HIGH);
    RelayStatus[3] = HIGH;
    }
  if (request.indexOf("R4=0") != -1)  
    {
    digitalWrite(Relay4, LOW);
    RelayStatus[3] = LOW;
    }

  // now I can give a 200/OK reponse 
  String p="HTTP/1.1 200 OK\r\n"; // string constructor
  p += "Content-Type: text/html\r\n";
  p += "\r\n"; // THIS IS IMPORTANT for separating response from page!
  // Doctype 4.01 Transitional
  // please refer to: http://www.w3schools.com/TAGS/tag_doctype.asp
  p += "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\r\n";
  p += "<head>\r\n";
  p += "<title>RelayRemote Demo By Bernardo Giovanni</title>\r\n";
  // adapt page for mobile devices
  p += "<meta name=\"viewport\" content=\"width=device-width\">\r\n"; 
  // I defined a CSS for rendering some elements
  p += "<style>\r\n";
  p += "a.btn:link {color:#0000FF; text-decoration:none; font-weight:bold; padding:10px; margin:3px; background-color:#C6C6C6;}\r\n";
  p += "a.btn:visited {color:#0000FF; text-decoration:none; font-weight:bold; padding:10px; margin:3px; background-color:#C6C6C6;}\r\n";
  p += "a.btn:hover {color:#0000FF; text-decoration:none; font-weight:bold; padding:10px; margin:3px; background-color:#C6C6C6;}\r\n";
  p += ".row {font-family:tahoma,arial; size:22pt; text-align:left; vertical-align:middle; padding:2px; margin:22px 1px 22px 3px; width:100%}\r\n";
  p += ".ison {color:#FFFFFF; font-weight:bold; padding:10px; margin:3px;  background-color:#00FF00;}\r\n";
  p += ".isoff {color:#FFFFFF; font-weight:bold; padding:10px; margin:3px;  background-color:#FF0000;}\r\n";
  p += "</style>\r\n";
  // print string "status:xxxx" used by android app to set current relay status on the interface
  // I'm using an html comment so this string will not visible on the generated webpage
  p += "<!-- relay status:";
  for (int u=0; u<4; u++)
    {
     p+= RelayStatus[u]?"1":"0";  
   }
  p += " -->\r\n";
  p += "</head>\r\n";
  p += "<html>\r\n";
  p += "<body>\r\n";
  // rows with relay status
  for (int i=0; i<4; i++)
    {
    // Relay X is XXX : Turn it YYY 
    p += "<div class=\"row\">Relay [";
    p += (i+1); // add 1 since relays on the app are 1-based index
    p += "] is ";
    p += RelayStatus[i]?"<span class=\"ison\">&nbsp;ON&nbsp;</span>":"<span class=\"isoff\">&nbsp;OFF&nbsp;</span>";
    p += " : ";
    p += "<a class=\"btn\" href=\"/?R";
    p += (i+1);
    p += "=";
    p += RelayStatus[i]?"0":"1";
    p += "\">&nbsp;Turn ";
    p += RelayStatus[i]?"OFF":"ON";
    p += "&nbsp;</a></div>\r\n";
    }
  p += "</body>\r\n";
  p += "</html>";
  client.print(p); // send page to client
  delay(1);
  #ifdef DEBUG
    Serial.println("Client disconnected");
    Serial.println("");
  #endif
  }
