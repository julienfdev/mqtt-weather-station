
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

void publishDiscoveryData(PubSubClient &client, String topic)
{
  // Preparing topics
  Serial.println("Publishing discovery data");
  DynamicJsonDocument docTemp(1024);
  DynamicJsonDocument docHR(1024);
  DynamicJsonDocument docIllu(1024);
  char buffer[1024];
  deserializeJson(docTemp, LittleFS.open("/mqtt/temperature.json", "r").readString());
  deserializeJson(docHR, LittleFS.open("/mqtt/humidity.json", "r").readString());
  deserializeJson(docIllu, LittleFS.open("/mqtt/illuminance.json", "r").readString());
  docTemp["state_topic"] = (String("homeassistant/") + topic);
  docHR["state_topic"] = (String("homeassistant/") + topic);
  docIllu["state_topic"] = (String("homeassistant/") + topic);

  String discoveryTemp = String("homeassistant/sensor/") + topic + String("T/config");
  String discoveryHR = String("homeassistant/sensor/") + topic + String("H/config");
  String discoveryIllu = String("homeassistant/sensor/") + topic + String("I/config");

  // Sending topics
  serializeJson(docTemp, buffer);
  client.publish(discoveryTemp.c_str(), buffer, true);
  serializeJson(docHR, buffer);
  client.publish(discoveryHR.c_str(), buffer, true);
  serializeJson(docIllu, buffer);
  client.publish(discoveryIllu.c_str(), buffer, true);

  // Spoof
  String spoof = "{ \"temperature\": 23.20, \"humidity\": 43.70, \"illuminance\": 454 }";
  client.publish("homeassistant/weathermonitor", spoof.c_str());
}

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

void reconnect(PubSubClient &client)
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
      // Once connected, we publish the discovery data
      publishDiscoveryData(client, stringTopic);
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