#include <types.h>
#include "task.h"
#include "global.h"
#include "descriptor.h"

#define SA_RPL1     1

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

// TODO: task->ldt[].attr need more improvement
void new_task(TASK_STRUCT *task, uint32_t eip, uint32_t stack3, uint32_t sel)
{
	memmove(task, &task0, sizeof(TASK_STRUCT));

	// set task->ldt[]
	memmove(task->ldt, &gdt[KER_CODE], sizeof(DESCRIPTOR)*LDT_SIZE);
	task->ldt[0].attr1 = 0x98 | 1<<5;
	task->ldt[1].attr1 = 0x92 | 1<<5;

	task->regs.cs = 0 & (TI_LDT << 2) | SA_RPL1;
	task->regs.ds = 8 & (TI_LDT << 2) | SA_RPL1;
	task->regs.es = 8 & (TI_LDT << 2) | SA_RPL1;
	task->regs.fs = 8 & (TI_LDT << 2) | SA_RPL1;
	task->regs.gs = 8 & (TI_LDT << 2) | SA_RPL1;
	task->regs.ss = 8 & (TI_LDT << 2) | SA_RPL1;

	task->ldt_sel = sel;
	task->regs.eip = eip;
	task->regs.esp = (uint32_t)stack3 + USR_STACK_SIZE;
	task->regs.eflags = 0x3202;

	// add ldt to gdt
	set_descriptor((DESCRIPTOR *)&gdt[sel], task->ldt,
			LDT_SIZE*sizeof(DESCRIPTOR)-1, 0x82);	// 0x82? 0xcf9a?
}

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
