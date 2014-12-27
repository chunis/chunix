//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#include <types.h>

#define BUF_LEN		(64+3)

void putc(char c)
{
	write(1, &c, 1);
}

static void printint(int n, int base, int sign)
{
	static char digits[] = "0123456789ABCDEF";
	char buf[BUF_LEN];
	int i = 0, flag = 0;
	uint32_t ui = n;

	if(sign && n < 0){
		ui = - n;
		flag = 1;
	}
	do{
		buf[i++] = digits[ui % base];
	}while((ui /= base) != 0);

	if(flag)
		buf[i++] = '-';

	if(base == 2)
		buf[i++] = 'b';
	else if(base == 8)
		buf[i++] = 'o';
	else if(base == 16)
		buf[i++] = 'x';

	if(base != 10)
		buf[i++] = '0';

	while(--i >= 0)
		putc(buf[i]);
}

// only understands %d, %b, %o, %x, %p, %s
void printf(char *fmt, ...)
{
	int i;
	char c, *s;
	uint32_t *ap;

	if(fmt == '\0')	// should panic here
		return;

	ap = (uint32_t *)(void *)(&fmt + 1);
	for(i=0; (c=fmt[i]) != '\0'; i++){
		if(c != '%'){
			putc(c);
			continue;
		}
		c = fmt[++i];
		if(c == '\0')
			break;
		switch(c){
			case 'd':
				printint(*ap++, 10, 1);
				break;
			case 'b':
				printint(*ap++, 2, 0);
				break;
			case 'o':
				printint(*ap++, 8, 0);
				break;
			case 'x':
			case 'p':
				printint(*ap++, 16, 0);
				break;
			case 's':
				for(s = (char *)*ap++; *s; s++)
					putc(*s);
				break;
			default:
				break;
		}
	}
}
