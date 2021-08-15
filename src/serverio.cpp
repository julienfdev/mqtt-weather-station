#include <Arduino.h>
#include "LittleFS.h"
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "ArduinoJson.h"
#include "serverio.h"
#include "boot.h"

AsyncWebServer server(80);
DynamicJsonDocument jsonSerializer(1024);

void onUpdate(AsyncWebServerRequest *request)
{
    //Handle Unknown Request
    String ssid;
    String password;
    String serialized;
    File wifiConfigFile;
    String wifiCfgFilePath = "/config/wifi.json";
    if (request->hasParam("ssid", true))
    {
        AsyncWebParameter *p = request->getParam("ssid", true);
        ssid = p->value();
    }

    if (request->hasParam("password", true))
    {
        AsyncWebParameter *p = request->getParam("password", true);
        password = p->value();
    }
    Serial.println("SSID And Password received from user, storing");
    Serial.print("SSID : ");
    Serial.println(ssid);

    jsonSerializer["ssid"] = ssid;
    jsonSerializer["password"] = password;
    serializeJson(jsonSerializer, serialized);
    wifiConfigFile = LittleFS.open(wifiCfgFilePath, "w");
    wifiConfigFile.println(serialized);

    request->send(200);
}

void setupAndStartConfigServer()
{

    // attaching handlers
    server.on("/update", HTTP_POST, onUpdate);
    server.serveStatic("/", LittleFS, "/s/").setDefaultFile("index.html");

    Serial.println("Starting Config Web Server");
    server.begin();
}