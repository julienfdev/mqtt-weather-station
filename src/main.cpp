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
  Serial.print("Got message from broker on topic ");
  Serial.println(topic);
}
void reconnect()
{
  const char *topic = getJsonField("mqttTopic");
  String stringTopic = String(topic);
  const char *login = getJsonField("mqttLogin");
  String stringLogin = String(login);
  const char *password = getJsonField("mqttPassword");
  String stringPassword = String(password);
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "wxmonitor-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), stringLogin.c_str(), stringPassword.c_str()))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(stringTopic.c_str(), "hello world");
      // ... and resubscribe

      stringTopic += "/set";
      client.subscribe(stringTopic.c_str());
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
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
      reconnect();
    }
    client.loop();
  }
}