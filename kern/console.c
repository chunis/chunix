#include <types.h>
#include "console.h"

static uint16_t *cons_buf;
static uint16_t cons_pos;

#define CEIL(num, base) (((num)/(base)+1)*(base))
#define TAB_WIDTH	8
#define BRIGHT_GREEN 0xa
#define BLANK_CHAR	(BRIGHT_GREEN << 0x8 | 0x20)


// clean screen, write all of cons_buf[] with space chars.
void clean_screen(void)
{
	int i;
	for(i=0; i<CONS_SIZE; i++)
		cons_buf[i] = BLANK_CHAR;

	// reset cons_pos to the start
	cons_pos = 0;
}

void cons_init(void)
{
	cons_buf = (uint16_t *)CGA_START;
	cons_pos = 0;

	clean_screen();
}

void put_color_c(char color, char c)
{
	int x = (color << 0x8 | c);

	if(cons_pos >= CONS_SIZE)
		cons_pos = 0;

	cons_buf[cons_pos++] = x;
}

void put_c(char c)
{
	int cnt;
	char blank = 0x20;

	switch(c){
		case '\b':
			cons_buf[--cons_pos] = BLANK_CHAR;
			break;
		case '\n':
			cnt = CEIL(cons_pos, CONS_COLS) - cons_pos;
			for(; cnt>0; cnt--)
				put_color_c(BRIGHT_GREEN, blank);
			break;
		case '\t':
			cnt = CEIL(cons_pos, TAB_WIDTH) - cons_pos;
			for(; cnt>0; cnt--)
				put_color_c(BRIGHT_GREEN, blank);
			break;
		default:
			put_color_c(BRIGHT_GREEN, c);
			break;
	}
}

void putstr(char *str)
{
	char *p = str;
	while(*p)
		put_c(*p++);
}
