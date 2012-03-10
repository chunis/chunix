#include "string.h"

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

	while(*s)
		len++;

	return len;
}
