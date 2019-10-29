#ifndef __config_h__
#define __config_h__
#ifdef __cplusplus
extern "C" {
#endif
#define DEBUG 0
#define AUTO_REBOOT 0 // enable auto reboot
#define REBOOT_TIMER 6 * 60 * 60 * 1000 // reboot every 6 hours
#define MAX_QUEUE_LENGTH 50
#ifdef __cplusplus
}
#endif
#endif
