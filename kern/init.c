#include <types.h>
#include <x86.h>
#include <mmu.h>

#include "global.h"
#include "string.h"
#include "console.h"
#include "printf.h"
#include "task.h"
#include "descriptor.h"
#include "const.h"
#include "mm.h"
#include <trap.h>

extern uint8_t gdt[];
extern uint8_t gdt_ptr[];
extern uint32_t isr_table[ISR_NUM];

typedef void (*int_handler)(void);

static uint32_t task1_stack3[USR_STACK_SIZE];
static uint32_t task2_stack3[USR_STACK_SIZE];

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

// add items to gdt, include kernel/user code/data segment and tss
void init_gdt(void)
{
	uint32_t *gdt_base;
	uint16_t *gdt_lim;

	memset(gdt, 0, sizeof(gdt));
	set_descriptor((DESCRIPTOR *)&gdt[KER_CODE], 0,
			0xfffff, 0xC0<<8 | DA_CR);
	set_descriptor((DESCRIPTOR *)&gdt[KER_DATA], 0,
			0xfffff, 0xC0<<8 | DA_DRW);
	set_descriptor((DESCRIPTOR *)&gdt[USR_CODE], 0,
			0xfffff, (0xC0<<8 | DA_DPL3 | DA_CR));
	set_descriptor((DESCRIPTOR *)&gdt[USR_DATA], 0,
			0xfffff, (0xC0<<8 | DA_DPL3 | DA_DRW));
	// for tss
	memset(&tss, 0, sizeof(tss));
	tss.ss0 = KER_DATA;
	set_descriptor((DESCRIPTOR *)&gdt[KER_TSS], (uint32_t)&tss,
			sizeof(tss)-1, DA_386TSS);

	// load gdt
	gdt_base = (uint32_t *)(&gdt_ptr[2]);
	gdt_lim = (uint16_t *)(&gdt_ptr[0]);
	*gdt_base = (uint32_t)&gdt;
	*gdt_lim = GDT_NUM * 8 - 1;
	__asm__ __volatile__("lgdt gdt_ptr");
}

void init_idt(void)
{
	uint32_t *idt_base;
	uint16_t *idt_lim;
	int i;

	memset(idt, 0, sizeof(idt));

	for(i=0; i<ISR_NUM; i++){
		set_gate(&idt[i], isr_table[i], 0x8e, KER_CODE);
	}

	// syscall, DPL=3
	set_gate(&idt[T_SYSCALL], isr_table[T_SYSCALL], 0xef, KER_CODE);

	idt_base = (uint32_t *)(&idt_ptr[2]);
	idt_lim = (uint16_t *)(&idt_ptr[0]);
	*idt_base = (uint32_t)&idt;
	*idt_lim = IDT_NUM * 8 - 1;
	__asm__ __volatile__("lidt idt_ptr");
}

void dump_gdt(void)
{
	int i;
	uint32_t *gp = (uint32_t *)&gdt;

	printf("\n------- dump_gdt() start ------\n");
	for(i=0; i<6; i++) {
		dump_descriptor((DESCRIPTOR *)gp);
		gp += 2;
	}
	printf("------- dump_gdt() end --------\n");
}

int main(void)
{
	char *os_str = "Welcome to ChuniX! :)\n";
	extern uint32_t edata[], end[];
	TASK_STRUCT *mytask;
	extern char _binary____user_hello_start[], _binary____user_hello_size[];

	// clear BSS section
	memset(edata, 0, end - edata);

	cons_init();
	printf("%s\n", os_str);

	mem_init();
	setupkvm();
	init_gdt();
	//dump_gdt();
	init_idt();

	init_8259A();
	install_timer(100);
	init_keyboard();
	init_hd();
	mkfs();
	init_fs();

	rootp = 0;
	mytask = task_create(_binary____user_hello_start,
			(uint32_t)_binary____user_hello_size);
	task_run(mytask);
	current = rootp;

	// we should setup all things before sti()
	__asm__("sti\n");
	printf("After sti()\n");

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
