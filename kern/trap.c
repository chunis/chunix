#include "printf.h"
#include "task.h"
#include <trap.h>

static const char *exceptions[] = {
	"Divide error",
	"Debug exceptions",
	"Non-maskable interrupt (NMI)",
	"Breakpoint",
	"Overflow",
	"Bounds check",
	"Invalid opcode",
	"Coprocessor not available",
	"Double fault",
	"Coprocessor segment overrun",
	"Invalid TSS",
	"Segment not present",
	"Stack exception",
	"General protection exception",
	"Page fault",
	"Intel reserved",
	"Coprocessor error",
};

void dump_tf(STACK_FRAME *tf)
{
	//clean_screen();

	printf("Exception %d: %s\n\n", tf->trapno, exceptions[tf->trapno]);
	printf("gs:\t%x,\tfs:\t%x\n", tf->gs, tf->fs);
	printf("es:\t%x,\tds:\t%x\n", tf->es, tf->ds);
	printf("edi:\t%x,\tesi:\t%x\n", tf->edi, tf->esi);
	printf("ebp:\t%x\nold_esp:%x\n", tf->ebp, tf->old_esp);
	printf("ebx:\t%x,\tedx:\t%x\n", tf->ebx, tf->edx);
	printf("ecx:\t%x,\teax:\t%x\n", tf->ecx, tf->eax);

	printf("trapno:\t%x,\terr:\t%x\n", tf->trapno, tf->err);
	printf("ret_addr:%x\n", tf->ret_addr);
	printf("eip:\t%x\tcs:\t%x\n", tf->eip, tf->cs);
	printf("eflags:\t%x\n", tf->eflags);
	printf("esp:\t%x\tss:\t%x\n", tf->esp, tf->ss);

	panic("dump_tf");

}
void trap(STACK_FRAME *tf)
{
	if(tf->trapno == T_IRQ0 + IRQ_TIMER){
		timer_isr();
		return;
	}
	if(tf->trapno == T_IRQ0 + IRQ_KBD){
		keyboard_isr();
		return;
	}
	if(tf->trapno == T_IRQ0 + IRQ_IDE){
		printf("IDE read/write interrupt\n");
		hd_isr();
		return;
	}

	if(tf->trapno == T_PGFLT){
		printf("page fault\n");
		dump_tf(tf);
	}

	if(tf->trapno == T_SYSCALL){
		syscall();
		return;
	}

	// something wrong happened
	printf("tf->trapno: %d\n", tf->trapno);
	dump_tf(tf);

	return;
}
