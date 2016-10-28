/*
 * R4WebRemote - ESP8266 Side
 * This application runs on an ESP8266-based board (such as the Linksprite Linknode R4)
 * connected via Wi-Fi in an already existent wi-fi network
 * You can control status of 4 relay connected on pins 12,13,14 and 16 using http request
 * or using the provided Android App
 * 
 * (C)2016 Bernardo Giovanni
 * http://www.settorezero.com
 *  
 */

#include <ESP8266WiFi.h>

// put here your wi-fi network data
const char* ssid = "[your SSID]";
const char* password = "[your password]";

// relays on Linknode R4
#define Relay1  16  // S2
#define Relay2  14  // S3
#define Relay3  12  // S4
#define Relay4  13  // S5
// comment this if you don't want to use debug
#define DEBUG

int RelayStatus[4]; // Here I'll store the status of 4 digital pins
WiFiServer server(80);
 
void setup() 
  {
  #ifdef DEBUG
    Serial.begin(115200);
    delay(10);
  #endif
  
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
 
  while (WiFi.status() != WL_CONNECTED) 
    {
    delay(500);
    #ifdef DEBUG
      Serial.print(".");
    #endif
    }

  #ifdef DEBUG
    Serial.println("");
    Serial.println("WiFi connected");
  #endif
  
  server.begin(); // start server
  
  #ifdef DEBUG
    Serial.println("Server started");
    Serial.print("Use this URL to connect: ");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");
  #endif
  } // end setup
 
void loop() 
  {
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
  
  while(!client.available())
    {
    delay(1);
    }
 
  // get the request until the carriage return
  String request = client.readStringUntil('\r');
  #ifdef DEBUG
    Serial.println(request);
  #endif
  
  client.flush();

  // request contains instructions for Relay1 ?
  if (request.indexOf("/R1=1") != -1)  
    {
    digitalWrite(Relay1, HIGH);
    RelayStatus[0] = HIGH;
    }
  if (request.indexOf("/R1=0") != -1)  
    {
    digitalWrite(Relay1, LOW);
    RelayStatus[0] = LOW;
    }
  // request contains instructions for Relay2 ?
  if (request.indexOf("/R2=1") != -1)  
    {
    digitalWrite(Relay2, HIGH);
    RelayStatus[1] = HIGH;
    }
  if (request.indexOf("/R2=0") != -1)  
    {
    digitalWrite(Relay2, LOW);
    RelayStatus[1] = LOW;
    }
  // request contains instructions for Relay3 ?
  if (request.indexOf("/R3=1") != -1)  
    {
    digitalWrite(Relay3, HIGH);
    RelayStatus[2] = HIGH;
    }
  if (request.indexOf("/R3=0") != -1)  
    {
    digitalWrite(Relay3, LOW);
    RelayStatus[2] = LOW;
    }
  // request contains instructions for Relay4 ?
  if (request.indexOf("/R4=1") != -1)  
    {
    digitalWrite(Relay4, HIGH);
    RelayStatus[3] = HIGH;
    }
  if (request.indexOf("/R4=0") != -1)  
    {
    digitalWrite(Relay4, LOW);
    RelayStatus[3] = LOW;
    }

  // now I can give a 200/OK reponse 
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); // THIS IS IMPORTANT!
  client.println("<!DOCTYPE HTML>");
  client.println("<head><title>R4WebRemote By Bernardo Giovanni</title>");
  // print string "status:xxxx" used by android app to set current relay status on the interface
  // I'm using a meta attribute so this string will not visible on the generated webpage
  client.println("<meta name=\"status\" content=\"status:");
  for (int u=0; u<4; u++)
    {
    RelayStatus[u]?client.print("1"):client.print("0");  
    }
  client.println("\">");
  client.println("<html><body>");
  // rows with relay status
  for (int i=0; i<4; i++)
    {
    // Relay X is XXX : Turn it YYY 
    client.print("<div>Relay ");
    client.print(i,DEC);
    client.print(" is ");
    RelayStatus[i]?client.print("ON"):client.print("OFF");
    client.print("&nbsp;<a href=\"R");
    client.print(i,DEC);
    client.print("=");
    RelayStatus[i]?client.print("0"):client.print("1");
    client.print("\"> : Turn it ");
    RelayStatus[i]?client.print("OFF"):client.print("ON");
    client.print("</a></div>");
    }
  client.println("</body></html>");
  delay(1);
  #ifdef DEBUG
    Serial.println("Client disconnected");
    Serial.println("");
  #endif
  }
