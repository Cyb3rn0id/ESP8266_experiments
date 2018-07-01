/*
 * Test Pushover by Giovanni Bernardo (https://www.settorezero.com)
 * Libreria Pushover by Arduino Hannover (https://github.com/ArduinoHannover/Pushover)
 * 
 * Tutti i diritti riservati, è fatto divieto di condividere il codice
 * o parti di esso senza il consenso esplicito dell'autore.
 * 
 * L'utilizzo del seguente esempio è vincolato ai termini di utilizzo:
 * https://www.settorezero.com/wordpress/info/termini-di-utilizzo-e-privacy/
 * 
 */

#include <ESP8266WiFi.h>
#include "Pushover.h"

// IO2 (D4 su NodeMCU Devkit, pin 3 su ESP-01)
// su NodeMCU all' IO2 è anche collegato il led blu del modulo
// per cui premendo il pulsante, vedremo il led accendersi
#define BUTTON 2

// impostare SSID e password della propria rete Wi-Fi
const char* ssid = "[YOUR-SSID]";
const char* password = "[YOUR-PASSPHRASE]";

// istanziare la libreria Pushover fornendo: API Token, User Key
Pushover pushover_button = Pushover("[PUSHOVER-API-TOKEN]","[PUSHOVER-USER-KEY]");

WiFiClient espClient;
 
void setup() 
  {
  Serial.begin(115200);
  pinMode(BUTTON, INPUT_PULLUP);
  setup_wifi();
  }
 
void setup_wifi() 
  {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) 
    {
    delay(500);
    Serial.print(".");
    }
 
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  }
 
 
void loop() 
  {

  // pulsante premuto
  if (digitalRead(BUTTON) == LOW) 
    {
    // attendo 100mS come antibounce e riverifico la pressione del pulsante
    delay(100);
    if ((digitalRead(BUTTON) == LOW)) 
      {
      Serial.println("button pressed");
      // titolo del messaggio. Compare nel pop-up e nell'elenco
      pushover_button.setTitle("Rilevato pulsante");
      // corpo del messaggio
      pushover_button.setMessage("Pulsante premuto!");
      // suono da emettere, l'elenco completo è sul sito di pushover come spiegato nell'articolo su settorezero
      pushover_button.setSound("alien");
      // priorità, di default è 0
      pushover_button.setPriority(1);
      // impostare il device se si vuole inviarlo ad un solo device, altrimenti lasciare commentato per inviare
      // il messaggio a tutti i device associati a pushover
      //pushover_button.setDevice("[PUSHOVER-DEVICE]"); 
      // parametri obbligatori per notifica di emergenza di tipo 2 (che richiede conferma di lettura)
      //pushover_button.setRetry(60);
      //pushover_button.setExpire(500);
      // il metodo .send invia il messaggio e restituisce un valore
      Serial.println(pushover_button.send());
      }
    }
  }
