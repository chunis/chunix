#include <types.h>
#include "task.h"
#include "global.h"
#include "descriptor.h"
#include "printf.h"
#include "const.h"
#include "fs.h"
#include "string.h"

TASK_STRUCT tasks[NPROC];
int nextpid = 1;

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

void task_init(void)
{
	int i;

	for(i=0; i<NPROC; i++){
		;
	}
	return;
}

TASK_STRUCT *task_alloc(void)
{
	TASK_STRUCT *tp;
	for(tp = tasks; tp < tasks + NPROC; tp++)
		if(tp->state == TS_UNUSED)
			break;
	// no task slot available
	if(tp == tasks + NPROC)
		return NULL;

	tp->pgdir = kalloc();
	if(tp->pgdir == NULL)
		return NULL;

	tp->pid = nextpid++;
	//TODO tp->ppid = ?
	tp->state = TS_STOPPED;

	return tp;
}

// TODO: task->ldt[].attr need more improvement
void new_task(TASK_STRUCT *task, const char *name, uint32_t eip, uint32_t stack3, uint32_t sel)
{
	memmove(task, &task0, sizeof(TASK_STRUCT));
	strncpy(task->name, name, 24);
	task->name[23] = '\0';

	// set task->ldt[]
	memmove(task->ldt, &gdt[KER_CODE], sizeof(DESCRIPTOR)*LDT_SIZE);
	task->ldt[0].attr1 = (DA_C + DA_32) | SA_RPL3<<5;
	task->ldt[1].attr1 = DA_DRW | SA_RPL3<<5;

	task->regs.cs = 0 | SA_TIL | SA_RPL3;
	task->regs.ds = 8 | SA_TIL | SA_RPL3;
	task->regs.es = 8 | SA_TIL | SA_RPL3;
	task->regs.fs = 8 | SA_TIL | SA_RPL3;
	task->regs.gs = 8 | SA_TIL | SA_RPL3;
	task->regs.ss = 8 | SA_TIL | SA_RPL3;

	task->ldt_sel = sel;
	task->regs.eip = eip;
	task->regs.esp = stack3;
	task->regs.eflags = 0x3202;

	// add ldt to gdt
	set_descriptor((DESCRIPTOR *)&gdt[sel], task->ldt,
			LDT_SIZE*sizeof(DESCRIPTOR)-1, DA_LDT);

	/* // output ldt for debug purpose
	int i;
	uint32_t *lp = (uint32_t *)task->ldt;
	printf("Dump_descriptor: ldt:\n");
	for(i=0; i<2; i++) {
		dump_descriptor((DESCRIPTOR *)lp);
		lp += 2;
	} */
	task->state = TS_RUNABLE;
	task->next = rootp;
	rootp = task;
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

	int fd;
	int len = 6;
	char buf[20];
	char *file = "/first_file";
	char *str = "My first file";

#if 0
	fd = open(file, O_CREAT);
	printf("fd = %d\n", fd);
	write(fd, str, strlen(str));
	close(fd);

	fd = open(file, O_RDWR);
	printf("fd = %d\n", fd);
	read(fd, buf, len);
	buf[len] = '\0';
	close(fd);
	printf("In taskA, buf = %s\n", buf);
#endif

#if 1
	while(1){
		printf("%xA.", i++);
		delay(120);
	}
#endif
	monitor();
}

void taskB(void)
{
	int i = 80;

	while(1){
		printf("%xB.", i++);
		delay(120);
	}
}
