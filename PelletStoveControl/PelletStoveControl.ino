/*
Web Remote control for Ungaro Pellet Stoves based on "Micronova" controller
(Micronova controller has a double led display and 6 pushbuttons)
Tested on Ungaro Maia Blend 34 model

Copyright (c) 2017 Giovanni Bernardo (CYB3rn0id)
http://www.settorezero.com
http://www.facebook.com/settorezero
http://www.twitter.com/settorezero

DESCRIPTION
This application runs on an NodeMCU Devkit connected via Wi-Fi in a local network 
and make same functions such the original IR remote command.
You can turn on/off the stove, set the power and water temperature
You can read the ambient temperature too

PREREQUISITES
- OneWire
- DallasTemperature
- IRremoteESP8266

MIT License

Copyright (c) 2017 Giovanni Bernardo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define BUZZER 16 // Active Buzzer on D0 (GPIO16) driven by BC237
#define ONE_WIRE_BUS 5 // DS1820 on D1 (GPIO5)
IRsend irsend(4); // IR Led on D2 (GPIO4)
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance
DallasTemperature Temperature(&oneWire); // Pass our oneWire reference to Dallas Temperature. 

#define USE_STATIC_IP // comment this if you want address assigned by DHCP

// SSID and Password of your Wi-Fi Network
const char* ssid = "[YOUR SSID]";
const char* password = "[YOUR PASSPHRASE]";

// data used for static IP configuration
#ifdef USE_STATIC_IP
  IPAddress subnet(255,255,255,0);
  IPAddress ip(192,168,1,31);
  IPAddress gateway(192,168,1,1); // router address
#endif

ESP8266WebServer server(80);

// IR remote commands obtained with IRrecvDumpV2.ino for Ungaro Maia 34 Blend pellet stove
// ON/OFF
uint16_t TOGGLE[21] = {6680, 2412,  3374, 1560,  920, 732,  918, 1594,  1720, 758,  894, 1584,  896, 732,  920, 1558,  1748, 758,  1722, 1584,  1720};  // UNKNOWN 9EB58962
// Power Up
uint16_t P_UP[19] = {6778, 2336,  3446, 1510,  938, 712,  938, 1556,  4270, 686,  936, 716,  938, 1540,  3438, 692,  936, 714,  1774};  // UNKNOWN 2B890461
// Power Down
uint16_t P_DN[21] = {6700, 2406,  3366, 1580,  892, 756,  892, 1598,  2562, 732,  918, 726,  922, 696,  948, 1544,  2544, 1558,  904, 734,  1728};  // UNKNOWN BD1A8437
// Temperature Up
uint16_t T_UP[23] = {6710, 2352,  3422, 1562,  894, 732,  918, 1572,  1746, 758,  1720, 758,  892, 734,  924, 1554,  1744, 756,  892, 756,  892, 756,  1736};  // UNKNOWN 4B3AB9D1
// Temperature Down
uint16_t T_DN[19] = {6726, 2390,  3394, 1536,  944, 734,  916, 1576,  3394, 1538,  942, 736,  918, 1534,  4246, 1536,  190, 86,  1602};  // UNKNOWN 56631A11

// Extract one decimal from a float value
int getDecimal(float val)
 {
 int intPart = int(val);
 // I want only 1 decimal
 int decPart=(val-intPart)*10;
 return decPart;
 }

// Return HTML page
String Index_Html(void)
	{
    float T=0;
    String stringTemp = ""; 
    Temperature.requestTemperatures();
    T=Temperature.getTempCByIndex(0);
    stringTemp+=String(int(T))+ "."+String(getDecimal(T));
	
	return
	"<!DOCTYPE HTML>"
	"<html>"
	"<head>"
	"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0\">"
	"<title>Stove Control</title>"
	"<style type=\"text/css\">"
	"body{font-family:arial; font-size:15pt; font-weight:bold;}\n"
	".bu{font-family:arial; font-weight:bold; font-size:28pt; color:#ffffff; text-align:center; padding:8px; margin:4px; border:0; border-radius:15px; box-shadow:0 8px #666666; outline:none;}\n"
	".bu:active{box-shadow:0 3px #333333; transform:translateY(4px);}\n"
	".sm {color:#585858; text-decoration:none; font-family:tahoma,arial; font-size:12pt; font-weight:bold; font-variant:small-caps; text-align:center; padding:8px; margin-top:10px; display:block;}\n"
	"</style>"
	"</head>"
	"<body>"
	"<div style=\"text-align:center\">"
	"<span>CONTROLLO STUFA</span><br/><br/>"
	"<div class=\"bu\" style=\"background-color:#996633; width:93%\">"+stringTemp+"&deg;C</div><br/>"
	"<form action=\"/\" method=\"post\">"
	"<input type=\"submit\" name=\"submit\" value=\"ON/OFF\" class=\"bu\" style=\"background-color:#ff6600; width:98%\"><br/><br/>"
	"<input type=\"submit\" name=\"submit\" value=\"Pow +\" class=\"bu\" style=\"background-color:#00FF00; width:47%;\">"
	"<input type=\"submit\" name=\"submit\" value=\"Temp +\" class=\"bu\" style=\"background-color:#0099FF; width:47%; float:right;\"><br/><br/>"
	"<input type=\"submit\" name=\"submit\" value=\"Pow -\" class=\"bu\" style=\"background-color:#00CC00; width:47%;\">"
	"<input type=\"submit\" name=\"submit\" value=\"Temp -\" class=\"bu\" style=\"background-color:#0099AA; width:47%; float:right;\">"
	"</form>"
	"</div>"
	"<div class=\"sm\">&copy;2017 Giovanni Bernardo</div>"
	"</body>"
	"</html>";
	
	// greater webpages can be sent using following method:
	// taken from https://github.com/esp8266/Arduino/issues/3205
	// server.setContentLength(HTML_PART_1().length() + HTML_PART_2().length());   (or hardcode the length)
	// server.send(200,"text/html",HTML_PART_1()); <---Initial send includes the header
	// server.sendContent((HTML_PART_2()); <--- Any subsequent sends use this function 
	}

void setup(void)
  {
  irsend.begin();
  Serial.begin(115200);
  Temperature.begin();
  pinMode(BUZZER,OUTPUT);
  digitalWrite(BUZZER,LOW);
  
  // workaround
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  
  #ifdef USE_STATIC_IP
    WiFi.config(ip,gateway,subnet);
  #endif
  WiFi.begin(ssid, password);
  
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) 
    {
    delay(500);
    Serial.print(".");
    }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // double beep for indicating Wi-Fi connected
  digitalWrite(BUZZER,HIGH);
  delay(200);
  digitalWrite(BUZZER,LOW);
  delay(200);
  digitalWrite(BUZZER,HIGH);
  delay(200);
  digitalWrite(BUZZER,LOW);
    
  // attach functions on server request
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();
  }

 
void handleRoot()
  {
  if (server.hasArg("submit"))
    {
	Serial.println("Form submitted");
    handleSubmit();
    }
  else 
    {
    server.send(200, "text/html", Index_Html());
    }
  }

void returnFail(String msg)
  {
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(500, "text/plain", msg + "\r\n");
  }

void handleSubmit()
  {
  if (!server.hasArg("submit")) return returnFail("BAD ARGS");
  String toExec=server.arg("submit");
  Serial.println(toExec+" command requested");
  if (toExec=="ON/OFF")
    {
	digitalWrite(BUZZER,HIGH);
    irsend.sendRaw(TOGGLE, 21, 38);
    digitalWrite(BUZZER,LOW);
    server.send(200, "text/html", Index_Html());
    }
  else if (toExec=="Pow +")
    { 
    digitalWrite(BUZZER,HIGH);
    irsend.sendRaw(P_UP, 19, 38);
    digitalWrite(BUZZER,LOW);
    server.send(200, "text/html", Index_Html());
    }
  else if (toExec=="Pow -")
    { 
    digitalWrite(BUZZER,HIGH);
    irsend.sendRaw(P_DN, 21, 38);
    digitalWrite(BUZZER,LOW);
    server.send(200, "text/html", Index_Html());
    }
  else if (toExec=="Temp +")
	{ 
    digitalWrite(BUZZER,HIGH);
	irsend.sendRaw(T_UP, 23, 38);
    digitalWrite(BUZZER,LOW);
    server.send(200, "text/html", Index_Html());
    }
  else if (toExec=="Temp -")
    { 
    digitalWrite(BUZZER,HIGH);
    irsend.sendRaw(T_DN, 19, 38);
    digitalWrite(BUZZER,LOW);
    server.send(200, "text/html", Index_Html());
    }
  else
    {
	Serial.println("Not a valid command");
    returnFail("Bad value");
    }
  }// \HandleSubmit

void returnOK()
  {
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "OK\r\n");
  }

void handleNotFound()
  {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++)
    {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
  server.send(404, "text/plain", message);
  }

void loop(void)
  {
  server.handleClient();
  }
