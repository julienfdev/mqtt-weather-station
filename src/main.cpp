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
#include "mqttHelpers.h"
#include <PubSubClient.h>

WiFiMode mode;

// MQTT handling
bool wifiReady = false;
WiFiClient espClient;
PubSubClient client(espClient);

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  char str[length + 1];
  unsigned int i = 0;
  for (i = 0; i < length; i++)
  {
    str[i] = (char)payload[i];
  }
  str[i] = 0; // Null termination
  Serial.println("Got message from broker :");
  Serial.println(String(str));
}

void setup()
{
  String stringAddress;
  pinMode(D0, OUTPUT);
  digitalWrite(D0, LOW);
  // Serial port for debugging purposes, starting filesystem
  Serial.begin(115200);
  delay(100);
  LittleFS.begin();
  mode = WiFiModeSwitch();
  //Depending on the result, we either start the AP web server or the MQTT client
  switch (mode)
  {
  case WiFiMode::WIFI_AP:
    setAccessPoint();
    break;
  default:
    Serial.println("Config file found, Swithing to Station Mode");
    if (wiFiConnect())
    {
      Serial.print("MQTT Server address : ");
      Serial.println();
      wifiReady = true;
      randomSeed(micros()); // seeding to get random client id
      client.setServer(getJsonField("mqttIp"), 1883);
      client.setCallback(mqttCallback);
    }
    break;
  }
  // Doesn't matter which mode we are in, we start the config server
  setupAndStartConfigServer();
}

void loop()
{
  if (wifiReady)
  {
    // mqtt loop logic
    if (!client.connected())
    {
      reconnect(client);
    }
    client.loop();
  }
}