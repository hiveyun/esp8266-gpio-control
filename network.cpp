#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "fsm_ext.h"

char wifiAP[32];
char wifiPassword[64];

unsigned long smartconfigTimer = millis();

void initWiFi(void) {
    WiFi.mode(WIFI_STA);

    for (int i = 0; i < 32; ++i) {
      wifiAP[i] = char(EEPROM.read(i));
    }

    for (int i = 32; i < 96; ++i) {
      wifiPassword[i - 32] = char(EEPROM.read(i));
    }

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

        for (int i = 0; i < 32; ++i) {
          EEPROM.write(i, wifiAP[i]);
        }

        for (int i = 32; i < 96; ++i) {
          EEPROM.write(i, wifiPassword[i - 32]);
        }
        EEPROM.commit();
    } else {
        if (millis() - smartconfigTimer > 120000) {
            smartconfig_timeout(NULL);
        }
    }
}
