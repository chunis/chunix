//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#include <syscall.h>
#include <types.h>
#include <const.h>
#include <printf.h>
#include "task.h"
#include "sched.h"

extern int sf_open(const char *path, int flags);
extern int sf_creat(const char *path, int flags);
extern int sf_read(int fd, void *buf, int n);
extern int sf_write(int fd, const void *buf, int n);
extern int sf_close(int fd);

// return the int at addr from task p.
int getint(struct task *tp, uint32_t addr)
{
	// TODO: need to check if addr is in tp's address space
	return *(int *)addr;
}

// get the nul-terminated string at addr from task p.
// just sets *pp to point at it, and returns the length of string
int getstr(struct task *tp, uint32_t addr, char **pp)
{
	char *s;

	*pp = (char *)addr;
	for(s = *pp; *s; s++)
		; // do nothing

	return s - *pp;
}

// get the nth int syscall argument
int argint(int n)
{
	return getint(current, current->tf->esp+4+4*n);
}

// get the nth int syscall argument used as a pointer to a block of memory
char *argptr(int n)
{
	return (char *)argint(n);
}

// get the nth int syscall argument used as a string pointer
int argstr(int n, char **pp)
{
	int addr;

	addr = argint(n);
	return (char *)getstr(current, addr, pp);
}

uint32_t sys_gticks(void)
{
	return timer_ticks;
}

int sys_open(void)
{
	int flag;
	char *path;

	argstr(0, &path);
	flag = argint(1);

	return sf_open(path, flag);
}

int sys_creat(void)
{
	int flag;
	char *path;

	argstr(0, &path);
	flag = argint(1);

	return sf_creat(path, flag);
}

int sys_read(void)
{
	int fd, len;
	char *p;
	int s;

	fd = argint(0);
	argstr(1, &p);
	len = argint(2);

	if(fd == 0){  // get data from stdin
		return stdin_read(p, len);
	} else {
		return sf_read(fd, p, len);
	}
}

int sys_write(void)
{
	int fd, len;
	char *p;
	int s;

	fd = argint(0);
	argstr(1, &p);
	len = argint(2);

	if(fd == 1 || fd == 2){  // print to stdout
		if(len == 1)
			put_c(*p);
		else
			printk("%s", p);
	} else {
		sf_write(fd, p, len);
	}

	return 0;
}

int sys_close(void)
{
	int fd = argint(0);

	return sf_close(fd);
}

int sys_fork(void)
{
	return fork();
}

int sys_exit(void)
{
	exit();
	return 0;
}

int sys_wait(void)
{
	return wait();
}

int sys_exec(void)
{
	char *path, *argv[MAXARGS];
	uint32_t uarg, uargv, i;
	char **p;

	argstr(0, &path);
	uargv = (uint32_t)argint(1);
	memset(argv, 0, sizeof(argv));

	for(i=0; ; i++){
		if(i >= MAXARGS)
			return -1;
		if(uargv == 0)  // no arguments
			break;

		uarg = (uint32_t)getint(current, uargv + i * 4);
		if(uarg < 0)
			return -1;
		if(uarg == 0){  // the last element of passed argv[]
			argv[i] = 0;
			break;
		}
		if(getstr(current, uarg, &argv[i]) < 0)
			return -1;
	}
	return exec(path, argv);
}

int sys_stat(void)
{
	char *path, *buf;

	argstr(0, &path);
	argstr(1, &buf);

	return sf_stat(path, (struct sfs_stat *)buf);
}

static int (*syscalls[])(void) = {
	[SYS_open] = sys_open,
	[SYS_creat] = sys_creat,
	[SYS_read] = sys_read,
	[SYS_write] = sys_write,
	[SYS_close] = sys_close,
	[SYS_get_ticks] = sys_gticks,
	[SYS_fork] = sys_fork,
	[SYS_wait] = sys_wait,
	[SYS_exit] = sys_exit,
	[SYS_exec] = sys_exec,
	[SYS_stat] = sys_stat,
};

void syscall(void)
{
	int num;

	// get syscall number
	num = current->tf->eax;

	if(syscalls[num]){
		current->tf->eax = syscalls[num]();
	} else {
		printk("Unknown syscall number: %d\n", num);
		current->tf->eax = -1;
	}
}
