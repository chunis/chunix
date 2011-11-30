#include <types.h>
#include "console.h"

static uint16_t *cons_buf;
static uint16_t cons_pos;

void cons_init(void)
{
	cons_buf = (uint16_t *)CGA_START;
	cons_pos = 0;
}

void put_color_c(char color, char c)
{
	int x = (color << 0x8 | c);
	cons_buf[cons_pos++] = x;
}

void put_c(char c)
{
	put_color_c(0xa, c);
}

void putstr(char *str)
{
	char *p = str;
	while(*p)
		put_c(*p++);
}
