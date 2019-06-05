// Copyright 2017 Bose Corporation.
// This software is released under the 3-Clause BSD License.
// The license can be viewed at https://github.com/smudgelang/smudge/blob/master/LICENSE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "queue.h"
#include "relay.h"
#include "relay_ext.h"

typedef enum {
    BUTTON1,
    BUTTON2,
    MQTT,
    NET,
    RELAY,
    RELAY1,
    RELAY2,
    LED,
} sm_id_t;

typedef struct {
    sm_id_t sm;
    union {
        button1_Event_Wrapper button1;
        button2_Event_Wrapper button2;
        mqtt_Event_Wrapper mqtt;
        net_Event_Wrapper net;
        relay_Event_Wrapper relay;
        relay1_Event_Wrapper relay1;
        relay2_Event_Wrapper relay2;
        led_Event_Wrapper led;
    } wrapper;
} system_message_t;

static queue_t *q;

void flushEventQueue(void) {
    // This function could be running in a parallel thread
    // concurrently with the rest of the system. The important thing
    // is that it pops messages off the queue and sends them to
    // xxx_Handle_Message.
    bool success;
    system_message_t *msg;

    while(size(q) > 0) {
        success = dequeue(q, (void **)&msg);
        if (!success) {
            fprintf(stderr, "Failed to dequeue element.\n");
            exit(-1);
        }
        switch(msg->sm) {
        case BUTTON1:
            // This actually sends the event into the state machine.
            button1_Handle_Message(msg->wrapper.button1);

            // This frees the event payload that's within the wrapper by
            // calling SMUDGE_free on it.
            button1_Free_Message(msg->wrapper.button1);
            break;
        case BUTTON2:
            button2_Handle_Message(msg->wrapper.button2);
            button2_Free_Message(msg->wrapper.button2);
            break;
        case MQTT:
            mqtt_Handle_Message(msg->wrapper.mqtt);
            mqtt_Free_Message(msg->wrapper.mqtt);
            break;
        case NET:
            net_Handle_Message(msg->wrapper.net);
            net_Free_Message(msg->wrapper.net);
            break;
        case RELAY:
            relay_Handle_Message(msg->wrapper.relay);
            relay_Free_Message(msg->wrapper.relay);
            break;
        case RELAY1:
            relay1_Handle_Message(msg->wrapper.relay1);
            relay1_Free_Message(msg->wrapper.relay1);
            break;
        case RELAY2:
            relay2_Handle_Message(msg->wrapper.relay2);
            relay2_Free_Message(msg->wrapper.relay2);
            break;
        case LED:
            led_Handle_Message(msg->wrapper.led);
            led_Free_Message(msg->wrapper.led);
            break;
        }
        // We still need to free the copy of the wrapper itself, since
        // it was malloc'd in turnstile_Send_Message.
        free(msg);
    }
}

void button1_Send_Message(button1_Event_Wrapper e) {
    bool success;
    system_message_t *msg;
    button1_Event_Wrapper *wrapper;

    // The event wrapper is passed in on the stack, so we have to
    // allocate some memory that we can put in the message queue.
    msg = malloc(sizeof(system_message_t));
    if (msg == NULL) {
        fprintf(stderr, "Failed to allocate message memory.\n");
        exit(-1);
    }
    msg->sm = BUTTON1;
    wrapper = &msg->wrapper.button1;
    memcpy(wrapper, &e, sizeof(e));

    // Put the event on the queue, to be popped off later and handled
    // in order.
    success = enqueue(q, msg);
    if (!success) {
        fprintf(stderr, "Failed to enqueue message.\n");
        exit(-1);
    }
}

void button2_Send_Message(button2_Event_Wrapper e) {
    bool success;
    system_message_t *msg;
    button2_Event_Wrapper *wrapper;

    // The event wrapper is passed in on the stack, so we have to
    // allocate some memory that we can put in the message queue.
    msg = malloc(sizeof(system_message_t));
    if (msg == NULL) {
        fprintf(stderr, "Failed to allocate message memory.\n");
        exit(-1);
    }
    msg->sm = BUTTON2;
    wrapper = &msg->wrapper.button2;
    memcpy(wrapper, &e, sizeof(e));

    // Put the event on the queue, to be popped off later and handled
    // in order.
    success = enqueue(q, msg);
    if (!success) {
        fprintf(stderr, "Failed to enqueue message.\n");
        exit(-1);
    }
}

void mqtt_Send_Message(mqtt_Event_Wrapper e) {
    bool success;
    system_message_t *msg;
    mqtt_Event_Wrapper *wrapper;

    // The event wrapper is passed in on the stack, so we have to
    // allocate some memory that we can put in the message queue.
    msg = malloc(sizeof(system_message_t));
    if (msg == NULL) {
        fprintf(stderr, "Failed to allocate message memory.\n");
        exit(-1);
    }
    msg->sm = MQTT;
    wrapper = &msg->wrapper.mqtt;
    memcpy(wrapper, &e, sizeof(e));

    // Put the event on the queue, to be popped off later and handled
    // in order.
    success = enqueue(q, msg);
    if (!success) {
        fprintf(stderr, "Failed to enqueue message.\n");
        exit(-1);
    }
}

void net_Send_Message(net_Event_Wrapper e) {
    bool success;
    system_message_t *msg;
    net_Event_Wrapper *wrapper;

    // The event wrapper is passed in on the stack, so we have to
    // allocate some memory that we can put in the message queue.
    msg = malloc(sizeof(system_message_t));
    if (msg == NULL) {
        fprintf(stderr, "Failed to allocate message memory.\n");
        exit(-1);
    }
    msg->sm = NET;
    wrapper = &msg->wrapper.net;
    memcpy(wrapper, &e, sizeof(e));

    // Put the event on the queue, to be popped off later and handled
    // in order.
    success = enqueue(q, msg);
    if (!success) {
        fprintf(stderr, "Failed to enqueue message.\n");
        exit(-1);
    }
}

void relay_Send_Message(relay_Event_Wrapper e) {
    bool success;
    system_message_t *msg;
    relay_Event_Wrapper *wrapper;

    // The event wrapper is passed in on the stack, so we have to
    // allocate some memory that we can put in the message queue.
    msg = malloc(sizeof(system_message_t));
    if (msg == NULL) {
        fprintf(stderr, "Failed to allocate message memory.\n");
        exit(-1);
    }
    msg->sm = RELAY;
    wrapper = &msg->wrapper.relay;
    memcpy(wrapper, &e, sizeof(e));

    // Put the event on the queue, to be popped off later and handled
    // in order.
    success = enqueue(q, msg);
    if (!success) {
        fprintf(stderr, "Failed to enqueue message.\n");
        exit(-1);
    }
}

void relay1_Send_Message(relay1_Event_Wrapper e) {
    bool success;
    system_message_t *msg;
    relay1_Event_Wrapper *wrapper;

    // The event wrapper is passed in on the stack, so we have to
    // allocate some memory that we can put in the message queue.
    msg = malloc(sizeof(system_message_t));
    if (msg == NULL) {
        fprintf(stderr, "Failed to allocate message memory.\n");
        exit(-1);
    }
    msg->sm = RELAY1;
    wrapper = &msg->wrapper.relay1;
    memcpy(wrapper, &e, sizeof(e));

    // Put the event on the queue, to be popped off later and handled
    // in order.
    success = enqueue(q, msg);
    if (!success) {
        fprintf(stderr, "Failed to enqueue message.\n");
        exit(-1);
    }
}

void relay2_Send_Message(relay2_Event_Wrapper e) {
    bool success;
    system_message_t *msg;
    relay2_Event_Wrapper *wrapper;

    // The event wrapper is passed in on the stack, so we have to
    // allocate some memory that we can put in the message queue.
    msg = malloc(sizeof(system_message_t));
    if (msg == NULL) {
        fprintf(stderr, "Failed to allocate message memory.\n");
        exit(-1);
    }
    msg->sm = RELAY2;
    wrapper = &msg->wrapper.relay2;
    memcpy(wrapper, &e, sizeof(e));

    // Put the event on the queue, to be popped off later and handled
    // in order.
    success = enqueue(q, msg);
    if (!success) {
        fprintf(stderr, "Failed to enqueue message.\n");
        exit(-1);
    }
}

void led_Send_Message(led_Event_Wrapper e) {
    bool success;
    system_message_t *msg;
    led_Event_Wrapper *wrapper;

    // The event wrapper is passed in on the stack, so we have to
    // allocate some memory that we can put in the message queue.
    msg = malloc(sizeof(system_message_t));
    if (msg == NULL) {
        fprintf(stderr, "Failed to allocate message memory.\n");
        exit(-1);
    }
    msg->sm = LED;
    wrapper = &msg->wrapper.led;
    memcpy(wrapper, &e, sizeof(e));

    // Put the event on the queue, to be popped off later and handled
    // in order.
    success = enqueue(q, msg);
    if (!success) {
        fprintf(stderr, "Failed to enqueue message.\n");
        exit(-1);
    }
}

void SMUDGE_debug_print(const char *a1, const char *a2, const char *a3) {
    fprintf(stderr, a1, a2, a3);
}

void SMUDGE_free(const void *a1) {
    free((void *)a1);
}

void SMUDGE_panic(void) {
    exit(-1);
}

void SMUDGE_panic_print(const char *a1, const char *a2, const char *a3) {
    fprintf(stderr, a1, a2, a3);
}

int initEventQueue() {
    q = newq();
    if (q == NULL) {
        fprintf(stderr, "Failed to get a queue.\n");
        return -1;
    }
    return 0;
}
