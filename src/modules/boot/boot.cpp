#include <ESP8266WiFi.h>
#include "LittleFS.h"
#include "boot.h"
#include <ArduinoJson.h>
#include <Arduino.h>

// Determines whether to start in AP or Station mode
String wifiCfgFilePath = "/config/config.json";
String apFilePath = "/config/ap.json";
File apFile;
File configFile;
String ap;

// IP addresses
IPAddress local(192, 168, 8, 18);
IPAddress gateway(192, 168, 8, 1);
IPAddress subnet(255, 255, 255, 0);

//JSON document placeholder
DynamicJsonDocument doc(1024);

WiFiMode WiFiModeSwitch()
{
    // We open the config file
    File configFile = LittleFS.open(wifiCfgFilePath, "r");
    if (configFile)
    {
        // we read the file as a string
        String config = configFile.readString();
        // deserializing json
        deserializeJson(doc, config);
        // testing if SSID and Password have length
        if (doc.containsKey("ssid") && doc.containsKey("password"))
        {
            const char *ssid = doc["ssid"];
            const char *password = doc["password"];
            if (strlen(ssid) > 0 && strlen(password) > 0)
            {
                Serial.println("SSID and Password configured, switching to station mode");
                return WiFiMode::WIFI_STA;
            }
            else
            {
                Serial.println("SSID and Password not in memory, switching to Access Point mode");
            }
        }
    }
    else
    {
        // We must switch to AP Mode
        return WiFiMode::WIFI_AP;
    }
    return WiFiMode::WIFI_AP;
}

void setAccessPoint()
{
    Serial.println("getting AP SSID name configured in filesystem");

    apFile = LittleFS.open(apFilePath, "r");
    ap = apFile.readString();
    // Getting values
    deserializeJson(doc, ap);
    // casting values
    const char *ssid = doc["ssid"];
    const char *password = doc["password"];

    Serial.println("Configuring AP");
    WiFi.softAPConfig(local, gateway, subnet);
    WiFi.softAP(ssid, password, 13);
    Serial.print("Server IP Address : ");
    Serial.println(WiFi.softAPIP());
}

void resetConfig()
{
    String serialized;
    Serial.println("Connection failed after 5 attempts, reinitializing configuration and switching to access point");
    File configFile = LittleFS.open(wifiCfgFilePath, "r");
    // we read the file as a string
    String config = configFile.readString();
    // deserializing json
    deserializeJson(doc, config);
    doc["ssid"] = "";
    doc["password"] = "";

    serializeJson(doc, serialized);
    configFile = LittleFS.open(wifiCfgFilePath, "w");
    configFile.println(serialized);

    configFile.close();
    Serial.println("Restarting...");
    delay(500);

    ESP.restart();
}

String switchWiFiError(wl_status_t error)
{
    switch (error)
    {
    case WL_DISCONNECTED:
        return "Disconnected";
    case WL_IDLE_STATUS:
        return "Idle";
    case WL_NO_SSID_AVAIL:
        return "SSID Unavailable";
    case WL_WRONG_PASSWORD:
        return "Wrong passphrase";
    case WL_CONNECT_FAILED:
        return "Connection Failed";
    default:
        return "Unknown error";
    }
}

bool wiFiConnect()
{
    int retry = 0;
    // Getting the SSID and Password configured in memory
    // We open the config file
    File configFile = LittleFS.open(wifiCfgFilePath, "r");
    // we read the file as a string
    String config = configFile.readString();
    // deserializing json
    deserializeJson(doc, config);
    const char *ssid = doc["ssid"];
    const char *password = doc["password"];
    // connecting and keeping track of failures
    Serial.print("Connecting to : ");
    Serial.print(ssid);
    Serial.print(" passphrase : ");
    Serial.println(password);
    for (retry = 0; retry < 5; retry++)
    {
        Serial.print("Attempt #");
        Serial.print(retry + 1);
        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println("Connection established! proceeding...");
            return true;
        }
        // Initializing
        WiFi.hostname("wxmonitor");
        WiFi.begin(ssid, password);
        for (int i = 0; i < 20; i++)
        {
            if (WiFi.status() != WL_CONNECTED)
            {
                Serial.print(".");
                delay(1000);
            }
            else
            {
                digitalWrite(D0, HIGH);
                Serial.println("Connection established! proceeding...");
                return true;
            }
        }
        Serial.print("Attempt failed : ");
        Serial.print(switchWiFiError(WiFi.status()));
        Serial.print(" - ");
        Serial.println(WiFi.status());
        WiFi.disconnect();
    }
    resetConfig();
    return false;
}
