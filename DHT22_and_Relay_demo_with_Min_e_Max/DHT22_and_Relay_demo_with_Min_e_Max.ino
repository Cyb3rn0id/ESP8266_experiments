/*
DHT22 and Relay Demo 
v2 (added min and max detected values)

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

(C)2017 Bernardo Giovanni
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
float h1,t1; // previous values
float max_t, min_t; // max and min temperature detected
String sensorRead; // html string with sensor values separated by comma

DHT dht(DHTPIN, DHTTYPE); // set-up DHT sensor
WiFiServer server(80);

// data used for static IP configuration
// comment line 79 if you want to use DHCP
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
    sensorRead += "&deg;C,";
    sensorRead += String(h,1);
    sensorRead += "%,";
    sensorRead += String(hi,1);
    sensorRead += "&deg;C";
    
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
    sensorRead += "&deg;C";
    // add maximum temperature
    sensorRead += ",";
    sensorRead += String(max_t,1);
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
    readSensor(false);  
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
    readSensor(true);  
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
    p += "<link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"data:image/x-icon;base64,AAABAAEAEBAQAAAAAAAoAQAAFgAAACgAAAAQAAAAIAAAAAEABAAAAAAAgAAAAAAAAAAAAAAAEAAAAAAAAAAAAAAA/4QAAP///wAA0P8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABEREREREREAEAAAAAAAAQACIiIiIiIgACIiIiIiIiIAIiIiIiIiIgAiIiIiIiIiAAIiIiIiIiAAEAACIiIAAwAREQIiIgMzABERECIgMzMAERERAAMzMwAREREDMzMzABERERAzMzAAEREREQAAAQAAAAAAAAAACAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAAQAA\">\n\r";

    String g = "<style type=\"text/css\">\r\n";
    g += ".st {color:#585858; text-decoration:none; font-family:tahoma,arial; font-size:12pt; font-weight:bold; font-variant:small-caps; text-align:center; padding:8px; margin-top:12px; display:block;}\r\n";
    g += ".bo {color:white; text-decoration:none; font-family:tahoma,arial; font-size:28pt; font-weight:bold; text-align:center; padding:8px; margin-top:1px; margin-bottom:15px; display:block; border-radius:15px; outline:none;}\r\n";
    g += ".bu {color:white; background-color:#cccccc; text-decoration:none; font-family:tahoma,arial; font-size:28pt; font-weight:bold; text-align:center; padding:8px; margin-top:1px; margin-bottom:15px; display:block; border-radius:15px; box-shadow:0 8px #666666; outline:none;}\r\n";
    g += ".bu:active {background-color:#999999; box-shadow:0 3px #333333; transform:translateY(4px);}\r\n";  
    g += "a.l:hover, a.l:link, a.l:visited {color:#0099cc; text-decoration:none; font-family:tahoma,arial; font-size:12pt; font-weight:normal; text-align:center; padding:8px; margin-top:50px; display:block;}\r\n";
    g += "</style>\r\n";
    g += "<script language=\"javascript\">\n\r";
    g += "xmlhttp=null;\n\r";
    g += "var sensorValues = [];\n\r";
    g += "function getValues()\n\r";
    g += "\t{\n\r";
    g += "\tsetTimeout('getValues()', 2000);\n\r";
    g += "\tif (window.XMLHttpRequest)\n\r";
    g += "\t\t{\n\r";
    g += "\t\txmlhttp=new XMLHttpRequest();\n\r";
    g += "\t\t}\n\r";
    g += "\telse\n\r";
    g += "\t\t{\n\r";
    g += "\t\txmlhttp=new ActiveXObject('Microsoft.XMLHTTP');\n\r";
    g += "\t\t}\n\r";
    g += "\txmlhttp.open('GET','?getValues',false);\n\r";
    g += "\txmlhttp.send(null);\n\r";
    g += "\tif (xmlhttp.responseText != \"\")\n\r";
    g += "\t\t{\n\r";
    g += "\t\tsensorValues = xmlhttp.responseText.split(\",\");\n\r";
    g += "\t\tif(sensorValues[0]!=\"---\"){\n\r";
    g += "\t\t\tdocument.getElementById(\"te\").innerHTML=sensorValues[0];\n\r";
    g += "\t\t\tdocument.getElementById(\"ur\").innerHTML=sensorValues[1];\n\r";
    g += "\t\t\tdocument.getElementById(\"hi\").innerHTML=sensorValues[2];\n\r";
    g += "\t\t\tdocument.getElementById(\"temin\").innerHTML=\"MIN&nbsp;\"+sensorValues[3];\n\r";
    g += "\t\t\tdocument.getElementById(\"temax\").innerHTML=\"MAX&nbsp;\"+sensorValues[4];}\n\r";
    g += "\t\t}\n\r";
    g += "\t}\n\r";
    g += "</script>\n\r";
    g += "</head>\n\r";
    
    g += "<body onLoad=\"getValues()\">\n\r"; 
    g += "<div style=\"text-align:center\">\n\r";
    
    g += "<div class=\"st\">temperatura</div>\n\r";
    g += "<a id=\"te\" class=\"bo\" style=\"background-color:#ff6600; margin-bottom:8px;\" href=\"?reset\">";
    g += String(t1,1);
    g += "&deg;C</a>\n\r";
    
    g += "<div style=\"display:block\">";
    g += "<div id=\"temin\" class=\"bo\" style=\"background-color:#ffc300; font-size:22px; display:inline-block; width:44%;\">MIN&nbsp;";
    g += String(min_t,1);
    g += "&deg;C</div>";
    g += "<div id=\"temax\" class=\"bo\" style=\"background-color:#ff4300; font-size:22px; display:inline-block; width:44%;\">MAX&nbsp;";
    g += String(max_t,1);
    g += "&deg;C</div>";
    g += "</div>\n\r";
    
    g += "<div class=\"st\">umidit&agrave; relativa</div>\n\r";
    g += "<div id=\"ur\" class=\"bo\" style=\"background-color:#006699\">";
    g += String(h1,1);
    g += "%</div>\n\r";
    
    g += "<div class=\"st\">temperatura percepita</div>\r\n";
    g += "<div id=\"hi\" class=\"bo\" style=\"background-color:#009966\">";
    g += String(hi,1);
    g += "&deg;C</div>\n\r";
    
    g += "<a class=\"bu\" style=\"margin-top:40px;\" href=\"?relay\">RELAY</a>\n\r";
    g += "<a class=\"l\" href=\"http://www.settorezero.com\">www.settorezero.com</a>\n\r";
    g += "</div>";
    g += "</body>\n\r";
    g += "</html>";
    
    client.print(p);
    client.print(g);
    }
  
  Serial.println("Client disconnected");
  Serial.println("");
  delay(10);
  }
