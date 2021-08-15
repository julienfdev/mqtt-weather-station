#include <Arduino.h>
#include "LittleFS.h"
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
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
    String wifiCfgFilePath = "/config/config.json";
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

    // get Current json config
    wifiConfigFile = LittleFS.open(wifiCfgFilePath, "r");
    deserializeJson(jsonSerializer, wifiConfigFile.readString());

    // Updating the values and serializing back
    jsonSerializer["ssid"] = ssid;
    jsonSerializer["password"] = password;
    serializeJson(jsonSerializer, serialized);
    wifiConfigFile = LittleFS.open(wifiCfgFilePath, "w");
    wifiConfigFile.println(serialized);

    request->send(200);
}

void onMqttUpdate(AsyncWebServerRequest *request, JsonVariant &json)
{
    File mqttConfigFile;
    String mqttCfgFilePath = "/config/config.json";
    String serialized;

    Serial.println("Received MQTT Config update request");
    // we cast the json as an object
    JsonObject jsonObj = json.as<JsonObject>();
    // checking keys
    if (jsonObj.containsKey("mqttIp") && jsonObj.containsKey("mqttLogin") && jsonObj.containsKey("mqttPassword") && jsonObj.containsKey("mqttTopic"))
    {
        Serial.println("Object is matching config, updating...");
        // Opening current config
        // get Current json config
        mqttConfigFile = LittleFS.open(mqttCfgFilePath, "r");
        deserializeJson(jsonSerializer, mqttConfigFile.readString());

        // updating mqtt related keys
        jsonSerializer["mqttIp"] = jsonObj["mqttIp"];
        jsonSerializer["mqttLogin"] = jsonObj["mqttLogin"];
        jsonSerializer["mqttPassword"] = jsonObj["mqttPassword"];
        jsonSerializer["mqttTopic"] = jsonObj["mqttTopic"];

        // Saving the file back
        serializeJson(jsonSerializer, serialized);
        mqttConfigFile = LittleFS.open(mqttCfgFilePath, "w");
        mqttConfigFile.println(serialized);

        request->send(200);
    }
    else
    {
        request->send(400);
    }
}

AsyncCallbackJsonWebHandler *onMqttUpdateHandler = new AsyncCallbackJsonWebHandler("/mqttupdate", onMqttUpdate);

void onGetConfig(AsyncWebServerRequest *request)
{
    String cfgFilePath = "/config/config.json";
    request->send(LittleFS, cfgFilePath, "application/json");
}

void setupAndStartConfigServer()
{

    // attaching handlers
    server.on("/update", HTTP_POST, onUpdate);
    server.on("/config", HTTP_GET, onGetConfig);
    server.addHandler(onMqttUpdateHandler);
    server.serveStatic("/", LittleFS, "/s/").setDefaultFile("index.html");

    Serial.println("Starting Config Web Server");
    server.begin();
}