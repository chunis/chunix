#include <x86.h>


#define COM1	0x3F8   // COM1

static int has_serial;
void write_serial(int c);
 
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

	outb(0x21, inb(0x21)&0xef);  // enable COM1

	while(*p)
		write_serial(*p++);
}

static int read_serial(void)
{
	if(!has_serial)
		return -1;

	//while((inb(COM1 + 5) & 0x1) == 0)
	//	;
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
	if(c == '\r')
		write_serial('\n');
}

void serial_isr(void)
{
	console_isr(read_serial);
	outb(0x20, 0x20);
}

void _serial_isr(void)
{
	int c;

	c = read_serial();
	if(c == -1)
		return;
	put_c((char)c);
}
