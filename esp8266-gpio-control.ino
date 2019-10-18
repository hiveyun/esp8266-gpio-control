#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "fsm.h"
#include "config.h"

void setup() {
    EEPROM.begin(512);
    #if DEBUG
    Serial.begin(115200);
    #endif
    delay(10);
}

void loop() {
  fsm_loop(NULL);
}
