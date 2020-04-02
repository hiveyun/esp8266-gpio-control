#ifndef __mqtt_h__
#define __mqtt_h__
#include "config.h"
bool mqttPublish(const char* topic, const char* payload);
bool mqttPublish1(const char* topic, const char* payload);
#if USE_SWITCH
void publishSwitchState(int index, int state);
#endif
#if USE_MQ
void publishMQState(int state);
#endif
#endif
