// https://github.com/bblanchon/ArduinoJson.git
#include <ArduinoJson.h>
// https://github.com/knolleary/pubsubclient.git
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <EEPROM.h>
#include "fsm_ext.h"
#include "mqtt.h"

void onMqttMessage(const char* topic, byte* payload, unsigned int length);

#define MQTT_USERNAME "8a7b722f5d671136231b"
#define MQTT_HOST "gw.huabot.com"
#define MQTT_PORT 11883

bool maybeNeedBind = false;

WiFiClient wifiClient;
PubSubClient client(wifiClient);
WiFiUDP udpServer;
char mqtt_password[40];
unsigned long mqttRetryTimer = millis();
unsigned long pingTimer = millis();

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
        char data[100];
        udpServer.read(data, packetsize);

        data[packetsize] = '\0';
        StaticJsonDocument<100> doc;
        deserializeJson(doc, data);

        const char * type = (const char*)doc["type"];
        char payload[100];

        if (strcmp(type, "Ping") == 0) {
            sprintf(payload, "%s", "{\"type\": \"Pong\"}");
        } else if (strcmp(type, "MqttPass") == 0) {
            strcpy(mqtt_password, doc["value"]);
            sprintf(payload, "%s", "{\"type\": \"Success\"}");
            for (int i = 96; i < 136; ++i) {
              EEPROM.write(i, mqtt_password[i - 96]);
            }
            EEPROM.commit();
            client.disconnect();
            mqtt_done(NULL);
        } else {
            sprintf(payload, "%s", "{\"type\": \"Error\", \"value\": \"Unknow type\"}");
        }

        IPAddress remoteip=udpServer.remoteIP();
        uint16_t remoteport=udpServer.remotePort();
        udpServer.beginPacket(remoteip,remoteport);

        udpServer.write(payload);

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

bool mqttPublish(const char* topic, const char* payload) {
    return client.publish(topic, payload);
}

bool mqttPublish1(const char* topic, const char* payload) {
    return client.publish(topic, payload, true);
}

void pingMqtt(const mqtt_loop_t *) {
    if (pingTimer + 60000 < millis()) {
        pingTimer = millis();
        char payload[100];
        sprintf(payload, "{\"timer\": %ld}", pingTimer);
        if (!mqttPublish1("/ping", payload)) {
            mqtt_unconnected(NULL);
            network_offline(NULL);
        }
    }
}
