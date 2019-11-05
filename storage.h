#ifndef __storage_h__
#define __storage_h__
#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"

void storage_read(char * buff, int addrStart, int length);
void storage_write(char * buff, int addrStart, int length);

#if KEEP_SWITCH_STATE

char switch_read(int idx);
void switch_write(char v, int idx);

#endif

#define WIFI_AP_ADDR 0
#define WIFI_AP_LENGTH 32
#define WIFI_PASSWORD_ADDR 32
#define WIFI_PASSWORD_LENGTH 64
#define MQTT_PASSWORD_ADDR 96
#define MQTT_PASSWORD_LENGTH 40

#if KEEP_SWITCH_STATE
#define SWITCH_START_ADDR 136
#endif

#ifdef __cplusplus
}
#endif
#endif
