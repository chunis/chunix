#include <printf.h>
#include <trap.h>
#include <x86.h>
#include <const.h>
#include "task.h"

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
	printk("----- Exception %d: %s -----\n",
			tf->trapno, exceptions[tf->trapno]);
	printk(" gs: %x\t fs: %x\t", tf->gs, tf->fs);
	printk(" es: %x\t ds: %x\n", tf->es, tf->ds);
	printk("edi: %x\tesi: %x\n", tf->edi, tf->esi);
	printk("ebp: %x\told_esp: %x\n", tf->ebp, tf->old_esp);
	printk("ebx: %x\tedx: %x\n", tf->ebx, tf->edx);
	printk("ecx: %x\teax: %x\n", tf->ecx, tf->eax);

	printk("trapno: %x\terr: %x\t", tf->trapno, tf->err);
	printk("ret_addr: %x\n", tf->ret_addr);
	printk("eip: %x\t cs: %x\t", tf->eip, tf->cs);
	printk("eflags: %x\n", tf->eflags);
	printk("esp: %x\t ss: %x\n", tf->esp, tf->ss);

	panic("dump_tf");

}

void trap(STACK_FRAME *tf)
{
	if(read_eflags() & FL_IF)
		panic("FL_IF should not be set!");

	if(tf->trapno == T_IRQ0 + IRQ_TIMER){
		timer_isr();
		return;
	}
	if(tf->trapno == T_IRQ0 + IRQ_KBD){
		keyboard_isr();
		return;
	}
	if(tf->trapno == T_IRQ0 + IRQ_IDE){
		printk("IDE read/write interrupt\n");
		hd_isr();
		return;
	}
	if(tf->trapno == T_IRQ0 + IRQ_SPURIOUS){
		printk("Spurious interrupt (irq 7) is ignored\n");
		return;
	}

	if(tf->trapno == T_PGFLT){
		printk("page fault\n");
		dump_tf(tf);
	}

	if(tf->trapno == T_SYSCALL){
		syscall();
		return;
	}

	// something wrong happened
	printk("tf->trapno: %d\n", tf->trapno);
	dump_tf(tf);

	return;
}
