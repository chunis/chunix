#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#define MDA_START	0xB0000
#define MDA_SIZE	0x02000	//  8K
#define CGA_START	0xB8000
#define CGA_SIZE	0x04000	// 16K
#define CGA_BASE	0x3D4
#define EGA_START	0xA0000
#define EGA_SIZE	0x08000	// 32K
#define EGA_START	0xA0000
#define EGA_SIZE	0x08000	// 32K

#define CONS_ROWS	25
#define CONS_COLS	80
#define CONS_SIZE	(CONS_ROWS * CONS_COLS)

void cons_init(void);
void put_c(char c);
char get_c(void);

struct console_buf {
	char buf[CONS_SIZE];
	int rpos;  // read position
	int wpos;  // write position
} cbuf;

#endif
