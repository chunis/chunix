#ifndef __SCHED_H__
#define __SCHED_H__

#include <types.h>

extern uint32_t timer_ticks;

// This function does not return.
void sched_yield(void) __attribute__((noreturn));

#endif
