#include <x86.h>


#define COM1	0x3F8   // COM1

static int has_serial;

 
void init_serial(void)
{
	char *p = "init serial done";

	outb(COM1 + 1, 0x00);    // disable interrupt

	outb(COM1 + 3, 0x80);    // enable DLAB (set baud rate divisor)
	outb(COM1 + 0, 115200/9600);  // (low byte) Set baud 9600
	outb(COM1 + 1, 0);            // (high byte)
	outb(COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit

	outb(COM1 + 2, 0);       // turn off the FIFO
	outb(COM1 + 4, 0);
	outb(COM1 + 1, 0x01);    // enable interrupt

	if(inb(COM1 + 5) == 0xFF)  // no serial port
		return;
	has_serial = 1;

	(void) inb(COM1 + 2);
	(void) inb(COM1 + 0);

	while(*p)
		write_serial(*p++);
}

static int read_serial(void)
{
	if(!has_serial)
		return -1;
	if((inb(COM1 + 5) & 0x1) == 0)
		return -1;
	return inb(COM1 + 0);
}

void write_serial(int c)
{
	if(!has_serial)
		return;
	while ( (inb(COM1 + 5) & 0x20) == 0)
		;

	outb(COM1 + 0, c);
}
