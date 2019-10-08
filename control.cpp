// https://github.com/bblanchon/ArduinoJson.git
#include <ArduinoJson.h>

#include "fsm_ext.h"
#include "mqtt.h"

#define JSON_PAYLOAD_LENGTH 256
StaticJsonDocument<JSON_PAYLOAD_LENGTH> jsonData;
char jsonPayload[JSON_PAYLOAD_LENGTH];
char tpl[JSON_PAYLOAD_LENGTH];
char stateName[20];

void genJson(const char * key, int val) {
    jsonData.clear();
    jsonData[key] = val;
    serializeJson(jsonData, jsonPayload);
}

void genStateJson(int index, int val) {
    tpl[0] = '\0';
    sprintf(tpl, "relay_%d_state", index);
    genJson(tpl, val);
}

void genResultJson(const char * val) {
    jsonData.clear();
    jsonData["result"] = val;
    serializeJson(jsonData, jsonPayload);
}

void genErrJson(const char * val) {
    jsonData.clear();
    jsonData["err"] = val;
    serializeJson(jsonData, jsonPayload);
}

void genRelayState(int index, const char * state) {
    if (strcmp(state, "on") == 0) {
        genStateJson(index, 1);
    } else if (strcmp(state, "off") == 0) {
        genStateJson(index, 0);
    } else if (strcmp(state, "ready") == 0) {
        genStateJson(index, 0);
    } else {
        genErrJson(state);
    }
}

void getRelayState(int index) {
    // Prepare relays JSON payload string
    stateName[0] = '\0';
    if (index == 1) {
        sprintf(stateName, "%s", relay1_Current_state_name());
    } else if (index == 2) {
        sprintf(stateName, "%s", relay2_Current_state_name());
    } else {
        sprintf(stateName, "Relay %d not exists.", index);
    }
    genRelayState(index, stateName);
}

void setRelayOn(int index) {
    // Prepare relays JSON payload string
    stateName[0] = '\0';
    sprintf(stateName, "%s", "on");
    if (index == 1) {
        relay1_on(NULL);
    } else if (index == 2) {
        relay2_on(NULL);
    } else {
        stateName[0] = '\0';
        sprintf(stateName, "Relay %d not exists.", index);
    }
    genRelayState(index, stateName);
}

void setRelayOff(int index) {
    // Prepare relays JSON payload string
    stateName[0] = '\0';
    sprintf(stateName, "%s", "off");
    if (index == 1) {
        relay1_off(NULL);
    } else if (index == 2) {
        relay2_off(NULL);
    } else {
        stateName[0] = '\0';
        sprintf(stateName, "Relay %d not exists.", index);
    }
    genRelayState(index, stateName);
}

void publishRelayState(int index, int state) {
    genStateJson(index, state);
    mqttPublish("/attributes", jsonPayload);
}

void onMessage(const mqtt_message_t * msg) {
    char json[msg -> length + 1];
    strncpy(json, (char*)msg -> payload, msg->length);
    json[msg -> length] = '\0';
    // Decode JSON request

    jsonData.clear();
    deserializeJson(jsonData, json);

    // Check request method
    const char * methodName = (const char*)jsonData["method"];
    // strstr
    const char needle[8] = "request";
    const char * tmpStr = strstr(msg->topic, needle);
    const size_t len = strlen(msg->topic);
    const size_t lenTmp = strlen(tmpStr);

    char topic[len + 2];

    char head[len - lenTmp];
    char tail[lenTmp];

    size_t i;

    for (i = 0; i < len - lenTmp; i ++) {
        head[i] = msg->topic[i];
    }

    for (i = 0; i < lenTmp - 7; i ++) {
        tail[i] = tmpStr[i+7];
    }

    sprintf(topic, "%s%s%s", head, "response", tail);

    if (strcmp(methodName, "relay_state") == 0) {
        getRelayState(jsonData["index"]);
    } else if (strcmp(methodName, "relay_on") == 0) {
        setRelayOn(jsonData["index"]);
    } else if (strcmp(methodName, "relay_off") == 0) {
        setRelayOff(jsonData["index"]);
    } else if (strcmp(methodName, "ping") == 0) {
        genResultJson("pong");
    } else {
        genErrJson("Not Support");
    }
    mqttPublish(topic, jsonPayload);
}
