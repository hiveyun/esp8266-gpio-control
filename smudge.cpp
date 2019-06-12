#include <ESP8266WiFi.h>
#include "fsm_ext.h"
#include "multism.h"

void SMUDGE_debug_print(const char *a1, const char *a2, const char *a3) {
}

void SMUDGE_free(const void *a1) {
    free((void *)a1);
}

void SMUDGE_panic(void) {
}

void SMUDGE_panic_print(const char *a1, const char *a2, const char *a3) {
}

void DEBUG_println(const char * a1) {
}
