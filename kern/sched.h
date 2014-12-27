//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#ifndef __SCHED_H__
#define __SCHED_H__

#include <types.h>

extern uint32_t timer_ticks;

void sched_yield(void);
void yield(void);

#endif
