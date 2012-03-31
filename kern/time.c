#include <types.h>
#include <x86.h>
#include "task.h"
#include "global.h"

void install_timer(int freq)
{
	uint8_t mask;
	uint32_t div = 1193180/freq;

	printf("install timer now...\n");

	outb(0x43, 0x36);
	outb(0x40, div & 0xff);
	outb(0x40, div >> 8);

	mask = inb(0x21);
	outb(0x21, mask & 0xfe);
}

uint32_t timer_ticks = 0;

#define TEST 0
#if(TEST == 0)
void timer_isr(void)
{
	int x, y;

	get_cursor(&x, &y);
	set_cursor(62, 24);
	printf("time: %d\n", timer_ticks++);
	set_cursor(x, y);

	outb(0x20, 0x20);
}

#else

char kernel_stack[512];
int reent = -1;

void timer_isr(void)
{
	int x, y;

	if(++reent != 0){
		--reent;
		return;
	}

	__asm__ ("movl %%eax, %%esp\n\t"::"a"(kernel_stack+512));
	outb(0x20, 0x20);

	get_cursor(&x, &y);
	set_cursor(62, 24);
	printf("time: %d\n", timer_ticks++);
	set_cursor(x, y);

	current = current->next;
	if(current == 0)
		current = rootp;
	tss.esp0 = current->ldt;
	//printf("current: %x, lldt: %d, ", current, current->ldt_sel);
	//printf("name: %s\n", current->name);

	--reent;
	__asm__ ("lldt %%ax\n\t"::"a"(current->ldt_sel));
	__asm__ ("movl %%eax, %%esp\n\t"::"a"((uint32_t)current));
}
#endif
