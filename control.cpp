// https://github.com/bblanchon/ArduinoJson.git
#include <ArduinoJson.h>

#include "fsm_ext.h"
#include "mqtt.h"

String genJson(String key, int val) {
    DynamicJsonDocument data(100);
    data[key] = val;
    char payload[100];
    serializeJson(data, payload);
    return String(payload);
}

String genStateJson(int index, int val) {
    return genJson("relay_" + String(index) + "_state", val);
}

String genResultJson(String val) {
    DynamicJsonDocument data(100);
    data["result"] = val;
    char payload[100];
    serializeJson(data, payload);
    return String(payload);
}

String genErrJson(String val) {
    DynamicJsonDocument data(100);
    data["err"] = val;
    char payload[100];
    serializeJson(data, payload);
    return String(payload);
}

String genRelayState(int index, String state) {
    if (state.equals("on")) {
        return genStateJson(index, 1);
    } else if (state.equals("off")) {
        return genStateJson(index, 0);
    } else {
        return genErrJson(state);
    }
}

String getRelayState(int index) {
    // Prepare relays JSON payload string
    String state;
    if (index == 1) {
        state = String(relay1_Current_state_name());
    } else if (index == 2) {
        state = String(relay2_Current_state_name());
    } else {
        state = "Relay " + String(index) + " not exists.";
    }
    return genRelayState(index, state);
}

String setRelayOn(int index) {
    // Prepare relays JSON payload string
    String state = "on";
    if (index == 1) {
        relay1_on(NULL);
    } else if (index == 2) {
        relay2_on(NULL);
    } else {
        state = "Relay " + String(index) + " not exists.";
    }
    return genRelayState(index, state);
}

String setRelayOff(int index) {
    // Prepare relays JSON payload string
    String state = "off";
    if (index == 1) {
        relay1_off(NULL);
    } else if (index == 2) {
        relay2_off(NULL);
    } else {
        state = "Relay " + String(index) + " not exists.";
    }
    return genRelayState(index, state);
}

void publishRelayState(int index, int state) {
    mqttPublish("/attributes", String(genStateJson(index, state)).c_str());
}

void onMessage(const mqtt_message_t * msg) {
    char json[msg -> length + 1];
    strncpy(json, (char*)msg -> payload, msg->length);
    json[msg -> length] = '\0';
    // Decode JSON request
    DynamicJsonDocument data(256);
    deserializeJson(data, json);

    // Check request method
    String methodName = String((const char*)data["method"]);
    String responseTopic = String(msg->topic);
    String rsp;
    responseTopic.replace("request", "response");

    if (methodName.equals("relay_state")) {
        rsp = getRelayState(data["index"]);
    } else if (methodName.equals("relay_on")) {
        rsp = setRelayOn(data["index"]);
    } else if (methodName.equals("relay_off")) {
        rsp = setRelayOff(data["index"]);
    } else if (methodName.equals("ping")) {
        rsp = genResultJson("pong");
    } else {
        rsp = genErrJson("Not Support");
    }
    mqttPublish(responseTopic.c_str(), rsp.c_str());
}
