#ifndef __config_h__
#define __config_h__
#ifdef __cplusplus
extern "C" {
#endif
#define DEBUG 0
#define AUTO_REBOOT 0 // enable auto reboot
#define REBOOT_TIMER 6 * 60 * 60 * 1000 // reboot every 6 hours
#define MAX_QUEUE_LENGTH 50

// switch state config
// lose power will keep the state
#define KEEP_SWITCH_STATE 0
#define SWITCH_STATE_ON 1
#define SWITCH_STATE_OFF 0
// set default switch state when started if no keep switch state
#define DEFAULT_SWITCH_STATE SWITCH_STATE_OFF

#ifdef __cplusplus
}
#endif
#endif
