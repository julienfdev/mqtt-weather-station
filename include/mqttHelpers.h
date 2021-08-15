#ifndef MQTT_DEF
#define MQTT_DEF
#include <PubSubClient.h>

const char *getJsonField(String field);
void reconnect(PubSubClient &client);

#endif