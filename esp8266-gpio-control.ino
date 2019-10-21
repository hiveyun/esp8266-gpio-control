#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "fsm.h"
#include "multism.h"
#include "config.h"

int minHeap = 0;
unsigned long heapDelay = 1000;
unsigned long heapTimer = millis();

void setup() {
    #if DEBUG
    Serial.begin(115200);
    #endif
    EEPROM.begin(512);
    initEventQueue();
    delay(10);
    minHeap = ESP.getFreeHeap() / 2;
    #if DEBUG
    Serial.printf("minHeap: %d\r\n", minHeap);
    #endif
}

#if AUTO_REBOOT
bool canReboot() {
    if (strcmp(switch1_Current_state_name(), "on") == 0) {
        return false;
    }
    return true;
}
#endif

void loop() {
  fsm_loop(NULL);
  flushEventQueue();
  #if AUTO_REBOOT
  if (REBOOT_TIMER < millis() && canReboot()) {
    ESP.reset();
  }
  #endif
  if (heapTimer + heapDelay < millis()) {
    heapTimer = millis();
    #if DEBUG
    Serial.printf("heap: %d\r\n", ESP.getFreeHeap());
    Serial.printf("heapFragmentation: %d\r\n", ESP.getHeapFragmentation());
    Serial.printf("maxFreeBlockSize: %d\r\n", ESP.getMaxFreeBlockSize());
    #endif
    if (minHeap > ESP.getFreeHeap()) {
        ESP.reset();
    }
  }
}
