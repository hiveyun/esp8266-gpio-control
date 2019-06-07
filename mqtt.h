#ifndef __mqtt_h__
#define __mqtt_h__
struct mqtt_publish_t {
    const char* topic;
    const char* payload;
};

struct mqtt_message_t {
    const char* topic;
    const char* payload;
};

void mqttPublish(const char* topic, const char* payload);
#endif

