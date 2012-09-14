#ifndef __SCHED_H__
#define __SCHED_H__

#include <types.h>

extern uint32_t timer_ticks;

void sched_yield(void);

#endif
