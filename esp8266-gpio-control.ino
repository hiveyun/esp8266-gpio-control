#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "fsm_ext.h"
#include "multism.h"

#define SOUND 15

void setup() {
    EEPROM.begin(512);
    pinMode(SOUND, OUTPUT);
    initEventQueue();
    delay(10);
}

void loop() {
  fsm_loop(NULL);
  flushEventQueue();
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
