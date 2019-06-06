// https://github.com/bblanchon/ArduinoJson.git
#include <ArduinoJson.h>
// https://github.com/knolleary/pubsubclient.git
#include <PubSubClient.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include <EEPROM.h>
#include <WiFiUDP.h>

#include "relay_ext.h"
#include "multism.h"

#define RELAY_1 12
#define RELAY_2 13
#define BUTTON_1 16
#define BUTTON_2 14
#define SOUND 15
#define LED 2

#define MQTT_USERNAME "a937e135a6881193af39"
#define MQTT_HOST "gw.huabot.com"
#define MQTT_PORT 11883

WiFiClient wifiClient;
PubSubClient client(wifiClient);

WiFiUDP udpServer;

char wifiAP[32];
char wifiPassword[64];

char mqtt_password[40];

unsigned long ledTimer = millis();
unsigned long ledDelay = 1000;

unsigned long button1PressTimer = millis();
unsigned long button2PressTimer = millis();
unsigned long smartconfigTimer = millis();

bool maybeNeedBind = false;

void setup() {
    EEPROM.begin(512);
    pinMode(RELAY_1, OUTPUT);
    pinMode(RELAY_2, OUTPUT);
    pinMode(BUTTON_1, INPUT_PULLUP);
    pinMode(BUTTON_2, INPUT_PULLUP);

    pinMode(SOUND, OUTPUT);
    pinMode(LED, OUTPUT);
    initEventQueue();
    delay(10);
}


void publishRelayState(int index, int state) {
    // Prepare relays JSON payload string
    DynamicJsonDocument data(100);
    if (index == 1) {
        data["relay_1_state"] = state;
    } else if (index == 2) {
        data["relay_2_state"] = state;
    } else {
        return;
    }
    char payload[100];
    serializeJson(data, payload);
    client.publish("/attributes", payload);
}

String getRelayState(int index) {
    // Prepare relays JSON payload string
    DynamicJsonDocument data(100);
    String state;
    if (index == 1) {
        state = String(relay1_Current_state_name());
        if (state.equals("on")) {
            data["relay_1_state"] = 1;
        } else {
            data["relay_1_state"] = 0;
        }
    } else if (index == 2) {
        state = String(relay2_Current_state_name());
        if (state.equals("on")) {
            data["relay_2_state"] = 1;
        } else {
            data["relay_2_state"] = 0;
        }
    } else {
        data["err"] = "relay not exists.";
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

String getLocalIP() {
  // Prepare relays JSON payload string
  DynamicJsonDocument data(1024);
  data["ip"] = WiFi.localIP().toString();
  char payload[30];
  serializeJson(data, payload);
  return String(payload);
}

void loop() {
  relay_check(NULL);
  flushEventQueue();
  client.loop();
  if (ledTimer + ledDelay < millis()) {
    ledTimer = millis();
    led_toggle(NULL);
  }
}

void initWiFi(void) {
    WiFi.mode(WIFI_STA);

    for (int i = 0; i < 32; ++i) {
      wifiAP[i] = char(EEPROM.read(i));
    }

    for (int i = 32; i < 96; ++i) {
      wifiPassword[i - 32] = char(EEPROM.read(i));
    }

    WiFi.begin(wifiAP, wifiPassword);
}

void initMqtt(void) {
    client.setServer(MQTT_HOST, MQTT_PORT);
    client.setCallback(onMqttMessage);
    for (int i = 96; i < 136; ++i) {
        mqtt_password[i - 96] = char(EEPROM.read(i));
    }
}

void button1Check(const button1_check_t *a1) {
    if (digitalRead(BUTTON_1)) {
        button1_released(NULL);
    } else {
        button1_pressed(NULL);
    }
}

void button1EnterPressed(void) {
    button1PressTimer = millis();
}

void button1Pressed(const button1_pressed_t *) {
    if (button1PressTimer + 1000 < millis()) {
        button1_longpressed(NULL);
    }
}

void button2Check(const button2_check_t *a1) {
    if (digitalRead(BUTTON_2)) {
        button2_released(NULL);
    } else {
        button2_pressed(NULL);
    }
}

void button2EnterPressed(void) {
    button2PressTimer = millis();
}

void button2Pressed(const button2_pressed_t *) {
    if (button2PressTimer + 2000 < millis()) {
        button2_longpressed(NULL);
    }
}

void connectCheck(const mqtt_check_t *a1) {
    if(client.connected()) {
        mqtt_connected(NULL);
    } else {
        mqtt_unconnected(NULL);
    }
}

void led1000(void) {
    ledDelay = 1000;
}

void led200(void) {
    ledDelay = 200;
}

void led500(void) {
    ledDelay = 500;
}

void ledOff(void) {
    digitalWrite(LED, HIGH);
}

void ledOn(void) {
    digitalWrite(LED, LOW);
}

void netCheck(const net_check_t *a1) {
    if (WiFi.status() == WL_CONNECTED) {
        net_online(NULL);
    } else {
        net_offline(NULL);
    }
}

void onConnected(void) {
    client.subscribe("/request/+");
}

void relay1Off(void) {
    digitalWrite(RELAY_1, LOW);
    publishRelayState(1, 0);
}

void relay1On(void) {
    digitalWrite(RELAY_1, HIGH);
    publishRelayState(1, 1);
}

void relay2Off(void) {
    digitalWrite(RELAY_2, LOW);
    publishRelayState(2, 0);
}

void relay2On(void) {
    digitalWrite(RELAY_2, HIGH);
    publishRelayState(2, 1);
}

void beginSmartconfig(void) {
    WiFi.disconnect();
    while(WiFi.status() == WL_CONNECTED) {
        delay(100);
    }
    WiFi.beginSmartConfig();
    maybeNeedBind = true;
    smartconfigTimer = millis();
}

void smartconfigDone(const smartconfig_check_t *) {
    if (WiFi.smartConfigDone()) {
        smartconfig_done(NULL);
        strcpy(wifiAP, WiFi.SSID().c_str());
        strcpy(wifiPassword, WiFi.psk().c_str());

        for (int i = 0; i < 32; ++i) {
          EEPROM.write(i, wifiAP[i]);
        }

        for (int i = 32; i < 96; ++i) {
          EEPROM.write(i, wifiPassword[i - 32]);
        }
        EEPROM.commit();
    } else {
        if (millis() - smartconfigTimer > 120000) {
            smartconfig_timeout(NULL);
        }
    }
}

void soundAlarm(void) {
    digitalWrite(SOUND, HIGH);
    delay(200);
    digitalWrite(SOUND, LOW);
}

void soundAlarm1(void) {
    digitalWrite(SOUND, HIGH);
    delay(1000);
    digitalWrite(SOUND, LOW);
}

void soundError(void) {
    digitalWrite(SOUND, HIGH);
    delay(2000);
    digitalWrite(SOUND, LOW);
}

void fetchToken(const mqtt_check_t *) {
    if (!maybeNeedBind) {
        return;
    }
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

void startUdpServer(void) {
    if (!maybeNeedBind) {
        return;
    }
    udpServer.begin(1234);
}

void stopUdpServer(void) {
    if (!maybeNeedBind) {
        return;
    }
    udpServer.stop();
}

void tryConnect(const mqtt_unconnected_t *) {
    if (client.connect("ESP8266 Relay", MQTT_USERNAME, mqtt_password)) {
        mqtt_connected(NULL);
        maybeNeedBind = false;
    } else {
        mqtt_unconnected(NULL);
    }
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
