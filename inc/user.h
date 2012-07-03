#ifndef __USER_H__
#define __USER_H__

#include <types.h>

// syscall
int open(const char *pathname, int flags);
int creat(const char *pathname, int flags);
int read(int fd, void *buf, int n);
int write(int fd, const void *buf, int n);
int close(int fd);
uint32_t get_ticks(void);

// printf.c
void printf(char *fmt, ...);

// utils.c
void sleep(int sec);

#endif
