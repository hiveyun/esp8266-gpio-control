#ifndef __multism_h__
#define __multism_h__
#ifdef __cplusplus
extern "C" {
#endif
void flushEventQueue(void);
int initEventQueue();
extern void DEBUG_println(const char *);
#define MAX_QUEUE_LENGTH 50
unsigned long getEventSize();
#ifdef __cplusplus
}
#endif
#endif

