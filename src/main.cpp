/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-async-web-server-espasyncwebserver-library/
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include "boot.h"
#include "serverio.h"

WiFiMode mode;

void setup()
{
  // Serial port for debugging purposes, starting filesystem
  Serial.begin(115200);
  LittleFS.begin();

  mode = WiFiModeSwitch();
  //Depending on the result, we either start the AP web server or the MQTT client
  switch (mode)
  {
  case WiFiMode::WIFI_AP:
    setAccessPoint();
    break;
  default:
    Serial.println("YEAH! y'a un truc dans le fichier de config");
    break;
  }
  // Doesn't matter which mode we are in, we start the config server
  setupAndStartConfigServer();
}

void loop()
{
}