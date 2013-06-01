#include <types.h>
#include <x86.h>
#include <mmu.h>
#include <console.h>
#include <kbd.h>
#include <multiboot.h>
#include <string.h>
#include <printf.h>

#include "task.h"
#include "descriptor.h"
#include "mm.h"
#include "sched.h"


static void init_8259A(void)
{
	outb(0x20, 0x11);
	outb(0xa0, 0x11);
	outb(0x21, 0x20);	// IRQ0 ~ IRQ7
	outb(0xa1, 0x28);	// IRQ8 ~ IRQ15
	outb(0x21, 0x04);
	outb(0xa1, 0x02);
	outb(0x21, 0x01);
	outb(0xa1, 0x01);

	// disable all interrupts
	outb(0x21, (0xff & ~(1<<2)));  // enable IRQ2 for slave 8259A
	outb(0xa1, 0xff);
}

int main(struct multiboot_info *mboot_ptr)
{
	char *os_str = "Welcome to ChuniX! :)\n";
	extern uint32_t edata[], end[];
	TASK_STRUCT *mytask1;
	TASK_STRUCT *mytask2;
	extern char _binary_user_hello_start[], _binary_user_hello_size[];
	extern char _binary_user_todo_start[], _binary_user_todo_size[];
	int eax;

	// save %eax to check if booted from multiboot loader
	__asm __volatile("movl %%eax, %0" : "=r" (eax));

	// TODO: memset will make system crash, why?
	// clear BSS section
	// memset(edata, 0, end - edata);

	cons_init();
	printf("%s\n", os_str);

	if(eax == MBT_BOOTLOADER_MAGIC){
		settextcolor(12, 0);
		dump_multiboot(mboot_ptr);
		settextcolor(13, 0);
		dump_ext2(*(uint32_t*)mboot_ptr->mods_addr);
		resettextcolor();
	}

	mem_init();
	setupkvm();
	init_gdt();
	//dump_gdt();
	init_idt();

	init_8259A();
	install_timer(100);
	init_keyboard();
	init_hd();
	init_buffer();

#if 0
	rootp = 0;
	current = rootp;
	mytask1 = task_create(_binary_user_hello_start,
			(uint32_t)_binary_user_hello_size);
	mytask2 = task_create(_binary_user_todo_start,
			(uint32_t)_binary_user_todo_size);
	//task_run(mytask1);
	//task_run(mytask2);
	if(!current)
		current = rootp;

	// now enable interrupt
	asm("sti");

	// loop forever, drop here whenever no task to run
	while(1)
		sched_yield();
#endif

	// would never get here. So we'll wait forever...
	wheel();
	return 0;
}

// boot page table
// should be aligned to PGSIZE
// use PTE_PS to enable 4MB pages
__attribute__((__aligned__(PGSIZE)))
pde_t entrypgdir[1024] = {
	// Map VA's [0, 4MB) to PA's [0, 4MB)
	[0] = (0) | PTE_P | PTE_W | PTE_PS,
	// Map VA's [KERNBASE, KERNBASE+4MB) to PA's [0, 4MB)
	[KERNBASE>>PDXSHIFT] = (0) | PTE_P | PTE_W | PTE_PS,
};
