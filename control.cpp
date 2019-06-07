// https://github.com/bblanchon/ArduinoJson.git
#include <ArduinoJson.h>

#include "fsm_ext.h"
#include "mqtt.h"

String getRelayState(int index) {
    // Prepare relays JSON payload string
    DynamicJsonDocument data(100);
    String state = "on";
    if (index == 1) {
        state = String(relay1_Current_state_name());
    } else if (index == 2) {
        state = String(relay2_Current_state_name());
    } else {
        data["err"] = "relay not exists.";
    }
    if (state.equals("on")) {
        data["relay_2_state"] = 1;
    } else {
        data["relay_2_state"] = 0;
    }
    char payload[100];
    serializeJson(data, payload);
    return String(payload);
}

String setRelayOn(int index) {
    // Prepare relays JSON payload string
    DynamicJsonDocument data(100);
    String state;
    if (index == 1) {
        relay1_on(NULL);
        data["relay_1_state"] = 1;
    } else if (index == 2) {
        relay2_on(NULL);
        data["relay_2_state"] = 1;
    } else {
        data["err"] = "relay not exists.";
    }
    char payload[100];
    serializeJson(data, payload);
    return String(payload);
}

String setRelayOff(int index) {
    // Prepare relays JSON payload string
    DynamicJsonDocument data(100);
    String state;
    if (index == 1) {
        relay1_off(NULL);
        data["relay_1_state"] = 0;
    } else if (index == 2) {
        relay2_off(NULL);
        data["relay_2_state"] = 0;
    } else {
        data["err"] = "relay not exists.";
    }
    char payload[100];
    serializeJson(data, payload);
    return String(payload);
}

void publishRelayState(int index, int state) {
    // Prepare relays JSON payload string
    DynamicJsonDocument data(100);
    if (index == 1) {
        data["relay_1_state"] = state;
    } else if (index == 2) {
        data["relay_2_state"] = state;
    } else {
        return;
    }
    char payload[100];
    serializeJson(data, payload);
    mqttPublish("/attributes", payload);
}

void onMessage(const mqtt_message_t * msg) {
    // Decode JSON request
    DynamicJsonDocument data(256);
    deserializeJson(data, msg->payload);

    // Check request method
    String methodName = String((const char*)data["method"]);
    String responseTopic = String(msg->topic);
    responseTopic.replace("request", "response");

    if (methodName.equals("relay_state")) {
        // Reply with GPIO status
        mqttPublish(responseTopic.c_str(), getRelayState(data["index"]).c_str());
    } else if (methodName.equals("relay_on")) {
        mqttPublish(responseTopic.c_str(), setRelayOn(data["index"]).c_str());
    } else if (methodName.equals("relay_off")) {
        mqttPublish(responseTopic.c_str(), setRelayOff(data["index"]).c_str());
    } else {
        DynamicJsonDocument data(100);
        data["err"] = "Not Support";
        char payload[100];
        serializeJson(data, payload);
        mqttPublish(responseTopic.c_str(), payload);
    }

}
