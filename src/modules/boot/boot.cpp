#include <ESP8266WiFi.h>
#include "LittleFS.h"
#include "boot.h"
#include <ArduinoJson.h>

// Determines whether to start in AP or Station mode
String wifiCfgFilePath = "/config/wifi.json";
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
    const char* ssid = doc["ssid"];
    const char* password = doc["password"];
            if (strlen(ssid) > 0 && strlen(password) > 0)
            {
                Serial.println("SSID and Password configured, switching to station mode");
                return WiFiMode::WIFI_STA;
            }
            else
            {
                Serial.println("SSID and Password not in memory, switching to Access Point mode");
                return WiFiMode::WIFI_AP;
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
    const char* ssid = doc["ssid"];
    const char* password = doc["password"];

    Serial.println("Configuring AP");
    WiFi.softAPConfig(local, gateway, subnet);
    WiFi.softAP(ssid, password, 13);
    Serial.print("Server IP Address : ");
    Serial.println(WiFi.softAPIP());
}