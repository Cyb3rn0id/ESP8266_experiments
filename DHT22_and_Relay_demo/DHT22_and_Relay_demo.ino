/*
DHT22 and Relay Demo

Copyright (c) 2017 Giovanni Bernardo (CYB3rn0id)
http://www.settorezero.com
http://www.facebook.com/settorezero
http://www.twitter.com/settorezero

DESCRIPTION
This application runs on an ESP8266-based board connected via Wi-Fi in a local network 
You can read Humidity, Temperature and Heat Index
You can control a Relay via web button too
Complete tutorial, in Italian language, here:
http://www.settorezero.com/wordpress/termometro-wi-fi-con-indicazione-umidita-temperatura-percepita-e-controllo-rele-mediante-esp8266/

PREREQUISITES
- Adafruit Unified Sensor Library (by Adafruit)
- DHT Sensor Library (by Adafruit)

LICENSE
The MIT License (MIT)

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
*/
 
#include <ESP8266WiFi.h>
#include "DHT.h"
#include "webpage.h"

#define LED     16 // D0
#define RELAY    4 // D2
#define DHTPIN  13 // D7
#define DHTTYPE DHT22 // Sensor used: DHT22 (aka AM2302) OR AM2321
#define USE_STATIC_IP // comment this if you want address assigned by DHCP

// change SSID and passphrase according your network
const char* ssid = "[YOUR SSID]";
const char* password = "[YOUR PASSPHRASE]";
String sdate,stime;

// data used for static IP configuration
#ifdef USE_STATIC_IP
  IPAddress ip(192,168,0,105);
  IPAddress gateway(192,168,0,1); // router address
  IPAddress subnet(255,255,255,0);
#endif

float h,t,hi; // humidity,temperature,heat index
float h1,t1; // last humidity and temperature good values
float max_t, min_t; // max and min temperature detected
String sensorRead; // html string with sensor values separated by comma, used for ajax refresh
const char* header="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n";
const char* doctype="<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\r\n";

DHT dht(DHTPIN, DHTTYPE); // set-up DHT sensor
WiFiServer server(80);
 
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
  
  #ifdef USE_STATIC_IP
    WiFi.config(ip,gateway,subnet);
  #endif
  
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
// parameter set to true will reset min/max values
void readSensor(bool resetvalues)
 {
 static bool first_read=true;
 h = dht.readHumidity();
 t = dht.readTemperature();

 if (resetvalues) {first_read=true;}
 
 sensorRead="";

 // sensorRead structure:
 // temperature,humidity,heatindex,minimum,maximum
 if (isnan(h) || isnan(t)) 
    {
    // values from sensor not good
    sensorRead="---,---,---,---,---";
    }
 else
    {
    // values good
    t1=t;
    h1=h;
    hi = dht.computeHeatIndex(t, h, false);
    sensorRead=String(t,1);
    sensorRead += ",";
    sensorRead += String(h,1);
    sensorRead += ",";
    sensorRead += String(hi,1);
       
    // set/reset min and max values
    if (first_read)
      {
      max_t=t;
      min_t=t;
      first_read=false;  
      }
   else
      {
      if (t>max_t) {max_t=t;}
      if (t<min_t) {min_t=t;}
      }
    
    // add minimum temperature
    sensorRead += ",";
    sensorRead += String(min_t,1);
    // add maximum temperature
    sensorRead += ",";
    sensorRead += String(max_t,1);
    }
    sensorRead += ",";
    sensorRead += sdate;
    sensorRead += ",";
    sensorRead += stime;
   }

void requestDateTime(void)
  {
   WiFiClient client;
   Serial.println("Requesting date and time");
  
   if (!client.connect("193.204.114.232", 13)) // ntp1.inrim.it 
    {
    Serial.println("connection failed");
    return;
    }
  client.print("HEAD / HTTP/1.1\r\nAccept: */*\r\nUser-Agent: Mozilla/4.0 (compatible; ESP8266;)\r\n\r\n");
  delay(10);

  while(!client.available())
    {
    delay(1);  
    }
  String line = client.readStringUntil('\r');
  // Example of string: 13 JAN 2017 19:05:58 CET
  Serial.println(line);
  sdate=line.substring(0, 11);
  stime=line.substring(12, 17);
 }
    
void loop() 
  {
  static bool flash=true; // used for blinking the led
    
  static unsigned long prevTime=0;
  // read DHT sensor every 2 seconds
  if (prevTime>millis()) // prevent millis() overflow
    {
    prevTime=0;
    }
  if ((millis()-2000)>prevTime)
    {
    prevTime=millis();
    flash ^= 1; // blink the led
    digitalWrite(LED,flash);  
    readSensor(false); // read sensor without reset min/max values
    requestDateTime();
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

  // reset values requested
  if (request.indexOf("reset")>0)
    {
    readSensor(true); // read sensor resetting min/max values
    }
    
  // javascript has requested sensor values
  if(request.indexOf("getValues") > 0)
    {
    client.print (header);
    client.print(sensorRead);
    Serial.print("Reading: ");
    Serial.println(sensorRead);
    request="";
    }
  else
    {
    // normal request, write html page
    // body
    String p3= "<body onLoad=\"getValues()\">\n\r"; 
    p3 += "<div style=\"text-align:center\">\n\r";
    
    p3 += "<div class=\"st\">temperatura</div>\n\r";
    p3 += "<a id=\"te\" class=\"bo\" style=\"background-color:#ff6600; margin-bottom:8px;\" href=\"?reset\">";
    p3 += String(t1,1);
    p3 += "&deg;C</a>\n\r";

    // used unicode arrows for min/max:
    // http://www.w3schools.com/charsets/ref_utf_arrows.asp
    p3 += "<div style=\"text-align:center; width:100%\">";
    // min
    p3 += "<div id=\"temin\" class=\"bom\" style=\"background-color:#ffc300; width:49%;\">&#8681;&nbsp;";
    p3 += String(min_t,1);
    p3 += "&deg;</div>\n\r";
    // max
    p3 += "<div id=\"temax\" class=\"bom\" style=\"background-color:#ff4300; width:49%;\">&#8679;&nbsp;";
    p3 += String(max_t,1);
    p3 += "&deg;</div>\n\r";
    p3 += "</div>\n\r";
    
    // heat index
    p3 += "<div id=\"hi\" class=\"bo\" style=\"background-color:#009966;\">&#128102;&nbsp;";
    p3 += String(hi,1);
    p3 += "&deg;C</div>\n\r";
    
    p3 += "<div class=\"st\">umidit&agrave; relativa</div>\n\r";
    p3 += "<div id=\"ur\" class=\"bo\" style=\"background-color:#006699\">";
    p3 += String(h1,1);
    p3 += "%</div>\n\r";

    p3 += "<div style=\"text-align:center\">";
    p3 += "<div class=\"st\" id=\"sdate\" style=\"display:inline-block\";></div>";
    p3 += "&nbsp;<div class=\"st\" id=\"stime\" style=\"display:inline-block\"></div>\n\r";
    p3 += "</div>";
    
    p3 += "<a class=\"bu\" style=\"margin-top:40px;\" href=\"?relay\">RELAY</a>\n\r";
    p3 += "<a class=\"l\" href=\"http://www.settorezero.com\">www.settorezero.com</a>\n\r";
    p3 += "</div>\n\r";
    p3 += "</body>\n\r";
    p3 += "</html>";

    client.print(header);
    client.print(doctype);
    client.print(p1);
    client.print(p2);
    client.print(p3);
    }
  
  Serial.println("Client disconnected");
  Serial.println("");
  delay(10);
  }
