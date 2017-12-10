// librerie:
// DallasTemperature
// OneWire
// IRremoteESP8266

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <OneWire.h>
#include <DallasTemperature.h>

IRsend irsend(4);  // IR Led on GPIO 4 (D2 on NodeMCU)

#define USE_STATIC_IP // comment this if you want address assigned by DHCP

const char* ssid = "SSID";
const char* password = "PASSWORD";

// data used for static IP configuration
#ifdef USE_STATIC_IP; // router address
  IPAddress subnet(255,255,255,0);
  IPAddress ip(192,168,1,31);
  IPAddress gateway(192,168,1,1);
#endif

ESP8266WebServer server(80);

// Settings for OneWire library
#define ONE_WIRE_BUS 5 // D1 on NodeMCU
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature Temperature(&oneWire);

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

//function to extract decimal part of float
long getDecimal(float val)
  {
  int intPart = int(val);
  long decPart = 1000*(val-intPart); //I am multiplying by 1000 assuming that the foat values will have a maximum of 3 decimal places. 
                                    //Change to match the number of decimal places you need
  // I want only 1 decimal
  if (decPart!=0)
    {
    decPart=decPart/1000;  
    }
  if(decPart>0)return(decPart);           //return the decimal part of float number if it is available 
  else if(decPart<0)return((-1)*decPart); //if negative, multiply by -1
  else if(decPart=0)return(00);           //return 0 if decimal part of float number is not available
  }

String Index_Html(void)
  {
    float T=0;
    String stringVal = ""; 
    Temperature.requestTemperatures();
    T=Temperature.getTempCByIndex(0);
    stringVal+=String(int(T))+ "."+String(getDecimal(T));
return
"<!DOCTYPE HTML>"
"<html>"
"<head>"
"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0\">"
"<title>Stove Control</title>"
"<style type=\"text/css\">"
"body {font-family:arial; font-size:15pt; font-weight:bold;}\n"
".bu {font-family:arial; font-weight:bold; font-size:28pt; color:#ffffff; text-align:center; padding:8px; margin:4px; border:0; border-radius:15px; box-shadow:0 8px #666666; outline:none;}\n"
".bu:active {box-shadow:0 3px #333333; transform:translateY(4px);}\n"
".c {width:100%; margin-bottom:10px; padding-top:5px;}\n"
"</style>"
"</head>"
"<body>"
"<div style=\"text-align:center\">"
"<span>CONTROLLO STUFA</span><br/><br/>"
"<div class=\"bu\" style=\"background-color:#996633; width:93%\">"+stringVal+"&deg;C</div><br/>"
"<form action=\"/\" method=\"post\">"
"<input type=\"submit\" name=\"submit\" value=\"ON/OFF\" class=\"bu\" style=\"background-color:#ff6600; width:98%\"><br/><br/>"
"<input type=\"submit\" name=\"submit\" value=\"Pow +\" class=\"bu\" style=\"background-color:#00FF00; width:47%;\">"
"<input type=\"submit\" name=\"submit\" value=\"Temp +\" class=\"bu\" style=\"background-color:#0099FF; width:47%; float:right;\"><br/><br/>"
"<input type=\"submit\" name=\"submit\" value=\"Pow -\" class=\"bu\" style=\"background-color:#00CC00; width:47%;\">"
"<input type=\"submit\" name=\"submit\" value=\"Temp -\" class=\"bu\" style=\"background-color:#0099AA; width:47%; float:right;\">"
"</form>"
"</div>"
"</body>"
"</html>";
}

void setup(void)
  {
  irsend.begin();
  Serial.begin(115200);
  Temperature.begin();
  
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
  
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();
  }
  
void handleRoot()
  {
  if (server.hasArg("submit"))
    {
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
  if (toExec=="ON/OFF")
      {
      irsend.sendRaw(TOGGLE, 21, 38);
      server.send(200, "text/html", Index_Html());
      }
  else if (toExec=="Pow +")
      { 
      irsend.sendRaw(P_UP, 19, 38);
      server.send(200, "text/html", Index_Html());
      }
  else if (toExec=="Pow -")
      { 
      irsend.sendRaw(P_DN, 21, 38);
      server.send(200, "text/html", Index_Html());
      }
  else if (toExec=="Temp +")
      { 
      irsend.sendRaw(T_UP, 23, 38);
      server.send(200, "text/html", Index_Html());
      }
  else if (toExec=="Temp -")
      { 
      irsend.sendRaw(T_DN, 19, 38);
      server.send(200, "text/html", Index_Html());
      }
  else
      {
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
