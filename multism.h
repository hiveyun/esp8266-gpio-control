#ifndef __multism_h__
#define __multism_h__
#ifdef __cplusplus
extern "C" {
#endif
bool flushEventQueue(void);
int initEventQueue();
extern void DEBUG_println(const char *);
#ifdef __cplusplus
}
#endif
#endif

