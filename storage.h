#ifndef __storage_h__
#define __storage_h__
#ifdef __cplusplus
extern "C" {
#endif

void storage_read(char * buff, int addrStart, int length);
void storage_write(char * buff, int addrStart, int length);

#define WIFI_AP_ADDR 0
#define WIFI_AP_LENGTH 32
#define WIFI_PASSWORD_ADDR 32
#define WIFI_PASSWORD_LENGTH 64
#define MQTT_PASSWORD_ADDR 96
#define MQTT_PASSWORD_LENGTH 40

#ifdef __cplusplus
}
#endif
#endif
