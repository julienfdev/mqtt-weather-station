#include <Arduino.h>
#include "LittleFS.h"
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "server.h"

AsyncWebServer server(80);

// Defining handlers
void handleHomePage(AsyncWebServerRequest *request){
  // serving home page
  request->send(LittleFS, "/server/index.html");
}

void setupAndStartConfigServer(){

// attaching handlers
server.on("/", HTTP_GET, handleHomePage);

Serial.println("Starting Config Web Server");
server.begin();
}
