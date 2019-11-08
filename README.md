![Rook](https://github.com/arturvc/ROOK/raw/master/Rook-logo-250x250.png "Rook")
#### PLAYFUL DATA VISUALIZATION FOR AIR QUALITY


The Rook project was developed in 2019 by Mattia Thibault and Artur Cordeiro in partnership with Waag, and with the collaboration of Yuri Alexsander, Mar Escarrabill, Luis Rodil-Fernandez and Jéssyca Rios.

This code is used for Rook to display color and mist based on the hourly data of Particulate Matter 2.5 micrometers (PM25) from the Hollandse Luchten sensor kit. For more information about Hollandse Luchten project, visit [hollandseluchten.waag.org](https://hollandseluchten.waag.org/)

To upload the code to Rook you will need to install the ESP32 board in the Arduino IDE. If you don't know how to do it, see [here](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/). The board used in the Rook project is the model DOIT ESP32 DEVKIT V1.

It is also necessary the libraries:
* WiFi.h - ESP32 WiFi library. 
* HTTPClient.h - HTTP library.
* ArduinoJson.h - JSON formatter library.
* ESPDMX.h - a modified version of the ESP DMX library for the light control, included in this project folder.

