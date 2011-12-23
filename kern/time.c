#include <types.h>
#include <x86.h>

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
void timer_isr(void)
{
	int x, y;

	get_cursor(&x, &y);
	set_cursor(62, 24);
	printf("time: %d\n", timer_ticks++);
	set_cursor(x, y);

	outb(0x20, 0x20);
}
