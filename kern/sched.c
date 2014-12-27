//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#include "sched.h"
#include "task.h"

// task scheduler. pick a task and run it.
void sched_yield(void)
{
	struct task *tp;

	if(!rootp)
		panic("No task!");

	if(!current)
		current = rootp;

	// search from current->next to the end of list
	for(tp = current->next; tp; tp = tp->next)
		if(tp->state == TS_RUNNABLE)
			task_run(tp);

	// no task runnable
	if(!tp){
		for(tp=rootp; tp != current; tp = tp->next)
			if(tp->state == TS_RUNNABLE)
				task_run(tp);
	}

	task_run(current);
}

void yield(void)
{
	current->state = TS_RUNNABLE;
	sched_yield();
}
