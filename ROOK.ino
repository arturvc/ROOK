//  ROOK
//  PLAYFUL DATA VISUALIZATION FOR AIR QUALITY

// The Rook project was developed in 2019 by Mattia Thibault and Artur Cordeiro in partnership with Waag,
// and with the collaboration of Yuri Alexsander, Mar Escarrabill, Luis Rodil-Fernandez and Jéssyca Rios.

// --------------------------

/*
  This code is used for Rook to display color and mist based on the hourly data of Particulate Matter 2.5 micrometers (PM25)
  from the Hollandse Luchten sensor kit.
  For more information about Hollandse Luchten project, visit https://hollandseluchten.waag.org/

  When the machine is turned on, first it connects to the specified WiFi network,
  then access the World Clock API to get the date and time now in Central Europe Standard Time (CET),
  the time is parsed in a start time and end time, and with this time information, and a specified sensor ID,
  Rook calls the Hollandse Luchten API to get the minimum, average and maximum hourly values of PM25
  referred to three hours before.
*/

#include <WiFi.h>  // ESP32 WiFi library. 
#include <HTTPClient.h> // HTTP library.
#include <ArduinoJson.h> // JSON formatter library.


const char* ssid = "WiFi-name";  // WiFi network name.
const char* password =  ""; // WiFi network password.

// Certificate SSL to access https://data.waag.org/api/
// The certificate can be copied from the web browser,
// see https://www.shellhacks.com/get-ssl-certificate-from-server-site-url-export-download/

// This certificate will expire on 2nd Dec 2019.
// ********************************************
const char* root_ca = \
                      "-----BEGIN CERTIFICATE-----\n" \
                      "MIIFUjCCBDqgAwIBAgISA0XQwSzE1uu7ZLjWsLoPXN5FMA0GCSqGSIb3DQEBCwUA\n" \
                      "MEoxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MSMwIQYDVQQD\n" \
                      "ExpMZXQncyBFbmNyeXB0IEF1dGhvcml0eSBYMzAeFw0xOTA5MDQwMzIzMjJaFw0x\n" \
                      "OTEyMDMwMzIzMjJaMBgxFjAUBgNVBAMTDWRhdGEud2FhZy5vcmcwggEiMA0GCSqG\n" \
                      "SIb3DQEBAQUAA4IBDwAwggEKAoIBAQDPVuweTySnn6pv8pDmDga4UsDKM3i7yfG9\n" \
                      "6gRjqVDp9JfyHBe5AWdnO+teNgOgvnGuiaoVmdzXRialdhFAotuWWu0/a7MTeatP\n" \
                      "lnMp3cC5BT8Q9hcH9nlcI4kBanwViZ2Rkj2E9o9hPDws5VsEmqBpsEP/BCAOci/G\n" \
                      "imdCZnJgp5lgrR/pijMv5MT4JH60XLDBBttMZjLgVDaTn6YyKp78TDeWwtl7KKUx\n" \
                      "RRYKLpa3rvNXLyIGZHp3993u8vpvP8diWkNnQhe7F56wU4izlySV8v9j3qgMBrtl\n" \
                      "HehQJGn9JIU66QmFnzRljOFZjkddKGJb1Kmele7G8z3sDSoKQ+uLAgMBAAGjggJi\n" \
                      "MIICXjAOBgNVHQ8BAf8EBAMCBaAwHQYDVR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUF\n" \
                      "BwMCMAwGA1UdEwEB/wQCMAAwHQYDVR0OBBYEFFC0vClw9WTU48uYW+nKzClFuKS5\n" \
                      "MB8GA1UdIwQYMBaAFKhKamMEfd265tE5t6ZFZe/zqOyhMG8GCCsGAQUFBwEBBGMw\n" \
                      "YTAuBggrBgEFBQcwAYYiaHR0cDovL29jc3AuaW50LXgzLmxldHNlbmNyeXB0Lm9y\n" \
                      "ZzAvBggrBgEFBQcwAoYjaHR0cDovL2NlcnQuaW50LXgzLmxldHNlbmNyeXB0Lm9y\n" \
                      "Zy8wGAYDVR0RBBEwD4INZGF0YS53YWFnLm9yZzBMBgNVHSAERTBDMAgGBmeBDAEC\n" \
                      "ATA3BgsrBgEEAYLfEwEBATAoMCYGCCsGAQUFBwIBFhpodHRwOi8vY3BzLmxldHNl\n" \
                      "bmNyeXB0Lm9yZzCCAQQGCisGAQQB1nkCBAIEgfUEgfIA8AB2AHR+2oMxrTMQkSGc\n" \
                      "ziVPQnDCv/1eQiAIxjc1eeYQe8xWAAABbPqDRRsAAAQDAEcwRQIgSMt1CSz7iIOI\n" \
                      "ItstWfnNXHQneBuT7yY1v9O5D8XAVx0CIQDd6gJNVD/kDB75AOGfnGcGM4tiETJe\n" \
                      "7i3RLPNAO2OFDgB2ACk8UZZUyDlluqpQ/FgH1Ldvv1h6KXLcpMMM9OVFR/R4AAAB\n" \
                      "bPqDRQoAAAQDAEcwRQIgRCqhNyUtv3HM2AI1/rhIt4ygIf+k9NdHmy1HCdErB9oC\n" \
                      "IQCNJwJHPRTUzezlaZ6RhJha0sKlsNPnp27I4vatqfv8JzANBgkqhkiG9w0BAQsF\n" \
                      "AAOCAQEAIjJxvgNnuGM/nETulbQvcGwwhrKsXgX8k4SybsD4rGLgakgbJMIIU52v\n" \
                      "yzF0Yix43DoK3HsBBB/YpggPLfM7rDNK5sYpIMUxk4RETwK2n2iZk2HeO5i33LsJ\n" \
                      "jD1f1Bw+mOgOYI1txPrOcTz29jUJj3R+bw/JUvJdpCLaoRWZDrMsnLKbPmhjbr7Y\n" \
                      "oo5F/6NF0nFRDRz3a1fap3kfjnm2ik+J4E9+s4owLwI3x7i6pN1D9pdLlNHjb3k/\n" \
                      "GuPCK5muX5SvMcvyFDX4kEJazUTIZHiPu/1zHvondWfJlScNcdJGdAB5uOpcE3tr\n" \
                      "8IYBsaFRQTjEFTkfuZflAjapNYNFzg==\n" \
                      "-----END CERTIFICATE-----\n";
// ********************************************


/*
  // This certificate will expire on 1st Fev 2020.
  // ********************************************
  const char* root_ca = \
                      "-----BEGIN CERTIFICATE-----\n" \
                      "MIIFUzCCBDugAwIBAgISA0SqhxdY/v3XyPqHzUbpY0LjMA0GCSqGSIb3DQEBCwUA\n" \
                      "MEoxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MSMwIQYDVQQD\n" \
                      "ExpMZXQncyBFbmNyeXB0IEF1dGhvcml0eSBYMzAeFw0xOTExMDMwNDIzMzdaFw0y\n" \
                      "MDAyMDEwNDIzMzdaMBgxFjAUBgNVBAMTDWRhdGEud2FhZy5vcmcwggEiMA0GCSqG\n" \
                      "SIb3DQEBAQUAA4IBDwAwggEKAoIBAQCkl4IabduxMFox8647PU+1MMPjAmPqUrpI\n" \
                      "hZroFsrp8ystFe5nYKBErpxQVnesO7mdVsf4n3YWZv4tymNtQ6aC+TfNsDrWvQGY\n" \
                      "F/zRDDsEjkrE5LxKTNS4E02od9hr1qmdr95I2FwAiSbk4hi24Cq1oyh5RFcLD5gq\n" \
                      "pV4dhP/y9pHS7QUyaQgy2A+V5PZcP6IQfOuPVt8vt5/WMZX0hV6C+ZVXEv9LsGHI\n" \
                      "WMBPmqVKtdXmtJFMB4R11jyaTfbTgQ035pR1A7Aa+cRKuTkH5T9g0hNTA+CWVdOy\n" \
                      "yMyvNDnC3n7Uy16BVImC11Deu0WM0lVRSk/afVpO3p7ek1xftl/vAgMBAAGjggJj\n" \
                      "MIICXzAOBgNVHQ8BAf8EBAMCBaAwHQYDVR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUF\n" \
                      "BwMCMAwGA1UdEwEB/wQCMAAwHQYDVR0OBBYEFNCm5DkIobd161D6ULwTNgQAFElN\n" \
                      "MB8GA1UdIwQYMBaAFKhKamMEfd265tE5t6ZFZe/zqOyhMG8GCCsGAQUFBwEBBGMw\n" \
                      "YTAuBggrBgEFBQcwAYYiaHR0cDovL29jc3AuaW50LXgzLmxldHNlbmNyeXB0Lm9y\n" \
                      "ZzAvBggrBgEFBQcwAoYjaHR0cDovL2NlcnQuaW50LXgzLmxldHNlbmNyeXB0Lm9y\n" \
                      "Zy8wGAYDVR0RBBEwD4INZGF0YS53YWFnLm9yZzBMBgNVHSAERTBDMAgGBmeBDAEC\n" \
                      "ATA3BgsrBgEEAYLfEwEBATAoMCYGCCsGAQUFBwIBFhpodHRwOi8vY3BzLmxldHNl\n" \
                      "bmNyeXB0Lm9yZzCCAQUGCisGAQQB1nkCBAIEgfYEgfMA8QB2APCVpFnyANGCQBAt\n" \
                      "L5OIjq1L/h1H45nh0DSmsKiqjrJzAAABbi+3/+sAAAQDAEcwRQIgKRD/eCnmg/I8\n" \
                      "MvC3mudazMUBHMJsEAVFFZKS2zdgd4sCIQDB6/xm9ot+ZiUpN+MCugIQwZ3R1faP\n" \
                      "M206WsIksC0rTAB3ALIeBcyLos2KIE6HZvkruYolIGdr2vpw57JJUy3vi5BeAAAB\n" \
                      "bi+3/9wAAAQDAEgwRgIhAI3I4Dx/6KnmSrRcraT1N4Z9CsPRYSBePcUS5NAy6clJ\n" \
                      "AiEAuD2MuRZn3T+40UVLJO3gID4WgbYgrRdRfNA5qrinGXowDQYJKoZIhvcNAQEL\n" \
                      "BQADggEBAGdOtAFFWKeHmM9YFIPkjU/vlj1VA2VQe4iixLo51BC5dl+XM5I7a7kd\n" \
                      "HhzDMdexXh/Eea7DoKi28F+5uHkUAmvR2c2V/+VMEgiL0hOgedqyVjgAGhGmFGSb\n" \
                      "W6xssHe/Tp0jgsIdX32iUebY44UBM+tZL5endbM7Z+8LVcnU9I06haxzqRX6ZIwC\n" \
                      "U63d9xcfJ7yK3CU6OpsJ2PupMGnRxWNPTokpEcGLBzOQ6mKRVLDTQZWzKhn5ZYSS\n" \
                      "KMwONIDXCRkT4QPSyh8KOgU00BU0OR6bCm6BIHCC+dsBr6mV1WcOFKnHoAyiBHai\n" \
                      "HOLbvFnTxcQdhWf+dMgJ/NpwLEGT3gM=\n" \
                      "-----END CERTIFICATE-----\n";
  // ********************************************
*/



// Hollandse Luchten API (HL API)
// ********************************************
/*
  In this code we use the HL API to get the data from a specific sensor in 1 (one) hour time length,
  according with this model:
  https://data.waag.org/api/getHourlyValuesSensor?sensor_id=[ID]&start=[STARTTIME]&end=[ENDTIME]
  the [ID] is the sensor ID;
  the [STARTTIME] and the [ENDTIME] are the start date and time and the end date and time, and have the following format,
  the date: 2019-10-24T
  and the time: 13:00:00Z
  For example:
  https://data.waag.org/api/getHourlyValuesSensor?sensor_id=67&start=2019-10-24T10:00:00Z&end=2019-10-24T11:00:00Z
*/

String HLapiCall = ""; // Variable with the URL to call the Hollandse Luchten API.
// The complete URL will be:
//  HLapiCall = url + id + "&start=" + dateNow + startTimeAPI + "&end=" + dateNow + endTimeAPI;

String url = "https://data.waag.org/api/getHourlyValuesSensor?sensor_id="; // Begin of the URL
String id = "67";  // The sensor ID.

// If you want to get the data from other sensor, change the sensor ID.
//  You can see the sensors IDs here: https://hollandseluchten.waag.org/kaart/


String dateNow = ""; // The date today that will be obtained with the World Clock API.
String timeNow = ""; // The time now that will be obtained with the World Clock API.

// The timeNow will be parsed to the start and end time, respectively, to the *startTimeAPI* and *endTimeAPI*.

String startTimeAPI = ""; // The start time with a difference of 3 hours from now (see the value of the variable *subtraction*).
String endTimeAPI = ""; // The end time with a differece of 1 hour from the start time.
int subtraction = 3; // Hour offset to be used as the start time in the HL API.

/*
  For some reason, the available data from the HL API is only available within 2 hours delay.
  For example, if now it's 16:28, the last data available is from 14:00.
  But there's an issue when the time is 10:00, 11:00, XX:00, o'clock time, and it only shows data within 3 hours delay.
  That's why the *subtraction* variable is equal to 3.
*/


// World Clock API
// ********************************************
// API to get the time now in the time zone "Central Europe Standard Time"
String urlTime = "http://worldclockapi.com/api/json/cet/now";


// DMX module and light control
// ********************************************
#include <ESPDMX.h> // DMX library for the light control.
DMXESPSerial dmx; // Instance of the library
int dmxPin = 10;
/*
  In this code we use a modified version of the ESP-DMX library - Copyright (C) 2015 Rick <ricardogg95@gmail.com>
  The original library is for the ESP8266, and Luis Rodil-Fernandez edited it, so we can use it with ESP32.

  If you want to use the original ESP-DMX library with ESP32, you can edit the ESPDMX.cpp file and change the line:
  int sendPin = 2;
  to
  int sendPin = 10;
  that is the serial pin of the ESP32 - version DOIT ESP32 DEVKIT -,
  and then remove the *dmxPin* variable from this code.

  For more information of the original library, visit https://github.com/Rickgg/ESP-Dmx

  The RGB light used in Rook (model Ibiza Light PAR-MINI-RGB3) has 7 DMX channels:
  the channel 4 is the brightness;
  the channel 5 is the red light;
  the channel 6 is the green light;
  the channel 7 is the blue light.
  For each channel, the values range from 0 to 255.

  For example, to set a bright red color,
  first it's necessary to set the brightness, otherwise no light will appear,
  the channel 4, brightness, is set to 255;
  the channel 5, red, is set to 255;
  the channel 6, green, is set to 0;
  the channel 7, blue, is set to 0.

  The code will be:
  dmx.write( 4, 255); // Brightness channel.
  dmx.write( 5, 255); // Red channel.
  dmx.write( 6, 0); // Green channel.
  dmx.write( 7, 0); // Blue channel.
  dmx.update(); // Update the DMX bus - This function is required to update the changes.
*/



float minPollution; // Variable to save the minimum value of PM25 from the HL API.
float meanPollution; // Variable to save the average (mean) value of PM25 from the HL API.
float maxPollution; // Variable to save the maximum value of PM25 from the HL API.

int colorMin[3] = {0, 0, 0}; // The RGB array of the minimum color value for the light.
int colorMean[3] = {0, 0, 0}; // The RGB array of the average color value for the light.
int colorMax[3] = {0, 0, 0}; // The RGB array of the maximum color value for the light.

int relayMist = 18; // The output pin in the ESP32 to control the Mist Maker ON/OFF.
int relayWind = 19; // The output pin in the ESP32 control the Ventilator ON/OFF.

int button1 = 21; // The input pin in the ESP32 for the Button 1.
int button2 = 22; // The input pin in the ESP32 for the Button 2.
int button3 = 23; // The input pin in the ESP32 for the Button 3.

unsigned long tempoTime; // Variable to control the request time to access the HL API and World Clock API.
int waiting = 60000; // Variable that defines the time interval to access the APIs, in milliseconds.

int buttonNumber; // Variable that tracks the last button pressed, the active button.
boolean button1Pressed = false; // Boolean variable to indicate if the Button 1 was pressed.
boolean button2Pressed = false; // Boolean variable to indicate if the Button 1 was pressed.
boolean button3Pressed = false; // Boolean variable to indicate if the Button 1 was pressed.
boolean countTime = true; // Boolean variable to control the time counting for the variable *tempoButton*.
unsigned long tempoButton; // Variable of time to activate the Mist Maker and the Ventilator after a button has been pressed.



void setup() {

  Serial.begin(9600); // Start the Serial library - for debugging purpose only.

  dmx.init(dmxPin); // Start the ESP-DMX library and set the pin 10 of the ESP32 for serial communication with the DMX module.
  dmx.write( 4, 255);  // Set the brightness, channel 4, of RGB light to 255.
  dmx.update(); // Update the DMX bus.

  pinMode(button1, INPUT); // Set the Button 1 as input.
  pinMode(button2, INPUT); // Set the Button 2 as input.
  pinMode(button3, INPUT); // Set the Button 3 as input.
  pinMode(relayMist, OUTPUT); // Set the relay that controls the Mist Maker as output.
  pinMode(relayWind, OUTPUT); // Set the relay that controls the Ventilator as output.
  digitalWrite(relayMist, LOW); // Set the relay that controls the Mist Maker as OFF.
  digitalWrite(relayWind, LOW); // Set the relay that controls the Ventilator as OFF.


  // WiFi connection
  // ********************************************
  WiFi.begin(ssid, password); // Begin the WiFi connection.

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // When it's connected it will display in the Serial Monitor the name of the WiFi network.
  Serial.print("Connected to the WiFi network: ");
  Serial.print(ssid);
  Serial.println("!");

  // Then it will flash 10 times a white light
  for (int i = 0; i < 10; i++) {
    dmx.write(5, 50);
    dmx.write(6, 50);
    dmx.write(7, 50);
    dmx.update();
    delay(50);
    dmx.write(5, 0);
    dmx.write(6, 0);
    dmx.write(7, 0);
    dmx.update();
    delay(50);
  }




  // Set the light brightness to 0, the light is turned OFF.
  dmx.write( 4, 0); // Set the brightness, channel 4, of RGB light to 0.
  dmx.update(); // Update the DMX bus.

  delay(3000);

  /*
    The function hlAPI() updates the variables *minPollution*, *meanPollution*, *maxPollution* with values of PM25.

    The function getRGBColorFromSensorValue() map the data value of PM25, that ranges from 0 to 150,
    and convert it to a RGB array, which is used to set the color of the light.

  */

  timeAPI(); // Call the World Clock API.
  hlAPI(); // Call the Hollandse Luchten API.
  getRGBColorFromSensorValue(minPollution, colorMin); // Convert the PM25 minimum value in RGB color.
  getRGBColorFromSensorValue(meanPollution, colorMean); // Convert the PM25 average value in RGB color.
  getRGBColorFromSensorValue(maxPollution, colorMax); // Convert the PM25 maximum value in RGB color.

  buttonNumber = 0; // Set the last button pressed to 0, so the light, the Mist Maker and the Ventilator are OFF.
}


void loop() {

  dmx.write( 4, 255); // Set the brightness, channel 4, of RGB light to 255.
  dmx.update(); // Update the DMX bus.


  // If it is connected to the WiFi network, it will run these functions and commands
  // ********************************************
  if ((WiFi.status() == WL_CONNECTED)) { // Check the current connection status.

    /*
      If no button have been pressed, and the time interval is greater than the *waiting* variable (60 seconds),
      Rook will access the World Clock API and HL API, and then update the minimum, average and maximum values of PM25.
    */
    if (!button1Pressed && !button2Pressed && !button3Pressed) {
      if (millis() - tempoTime > waiting) {
        Serial.println("Running the API...");
        timeAPI();
        hlAPI();
        getRGBColorFromSensorValue(minPollution, colorMin);
        getRGBColorFromSensorValue(meanPollution, colorMean);
        getRGBColorFromSensorValue(maxPollution, colorMax);
      }
      else {
      }
    }



    // Check if a button has been pressed.
    // ********************************************

    // If Button 1 is pressed AND Button 2 AND Button 3 have not been pressed.
    if (digitalRead(button1) == 1 && button2Pressed == false && button3Pressed == false) {
      // If *countTime* is true.
      if (countTime) {
        countTime = false; // Set the *countTime* to false.
        tempoButton = millis(); // Reset the *tempoButton* to the current elapsed time of the ESP32.
      }

      button1Pressed = true; // Set the Button 1 as pressed.
      buttonNumber = 1; // Set the active button to the number 1.
    }
    // If Button 2 is pressed AND Button 1 AND Button 3 have not been pressed.
    else if (digitalRead(button2) == 1 && button1Pressed == false && button3Pressed == false) {
      // If *countTime* is true.
      if (countTime) {
        countTime = false; // Set the *countTime* to false.
        tempoButton = millis(); // Reset the *tempoButton* to the current elapsed time of the ESP32.
      }

      button2Pressed = true; // Set the Button 2 as pressed.
      buttonNumber = 2; // Set the active button to the number 2.
    }
    // If Button 3 is pressed AND Button 1 AND Button 2 have not been pressed.
    else if (digitalRead(button3) == 1 && button1Pressed == false && button2Pressed == false) {
      // If *countTime* is true.
      if (countTime) {
        countTime = false; // Set the *countTime* to false.
        tempoButton = millis(); // Reset the *tempoButton* to the current elapsed time of the ESP32.
      }
      button3Pressed = true; // Set the Button 3 as pressed.
      buttonNumber = 3; // Set the active button to the number 3.
    }


    // If Button 1 OR Button 2 OR Button 3 is set to pressed.
    if (button1Pressed || button2Pressed || button3Pressed) {
      // If it has passed more than 25 seconds since a button has been pressed.
      if (millis() - tempoButton > 25000) {
        buttonNumber = 0; // Set the active button to the number 0.
        button1Pressed = false; // Set the Button 1 as not pressed.
        button2Pressed = false; // Set the Button 2 as not pressed.
        button3Pressed = false; // Set the Button 3 as not pressed.
        countTime = true; //  Set the *countTime* to true, so it can reset the *tempoButton* when a button is pressed.
      }
      // If it has passed more than 15 seconds since a button has been pressed.
      else if (millis() - tempoButton > 15000) {
        buttonNumber = 5; // Set the active button to the number 5.
      }
      // If it has passed more than 10 seconds since a button has been pressed.
      else if (millis() - tempoButton > 10000) {
        buttonNumber = 4; // Set the active button to the number 4.
      }
    }

    // Accordingly with the active button number, Rook will turn ON or OFF the light, the Mist Maker and the Ventilator.
    switch (buttonNumber) {

      // Case the *buttonNumber* is 0,
      case 0:
        colorLight(0, 0, 0); // The light is OFF.
        digitalWrite(relayMist, LOW); // The Mist Maker is OFF.
        digitalWrite(relayWind, LOW); // The Ventilator is OFF.
        break;

      // Case the *buttonNumber* is 1,
      case 1:
        colorLight(colorMin[0], colorMin[1], 0); // The light is set to the minimum value color of PM25.
        digitalWrite(relayMist, HIGH); // The Mist Maker is ON.
        digitalWrite(relayWind, LOW); // The Ventilator is OFF.
        break;

      // Case the *buttonNumber* is 2,
      case 2:
        colorLight(colorMean[0], colorMean[1], 0); // The light is set to the average value color of PM25
        digitalWrite(relayMist, HIGH); // The Mist Maker is ON.
        digitalWrite(relayWind, LOW); // The Ventilator is OFF.
        break;

      // Case the *buttonNumber* is 3,
      case 3:
        colorLight(colorMax[0], colorMax[1], 0); // The light is set to the maximum value color of PM25
        digitalWrite(relayMist, HIGH); // The Mist Maker is ON.
        digitalWrite(relayWind, LOW); // The Ventilator is OFF.
        break;

      // Case the *buttonNumber* is 4,
      case 4:
        digitalWrite(relayMist, HIGH); // The Mist Maker is ON.
        digitalWrite(relayWind, HIGH); // The Ventilator is ON.
        break;

      // Case the *buttonNumber* is 5,
      case 5:
        digitalWrite(relayMist, LOW); // The Mist Maker is OFF.
        digitalWrite(relayWind, HIGH); // The Ventilator is ON.
        break;
    }
  }
}



// *****************************************************************************************************
/*
  -------------------------------------------------------------------------------------------
  Function to map the PM25 value to RGB array for the light color.
  Developed with the collaboration of Yuri Alexsander.

  The function getRGBColorFromSensorValue() map the data value of PM25, that ranges from 0μg/m³ to 150μg/m³,
  and convert it to a RGB array, which is used to set the color of the light.

  The light color exhibited is equivalent to the color displayed in the Hollandse Luchten map:
  https://hollandseluchten.waag.org/kaart

  Observation: the blue color is not used to represent the PM25 value.

  It ranges from green, 0μg/m³, to red, 150μg/m³.
  When its below 75μg/m³ the color is “greenish”,
  if it is 75μg/m³ the color is yellow,
  and above 75μg/m³ the color is “reddish”.
  Any value above 150μg/m³ will be red,
  and any value below 0μg/m³ will be green,
*/

int  * getRGBColorFromSensorValue(int value, int rgb[3]) {

  /*
    The *value* is the variable of the PM25 value,
    the array *rgb* corresponds to the three values for the RGB color:
    rgb[0] the first value of the array is the intensity of red color, it ranges from 0 to 255;
    rgb[1] the second value of the array is the intensity of green color, it ranges from 0 to 255;
    rgb[2] the third value of the array is the intensity of blue color, it ranges from 0 to 255;
  */

  // If the PM25 value is greater than 75, the color will be reddish.
  if (value > 75) {
    // In case of a value above the maximum of 150, the color is set to red.
    if (value > 150) {
      rgb[0] = 255; // Full intensity of red.
      rgb[1] = 0; // Zero intensity of green.
    }
    // If the value is not greater than 150, the green intensity is mapped accordingly with the PM25 value.
    else {
      rgb[0] = 255; // Full intensity of red.
      rgb[1] = map(value, 75, 150, 255, 0); // The green intensity is mapped accordingly with the PM25 value.
    }
  }

  // If the PM25 value is less than 75, the color will be greenish.
  else if (value < 75) {
    // In case of a value below the minimum of 0, the color is set to green.
    if (value < 0) {
      rgb[0] = 0; // Zero intensity of red.
      rgb[1] = 255; // Full intensity of green.
    }
    // If the value is not less than 0, the red intensity is mapped accordingly with the PM25 value.
    else {
      rgb[0] = map(value, 0, 75, 0, 255); // The red intensity is mapped accordingly with the PM25 value.
      rgb[1] = 255; // Full intensity of green.
    }
  }
  // If the PM25 value is equal to 75, the color will be yellow.
  else if (value == 75) {
    rgb[0] = 255; // Full intensity of red.
    rgb[1] = 255; // Full intensity of green.
  }
  else {
  }
  return rgb;
  /*
    The fuction returns the RGB array:
    rgb[0] value with the intensity of red;
    rgb[1] value with the intensity of green;
    rgb[2] value would be with the intensity of blue but it is not used in this fuction,
    because the blue color is not used to represent the PM25 value.
  */
}

// *****************************************************************************************************
/*
  -------------------------------------------------------------------------------------------
  Function of the RGB Light using the ESP-DMX library.
*/
void colorLight (int colorR, int colorG, int colorB) {
  dmx.write(5, colorR); // Write the value to the red channel, it ranges from 0 to 255.
  dmx.write(6, colorG); // Write the value to the green channel, it ranges from 0 to 255.
  dmx.write(7, colorB); // Write the value to the blue channel, it ranges from 0 to 255.
  dmx.write(4, 255);  // Set the brightness, channel 4, of RGB light to 255.
  dmx.update(); // Update the DMX bus.
}




// *****************************************************************************************************
/*
  -------------------------------------------------------------------------------------------
  World Clock API
  Fuction to access the API to get the time now in the time zone "Central Europe Standard Time".
  http://worldclockapi.com/api/json/cet/now
*/
void timeAPI() {
  tempoTime = millis(); // Reset the *tempoTime* to the current elapsed time of the ESP32.
  digitalWrite(relayMist, LOW); // Set the Mist Maker OFF.
  digitalWrite(relayWind, LOW); // Set the Ventilator OFF.
  dmx.write( 4, 255); // Set the brightness, channel 4, of RGB light to 255.
  dmx.update(); //  Update the DMX bus.

  // Flash 5 times a blue light.
  for (int i = 0; i < 5; i++) {
    dmx.write(5, 0); // Zero intensity of red.
    dmx.write(6, 0); // Zero intensity of green.
    dmx.write(7, 50); // Value 50 as intensity of blue.
    dmx.update();  // Update the DMX bus.
    delay(50);
    dmx.write(5, 0); // Zero intensity of red.
    dmx.write(6, 0); // Zero intensity of green.
    dmx.write(7, 0); // Zero intensity of blue.
    dmx.update(); // Update the DMX bus.
    delay(50);
  }

  HTTPClient httpTime; // Create a instance of the HTTPClient library.
  httpTime.begin(urlTime); //Specify the URL.
  int httpCodeTime = httpTime.GET(); // Make the request to the World Clock API.


  // Check for the returning code.
  if (httpCodeTime > 0) {
    String payloadTime = httpTime.getString(); // Save the content of the get request to the variable *payloadTime*.
    Serial.println("**************");
    Serial.print("HTTP Status Code Time API: ");
    Serial.println(httpCodeTime);
    // Serial.println(payloadTime);


    /*
      JSON parsing with the Arduino JSON library.
      For more information see https://arduinojson.org/v6/assistant
    */
    const size_t capacityTime = JSON_OBJECT_SIZE(9) + 230;
    DynamicJsonDocument doc(capacityTime);

    deserializeJson(doc, payloadTime);
    const char* currentDateTime = doc["currentDateTime"]; // "2019-10-08T13:30+02:00"
    Serial.print("Current date and time: ");
    Serial.println(currentDateTime);


    // Parsing the date and time obtained in the World Clock API to the variables used in the HL API.
    if (currentDateTime != 0) {
      dateNow = currentDateTime;
      timeNow = currentDateTime;
      dateNow = dateNow.substring(0, 10) + "T";
      timeNow = timeNow.substring(11, 13);
      int timeNowInt = timeNow.toInt();
      timeNowInt -= subtraction;
      int timeEnd = timeNowInt + 1;
      if (timeNowInt < 10) {
        startTimeAPI = "0" + String(timeNowInt);
      }
      else {
        startTimeAPI = String(timeNowInt);
      }
      if (timeEnd < 10) {
        endTimeAPI = "0" + String(timeEnd);
      }
      else {
        endTimeAPI = String(timeEnd);
      }
      startTimeAPI = startTimeAPI + ":00Z";
      endTimeAPI = endTimeAPI + ":00Z";
    }
    // --------------------------
    Serial.print("Time API");
    Serial.print(" | dateAPI : ");
    Serial.print(dateNow);
    Serial.print(" | startAPI : ");
    Serial.print(startTimeAPI);
    Serial.print(" | endAPI : ");
    Serial.println(endTimeAPI);
  }

  else {
    Serial.println("Error on HTTP request");
  }
  httpTime.end(); //Free the resources
}


// *****************************************************************************************************
/*
    -------------------------------------------------------------------------------------------
  Hollandse Luchten API
  Function to access the API to get data of PM25 from the Hollandse Luchen sensor kit.
*/
void hlAPI() {
  HTTPClient http; // Create a instance of the HTTPClient library.
  HLapiCall = url + id + "&start=" + dateNow + startTimeAPI + "&end=" + dateNow + endTimeAPI; // Update the URL string.
  http.begin(HLapiCall, root_ca); // Specify the URL and certificate

  int httpCode = http.GET(); // Make the request to the HL API.

  // Check for the returning code.
  if (httpCode > 0) {
    String payload = http.getString(); // Save the content of the get request to the variable *payload*.
    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode);
    //Serial.println(payload);

    /*
      JSON parsing with the Arduino JSON library.
      For more information see https://arduinojson.org/v6/assistant
    */
    const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(20) + 240;
    DynamicJsonDocument doc(capacity);

    //const char* json = "[{\"time\":\"2019-09-25T14:00:00.000Z\",\"pm25_mean\":6.072500000000001,\"pm25_min\":4.69,\"pm25_max\":7.1,\"pm10_mean\":11.175,\"pm10_min\":10.5,\"pm10_max\":12,\"no2a_mean\":null,\"no2a_min\":null,\"no2a_max\":null,\"no2b_mean\":null,\"no2b_min\":null,\"no2b_max\":null,\"t_out_mean\":0,\"t_out_min\":0,\"t_out_max\":0,\"h_out_mean\":0,\"h_out_min\":0,\"h_out_max\":0,\"id\":\"90\"}]";
    //deserializeJson(doc, json);

    deserializeJson(doc, payload);

    JsonObject root_0 = doc[0];
    const char* root_0_time = root_0["time"]; // "2019-09-25T14:00:00.000Z"
    float root_0_pm25_mean = root_0["pm25_mean"]; // 6.072500000000001
    float root_0_pm25_min = root_0["pm25_min"]; // 4.69
    float root_0_pm25_max = root_0["pm25_max"]; // 7.1
    //      const char* root_0_id = root_0["id"]; // "90"

    meanPollution = root_0_pm25_mean; // Update the PM25 average value to the variable *meanPollution*.
    minPollution = root_0_pm25_min; // Update the PM25 minimum value to the variable *minPollution*.
    maxPollution = root_0_pm25_max; // Update the PM25 maximum value to the variable *maxPollution*.

    Serial.println("");
    Serial.print("Time: ");
    Serial.println(root_0_time);
    Serial.print(" | PM25 min: ");
    Serial.print(root_0_pm25_min);
    Serial.print(" | PM25 mean: ");
    Serial.print(root_0_pm25_mean);
    Serial.print(" | PM25 max: ");
    Serial.println(root_0_pm25_max);
  }

  else {
    Serial.println("Error on HTTP request");
  }

  http.end(); //Free the resources
}
