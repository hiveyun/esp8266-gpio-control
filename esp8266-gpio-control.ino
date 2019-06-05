// https://github.com/bblanchon/ArduinoJson.git
#include <ArduinoJson.h>
// https://github.com/knolleary/pubsubclient.git
#include <PubSubClient.h>
// https://github.com/ivanseidel/ArduinoThread.git
// #include <Thread.h>
// #include <ThreadController.h>

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
// #include <EEPROM.h>
#include <WiFiClient.h>
// #include <ESP8266WebServer.h>

#include "relay_ext.h"
#include "multism.h"

// #define SMART_CONFIG_BUTTON 0
// #define SMART_CONFIG_LED 2

#define RELAY_1 12
#define RELAY_2 13
#define BUTTON_1 16
#define BUTTON_2 14
#define SOUND 12
#define LED 2

bool blinkStatus = false;

char hiveyunServer[] = "gw.huabot.com";

WiFiClient wifiClient;

PubSubClient client(wifiClient);

// ESP8266WebServer server(80);

char wifiAP[32] = "格物云";
char wifiPassword[64] = "gewuyun@520";
// char token[30];

// ThreadController that will controll all threads
// ThreadController controll = ThreadController();
// Thread* blink5000Thread = new Thread();
// Thread* blink500Thread = new Thread();
// Thread* blink200Thread = new Thread();
//
// void enable5000() {
//   blink5000Thread -> enabled = true;
//   blink500Thread  -> enabled = false;
//   blink200Thread  -> enabled = false;
// }
//
// void enable500() {
//   blink5000Thread -> enabled = false;
//   blink500Thread  -> enabled = true;
//   blink200Thread  -> enabled = false;
// }
//
// void enable200() {
//   blink5000Thread -> enabled = false;
//   blink500Thread  -> enabled = false;
//   blink200Thread  -> enabled = true;
// }

// void threadDelay(unsigned long timeout) {
//   unsigned long lastCheck = millis();
//   while (millis() - lastCheck < timeout) {
//     // controll.run();
//   }
// }

// void threadDelay1(unsigned long timeout) {
//   // delay(timeout);
//   unsigned long lastCheck = millis();
//   while (millis() - lastCheck < timeout) {
//     yield();
//     delay(1);
//     // controll.run();
//     // smartConfig();
//     // server.handleClient();
//   }
// }


void setup() {
  Serial.begin(9600);
  // EEPROM.begin(512);
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  // pinMode(SMART_CONFIG_LED, OUTPUT);
  // digitalWrite(SMART_CONFIG_LED, HIGH);
  // pinMode(SMART_CONFIG_BUTTON, INPUT_PULLUP);
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);

  pinMode(SOUND, OUTPUT);
  pinMode(LED, OUTPUT);
  initEventQueue();
  delay(10);

  // Configure Thread
  // blink5000Thread -> onRun(smartBlink);
  // blink5000Thread -> setInterval(5000);
  // blink500Thread  -> onRun(smartBlink);
  // blink500Thread  -> setInterval(500);
  // blink200Thread  -> onRun(smartBlink);
  // blink200Thread  -> setInterval(200);

  // controll.add(blink5000Thread);
  // controll.add(blink500Thread);
  // controll.add(blink200Thread);

  // for (int i = 0; i < 32; ++i) {
  //   wifiAP[i] = char(EEPROM.read(i));
  // }

  // for (int i = 32; i < 96; ++i) {
  //   wifiPassword[i - 32] = char(EEPROM.read(i));
  // }

  // for (int i = 96; i < 126; ++i) {
  //   token[i - 96] = char(EEPROM.read(i));
  // }

  InitWiFi();
  client.setServer(hiveyunServer, 11883);
  client.setCallback(onMqttMessage);

  // server.on("/update_token", HTTP_POST, handleSetToken);
  // server.onNotFound(handleNotFound);
  // server.begin();
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

// void smartConfig() {
//   if (digitalRead(SMART_CONFIG_BUTTON)) {
//     return;
//   }
//
//   threadDelay(2000);
//   if (digitalRead(SMART_CONFIG_BUTTON)) {
//     return;
//   }
//   WiFi.disconnect();
//
//   while(WiFi.status() != WL_CONNECTED) {
//     enable500();
//     threadDelay(500);
//     WiFi.beginSmartConfig();
//     while(1) {
//       enable200();
//       threadDelay(200);
//       if (WiFi.smartConfigDone()) {
//         break;
//       }
//     }
//   }
//
//   enable5000();
//
//   strcpy(wifiAP, WiFi.SSID().c_str());
//   strcpy(wifiPassword, WiFi.psk().c_str());
//
//   for (int i = 0; i < 32; ++i) {
//     EEPROM.write(i, wifiAP[i]);
//   }
//
//   for (int i = 32; i < 96; ++i) {
//     EEPROM.write(i, wifiPassword[i - 32]);
//   }
//   EEPROM.commit();
// }

// void smartBlink() {
//   if (blinkStatus) {
//     closeBlink();
//   } else {
//     openBlink();
//   }
// }

// void openBlink() {
//   digitalWrite(SMART_CONFIG_LED, LOW);
//   blinkStatus = true;
// }
//
// void closeBlink() {
//   digitalWrite(SMART_CONFIG_LED, HIGH);
//   blinkStatus = false;
// }

void loop() {
  relay_check(NULL);
  flushEventQueue();
  client.loop();
}

void InitWiFi() {
  // attempt to connect to WiFi network
  // set for STA mode
  WiFi.mode(WIFI_STA);

  WiFi.begin(wifiAP, wifiPassword);
  // configWiFiWithSmartConfig();
  Serial.println("InitWiFi ");
}

// void configWiFiWithSmartConfig() {
//   while (WiFi.status() != WL_CONNECTED) {
//     enable5000();
//     threadDelay(1000);
//     // smartConfig();
//   }
//   enable5000();
// }

// void reconnect() {
//   Serial.println("reconnect ");
//   // Loop until we're reconnected
//   while (!client.connected()) {
//     Serial.println("reconnect1 ");
//     if (WiFi.status() != WL_CONNECTED) {
//       // Serial.println("reconnect2 ");
//       // enable5000();
//       Serial.println("reconnect3 ");
//       // threadDelay1(1000);
//       Serial.println("reconnect4 ");
//       continue;
//       // configWiFiWithSmartConfig();
//     }
//     // Attempt to connect (clientId, username, password)
//     Serial.println("Attempt to connect (clientId, username, password) ");
//     if (client.connect("ESP8266 Relay", "a937e135a6881193af39", "0d65b112c7b14f59b5ed69122958bb08")) {
//       client.subscribe("/request/+");
//       client.publish("/attributes", getRelayStatus().c_str());
//       client.publish("/attributes", getLocalIP().c_str());
//     } else {
//       // Wait 5 seconds before retrying
//       // enable5000();
//       // threadDelay1(5000);
//     }
//   }
//   // enable5000();
// }

// void handleSetToken() {
//   boolean updated = false;
//   for (uint8_t i=0; i<server.args(); i++){
//     if (server.argName(i) == "token") {
//         strcpy(token, server.arg(i).c_str());
//         updated = true;
//         break;
//     };
//   }
//   if (updated) {
//     for (int i = 96; i < 126; ++i) {
//       EEPROM.write(i, token[i - 96]);
//     }
//     EEPROM.commit();
//     client.disconnect();
//   }
//   server.send(200, "text/plain", "OK");
// }
//
// void handleNotFound(){
//   server.send(404, "text/plain", "Not Found");
// }

void button1Check(const button1_check_t *a1) {
    if (digitalRead(BUTTON_1)) {
        button1_released(NULL);
    } else {
        button1_pressed(NULL);
    }
}

void button2Check(const button2_check_t *a1) {
    if (digitalRead(BUTTON_2)) {
        button2_released(NULL);
    } else {
        button2_pressed(NULL);
    }
}

void connectCheck(const mqtt_check_t *a1) {
    if(client.connected()) {
        mqtt_connected(NULL);
    } else {
        mqtt_unconnected(NULL);
    }
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

void onPublish(const mqtt_publish_t *a1) {
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

void soundAlarm(void) {
    digitalWrite(15, HIGH);
    delay(200);
    digitalWrite(15, LOW);
}

void soundError(void) {
    digitalWrite(15, HIGH);
    delay(200);
    digitalWrite(15, LOW);
}

void tryConnect(const mqtt_unconnected_t *) {
    if (client.connect("ESP8266 Relay", "a937e135a6881193af39", "0d65b112c7b14f59b5ed69122958bb08")) {
        mqtt_connected(NULL);
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
