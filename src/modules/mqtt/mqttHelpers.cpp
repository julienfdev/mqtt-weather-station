
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

//JSON document placeholder

const char *getJsonField(String field)
{
    String cfgFilePath = "/config/config.json";
    File configFile;
    DynamicJsonDocument doc(1024);
    configFile = LittleFS.open(cfgFilePath, "r");
    deserializeJson(doc, configFile.readString());
    configFile.close();
    const char *jsonField = doc[field].as<const char *>();

    return jsonField;
}
