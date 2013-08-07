#ifndef __TASK_H__
#define __TASK_H__

#include <types.h>
#include "descriptor.h"
#include <const.h>

#define INIT_PRIO	20
#define LDT_SIZE	2
#define USR_STACK_SIZE 1024

enum TS_STATE {
	TS_UNUSED = 0,
	TS_RUNNING,
	TS_RUNNABLE,
	TS_STOPPED,
	TS_ZOMBIE,
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
	uint32_t	trapno;

	// defined by x86 hardware
	uint32_t	err;
	uint32_t	eip;
	uint32_t	cs;
	uint32_t	eflags;

	uint32_t	esp;
	uint32_t	ss;
} STACK_FRAME;

// saved registers for kernel context switches.
// Contexts are stored at the bottom of the stack they
// describe; the stack pointer is the address of the context.
// The layout of the context matches the layout of the stack in swtch.S
// at the "Switch stacks" comment. Switch doesn't save eip explicitly,
struct context {
	uint32_t edi;
	uint32_t esi;
	uint32_t ebx;
	uint32_t ebp;
	uint32_t eip;
};

typedef struct _task {
	STACK_FRAME *tf;
	struct context *context;     // saved in swtch()

	uint32_t pid;
	uint32_t ppid;	// parent's id
	char name[24];
	pde_t *pgdir;
	char *kstack;	// bottom of kernel stack for this task
	//struct file *ofile[NOFILE];  // opened file
	struct sfs_file_desc *ofile[NOFILE];  // opened file
	enum TS_STATE state;
	int priority;
	void *chan;	// if chan != 0, sleeping on it
	struct inode *cwd;
	struct _task *next;
} TASK_STRUCT;

TSS_STRUCT tss;
TASK_STRUCT *rootp, *current;
TASK_STRUCT *task_create(uint8_t *binary, uint32_t size);
void task_run(TASK_STRUCT *tp);
void task_destroy(TASK_STRUCT *tp);

#endif
