/*
 * ESP-01 as remote relay controller in AP (Access Point) mode
 * Connect to AP "relay", password = "password"
 * Open browser, visit 192.168.4.1
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

const char *ssid = "relay";
const char *password = "password";

// relay on GPIO2
// relay will turn on putting GPIO2 to low level
#define RELAY 2     // relay on GPIO2
int relayState = 1; // relay off

// html page
const String HtmlHtml = "<html><head>"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" /></head>";
const String HtmlHtmlClose = "</html>";
const String HtmlTitle = "<h1>Relay control</h1><br/>\n";
const String HtmlRelayStateLow = "<big>Il Relay &egrave; <b>acceso</b></big><br/>\n";
const String HtmlRelayStateHigh = "<big>Il Relay &egrave; <b>spento</b></big><br/>\n";
const String HtmlButtons = 
    "<a href=\"RELAYOn\"><button style=\"display: block; width: 100%;\">ON</button></a><br/>"
    "<a href=\"RELAYOff\"><button style=\"display: block; width: 100%;\">OFF</button></a><br/>";

ESP8266WebServer server(80);

// html page requested
void handleRoot() 
  {
  response();
  }

// html page requested with /RELAYon
void turnRelayOn() 
  {
  relayState=0;
  digitalWrite(RELAY,0);
  response();
  }

// html page requested with /RELAYoff
void turnRelayOff() 
  {
  relayState=1;
  digitalWrite(RELAY,1);
  response();
  }

// send html page to client
void response()
  {
  String htmlRes = HtmlHtml + HtmlTitle;
  if(relaySTATE == 0)
    {
    htmlRes += HtmlRelayStateLow;
    }
    else
    {
    htmlRes += HtmlRelayStateHigh;
    }
  htmlRes += HtmlButtons;
  htmlRes += HtmlHtmlClose;
  server.send(200, "text/html", htmlRes);
  }

void setup() 
  {
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, relayState);
  
  delay(1000);
  
  Serial.begin(9600);
  Serial.println();

  WiFi.softAP(ssid, password);

  IPAddress apip = WiFi.softAPIP();
  Serial.print("Board IP address is: \n");
  Serial.println(apip);
  
  // attach functions on client requests
  server.on("/", handleRoot);
  server.on("/RELAYOn", turnRelayOn);
  server.on("/RELAYOff", turnRelayOff);
  
  // start server
  server.begin();
  
  Serial.println("HTTP server started");
  }

void loop() 
  {
  server.handleClient();
  }
