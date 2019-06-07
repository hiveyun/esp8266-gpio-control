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

String getRelayState(int index) {
    // Prepare relays JSON payload string
    DynamicJsonDocument data(100);
    String state = "on";
    if (index == 1) {
        state = String(relay1_Current_state_name());
    } else if (index == 2) {
        state = String(relay2_Current_state_name());
    } else {
        data["err"] = "relay not exists.";
    }
    if (state.equals("on")) {
        data["relay_2_state"] = 1;
    } else {
        data["relay_2_state"] = 0;
    }
    char payload[100];
    serializeJson(data, payload);
    return String(payload);
}

String setRelayOn(int index) {
    // Prepare relays JSON payload string
    DynamicJsonDocument data(100);
    String state;
    if (index == 1) {
        relay1_on(NULL);
        data["relay_1_state"] = 1;
    } else if (index == 2) {
        relay2_on(NULL);
        data["relay_2_state"] = 1;
    } else {
        data["err"] = "relay not exists.";
    }
    char payload[100];
    serializeJson(data, payload);
    return String(payload);
}

String setRelayOff(int index) {
    // Prepare relays JSON payload string
    DynamicJsonDocument data(100);
    String state;
    if (index == 1) {
        relay1_off(NULL);
        data["relay_1_state"] = 0;
    } else if (index == 2) {
        relay2_off(NULL);
        data["relay_2_state"] = 0;
    } else {
        data["err"] = "relay not exists.";
    }
    char payload[100];
    serializeJson(data, payload);
    return String(payload);
}

// The callback for when a PUBLISH message is received from the server.
void onMqttMessage(const char* topic, byte* payload, unsigned int length) {
    char json[length + 1];
    strncpy(json, (char*)payload, length);
    json[length] = '\0';

    // Decode JSON request
    DynamicJsonDocument data(1024);
    deserializeJson(data, json);

    // Check request method
    String methodName = String((const char*)data["method"]);
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");

    if (methodName.equals("relay_state")) {
        // Reply with GPIO status
        client.publish(responseTopic.c_str(), getRelayState(data["index"]).c_str());
    } else if (methodName.equals("relay_on")) {
        client.publish(responseTopic.c_str(), setRelayOn(data["index"]).c_str());
    } else if (methodName.equals("relay_off")) {
        client.publish(responseTopic.c_str(), setRelayOff(data["index"]).c_str());
    } else {
        DynamicJsonDocument data(100);
        data["err"] = "Not Support";
        char payload[100];
        serializeJson(data, payload);
        client.publish(responseTopic.c_str(), payload);
    }
}

