//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#include <user.h>

void sleep(int sec)
{
	int start;

	start = get_ticks();

	while(1){
		if(get_ticks() - start > sec * 100)
			break;
	}
}

void msleep(int msec)
{
	int start;

	start = get_ticks();

	while(1){
		if(get_ticks() - start > msec / 10)
			break;
	}
}

// get at most n chars to buf, ends with '\n' if less than n chars
void gets(char *buf, int n)
{
	int r, i = 0;
	char c;

	while(i+1 < n){
		r = read(0, &c, 1);
		if(r == -1)
			continue;
		if(r == 0)
			break;

		buf[i++] = c;
		if(c == '\n' || c == '\r')
			break;
	}
	buf[i] = '\0';
}
