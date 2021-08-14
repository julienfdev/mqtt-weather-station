#include <ESP8266WiFi.h>
#include "LittleFS.h"
#include "boot.h"

// Determines whether to start in AP or Station mode
String wifiCfgFile = "/config/wifi";
File apFile;
File configFile;
String ap;

// IP addresses
IPAddress local(192, 168, 8, 18);
IPAddress gateway(192, 168, 8, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiMode WiFiModeSwitch()
{
    // We open the config file
    File configFile = LittleFS.open(wifiCfgFile, "r");
    if (configFile)
    {
        // we read the file as a string
        String config = configFile.readString();
        if (config.startsWith("ssid="))
        {
            Serial.println("SSID found in config file, switching to station mode");
            return WiFiMode::WIFI_STA;
        }
        else
        {
            Serial.println("SSID doesn't seem to be configured, switching to AP mode");
            return WiFiMode::WIFI_AP;
        }
    }
    else
    {
        // We must switch to AP Mode
        return WiFiMode::WIFI_AP;
    }
    return WiFiMode::WIFI_AP;
}

void setAccessPoint(){
       Serial.println("getting AP SSID name configured in filesystem");
    apFile = LittleFS.open("/config/apconfig", "r");
    ap = apFile.readString();
    Serial.println("Configuring AP");
    WiFi.softAPConfig(local, gateway, subnet);
    WiFi.softAP("iot_quimrendouf", "password1337", 13);
    Serial.print("Server IP Address : ");
    Serial.println(WiFi.softAPIP());
}