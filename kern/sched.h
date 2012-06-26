#ifndef __SCHED_H__
#define __SCHED_H__

// This function does not return.
void sched_yield(void) __attribute__((noreturn));

#endif
