#include <ESP8266WiFi.h>
#include "fsm_ext.h"
#include "config.h"

void SMUDGE_debug_print(const char *a1, const char *a2, const char *a3) {
    #if DEBUG
    Serial.printf("%s %s %s\n", a1, a2, a3);
    #endif
}

void SMUDGE_free(const void *a1) {
    // free((void *)a1);
}

void SMUDGE_panic(void) {
    #if DEBUG
    Serial.println("SMUDGE_panic");
    #endif
}

void SMUDGE_panic_print(const char *a1, const char *a2, const char *a3) {
    #if DEBUG
    Serial.printf("%s %s %s\n", a1, a2, a3);
    #endif
}

void DEBUG_println(const char * a1) {
    #if DEBUG
    Serial.println(a1);
    #endif
}
