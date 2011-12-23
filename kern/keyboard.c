#include <types.h>
#include <x86.h>

const char kb_map[] = {
	0x0, 0x0, '1', '2', '3', '4', '5', '6',
	'7', '8', '9', '0', '-', '=', '\b', '\t',
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
	'o', 'p', '[', ']', '\n', 0x0, 'a', 's',
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
	'\'', '`', 0x0, '\\', 'z', 'x', 'c', 'v',
	'b', 'n', 'm', ',', '.', '/', 0x0, '*',
	0x0, ' '
};

char read_char(void)
{
	char val;
	int scan_code = inb(0x60);

	if(! (scan_code & 0x80))
		put_c(kb_map[scan_code&0x7f]);

	/* finish handle key and re-enable interrupt */
	val = inb(0x61);
	outb(0x61, val|0x80);
	outb(0x61, val&0x7f);

	outb(0x20, 0x20);

}

void keyboard_isr(void)
{
	read_char();
}

void init_keyboard(void)
{
	outb(0x21, inb(0x21)&0xfd);
}

