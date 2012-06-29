#ifndef __USER_H__
#define __USER_H__

#include <types.h>

// syscall
int write(int, void*, int);
uint32_t get_ticks(void);

// printf.c
void printf(char *fmt, ...);

// utils.c
void sleep(int sec);

#endif
