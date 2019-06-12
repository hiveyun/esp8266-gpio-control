// https://github.com/bblanchon/ArduinoJson.git
#include <ArduinoJson.h>
// https://github.com/knolleary/pubsubclient.git
#include <PubSubClient.h>
#include <WiFiUDP.h>
#include <EEPROM.h>
#include "fsm_ext.h"
#include "network.h"
#include "mqtt.h"

void onMqttMessage(const char* topic, byte* payload, unsigned int length);

#define MQTT_USERNAME "a937e135a6881193af39"
#define MQTT_HOST "gw.huabot.com"
#define MQTT_PORT 11883

bool maybeNeedBind = false;

WiFiClient wc = getWifiClient();
PubSubClient client(wc);
WiFiUDP udpServer;
char mqtt_password[40];
unsigned long mqttRetryTimer = millis();

PubSubClient getMqttClient() {
    return client;
}

void setMaybeNeedBind(void) {
    maybeNeedBind = true;
}

void initMqtt(void) {
    client.setServer(MQTT_HOST, MQTT_PORT);
    client.setCallback(onMqttMessage);
    for (int i = 96; i < 136; ++i) {
        mqtt_password[i - 96] = char(EEPROM.read(i));
    }
}

void connectCheck(const mqtt_loop_t *a1) {
    if(client.connected()) {
        mqtt_connected(NULL);
    } else {
        mqtt_unconnected(NULL);
    }
    client.loop();
}

void onConnected(void) {
    client.subscribe("/request/+");
}

void fetchToken(const mqtt_loop_t *) {
    char message = udpServer.parsePacket();
    int packetsize = udpServer.available();
    if (message) {
        char data[200];
        udpServer.read(data,packetsize);

        data[packetsize] = '\0';
        DynamicJsonDocument doc(200);
        deserializeJson(doc, data);
        String type = String((const char*)doc["type"]);
        DynamicJsonDocument rsp(200);

        if (type.equals("Ping")) {
            rsp["type"] = "Pong";
        } else if (type.equals("MqttPass")) {
            strcpy(mqtt_password, doc["value"]);
            rsp["type"] = "Success";
            for (int i = 96; i < 136; ++i) {
              EEPROM.write(i, mqtt_password[i - 96]);
            }
            EEPROM.commit();
            client.disconnect();
            mqtt_done(NULL);
        } else {
            rsp["type"] = "Error";
            rsp["value"] = "Unknow type";
        }

        IPAddress remoteip=udpServer.remoteIP();
        uint16_t remoteport=udpServer.remotePort();
        udpServer.beginPacket(remoteip,remoteport);

        serializeJson(rsp, data);

        udpServer.write(data);

        udpServer.endPacket();
    }
}

void checkPassword(const mqtt_loop_t *) {
    if (mqtt_password[0] == '\0') {
        mqtt_invalid(NULL);
    } else {
        mqtt_valid(NULL);
    }
}

void startUdpServer(void) {
    udpServer.begin(1234);
}

void stopUdpServer(void) {
    udpServer.stop();
}

void tryConnect(const mqtt_unconnected_t *) {
    if (mqttRetryTimer + 5000 > millis()) {
        return;
    }

    mqttRetryTimer = millis();
    if (client.connect("ESP8266 Relay", MQTT_USERNAME, mqtt_password)) {
        maybeNeedBind = false;
    } else {
        if (maybeNeedBind) {
            mqtt_failed(NULL);
        }
    }
}

// The callback for when a PUBLISH message is received from the server.
void onMqttMessage(const char* topic, uint8_t * payload, unsigned int length) {
    mqtt_message_t *msg;
    msg = (mqtt_message_t *)malloc(sizeof(mqtt_message_t));
    msg -> topic = topic;
    msg -> payload = payload;
    msg -> length = length;
    mqtt_message(msg);
}

void onPublish(const mqtt_publish_t * msg) {
    client.publish(msg->topic, msg->payload);
}

void mqttPublish(const char* topic, const char* payload) {
    mqtt_publish_t *msg;
    msg = (mqtt_publish_t *)malloc(sizeof(mqtt_publish_t));
    msg -> topic = topic;
    msg -> payload = payload;
    mqtt_publish(msg);
}

void mqttPublish1(const char* topic, const char* payload) {
    client.publish(topic, payload);
}
