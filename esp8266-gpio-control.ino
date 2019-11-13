#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "fsm.h"
#include "multism.h"
#include "config.h"

int minHeap = 0;
unsigned long heapDelay = 1000;
unsigned long heapTimer = 0;

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

void loop() {
  if (!flushEventQueue()) {
    ESP.reset();
  }
  #if AUTO_REBOOT
  if (REBOOT_TIMER < millis()) {
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
    if (minHeap > (int)ESP.getFreeHeap()) {
        ESP.reset();
    }
  }
}
