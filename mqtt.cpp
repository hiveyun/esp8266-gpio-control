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

#define JSON_PAYLOAD_LENGTH 256
StaticJsonDocument<JSON_PAYLOAD_LENGTH> jsonData;
char jsonPayload[JSON_PAYLOAD_LENGTH];
char tpl[JSON_PAYLOAD_LENGTH];
char stateName[20];

void genJson(const char * key, int val) {
    jsonData.clear();
    jsonData[key] = val;
    serializeJson(jsonData, jsonPayload);
}

void genStateJson(int index, int val) {
    tpl[0] = '\0';
    sprintf(tpl, "switch_%d_state", index);
    genJson(tpl, val);
}

void genResultJson(const char * val) {
    jsonData.clear();
    jsonData["result"] = val;
    serializeJson(jsonData, jsonPayload);
}

void genErrJson(const char * val) {
    jsonData.clear();
    jsonData["err"] = val;
    serializeJson(jsonData, jsonPayload);
}

void genRelayState(int index, const char * state) {
    if (strcmp(state, "on") == 0) {
        genStateJson(index, 1);
    } else if (strcmp(state, "off") == 0) {
        genStateJson(index, 0);
    } else if (strcmp(state, "ready") == 0) {
        genStateJson(index, 0);
    } else {
        genErrJson(state);
    }
}

void getRelayState(int index) {
    // Prepare switchs JSON payload string
    stateName[0] = '\0';
    if (index == 1) {
        sprintf(stateName, "%s", switch1_Current_state_name());
    } else {
        sprintf(stateName, "Relay %d not exists.", index);
    }
    genRelayState(index, stateName);
}

void setRelayOn(int index) {
    // Prepare switchs JSON payload string
    stateName[0] = '\0';
    sprintf(stateName, "%s", "on");
    if (index == 1) {
        switch1_on(NULL);
    } else {
        stateName[0] = '\0';
        sprintf(stateName, "Relay %d not exists.", index);
    }
    genRelayState(index, stateName);
}

void setRelayOff(int index) {
    // Prepare switchs JSON payload string
    stateName[0] = '\0';
    sprintf(stateName, "%s", "off");
    if (index == 1) {
        switch1_off(NULL);
    } else {
        stateName[0] = '\0';
        sprintf(stateName, "Relay %d not exists.", index);
    }
    genRelayState(index, stateName);
}

void publishRelayState(int index, int state) {
    genStateJson(index, state);
    mqttPublish("/attributes", jsonPayload);
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
    jsonData.clear();
    deserializeJson(jsonData, payload);

    // Check request method
    const char * methodName = (const char*)jsonData["method"];
    // strstr
    const char needle[8] = "request";
    const char * tmpStr = strstr(topic, needle);
    const size_t len = strlen(topic);
    const size_t lenTmp = strlen(tmpStr);

    char new_topic[len + 2];

    char head[len - lenTmp];
    char tail[lenTmp];

    size_t i;

    for (i = 0; i < len - lenTmp; i ++) {
        head[i] = topic[i];
    }

    for (i = 0; i < lenTmp - 7; i ++) {
        tail[i] = tmpStr[i+7];
    }

    sprintf(new_topic, "%s%s%s", head, "response", tail);

    if (strcmp(methodName, "switch_state") == 0) {
        getRelayState(jsonData["index"]);
    } else if (strcmp(methodName, "switch_on") == 0) {
        setRelayOn(jsonData["index"]);
    } else if (strcmp(methodName, "switch_off") == 0) {
        setRelayOff(jsonData["index"]);
    } else if (strcmp(methodName, "ping") == 0) {
        genResultJson("pong");
    } else {
        genErrJson("Not Support");
    }
    mqttPublish(new_topic, jsonPayload);
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
