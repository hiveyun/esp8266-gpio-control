#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "fsm.h"

void setup() {
    EEPROM.begin(512);
    delay(10);
}

void loop() {
  fsm_loop(NULL);
}
