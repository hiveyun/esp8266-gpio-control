#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define SMART_CONFIG_BUTTON 0
#define SMART_CONFIG_LED 2

#define GPIO0 5
#define GPIO0_PIN 1

// We assume that all GPIOs are LOW
boolean gpioState = false;

bool led_status = false;

char thingsboardServer[] = "tb.codecard.cn";

WiFiClient wifiClient;

PubSubClient client(wifiClient);

ESP8266WebServer server(80);

char wifi_ap[32];
char wifi_password[64];
char token[30];

void setup() {
  EEPROM.begin(512);
  pinMode(GPIO0, OUTPUT);
  pinMode(SMART_CONFIG_LED, OUTPUT);
  digitalWrite(SMART_CONFIG_LED, HIGH);
  pinMode(SMART_CONFIG_BUTTON, INPUT_PULLUP);
  delay(10);

  for (int i = 0; i < 32; ++i) {
    wifi_ap[i] = char(EEPROM.read(i));
  }

  for (int i = 32; i < 96; ++i) {
    wifi_password[i - 32] = char(EEPROM.read(i));
  }

  for (int i = 96; i < 126; ++i) {
    token[i - 96] = char(EEPROM.read(i));
  }


  InitWiFi();
  client.setServer( thingsboardServer, 1883 );
  client.setCallback(on_message);

  server.on("/update_token", HTTP_POST, handleSetToken);
  server.onNotFound(handleNotFound);
  server.begin();
}

// The callback for when a PUBLISH message is received from the server.
void on_message(const char* topic, byte* payload, unsigned int length) {

  char json[length + 1];
  strncpy (json, (char*)payload, length);
  json[length] = '\0';

  // Decode JSON request
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& data = jsonBuffer.parseObject((char*)json);

  if (!data.success())
  {
    return;
  }

  // Check request method
  String methodName = String((const char*)data["method"]);

  if (methodName.equals("getGpioStatus")) {
    // Reply with GPIO status
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    client.publish(responseTopic.c_str(), get_gpio_status().c_str());
  } else if (methodName.equals("setGpioStatus")) {
    // Update GPIO status and reply
    set_gpio_status(data["params"]["pin"], data["params"]["enabled"]);
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    client.publish(responseTopic.c_str(), get_gpio_status().c_str());
    client.publish("v1/devices/me/attributes", get_gpio_status().c_str());
  }
}

String get_gpio_status() {
  // Prepare gpios JSON payload string
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& data = jsonBuffer.createObject();
  data[String(GPIO0_PIN)] = gpioState ? true : false;
  char payload[256];
  data.printTo(payload, sizeof(payload));
  String strPayload = String(payload);
  return strPayload;
}

String get_driver_ip() {
  // Prepare gpios JSON payload string
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& data = jsonBuffer.createObject();
  data["ip"] = WiFi.localIP().toString();
  char payload[256];
  data.printTo(payload, sizeof(payload));
  String strPayload = String(payload);
  return strPayload;
}

void set_gpio_status(int pin, boolean enabled) {
  if (pin == GPIO0_PIN) {
    // Output GPIOs state
    digitalWrite(GPIO0, enabled ? HIGH : LOW);
    // Update GPIOs state
    gpioState = enabled;
  }
}

void smart_config() {
  if (digitalRead(SMART_CONFIG_BUTTON)) {
    return;
  }

  delay(2000);
  if (digitalRead(SMART_CONFIG_BUTTON)) {
    return;
  }
  WiFi.disconnect();

  bool led_status = true;
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    smart_blink();
    WiFi.beginSmartConfig();
    while(1){
      delay(200);
      smart_blink();
      if (WiFi.smartConfigDone()) {
        break;
      }
    }
  }

  digitalWrite(SMART_CONFIG_LED, HIGH);

  strcpy(wifi_ap, WiFi.SSID().c_str());
  strcpy(wifi_password, WiFi.psk().c_str());

  for (int i = 0; i < 32; ++i) {
    EEPROM.write(i, wifi_ap[i]);
  }

  for (int i = 32; i < 96; ++i) {
    EEPROM.write(i, wifi_password[i - 32]);
  }
  EEPROM.commit();
}

void smart_blink() {
  if (led_status) {
    digitalWrite(SMART_CONFIG_LED, HIGH);
    led_status = false;
  } else {
    digitalWrite(SMART_CONFIG_LED, LOW);
    led_status = true;
  }
}

void loop() {
  if ( !client.connected() ) {
    reconnect();
  }

  smart_config();
  client.loop();
  server.handleClient();
}

void InitWiFi() {
  // attempt to connect to WiFi network
  // set for STA mode
  WiFi.mode(WIFI_STA);

  WiFi.begin(wifi_ap, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    smart_blink();
    smart_config();
  }
  digitalWrite(SMART_CONFIG_LED, HIGH);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    if ( WiFi.status() != WL_CONNECTED) {
      while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        smart_blink();
        smart_config();
      }
    }
    // Attempt to connect (clientId, username, password)
    if ( client.connect("ESP8266 Relay", token, NULL) ) {
      client.subscribe("v1/devices/me/rpc/request/+");
      client.publish("v1/devices/me/attributes", get_gpio_status().c_str());
      client.publish("v1/devices/me/attributes", get_driver_ip().c_str());
    } else {
      // Wait 5 seconds before retrying

      unsigned long lastSend = millis();
      while (millis() - lastSend < 5000) {
        smart_config();
        server.handleClient();
      }
      smart_blink();
    }
  }
  digitalWrite(SMART_CONFIG_LED, HIGH);
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
