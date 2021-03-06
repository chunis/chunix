//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#include <types.h>
#include <x86.h>
#include <mmu.h>
#include <console.h>
#include <kbd.h>
#include <multiboot.h>
#include <string.h>
#include <printf.h>
#include <fs.h>

#include "task.h"
#include "descriptor.h"
#include "mm.h"
#include "sched.h"


extern struct fs_node *fs_root;	// the root of the file system
extern struct task *inittask;

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
	struct task *mytask1;
	struct task *mytask2;
	extern char _binary_user_init_start[], _binary_user_init_size[];
	extern char _binary_user_hello_start[], _binary_user_hello_size[];
	int eax;

	struct multiboot_mod_list *mod;
	uint32_t *mm_start = end;
	uint32_t mm_size = 0;
	uint32_t ramhd_start;

	// save %eax to check if booted from multiboot loader
	__asm __volatile("movl %%eax, %0" : "=r" (eax));

	// TODO: memset will make system crash, why?
	// clear BSS section
	// memset(edata, 0, end - edata);

	cons_init();
	printk("%s\n", os_str);

	if(eax != MBT_BOOTLOADER_MAGIC){  // we must boot from grub now
		printk("PANIC!! Grub is needed for booting ChuniX\n");
		for(;;);
	}

	// check memory
	printk("mm lower: %d\n", mboot_ptr->mem_lower);
	printk("mm upper: %d\n", mboot_ptr->mem_upper);
	mm_size = mboot_ptr->mem_upper + 1024;	// KB

	mod = (struct multiboot_mod_list *)mboot_ptr->mods_addr;
	printk("mod start: %x\n", mod->mod_start);
	printk("mod end: %x\n", mod->mod_end);
	printk("mod cmdline: %s\n", mod->cmdline);
	mm_start = P2V(mod->mod_end);
	ramhd_start = mod->mod_start;
	if(mm_start < end)
		mm_start = end;
	printk("mm_start: %x, mm_size: %d KB\n", mm_start, mm_size);

	settextcolor(12, 0);
	dump_multiboot(mboot_ptr);
	resettextcolor();

	init_memhd((uint8_t *)P2V(mod->mod_start),
			(mod->mod_end - mod->mod_start));

	mem_init1(mm_start, mm_size);
	setupkvm();
	mem_init2();	// init mem between [4M, mm_size]
	init_gdt();
	//dump_gdt();
	init_idt();

	init_8259A();
	install_timer(100);
	init_keyboard();
	init_serial();
	get_time();

	init_hd();
	init_buffer();

	//settextcolor(13, 0);
	//dump_ext2((uint8_t *)P2V(ramhd_start));
	//resettextcolor();

	fs_root = init_ext2();
	open_fs(fs_root, 0);

	//init_sfs();
	//fs_root = init_initrd_sfs();
	//open_fs(fs_root, 0);
	//check_initrd_fs();

#if 0
	rootp = 0;
	current = rootp;
	mytask1 = task_create(_binary_user_init_start,
			(uint32_t)_binary_user_init_size);
	mytask2 = task_create(_binary_user_hello_start,
			(uint32_t)_binary_user_hello_size);
	inittask = mytask1;  // this is our init task, means task 0
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
