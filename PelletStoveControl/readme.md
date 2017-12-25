# Pellet Stove Control

Code for NodeMCU Devkit (ESP8266) programmed with Arduino IDE.

Remote control is tested on Ungaro Maia Blend 34, I think will work with every stove based on Micronova board with the 5-buttons IR remote command.

!(https://github.com/Cyb3rn0id/ESP8266_experiments/blob/master/PelletStoveControl/docs/webpage1.png)

## STEP 1

In Arduino IDE install required libraries:

* OneWire => https://www.pjrc.com/teensy/td_libs_OneWire.html
* DallasTemperature => https://github.com/milesburton/Arduino-Temperature-Control-Library
* IRremoteESP8266 => https://github.com/markszabo/IRremoteESP8266 by Markszabo

## STEP 2

Mount an 38KHz IR receiver as in schematic and upload the IRrecvDumpV2.ino sketch in the example folder.
From Serial Monitor copy codes given after pressing buttons on the original remote command of the stove

## STEP 3

Edit source code writing your SSID and Passphrase and paste the codes obtained in step 2

## STEP 4

Point the IR led on the stove and try to connect to board with your phone, a pin is required first than send any command, default one is 1234

## STEP 5

Condider to make a donation: http://www.settorezero.com/wordpress/donazioni/


