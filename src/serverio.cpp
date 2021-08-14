#include <Arduino.h>
#include "LittleFS.h"
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "server.h"

AsyncWebServer server(80);

void setupAndStartConfigServer(){

// attaching handlers
server.serveStatic("/", LittleFS, "/server/").setDefaultFile("index.html");

Serial.println("Starting Config Web Server");
server.begin();
}
