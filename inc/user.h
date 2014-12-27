//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#ifndef __USER_H__
#define __USER_H__

#include <types.h>
#include <stat.h>

// for debug
#define USER_DEBUG 1

#if USER_DEBUG
#define dprintf printf
#else
#define dprintf
#endif

// syscall
int open(const char *pathname, int flags);
int creat(const char *pathname, int flags);
int read(int fd, void *buf, int n);
int write(int fd, const void *buf, int n);
int close(int fd);
uint32_t get_ticks(void);
int fork(void);
int wait(void);
int exit(void) __attribute__((noreturn));
int exec(char *path, char **argv);
int stat(const char *filename, struct stat *stat_buf);

// printf.c
void printf(char *fmt, ...);

// utils.c
void sleep(int sec);
void msleep(int msec);
void gets(char *buf, int n);

#endif
