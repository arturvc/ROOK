![Rook](https://github.com/arturvc/ROOK/raw/master/Rook-logo-250x250.png "Rook")
#### PLAYFUL DATA VISUALIZATION FOR AIR QUALITY

The Rook project is a playful data visualization from the Hollandse Luchten sensors on air quality, using light and water mist. The code in the file **ROOK.ino** is used for Rook to display color and mist based on the hourly data of Particulate Matter 2.5 micrometers (PM25) using the Hollandse Luchten API. For more information about Hollandse Luchten project, visit [hollandseluchten.waag.org](https://hollandseluchten.waag.org/)

To upload the code to Rook you will need to install the ESP32 board in the Arduino IDE. If you don't know how to do it, see [here](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/). The board used in the Rook project is the model DOIT ESP32 DEVKIT V1. 

It is also necessary the libraries:
* WiFi.h - ESP32 WiFi library. This library comes with the ESP32 board package. 
* HTTPClient.h - HTTP library. This library comes with the ESP32 board package.
* ArduinoJson.h - JSON formatter library. This library is available here https://arduinojson.org/v6/doc/installation/ 
* ESPDMX.h - a modified version of the ESP DMX library for the light control, included in this project folder. The original library is for the ESP8266, and Luis Rodil-Fernandez edited it, so we can use it with ESP32. If you want to use the original ESP-DMX library with ESP32, you can edit the ESPDMX.cpp file and change the line: `int sendPin = 2;` to `int sendPin = 10;` that is the serial pin of the ESP32 - version DOIT ESP32 DEVKIT -, and then remove the `*dmxPin*` variable from the *ROOK.ino* code. For more information about the original library, and to downloaded it visit https://github.com/Rickgg/ESP-Dmx

Before uploading, specify the WiFi network in  `const char* ssid = "WiFi-name";`, change the `"WiFi-name"` for the the name of your network, between quotemarks. Then add the password of the WiFi network in `const char* password =  "";`, between quotemarks. If there's no password, leave as it is.

#### Credits and acknowledgement

The Rook project was developed by [Mattia Thibault](http://urban-play.eu/) and [Artur Cordeiro](http://arturvc.net.br) in partnership with Waag, and with the collaboration of Yuri Alexsander, Mar Escarrabill, Luis Rodil Fernandez and Jéssyca Rios.

We acknowledge Lectorate of Play & Civic Media from Faculty of Digital Media and Creative Industry of Amsterdam University of Applied Sciences, for offering spaces and competences for the creation of the project.

This project has received funding from the European Union’s Horizon 2020 research and innovation programme under grant agreement No 723521 and under the Marie Sklodowska-Curie grant agreement No 793835.

We acknowledge São Paulo Research Foundation (FAPESP) for the grant #2018/26544-0.
“The opinions, hypotheses and conclusions or recommendations expressed in this material are the responsibility of the author(s) and do not necessarily reflect the vision of FAPESP”.
