#ifndef __TASK_H__
#define __TASK_H__

#include <types.h>
#include "descriptor.h"
#include "fs.h"

#define INIT_PRIO	20
#define LDT_SIZE	2
#define USR_STACK_SIZE 1024

enum TS_STATE {
	TS_RUNNING,
	TS_RUNABLE,
	TS_STOPPED
};

typedef struct {
	uint32_t	back_link;
	uint32_t	esp0, ss0;
	uint32_t	esp1, ss1;
	uint32_t	esp2, ss2;
	uint32_t	cr3, eip, eflags;
	uint32_t	eax, ecx, edx, ebx;
	uint32_t	esp, ebp;
	uint32_t	esi, edi;
	uint32_t	es, cs, ss, ds, fs, gs;
	uint32_t	ldt;
	uint32_t	trace_bitmap;
} TSS_STRUCT;

typedef struct {
	uint32_t	gs;
	uint32_t	fs;
	uint32_t	es;
	uint32_t	ds;
	uint32_t	edi;
	uint32_t	esi;
	uint32_t	ebp;
	uint32_t	old_esp;
	uint32_t	ebx;
	uint32_t	edx;
	uint32_t	ecx;
	uint32_t	eax;
	uint32_t	ret_addr;
	uint32_t	eip;
	uint32_t	cs;
	uint32_t	eflags;
	uint32_t	esp;
	uint32_t	ss;
} STACK_FRAME;

typedef struct _task {
	STACK_FRAME regs;

	DESCRIPTOR ldt[LDT_SIZE];
	uint16_t ldt_sel;
	uint32_t pid;
	char name[24];
	struct file_desp *fdp[OFILE];
	enum TS_STATE state;
	int priority;
	struct _task *next;
} TASK_STRUCT;

TASK_STRUCT *current;

void new_task(TASK_STRUCT *task, uint32_t eip, uint32_t stack3, uint32_t sel);
void taskA(void);
void taskB(void);

#endif
