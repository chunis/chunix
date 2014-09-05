#include <types.h>
#include <printf.h>
#include <const.h>
#include <string.h>
#include <mmu.h>
#include <elf.h>
#include <x86.h>
#include "mm.h"
#include "task.h"
#include "descriptor.h"

struct task tasks[NPROC];
int nextpid = 1;
extern pde_t *kpgdir;
extern void trap_ret(void);

void do_nothing(void)
{
	return;
}

struct task *task_alloc(void)
{
	struct task *tp;
	char *sp;

	for(tp = tasks; tp < tasks + NPROC; tp++)
		if(tp->state == TS_UNUSED)
			break;

	// no task slot available
	if(tp == tasks + NPROC)
		return NULL;

	if((tp->kstack = kalloc_page()) == NULL){
		return NULL;
	}

	tp->pid = nextpid++;
	tp->ppid = 0;	// set parent to 0
	tp->state = TS_STOPPED;
	tp->priority = INIT_PRIO;

	tp->heap_top = HEAP_START;
	tp->heap_start = NULL;

	sp = tp->kstack + KSTACKSIZE - sizeof(*tp->tf);
	tp->tf = (struct stack_frame *)sp;

	// Set up new context to start executing at forkret,
	// which returns to trap_ret.
	sp -= 4;
	*(uint32_t *)sp = (uint32_t)trap_ret;

	sp -= sizeof *tp->context;
	tp->context = (struct context *)sp;
	memset(tp->context, 0, sizeof *tp->context);
	tp->context->eip = (uint32_t)do_nothing;

	return tp;
}

// Set up the initial program binary, stack, and processor flags
// for a user process.
// use region_alloc() to make it easier.
static uint32_t load_icode(struct task *tp, uint8_t *binary, uint32_t size)
{
	struct proghdr *ph, *eph;
	struct elf *elfhdr;
	uint32_t offset, range = 0;

	elfhdr = (struct elf *)binary;
	if(elfhdr->e_magic != ELF_MAGIC)
		panic("load_icode failed: not a elf file.");

	ph = (struct proghdr *)(binary + elfhdr->e_phoff);
	eph = ph + elfhdr->e_phnum;
	for(; ph < eph; ph++){
		if(ph->ph_type != ELF_PROG_LOAD)
			continue;

		region_alloc(tp->pgdir, (void *)ph->ph_va, ph->ph_memsize, (PTE_W | PTE_U));
		memmove((void *)ph->ph_va, (void *)(binary+ph->ph_offset), ph->ph_filesize);
		memset((void *)(ph->ph_va + ph->ph_filesize), 0, ph->ph_memsize - ph->ph_filesize);
		offset = ph->ph_va + ph->ph_memsize;
		range = (range >= offset ? range : offset);
	}

	// setup tp's eip to e_entry
	tp->tf->eip = elfhdr->e_entry;

	// map one page for the program's initial stack
	region_alloc(tp->pgdir, (void *)USTACKTOP - PGSIZE, PGSIZE, (PTE_W | PTE_U));

	return range;
}

// allocates a new task with task_alloc(), loads the named elf
// binary into it with load_icode().
struct task *task_create(uint8_t *binary, uint32_t size)
{
	struct task *tp = NULL;
	uint32_t range;

	tp = task_alloc();
	if(tp == NULL){
		printk("create task failed\n");
		return NULL;
	}
	if((tp->pgdir = mapkvm()) == NULL){
		kfree_page(tp->kstack);
		return NULL;
	}

	// setup trap frame
	memset(tp->tf, 0, sizeof(*tp->tf));
	tp->tf->cs = USR_CODE | SA_RPL3;
	tp->tf->ds = USR_DATA | SA_RPL3;
	tp->tf->es = USR_DATA | SA_RPL3;
	tp->tf->fs = USR_DATA | SA_RPL3;
	tp->tf->gs = USR_DATA | SA_RPL3;
	tp->tf->ss = USR_DATA | SA_RPL3;
	tp->tf->eflags = FL_IF;
	tp->tf->esp = USTACKTOP;
	// we will set tp->tf->eip later.

	lcr3((uint32_t *)P2V((uint32_t)tp->pgdir));
	range = load_icode(tp, binary, size);
	lcr3((uint32_t *)P2V((uint32_t)kpgdir));

	tp->sz = range;
	tp->state = TS_RUNNABLE;
	tp->next = rootp;
	rootp = tp;
	printk("Added new task: %d\n", tp->pid);

	return tp;
}

// Restores register values in the struct stack_frame with the 'iret' instruction.
// This exits the kernel and starts executing some task's code.
void task_pop_tf(struct stack_frame *tf)
{
	__asm __volatile("movl %0,%%esp\n"
		"\tpopl %%gs\n"
		"\tpopl %%fs\n"
		"\tpopl %%es\n"
		"\tpopl %%ds\n"
		"\tpopal\n"
		"\taddl $0xC,%%esp\n" //skip trapno, err and ret_addr
		"\tiret"
		: : "g" (tf) : "memory");
	panic("iret failed");
}

// Context switch from current to task tp.
// Note: if this is the first call to task_run, current is NULL.
void task_run(struct task *tp)
{
	static int ltrflag = 0;
	struct task *old_tp = current;

	if(tp != current){
		if(current && current->state == TS_RUNNING)
			current->state = TS_RUNNABLE;
		current = tp;
		current->state = TS_RUNNING;
		current->priority--;
		tss.ss0 = KER_DATA;
		tss.esp0 = current->kstack + KSTACKSIZE;
		if(ltrflag == 0){  // TODO: better to move 'ltrw' to other place
			__asm__ __volatile__("ltrw  %%ax\n\t"::"a"(KER_TSS));
			ltrflag = 1;
		}
		lcr3((uint32_t *)P2V((uint32_t)current->pgdir));
	}

	// task_pop_tf(current->tf);
	swtch(old_tp->context, current->context);
}

void task_destroy(struct task *tp)
{
	return;
}

int fork(void)
{
	int i, pid;
	struct task *tp = NULL;

	tp = task_alloc();
	if(tp == NULL)
		goto fork_fail;

	// copy process state from process 'current'
	if((tp->pgdir = copy_uvm(current->pgdir, current->sz)) == 0)
		goto fork_fail;

	tp->sz = current->sz;
	tp->parent = current;
	*tp->tf = *current->tf;

	// set up task's user stack and copy current task's stack
	if(copy_page(tp->pgdir, (void *)(USTACKTOP - PGSIZE)) == NULL)
		goto fork_fail;

	// clear %eax so that fork returns 0 in the child.
	tp->tf->eax = 0;

	/* TODO
	for(i = 0; i < NOFILE; i++)
		if(current->ofile[i])
			tp->ofile[i] = filedup(current->ofile[i]);
	tp->cwd = idup(current->cwd);
	*/

	pid = tp->pid;
	tp->state = TS_RUNNABLE;
	//strcpy(tp->name, current->name, sizeof(current->name));
	tp->next = rootp;
	rootp = tp;

	return pid;

fork_fail:
	kfree_page(tp->kstack);
	tp->kstack = 0;
	tp->state = TS_UNUSED;
	return -1;
}

void sleep_on(void *chan)
{
	if(!current)
		panic("sleep: current == 0");

	current->chan = chan;
	current->state = TS_STOPPED;
	sched_yield();

	// clean chan
	current->chan = 0;
}

void wakeup(void *chan)
{
	struct task *tp;

	for(tp = rootp; tp; tp = tp->next){
		if(tp->state == TS_STOPPED && tp->chan == chan)
			tp->state = TS_RUNNABLE;
	}
}

int wait(void)
{
	printk("------ in wait() ------\n");
	return -1;
}

void exit(void)
{
	printk("------ in exit() ------\n");
	return;
}
