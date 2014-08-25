#ifndef __SPINLOCK_H__
#define __SPINLOCK_H__

#include <types.h>

#define is_hold(slock) (slock->hold == 1)

struct spinlock {
	uint32_t hold;
	char *name;	// name for debug
};

void init_lock(struct spinlock *slock, char *name);
void acquire(struct spinlock *slock);
void release(struct spinlock *slock);

#endif
