# RelayRemote Demo

This application runs on an ESP8266-based board connected via Wi-Fi in a local network.

Suitable for Linknode R4 board.

You can control status of 4 relay connected on ESP8266 GPIOS 12,13,14 and 16 using http request or using the Android App "Relay Remote" on Google Play Store: [link](https://play.google.com/store/apps/details?id=appinventor.ai_settorezero.RelayRemote).

You can control individual relay from browser using: hxxp://[BOARD_IP]/?Rx=[1 for turning on, 0 for turning off] where x=1,2,3 or 4

Tutorial here: [part1](https://www.settorezero.com/wordpress/il-modulo-esp8266-e-il-nodemcu-devkit-parte-1-introduzione-e-preparazione-ambiente-di-sviluppo-con-arduino-ide/) - [part2](https://www.settorezero.com/wordpress/il-modulo-esp8266-e-il-nodemcu-devkit-parte-2-controlliamo-4-rele-tramite-la-rete-wi-fi-di-casa/)
