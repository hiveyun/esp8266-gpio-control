#include <ESP8266WiFi.h>
#include "fsm_ext.h"
#include "storage.h"

char wifiAP[32];
char wifiPassword[64];

unsigned long smartconfigTimer = 0;

void initWiFi(void) {
    WiFi.mode(WIFI_STA);

    storage_read(wifiAP, WIFI_AP_ADDR, WIFI_AP_LENGTH);
    storage_read(wifiPassword, WIFI_PASSWORD_ADDR, WIFI_PASSWORD_LENGTH);

    WiFi.begin(wifiAP, wifiPassword);
}

void networkCheck(const network_loop_t *a1) {
    if (WiFi.status() == WL_CONNECTED) {
        network_online(NULL);
    } else {
        network_offline(NULL);
    }
}

void reconnect(void) {
    WiFi.reconnect();
}

void beginSmartconfig(void) {
    WiFi.disconnect();
    while(WiFi.status() == WL_CONNECTED) {
        delay(100);
    }
    WiFi.beginSmartConfig();
    smartconfigTimer = millis();
}

void smartconfigDone(const smartconfig_loop_t *) {
    if (WiFi.smartConfigDone()) {
        smartconfig_done(NULL);
        strcpy(wifiAP, WiFi.SSID().c_str());
        strcpy(wifiPassword, WiFi.psk().c_str());

        storage_write(wifiAP, WIFI_AP_ADDR, WIFI_AP_LENGTH);
        storage_write(wifiPassword, WIFI_PASSWORD_ADDR, WIFI_PASSWORD_LENGTH);

    } else {
        if (millis() - smartconfigTimer > 120000) {
            smartconfig_timeout(NULL);
        }
    }
}
