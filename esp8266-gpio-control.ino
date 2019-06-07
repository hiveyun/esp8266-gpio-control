#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "fsm_ext.h"
#include "multism.h"
#include "mqtt.h"

#define RELAY_1 12
#define RELAY_2 13
#define BUTTON_1 16
#define BUTTON_2 14
#define SOUND 15

unsigned long button1PressTimer = millis();
unsigned long button2PressTimer = millis();

void setup() {
    EEPROM.begin(512);
    pinMode(RELAY_1, OUTPUT);
    pinMode(RELAY_2, OUTPUT);
    pinMode(BUTTON_1, INPUT_PULLUP);
    pinMode(BUTTON_2, INPUT_PULLUP);

    pinMode(SOUND, OUTPUT);
    initEventQueue();
    delay(10);
}

void loop() {
  fsm_check(NULL);
  flushEventQueue();
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
    if (button1PressTimer + 2000 < millis()) {
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
    getMqttClient().publish("/attributes", payload);
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
