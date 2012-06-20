#include <types.h>
#include "task.h"
#include "global.h"
#include "descriptor.h"
#include "printf.h"
#include "const.h"
#include "fs.h"
#include "string.h"
#include "mm.h"
#include <mmu.h>
#include <elf.h>
#include <x86.h>

TASK_STRUCT tasks[NPROC];
int nextpid = 1;
extern pde_t *kpgdir;

TASK_STRUCT *task_alloc(void)
{
	TASK_STRUCT *tp;

	for(tp = tasks; tp < tasks + NPROC; tp++)
		if(tp->state == TS_UNUSED)
			break;

	// no task slot available
	if(tp == tasks + NPROC)
		return NULL;

	if((tp->kstack = kalloc()) == NULL){
		return NULL;
	}
	if((tp->pgdir = mapkvm()) == NULL){
		kfree(tp->kstack);
		return NULL;
	}

	tp->pid = nextpid++;
	tp->ppid = 0;	// set parent to 0
	tp->state = TS_STOPPED;
	tp->priority = INIT_PRIO;

	memset(&tp->tf, 0, sizeof(tp->tf));

	tp->tf->cs = USR_CODE | SA_RPL3;
	tp->tf->ds = USR_DATA | SA_RPL3;
	tp->tf->es = USR_DATA | SA_RPL3;
	tp->tf->fs = USR_DATA | SA_RPL3;
	tp->tf->gs = USR_DATA | SA_RPL3;
	tp->tf->ss = USR_DATA | SA_RPL3;

	tp->tf->eflags = FL_IF;
	tp->tf->esp = PGSIZE;
	// we will set tp->tf->eip later.

	tp->state = TS_RUNNABLE;
	tp->next = rootp;
	rootp = tp;
	printf("Added new task: %d\n", tp->pid);

	return tp;
}

// allocate len bytes of physical memory for task tp,
// and map it at virtual address va.
static void
region_alloc(TASK_STRUCT *tp, void *va, uint32_t len)
{
	pte_t *pte;
	char *p;
	int _va = PGROUNDDOWN((uint32_t)va);
	int _va_end = PGROUNDUP((uint32_t)va+len);

	while(_va < _va_end){
		pte = pgdir_walk(tp->pgdir, (void *)_va, 1);
		if(!pte)
			panic("region_alloc failed!");
		p = kalloc();
		if(!p)
			panic("region_alloc failed!");
		*pte = PTE_ADDR(V2P((uint32_t)p)) | PTE_U | PTE_W | PTE_P;
		_va += PGSIZE;
	}
}

// Set up the initial program binary, stack, and processor flags
// for a user process.
// use region_alloc() to make it easier.
static void
load_icode(TASK_STRUCT *tp, uint8_t *binary, uint32_t size)
{
	struct proghdr *ph, *eph;
	struct elf *elfhdr;

	elfhdr = (struct elf *)binary;
	if(elfhdr->e_magic != ELF_MAGIC)
		panic("load_icode failed: not a elf file.");

	ph = (struct proghdr *)(binary + elfhdr->e_phoff);
	eph = ph + elfhdr->e_phnum;
	for(; ph < eph; ph++){
		if(ph->ph_type != ELF_PROG_LOAD)
			continue;

		region_alloc(tp, (void *)ph->ph_va, ph->ph_memsize);
		memmove((void *)ph->ph_va, (void *)(binary+ph->ph_offset), ph->ph_filesize);
		memset((void *)(ph->ph_va + ph->ph_filesize), 0, ph->ph_memsize - ph->ph_filesize);
	}

	// setup tp's eip to e_entry
	tp->tf->eip = elfhdr->e_entry;

	// map one page for the program's initial stack
	region_alloc(tp, (void*)USTACKTOP - PGSIZE, PGSIZE);
}

// allocates a new task with task_alloc(), loads the named elf
// binary into it with load_icode().
void
task_create(uint8_t *binary, uint32_t size)
{
	TASK_STRUCT *tp = NULL;

	tp = task_alloc();
	if(tp){
		lcr3((uint32_t *)P2V((uint32_t)tp->pgdir));
		load_icode(tp, binary, size);
		lcr3((uint32_t *)P2V((uint32_t)kpgdir));
	}
}

// Restores register values in the STACK_FRAME with the 'iret' instruction.
// This exits the kernel and starts executing some task's code.
void
task_pop_tf(STACK_FRAME *tf)
{
	__asm __volatile("movl %0,%%esp\n"
		"\tpopal\n"
		"\tpopl %%es\n"
		"\tpopl %%ds\n"
		"\taddl $0x8,%%esp\n" /* skip tf_trapno and tf_errcode */
		"\tiret"
		: : "g" (tf) : "memory");
	panic("iret failed");
}

// Context switch from current to task tp.
// Note: if this is the first call to task_run, current is NULL.
void
task_run(TASK_STRUCT *tp)
{
	if(tp != current){
		if(current && current->state == TS_RUNNING)
			current->state = TS_RUNNABLE;
		current = tp;
		current->state = TS_RUNNING;
		current->priority--;
		lcr3((uint32_t *)P2V((uint32_t)current->pgdir));
	}

	task_pop_tf(&current->tf);
}

void task_destroy(TASK_STRUCT *tp)
{
	return;
}
