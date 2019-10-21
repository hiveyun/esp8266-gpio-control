#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "fsm.h"
#include "multism.h"
#include "config.h"

void setup() {
    EEPROM.begin(512);
    initEventQueue();
    #if DEBUG
    Serial.begin(115200);
    #endif
    delay(10);
}

bool canRebot() {
    if (strcmp(switch1_Current_state_name(), "on") == 0) {
        return false;
    }
    return false;
}

void loop() {
  fsm_loop(NULL);
  flushEventQueue();
  #if AUTO_REBOOT
  if (REBOOT_TIMER < millis() && canRebot()) {
    ESP.reset();
  }
  #endif
}
