/*
 * ESP-01 as remote relay controller in AP (Access Point) mode
 * Connect to AP "relay", password = "password"
 * Open browser, visit 192.168.4.1
 * Relay working mode: bistable (every push will invert relay state)
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

const char *ssid = "relay";
const char *password = "password";

// relay on GPIO2
// relay will turn on putting GPIO2 to low level
#define RELAY 2     // relay on GPIO2
// esp-01 has also blue led on GPIO2
int relayState = 1; // relay off

// html page
const String HtmlHtml = "<html><head>"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" />"
    "<style type=\"text/css\">\r\n"
    ".bu {color:white; background-color:#cccccc; text-decoration:none; font-family:tahoma,arial; font-size:28pt; font-weight:bold; text-align:center; padding:8px; margin-top:1px; margin-bottom:12px; display:block; border-radius:15px; box-shadow:0 8px #666666; outline:none;}\r\n"
    ".bu:active {background-color:#999999; box-shadow:0 3px #333333; transform:translateY(4px);}\r\n"
    "a.l:hover, a.l:link, a.l:visited {color:#0099cc; text-decoration:none; font-family:tahoma,arial; font-size:12pt; font-weight:normal; text-align:center; padding:8px; margin-top:50px; display:block;}\r\n"
    "</style>\r\n"
    "</head><body>";
const String HtmlTitle = "<h1>Relay control</h1><br/>\n";
const String HtmlRelayStateLow = "<big>Relay is <b>ON</b></big><br/>\n";
const String HtmlRelayStateHigh = "<big>Relay is <b>OFF</b></big><br/>\n";
const String HtmlButtons = 
    "<a href=\"RELAYOn\" class=\"bu\">ON</a><br/>"
    "<a href=\"RELAYOff\" class=\"bu\">OFF</a><br/>";
const String HtmlHtmlClose = "</body></html>";

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
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY,0);
  response();
  }

// html page requested with /RELAYoff
void turnRelayOff() 
  {
  relayState=1;
  pinMode(RELAY, INPUT);
  //digitalWrite(RELAY,1);
  response();
  }

// send html page to client
void response()
  {
  String htmlRes = HtmlHtml + HtmlTitle;
  if (relayState == 0)
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
  pinMode(RELAY, INPUT);
  
  
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
