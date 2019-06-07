#include <ESP8266WiFi.h>
#include "fsm_ext.h"

#define LED 2

unsigned long ledTimer = millis();
unsigned long ledDelay = 1000;

void initLed(void) {
    pinMode(LED, OUTPUT);
}

void ledLoop(const led_loop_t *) {
    if (ledTimer + ledDelay < millis()) {
        led_toggle(NULL);
        ledTimer = millis();
    }
}

void led1000(void) {
    ledDelay = 1000;
}

void led200(void) {
    ledDelay = 200;
}

void led500(void) {
    ledDelay = 500;
}

void ledOff(void) {
    digitalWrite(LED, HIGH);
}

void ledOn(void) {
    digitalWrite(LED, LOW);
}
