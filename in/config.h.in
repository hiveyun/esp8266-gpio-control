#ifndef __config_h__
#define __config_h__
#ifdef __cplusplus
extern "C" {
#endif

#define DEBUG 0
// collect heap for memory debug
#define DEBUG_HEAP 0


#define AUTO_REBOOT 0 // enable auto reboot
#define REBOOT_TIMER 6 * 60 * 60 * 1000 // reboot every 6 hours
#define MAX_QUEUE_LENGTH 50

// switch state config
// lose power will keep the state
#define KEEP_SWITCH_STATE 1
#define SWITCH_STATE_ON 1
#define SWITCH_STATE_OFF 0
// set default switch state when started if no keep switch state
#define DEFAULT_SWITCH_STATE SWITCH_STATE_OFF

// mqtt server config
#define MQTT_USERNAME "product_key"
#define MQTT_HOST "gw.huabot.com"
#define MQTT_PORT 11883

#ifdef __cplusplus
}
#endif
#endif
