#ifndef __mqtt_h__
#define __mqtt_h__
struct mqtt_publish_t {
    const char* topic;
    const char* payload;
};

struct mqtt_message_t {
    const char* topic;
    uint8_t * payload;
    unsigned int length;
};

bool mqttPublish(const char* topic, const char* payload);
bool mqttPublish1(const char* topic, const char* payload);
#endif
