#include <types.h>
#include "task.h"
#include "global.h"

#define STACK_SIZE	256

TASK_STRUCT task0 = {
	{ // STACK_FRAME
		USR_DATA, // gs
		USR_DATA, // fs
		USR_DATA, // es
		USR_DATA, // ds
		0, // edi
		0, // esi
		0, // ebp
		0, // old_esp
		0, // ebx
		0, // edx
		0, // ecx
		0, // eax
		0, // ret_addr
		0, // eip
		USR_CODE, // cs
		0, // eflags
		0, // esp
		USR_DATA, // ss
	},
	{
		0, 0, // ldt[2]
	},
	0, // ldt_sel
	0, // pid
	"", // name[24]
	TS_RUNNING, // state
	INIT_PRIO, // priority
	0, // next
};

static void delay(int time)
{
	int i, j, k;
	for(i=0; i<time; i++)
		for(j=0; j<100; j++)
			for(k=0; k<1000; k++)
				;
}

void taskA(void)
{
	int i = 0;

	__asm__ ("movb	%%al, 0xb8000+160*24+6"::"a"(i++));

	/* while(1){
		put_c(i++);
		delay(1);
	} */
}

void taskB(void)
{
	int i = 80;

	__asm__ ("movb	%%al, 0xb8000+160*24+20"::"a"(i++));
}
