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

void init_gdt(void)
{
	uint32_t *gdt_base;
	uint16_t *gdt_lim;

	memmove(&gdt, 	                  // New GDT
	(void*)(*((uint32_t *)(&gdt_ptr[2]))),   // Base  of Old GDT
	*((uint16_t *)(&gdt_ptr[0])) + 1      // Limit of Old GDT
	);

	gdt_base = (uint32_t *)(&gdt_ptr[2]);
	gdt_lim = (uint16_t *)(&gdt_ptr[0]);
	*gdt_base = (uint32_t)&gdt;
	*gdt_lim = GDT_NUM * 8 - 1;
}

void idt_entry(uint8_t *idtp, int_handler handler, uint16_t sel)
{
	uint32_t offset = (uint32_t)handler;
	uint16_t *p = (uint16_t *)idtp;

	*(p+0) = offset & 0xffff;
	*(p+1) = (uint16_t)sel;
	*(p+2) = 0x8e00;	// attributes
	*(p+3) = (offset >> 16) & 0xffff;
}

void init_idt(void)
{
	uint32_t *idt_base;
	uint16_t *idt_lim;
	int i;

	for(i=0; i<ISR_NUM; i++){
		idt_entry(&idt[i<<3], isr_table[i], KER_CODE);
	}

	idt_base = (uint32_t *)(&idt_ptr[2]);
	idt_lim = (uint16_t *)(&idt_ptr[0]);
	*idt_base = (uint32_t)&idt;
	*idt_lim = IDT_NUM * 8 - 1;
}

// add tss to gdt
void setup_tss(void)
{
	set_descriptor((DESCRIPTOR *)&gdt[KER_TSS], (uint32_t)&tss, sizeof(tss)-1, DA_386TSS);
	memmove(&tss, 0, sizeof(tss));
	tss.ss0 = KER_DATA;
	//tss.iobase = sizeof(tss);

	__asm__ ("ltrw  %%ax\n\t"::"a"(KER_TSS));
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
	int i = 0;
	char wheel[] = { '\\', '|', '/', '-' };
	char *os_str = "Welcome to ChuniX! :)\n";
	extern edata[], end[];

	// clear BSS section
	memset(edata, 0, end - edata);

	cons_init();
	printf("%s\n", os_str);

	mem_init();

	init_8259A();
	install_timer(100);
	init_keyboard();
	init_hd();
	mkfs();
	init_fs();

	rootp = 0;
	new_task(&task1, "TaskA", (uint32_t)taskA, (uint32_t)&task1_stack3+USR_STACK_SIZE, KER_LDT1);
	new_task(&task2, "TaskB", (uint32_t)taskB, (uint32_t)&task2_stack3+USR_STACK_SIZE, KER_LDT2);
	current = rootp;

	setup_tss();
	tss.esp0 = current->ldt;

	// we should setup new_task first, then sti()
	__asm__("sti\n");

	//dump_gdt();
	__asm__ ("lldt %%ax\n\t"::"a"(current->ldt_sel));
	__asm__ ("movl %%eax, %%esp\n\t"::"a"((uint32_t)current));
	__asm__ ("popl %gs\n\t" \
			"popl %fs\n\t" \
			"popl %es\n\t" \
			"popl %ds\n\t" \
			"popal\n\t" \
			"addl $0x4, %esp\n\t" \
			"iret\n\t");

	for(;;){
		__asm__ ("movb	%%al, 0xb8000+160*24"::"a"(wheel[i]));
		if(i == sizeof wheel)
			i = 0;
		else
			i++;
	}

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
