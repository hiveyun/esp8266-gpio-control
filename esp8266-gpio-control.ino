// https://github.com/bblanchon/ArduinoJson.git
#include <ArduinoJson.h>
// https://github.com/knolleary/pubsubclient.git
#include <PubSubClient.h>
// https://github.com/ivanseidel/ArduinoThread.git
#include <Thread.h>
#include <ThreadController.h>

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define SMART_CONFIG_BUTTON 0
#define SMART_CONFIG_LED 2

#define RELAY 5
#define RELAY_PIN 1

// We assume that all GPIOs are LOW
boolean relayState = false;

bool blinkStatus = false;

char hiveyunServer[] = "platform.hiveyun.com";

WiFiClient wifiClient;

PubSubClient client(wifiClient);

ESP8266WebServer server(80);

char wifiAP[32];
char wifiPassword[64];
char token[30];

// ThreadController that will controll all threads
ThreadController controll = ThreadController();
Thread* blink5000Thread = new Thread();
Thread* blink500Thread = new Thread();
Thread* blink200Thread = new Thread();

void enable5000() {
  blink5000Thread -> enabled = true;
  blink500Thread  -> enabled = false;
  blink200Thread  -> enabled = false;
}

void enable500() {
  blink5000Thread -> enabled = false;
  blink500Thread  -> enabled = true;
  blink200Thread  -> enabled = false;
}

void enable200() {
  blink5000Thread -> enabled = false;
  blink500Thread  -> enabled = false;
  blink200Thread  -> enabled = true;
}

void threadDelay(unsigned long timeout) {
  unsigned long lastCheck = millis();
  while (millis() - lastCheck < timeout) {
    controll.run();
  }
}

void threadDelay1(unsigned long timeout) {
  unsigned long lastCheck = millis();
  while (millis() - lastCheck < timeout) {
    controll.run();
    smartConfig();
    server.handleClient();
  }
}


void setup() {
  EEPROM.begin(512);
  pinMode(RELAY, OUTPUT);
  pinMode(SMART_CONFIG_LED, OUTPUT);
  digitalWrite(SMART_CONFIG_LED, HIGH);
  pinMode(SMART_CONFIG_BUTTON, INPUT_PULLUP);
  delay(10);

  // Configure Thread
  blink5000Thread -> onRun(smartBlink);
  blink5000Thread -> setInterval(5000);
  blink500Thread  -> onRun(smartBlink);
  blink500Thread  -> setInterval(500);
  blink200Thread  -> onRun(smartBlink);
  blink200Thread  -> setInterval(200);

  controll.add(blink5000Thread);
  controll.add(blink500Thread);
  controll.add(blink200Thread);

  for (int i = 0; i < 32; ++i) {
    wifiAP[i] = char(EEPROM.read(i));
  }

  for (int i = 32; i < 96; ++i) {
    wifiPassword[i - 32] = char(EEPROM.read(i));
  }

  for (int i = 96; i < 126; ++i) {
    token[i - 96] = char(EEPROM.read(i));
  }

  InitWiFi();
  client.setServer(hiveyunServer, 1883);
  client.setCallback(onMessage);

  server.on("/update_token", HTTP_POST, handleSetToken);
  server.onNotFound(handleNotFound);
  server.begin();
}


// The callback for when a PUBLISH message is received from the server.
void onMessage(const char* topic, byte* payload, unsigned int length) {

  char json[length + 1];
  strncpy(json, (char*)payload, length);
  json[length] = '\0';

  // Decode JSON request
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& data = jsonBuffer.parseObject((char*)json);

  if (!data.success()) {
    return;
  }

  // Check request method
  String methodName = String((const char*)data["method"]);

  if (methodName.equals("getRelayStatus")) {
    // Reply with GPIO status
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    client.publish(responseTopic.c_str(), getRelayStatus().c_str());
  } else if (methodName.equals("setRelayStatus")) {
    // Update GPIO status and reply
    setRelayStatus(data["params"]["pin"], data["params"]["enabled"]);
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    client.publish(responseTopic.c_str(), getRelayStatus().c_str());
    client.publish("v1/devices/me/attributes", getRelayStatus().c_str());
  }
}

String getRelayStatus() {
  // Prepare relays JSON payload string
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& data = jsonBuffer.createObject();
  data[String(RELAY_PIN)] = relayState ? true : false;
  char payload[30];
  data.printTo(payload, sizeof(payload));
  return String(payload);
}

String getLocalIP() {
  // Prepare relays JSON payload string
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& data = jsonBuffer.createObject();
  data["ip"] = WiFi.localIP().toString();
  char payload[30];
  data.printTo(payload, sizeof(payload));
  return String(payload);
}

void setRelayStatus(int pin, boolean enabled) {
  if (pin == RELAY_PIN) {
    // Output GPIOs state
    digitalWrite(RELAY, enabled ? HIGH : LOW);
    // Update GPIOs state
    relayState = enabled;
  }
}

void smartConfig() {
  if (digitalRead(SMART_CONFIG_BUTTON)) {
    return;
  }

  threadDelay(2000);
  if (digitalRead(SMART_CONFIG_BUTTON)) {
    return;
  }
  WiFi.disconnect();

  while(WiFi.status() != WL_CONNECTED) {
    enable500();
    threadDelay(500);
    WiFi.beginSmartConfig();
    while(1) {
      enable200();
      threadDelay(200);
      if (WiFi.smartConfigDone()) {
        break;
      }
    }
  }

  enable5000();

  strcpy(wifiAP, WiFi.SSID().c_str());
  strcpy(wifiPassword, WiFi.psk().c_str());

  for (int i = 0; i < 32; ++i) {
    EEPROM.write(i, wifiAP[i]);
  }

  for (int i = 32; i < 96; ++i) {
    EEPROM.write(i, wifiPassword[i - 32]);
  }
  EEPROM.commit();
}

void smartBlink() {
  if (blinkStatus) {
    closeBlink();
  } else {
    openBlink();
  }
}

void openBlink() {
  digitalWrite(SMART_CONFIG_LED, LOW);
  blinkStatus = true;
}

void closeBlink() {
  digitalWrite(SMART_CONFIG_LED, HIGH);
  blinkStatus = false;
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  smartConfig();
  server.handleClient();
  controll.run();
}

void InitWiFi() {
  // attempt to connect to WiFi network
  // set for STA mode
  WiFi.mode(WIFI_STA);

  WiFi.begin(wifiAP, wifiPassword);
  configWiFiWithSmartConfig();
}

void configWiFiWithSmartConfig() {
  while (WiFi.status() != WL_CONNECTED) {
    enable5000();
    threadDelay(1000);
    smartConfig();
  }
  enable5000();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    if (WiFi.status() != WL_CONNECTED) {
      configWiFiWithSmartConfig();
    }
    // Attempt to connect (clientId, username, password)
    if (client.connect("ESP8266 Relay", token, NULL)) {
      client.subscribe("v1/devices/me/rpc/request/+");
      client.publish("v1/devices/me/attributes", getRelayStatus().c_str());
      client.publish("v1/devices/me/attributes", getLocalIP().c_str());
    } else {
      // Wait 5 seconds before retrying
      enable5000();
      threadDelay1(5000);
    }
  }
  enable5000();
}

void handleSetToken() {
  boolean updated = false;
  for (uint8_t i=0; i<server.args(); i++){
    if (server.argName(i) == "token") {
        strcpy(token, server.arg(i).c_str());
        updated = true;
        break;
    };
  }
  if (updated) {
    for (int i = 96; i < 126; ++i) {
      EEPROM.write(i, token[i - 96]);
    }
    EEPROM.commit();
    client.disconnect();
  }
  server.send(200, "text/plain", "OK");
}

void handleNotFound(){
  server.send(404, "text/plain", "Not Found");
}
