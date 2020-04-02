#include <ESP8266WiFi.h>
#include "fsm_ext.h"
#include "storage.h"

char wifiAP[32];
char wifiPassword[64];

unsigned long offlineTimer = 0;
bool offline = true;

void initWiFi(void) {
    WiFi.mode(WIFI_STA);

    storage_read(wifiAP, WIFI_AP_ADDR, WIFI_AP_LENGTH);
    storage_read(wifiPassword, WIFI_PASSWORD_ADDR, WIFI_PASSWORD_LENGTH);

    #if DEBUG
    Serial.printf("Storage Read\r\n");
    Serial.printf("WiFi Ap: %s\r\n", wifiAP);
    Serial.printf("WiFi Password: %s\r\n", wifiPassword);
    #endif

    WiFi.begin(wifiAP, wifiPassword);
}

void networkCheck(const network_loop_t *a1) {
    if (WiFi.status() == WL_CONNECTED) {
        offline = false;
        network_online(NULL);
    } else {
        if (offline) {
            // if connect mqtt server failed more then 10 minutes, reset system.
            if (offlineTimer + 600000 < millis()) {
                ESP.reset();
            }
        } else {
            offline = true;
            offlineTimer = millis();
        }
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
}

void smartconfigDone(const smartconfig_loop_t *) {
    if (WiFi.smartConfigDone()) {
        smartconfig_done(NULL);
        strcpy(wifiAP, WiFi.SSID().c_str());
        strcpy(wifiPassword, WiFi.psk().c_str());

        #if DEBUG
        Serial.printf("Smartconfig Done\r\n");
        Serial.printf("WiFi Ap: %s\r\n", wifiAP);
        Serial.printf("WiFi Password: %s\r\n", wifiPassword);
        #endif

        storage_write(wifiAP, WIFI_AP_ADDR, WIFI_AP_LENGTH);
        storage_write(wifiPassword, WIFI_PASSWORD_ADDR, WIFI_PASSWORD_LENGTH);

    }
}
