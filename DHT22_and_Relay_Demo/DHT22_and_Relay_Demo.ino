/*
DHT22 and Relay Demo

This application runs on an ESP8266-based board
connected via Wi-Fi in a local network
You can read Humidity, Tempearature and Heat Index
and control a Relay via web button.

INSTALL:
-Adafruit Unified Sensor Library (by Adafruit)
-DHT Sensor Library (by Adafruit)

The MIT License (MIT)

Copyright (c) 2017 Giovanni Bernardo

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
#include "DHT.h"

#define LED     16 // D0
#define RELAY    4 // D2
#define DHTPIN  13 // D7
#define DHTTYPE DHT22 // DHT 22  (AM2302), AM2321

const char* ssid = "[YOUR SSID]";
const char* password = "[YOUR PASSPHRASE]";

float h,t,hi; // humidity,temperature,heat index
String sensorRead; // html string with sensor values separated by comma

DHT dht(DHTPIN, DHTTYPE); // set-up DHT sensor
WiFiServer server(80);

// data used for static IP configuration
// comment line 76 if you want to use DHCP
IPAddress ip(192,168,0,105);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);
 
void setup() 
  {
  Serial.begin(9600);
  dht.begin();
  delay(10);
  pinMode(LED,OUTPUT);
  pinMode(RELAY,OUTPUT);
  digitalWrite(LED,LOW);
  digitalWrite(RELAY,LOW);
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
  WiFi.config(ip,gateway,subnet); // comment if you want DHCP

  // Connect to WiFi network
  while (WiFi.status() != WL_CONNECTED) 
    {
    delay(500);
    Serial.print(".");
    }
  
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  digitalWrite(LED,HIGH);
  }

// read DHT sensor and fill string
void readSensor()
 {
 h = dht.readHumidity();
 t = dht.readTemperature();
 sensorRead="";
  
 if (isnan(h) || isnan(t)) 
    {
    sensorRead="---,---,---";
    }
 else
    {
    hi = dht.computeHeatIndex(t, h, false);
    sensorRead=String(t,1);
    sensorRead += "&deg;C,";
    sensorRead += String(h,1);
    sensorRead += "%,";
    sensorRead += String(hi,1);
    sensorRead += "&deg;C";
    }
 }

void loop() 
  {
  static bool flash=true; // used for blink the led
    
  static unsigned long prevTime=0;
  // read DHT sensor every 2 seconds
  if ((millis()-2000)>prevTime)
    {
    prevTime=millis();
    flash ^= 1;
    digitalWrite(LED,flash);  
    readSensor();  
    }
           
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) 
    {
    return;
    }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available())
    {
    delay(1);
    }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // relay requested: ON, 500ms pause, OFF
  if (request.indexOf("relay")>0)
    {
    digitalWrite(RELAY,HIGH);  
    delay(500);
    digitalWrite(RELAY,LOW);  
    }

  // javascript requested values
  if(request.indexOf("getValues") > 0)
    {
    client.print ("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n");
    client.print(sensorRead);
    Serial.print("Reading: ");
    Serial.println(sensorRead);
    request="";
    }
  else
    {
    // normal request, write html page
    String p="HTTP/1.1 200 OK\r\n";
    p += "Content-Type: text/html\r\n";
    p += "\r\n";
    // Doctype 4.01 Transitional, please refer to: http://www.w3schools.com/TAGS/tag_doctype.asp
    p += "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\r\n";
    p += "<html>\r\n";
    p += "<head>\r\n";
    p += "<title>Ambient Sensor</title>\r\n";
    p += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\r\n";
    p += "<style type=\"text/css\">\r\n";
    p += ".st {color:black; text-decoration:none; font-family:tahoma,arial; font-size:12pt; font-weight:normal; font-variant:small-caps; text-align:center; padding:8px; margin-top:12px; display:block;}\r\n";
    p += ".bo {color:white; text-decoration:none; font-family:tahoma,arial; font-size:28pt; font-weight:bold; text-align:center; padding:8px; margin-top:1px; margin-bottom:15px; display:block; border-radius:15px; outline:none;}\r\n";
    p += ".bu {color:white; background-color:#cccccc; text-decoration:none; font-family:tahoma,arial; font-size:28pt; font-weight:bold; text-align:center; padding:8px; margin-top:1px; margin-bottom:15px; display:block; border-radius:15px; box-shadow:0 8px #666666; outline:none;}\r\n";
    p += ".bu:active {background-color:#999999; box-shadow:0 3px #333333; transform:translateY(4px);}\r\n";  
    p += "a.l:hover, a.l:link, a.l:visited {color:#0099cc; text-decoration:none; font-family:tahoma,arial; font-size:12pt; font-weight:normal; text-align:center; padding:8px; margin-top:50px; display:block;}\r\n";
    p += "</style>\r\n";
    p += "<script language=\"javascript\">\n\r";
    p += "xmlhttp=null;\n\r";
    p += "var sensorValues = [];\n\r";
    p += "function getValues()\n\r";
    p += "\t{\n\r";
    p += "\tsetTimeout('getValues()', 2000);\n\r";
    p += "\tif (window.XMLHttpRequest)\n\r";
    p += "\t\t{\n\r";
    p += "\t\txmlhttp=new XMLHttpRequest();\n\r";
    p += "\t\t}\n\r";
    p += "\telse\n\r";
    p += "\t\t{\n\r";
    p += "\t\txmlhttp=new ActiveXObject('Microsoft.XMLHTTP');\n\r";
    p += "\t\t}\n\r";
    p += "\txmlhttp.open('GET','?getValues',false);\n\r";
    p += "\txmlhttp.send(null);\n\r";
    p += "\tif (xmlhttp.responseText != \"\")\n\r";
    p += "\t\t{\n\r";
    p += "\t\tsensorValues = xmlhttp.responseText.split(\",\");\n\r";
    p += "\t\tif(sensorValues[0]!=\"---\"){\n\r";
    p += "\t\t\tdocument.getElementById(\"te\").innerHTML=sensorValues[0];\n\r";
    p += "\t\t\tdocument.getElementById(\"ur\").innerHTML=sensorValues[1];\n\r";
    p += "\t\t\tdocument.getElementById(\"hi\").innerHTML=sensorValues[2];}\n\r";
    p += "\t\t}\n\r";
    p += "\t}\n\r";
    p += "</script>\n\r";
    p += "</head>\n\r";
    p += "<body onLoad=\"getValues()\">\n\r"; 
    p += "<div style=\"text-align:center\">\n\r";
    p += "<div class=\"st\">temperatura</div>\n\r";
    p += "<div id=\"te\" class=\"bo\" style=\"background-color:#ff6600\">--.-</div>\n\r";
    p += "<div class=\"st\">umidit&agrave; relativa</div>\n\r";
    p += "<div id=\"ur\" class=\"bo\" style=\"background-color:#006699\">--.-</div>\n\r";
    p += "<div class=\"st\">temperatura percepita</div>\r\n";
    p += "<div id=\"hi\" class=\"bo\" style=\"background-color:#009966\">--.-</div>\n\r";
    p += "<a class=\"bu\" style=\"margin-top:50px;\" href=\"?relay\">RELAY</a>\n\r";
    p += "<a class=\"l\" href=\"http://www.settorezero.com\">www.settorezero.com</a>\n\r";
    p += "</div>";
    p += "</body>\n\r";
    p += "</html>";
    client.print(p);
    }
  
  Serial.println("Client disconnected");
  Serial.println("");
  delay(10);
  }
