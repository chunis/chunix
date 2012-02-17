#include <types.h>
#include <x86.h>
#include "hd.h"

// print some info about hard disk
static void hd_identify(int drv)
{
	printf("Hello, in hd_identify now\n");
}

// enable interrupt, and setup isr, then do something else
void init_hd(void)
{
	outb(0x22, inb(0x22)&0xfb);  // IRQ 2 locates at master
	outb(0x2e, inb(0x2e)&0xff);  // IRQ 14 locates at slave

	hd_identify(0);
}

void hd_isr(void)
{
	uint8_t sta = inb(HDR_STATUS);
}
