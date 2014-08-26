#include <types.h>
#include <x86.h>
#include <printf.h>
#include "task.h"
#include "sched.h"
#include <console.h>

void install_timer(int freq)
{
	uint8_t mask;
	uint32_t div = 1193180/freq;

	printk("install timer now...\n");

	outb(0x43, 0x36);
	outb(0x40, div & 0xff);
	outb(0x40, div >> 8);

	mask = inb(0x21);
	outb(0x21, mask & 0xfe);
}

uint32_t timer_ticks = 0;
int reent = -1;
uint8_t timer_flag;

void timer_isr(void)
{
	int x, y;

	if(++reent != 0){
		--reent;
		return;
	}

	outb(0x20, 0x20);

	if(timer_ticks++ % 100 == 0){
		get_cursor(&x, &y);
		set_cursor(64, 0);
		timer_flag = 1;
		printk("time: %d sec\n", timer_ticks/100);
		timer_flag = 0;
		set_cursor(x, y);
	}

	--reent;
	sched_yield();
}
