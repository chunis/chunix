//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#include <string.h>
#include <x86.h>

void *memmove(void *dst, const void *src, uint32_t n)
{
	const char *s = src;
	char *d = dst;

	if(s < d && s+n > d){
		s += n;
		d += n;
		while(n-- > 0)
			*--d = *--s;
	} else {
		while(n-- > 0)
			*d++ = *s++;
	}

	return dst;
}

void strncpy(char *d, const char *s, uint32_t n)
{
	while(n-- > 0)
		*d++ = *s++;
	*d = '\0';
}

void strcpy(char *d, const char *s)
{
	do {
		*d++ = *s;
	} while(*s++);
}

int strlen(char const *s)
{
	int len = 0;

	while(*s++)
		len++;

	return len;
}

int strcmp(char const *s, char const *t)
{
	while(*s && *t && *s == *t){
		s++;
		t++;
	}
	if(*s == '\0' && *t == '\0')
		return 0;
	else if(*s == '\0')
		return -1;
	else if(*t == '\0')
		return 1;
	else
		return *s - *t;
}

int strncmp(char const *s, char const *t, int n)
{
	while(s && t && *s == *t && n>0){
		s++;
		t++;
		n--;
	}
	if(n == 0)
		return 0;
	else if(s == NULL && t == NULL)
		return 0;
	else if(s == NULL)
		return -1;
	else if(t == NULL)
		return 1;
	else
		return *s - *t;
}

void *memset(void *s, char c, int n)
{
	stosb(s, c, n);
	return s;
}
