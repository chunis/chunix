#include <types.h>
#include <x86.h>
#include <mmu.h>
#include <string.h>
#include <spinlock.h>
#include "console.h"

#define CEIL(num, base)	(((num)/(base)+1)*(base))
#define TAB_WIDTH	8
#define BRIGHT_GREEN	0xa
#define BLANK_CHAR	(BRIGHT_GREEN << 0x8 | 0x20)

static uint16_t *cons_buf;
static uint16_t cons_pos;
uint8_t default_color = BRIGHT_GREEN;
static uint8_t dcolor = BRIGHT_GREEN;
struct spinlock cons_lock;

static uint16_t locate_cursor(void)
{
	uint16_t pos;

	outb(CGA_BASE, 14);
	pos = inb(CGA_BASE + 1) << 8;
	outb(CGA_BASE, 15);
	pos |= inb(CGA_BASE + 1);
	return pos;
}

static void put_cursor(uint16_t pos)
{
	outb(CGA_BASE, 14);
	outb(CGA_BASE + 1, pos >> 8);
	outb(CGA_BASE, 15);
	outb(CGA_BASE + 1, pos);

}

// clean screen, write all of cons_buf[] with space chars.
void clean_screen(void)
{
	int i;
	for(i=0; i<CONS_SIZE; i++)
		cons_buf[i] = BLANK_CHAR;

	// reset cons_pos to the start
	cons_pos = 0;
	put_cursor(cons_pos);
}

void get_cursor(int *x, int *y)
{
	*x = cons_pos % CONS_COLS;
	*y = cons_pos / CONS_COLS;
}

void set_cursor(int x, int y)
{
	cons_pos = y * CONS_COLS + x;
	put_cursor(cons_pos);
}

void cons_init(void)
{
	cons_buf = (uint16_t *)P2V(CGA_START);
	cons_pos = 0;
	cbuf.wpos = 0;
	cbuf.rpos = 0;

	init_lock(&cons_lock, "cons_lock");
	clean_screen();
}

void settextcolor(uint8_t forecolor, uint8_t backcolor)
{
	dcolor = (backcolor << 4) | (forecolor & 0x0F);
}

void resettextcolor(void)
{
	dcolor = default_color;
}

void put_color_c(char color, char c)
{
	char oldcolor;

	oldcolor = dcolor;
	dcolor = color;
	put_c(c);
	dcolor = oldcolor;
}

void put_c(char c)
{
	int cnt;
	char blank = 0x20;
	uint16_t color = dcolor << 8;

	switch(c){
		case '\b':
			if(cons_pos > 0)
				cons_buf[--cons_pos] = BLANK_CHAR;
			break;
		case '\n':
			cons_pos += CONS_COLS;
			cons_pos -= (cons_pos % CONS_COLS);
			break;
		case '\t':
			cnt = CEIL(cons_pos, TAB_WIDTH) - cons_pos;
			for(; cnt>0; cnt--)
				cons_buf[cons_pos++] = (color | blank);
			break;
		default:
			cons_buf[cons_pos++] = (color | c);
			break;
	}

	// scroll if necessory. we leave the top row as 'statusbar'
	if(cons_pos >= CONS_SIZE){
		memmove(cons_buf + CONS_COLS, cons_buf + CONS_COLS * 2,
			(CONS_SIZE - CONS_COLS * 2) * sizeof(uint16_t));
		for(cnt = CONS_SIZE - CONS_COLS; cnt < CONS_SIZE; cnt++)
			cons_buf[cnt] = (color | blank);
		cons_pos -= CONS_COLS;
	}

	// move cursor to follow
	put_cursor(cons_pos);
}

// argument:
// 'block' = 1: sleep until return with a char.
// 'block' = 0: don't block, just return -1 to mean no char returned
int get_c(int block)
{
	char c;

	if(cbuf.rpos == cbuf.wpos){  // no chars available in cbuf.buf[]
		if(block)
			sleep_on(&cbuf.rpos);
		else
			return -1;
	}

	c = cbuf.buf[cbuf.rpos++];
	if(cbuf.rpos == CONS_SIZE)
		cbuf.rpos = 0;

	return c;
}
