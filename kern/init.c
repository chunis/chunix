#include <types.h>
#include <x86.h>

#include "global.h"
#include "string.h"
#include "console.h"

extern uint8_t gdt[];
extern uint8_t gdt_ptr[];

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
	outb(0x21, 0xff);
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

void idt_entry(uint8_t *idtp, uint32_t offset, int *sel)
{
	uint16_t *p = idtp;
	*(p+0) = offset & 0xffff;
	*(p+1) = 0x8e00;
	*(p+2) = (uint16_t)sel;
	*(p+3) = (offset >> 16) & 0xffff;
}

void init_idt(void)
{
	uint32_t *idt_base;
	uint16_t *idt_lim;

	idt_base = (uint32_t *)(&idt_ptr[2]);
	idt_lim = (uint16_t *)(&idt_ptr[0]);
	*idt_base = (uint32_t)&gdt;
	*idt_lim = IDT_NUM * 8 - 1;
}

int main(void)
{
	int i = 0;
	char wheel[] = { '\\', '|', '/', '-' };
	char *os_str = "Welcome to ChuniX! :)";

	cons_init();
	putstr(os_str);

	//init_8259A();

	for(;;){
		__asm__ ("movb	%%al, 0xb8000+160*24"::"a"(wheel[i]));
		if(i == sizeof wheel)
			i = 0;
		else
			i++;
	}

	return 0;
}
