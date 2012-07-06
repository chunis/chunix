#include "sched.h"
#include "task.h"

// task scheduler. pick a task and run it.
void sched_yield(void)
{
	TASK_STRUCT *tp;

	asm("sti");

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

