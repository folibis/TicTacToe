#ifndef TIMER_H
#define TIMER_H

#include "common.h"


typedef void (*timeout_callback)(void);

extern int add_timer(timeout_callback callback, unsigned int delay, int loop, int active);
extern result_t reset_timer(int id);
extern result_t start_timer(int id);
extern result_t stop_timer(int id);
extern result_t close_all_timers();


#endif // TIMER_H
