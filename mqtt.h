#ifndef __mqtt_h__
#define __mqtt_h__
bool mqttPublish(const char* topic, const char* payload);
bool mqttPublish1(const char* topic, const char* payload);
void publishRelayState(int index, int state);
#endif
