#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "fsm.h"
#include "multism.h"

void setup() {
    EEPROM.begin(512);
    initEventQueue();
    delay(10);
}

void loop() {
  fsm_loop(NULL);
  flushEventQueue();
}
