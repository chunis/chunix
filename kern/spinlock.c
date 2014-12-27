//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#include <spinlock.h>
#include <printf.h>
#include <x86.h>

void init_lock(struct spinlock *slock, char *name)
{
	slock->name = name;
	slock->hold = 0;
}

void acquire(struct spinlock *slock)
{
	if(is_hold(slock))
		panic("acquire spinlock");
	while(xchg(&slock->hold, 1) != 0)
		;  // do nothing, just wait
}

void release(struct spinlock *slock)
{
	if(!is_hold(slock))
		panic("release spinlock");
	xchg(&slock->hold, 0);
}
