#ifndef __STRING_H__
#define __STRING_H__

#include <types.h>

void *memmove(void *dst, const void *src, uint32_t n);
void strncpy(char *d, const char *s, uint32_t n);
void strcpy(char *d, const char *s);
int strlen(char const *s);
int strcmp(char const *s, char const *t);
int strncmp(char const *s, char const *t, int n);

#endif
